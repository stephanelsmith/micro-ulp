
#include "py/obj.h"
#include "py/runtime.h"


/*********************************************************************
 *  ULP class definition
 *  Every MicroPython class implemented in C needs:
 *     - a struct to represent the object
 *     - a constructor (make_new)
 *     - a locals dict of methods/attributes
 *     - a type struct describing the class
 *********************************************************************/

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



/*********************************************************************
 *  locals_dict — dictionary of methods exposed on the class
 *  This is equivalent to:
 *      class Adder:
 *          def sum(self, a, b): ...
 *********************************************************************/
static const mp_rom_map_elem_t ulp_locals_dict_table[] = {
    //{ MP_ROM_QSTR(MP_QSTR_set_wakeup_period), MP_ROM_PTR(&esp32_ulp_set_wakeup_period_obj) },
    //{ MP_ROM_QSTR(MP_QSTR_run), MP_ROM_PTR(&esp32_ulp_load_and_run_obj) },
    //{ MP_ROM_QSTR(MP_QSTR_run_embedded), MP_ROM_PTR(&esp32_ulp_load_and_run_embedded_obj) },
    //{ MP_ROM_QSTR(MP_QSTR_pause), MP_ROM_PTR(&esp32_ulp_pause_obj) },
    //{ MP_ROM_QSTR(MP_QSTR_resume), MP_ROM_PTR(&esp32_ulp_resume_obj) },
    //{ MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&esp32_ulp_read_obj) },
    //{ MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&esp32_ulp_write_obj) },
    //{ MP_ROM_QSTR(MP_QSTR_rtc_init), MP_ROM_PTR(&esp32_ulp_rtc_init_obj) },
    //{ MP_ROM_QSTR(MP_QSTR_rtc_deinit), MP_ROM_PTR(&esp32_ulp_rtc_deinit_obj) },
    //{ MP_ROM_QSTR(MP_QSTR_adc_init), MP_ROM_PTR(&esp32_ulp_adc_init_obj) },
    //{ MP_ROM_QSTR(MP_QSTR_RESERVE_MEM), MP_ROM_INT(CONFIG_ULP_COPROC_RESERVE_MEM) },
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

