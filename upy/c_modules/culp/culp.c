
#include "py/obj.h"
#include "py/runtime.h"

#include "ulp_riscv.h"

#include "hal/adc_types.h"
#include "driver/rtc_io.h"
#include "ulp_adc.h"
#include "py/mphal.h"

/*********************************************************************
 *  ULP class definition
 *  Every MicroPython class implemented in C needs:
 *     - a struct to represent the object
 *     - a constructor (make_new)
 *     - a locals dict of methods/attributes
 *     - a type struct describing the class
 *********************************************************************/

extern const uint8_t ulp_main_bin_start[] asm ("_binary_ulp_embedded_bin_start");
extern const uint8_t ulp_main_bin_end[]   asm ("_binary_ulp_embedded_bin_end");

// Internal C struct for instances of ULP.
// For this simple example, the class has no member data.
typedef struct _ulp_obj_t {
    mp_obj_base_t base;   // MUST be first: identifies the object as a Python object
} ulp_obj_t;


/*********************************************************************
 *  Constructor for Adder()
 *  Called whenever Python does:
 *      a = Adder()
 *********************************************************************/
static mp_obj_t ulp_make_new(const mp_obj_type_t *type,
                               size_t n_args, size_t n_kw,
                               const mp_obj_t *args) {
    // Allocate memory for the object
    ulp_obj_t *self = m_new_obj(ulp_obj_t);

    // Assign the object's type (critical)
    self->base.type = type;

    // Return as a Python object
    return MP_OBJ_FROM_PTR(self);
}



static mp_obj_t esp32_ulp_set_wakeup_period(
    mp_obj_t self_in,
    mp_obj_t period_index_in,
    mp_obj_t period_us_in
    ) {
    mp_uint_t period_index = 0;
    period_index = mp_obj_get_int(period_index_in);

    mp_uint_t period_us = mp_obj_get_int(period_us_in);
    int _errno = ulp_set_wakeup_period(period_index, period_us);
    if (_errno != ESP_OK) {
        mp_raise_OSError(_errno);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(esp32_ulp_set_wakeup_period_obj, esp32_ulp_set_wakeup_period);


static mp_obj_t esp32_ulp_load_and_run_embedded(mp_obj_t self_in) {
    int _errno;
    _errno = ulp_riscv_load_binary(ulp_main_bin_start, (ulp_main_bin_end - ulp_main_bin_start));
    if (_errno != ESP_OK) {
        mp_raise_OSError(_errno);
    }

    _errno = ulp_riscv_run();
    if (_errno != ESP_OK) {
        mp_raise_OSError(_errno);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(esp32_ulp_load_and_run_embedded_obj, esp32_ulp_load_and_run_embedded);

static mp_obj_t esp32_ulp_pause(mp_obj_t self_in) {
    ulp_riscv_timer_stop();
    ulp_riscv_halt();
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(esp32_ulp_pause_obj, esp32_ulp_pause);

static mp_obj_t esp32_ulp_resume(mp_obj_t self_in) {
    ulp_riscv_timer_resume();
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(esp32_ulp_resume_obj, esp32_ulp_resume);


static mp_obj_t esp32_ulp_read(mp_obj_t self_in, mp_obj_t address) {
    uint32_t addr = mp_obj_get_int(address);
    if (addr < (uintptr_t)RTC_SLOW_MEM) {
        addr += (uintptr_t)RTC_SLOW_MEM;
    }
    if (addr > ((uintptr_t)(RTC_SLOW_MEM) + CONFIG_ULP_COPROC_RESERVE_MEM)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid address"));
    }

    uint32_t val = *(uint32_t *)addr;
    return mp_obj_new_int(val);
}
static MP_DEFINE_CONST_FUN_OBJ_2(esp32_ulp_read_obj, esp32_ulp_read);

static mp_obj_t esp32_ulp_write(mp_obj_t self_in, mp_obj_t address, mp_obj_t value) {
    uintptr_t addr = mp_obj_get_int(address);
    if (addr < (uintptr_t)RTC_SLOW_MEM) {
        addr += (uintptr_t)RTC_SLOW_MEM;
    }
    if (addr > ((uintptr_t)(RTC_SLOW_MEM) + CONFIG_ULP_COPROC_RESERVE_MEM)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid address"));
    }

    *(uint32_t *)addr = mp_obj_get_int(value);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(esp32_ulp_write_obj, esp32_ulp_write);


/*
RTC pin functionality, can be moved to Pin class if needed
*/

static mp_obj_t esp32_ulp_rtc_init(mp_obj_t self_in, mp_obj_t pin_in) {
    gpio_num_t gpio_id = mp_obj_get_int(pin_in);

    if (!rtc_gpio_is_valid_gpio(gpio_id)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid pin"));
    }
    rtc_gpio_init(gpio_id);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(esp32_ulp_rtc_init_obj, esp32_ulp_rtc_init);


static mp_obj_t esp32_ulp_rtc_deinit(mp_obj_t self_in, mp_obj_t pin_in) {
    gpio_num_t gpio_id = mp_obj_get_int(pin_in);

    if (!rtc_gpio_is_valid_gpio(gpio_id)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid pin"));
    }
    rtc_gpio_deinit(gpio_id);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(esp32_ulp_rtc_deinit_obj, esp32_ulp_rtc_deinit);


static mp_obj_t esp32_ulp_adc_init(mp_obj_t self_in, mp_obj_t channel_in) {
    int channel = mp_obj_get_int(channel_in);
    ulp_adc_cfg_t cfg = {
        .adc_n = ADC_UNIT_1,
        .channel = channel,
        .width = ADC_BITWIDTH_13,
        .atten = ADC_ATTEN_DB_11,
        .ulp_mode = ADC_ULP_MODE_RISCV,
    };
    int _errno = ulp_adc_init(&cfg);
    if (_errno != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("ADC unit already in use or invalid channel"));
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(esp32_ulp_adc_init_obj, esp32_ulp_adc_init);



/*********************************************************************
 *  locals_dict — dictionary of methods exposed on the class
 *  This is equivalent to:
 *      class Adder:
 *          def sum(self, a, b): ...
 *********************************************************************/
static const mp_rom_map_elem_t ulp_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_set_wakeup_period), MP_ROM_PTR(&esp32_ulp_set_wakeup_period_obj) },
    { MP_ROM_QSTR(MP_QSTR_run_embedded), MP_ROM_PTR(&esp32_ulp_load_and_run_embedded_obj) },
    { MP_ROM_QSTR(MP_QSTR_pause), MP_ROM_PTR(&esp32_ulp_pause_obj) },
    { MP_ROM_QSTR(MP_QSTR_resume), MP_ROM_PTR(&esp32_ulp_resume_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&esp32_ulp_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&esp32_ulp_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_rtc_init), MP_ROM_PTR(&esp32_ulp_rtc_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_rtc_deinit), MP_ROM_PTR(&esp32_ulp_rtc_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_adc_init), MP_ROM_PTR(&esp32_ulp_adc_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_RESERVE_MEM), MP_ROM_INT(CONFIG_ULP_COPROC_RESERVE_MEM) },
};

// Build the actual dict object
static MP_DEFINE_CONST_DICT(ulp_locals_dict, ulp_locals_dict_table);



/*********************************************************************
 *  The Adder type object
 *  This defines the Python class:
 *      name
 *      constructor
 *      method table
 *  Required for any class implemented in C.
 *********************************************************************/
MP_DEFINE_CONST_OBJ_TYPE(
    ulp_type,
    MP_QSTR_ULP,
    MP_TYPE_FLAG_NONE,
    make_new, ulp_make_new,
    locals_dict, &ulp_locals_dict
    );


/*********************************************************************
 *  Module definition
 *  Exposes:
 *      ULP
 *  So Python can do:
 *      import culp
 *      a = culp.ULP()
 *********************************************************************/
static const mp_rom_map_elem_t cmodule_culp_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_mymodule) },
    { MP_ROM_QSTR(MP_QSTR_ULP), MP_ROM_PTR(&ulp_type) },
};

// Build module globals dict
static MP_DEFINE_CONST_DICT(cmodule_culp_globals, cmodule_culp_globals_table);

// Define module object
const mp_obj_module_t cmodule_culp = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&cmodule_culp_globals,
};


/*********************************************************************
 *  Module Registration
 *
 *  Makes the module available to MicroPython under the name "culp".
 *
 *  Required for user C modules.
 *********************************************************************/
MP_REGISTER_MODULE(MP_QSTR_culp, cmodule_culp);

