
#include "py/obj.h"
#include "py/runtime.h"

/*********************************************************************
 *  Adder.sum(self, a, b)
 *
 *  This is the method implementation. It:
 *     - receives two Python objects (a, b)
 *     - converts them to C integers
 *     - adds them
 *     - returns a new Python integer object
 *********************************************************************/
static mp_obj_t adder_sum(mp_obj_t self_in, mp_obj_t a_in, mp_obj_t b_in) {
    // Convert arguments to C ints
    mp_int_t a = mp_obj_get_int(a_in);
    mp_int_t b = mp_obj_get_int(b_in);

    // Return Python int object
    return mp_obj_new_int(a + b);
}
static MP_DEFINE_CONST_FUN_OBJ_3(adder_sum_obj, adder_sum);



/*********************************************************************
 *  Adder class definition
 *
 *  Every MicroPython class implemented in C needs:
 *     - a struct to represent the object
 *     - a constructor (make_new)
 *     - a locals dict of methods/attributes
 *     - a type struct describing the class
 *********************************************************************/

// Internal C struct for instances of Adder.
// For this simple example, the class has no member data.
typedef struct _adder_obj_t {
    mp_obj_base_t base;   // MUST be first: identifies the object as a Python object
} adder_obj_t;


/*********************************************************************
 *  Constructor for Adder()
 *  Called whenever Python does:
 *      a = Adder()
 *********************************************************************/
static mp_obj_t adder_make_new(const mp_obj_type_t *type,
                               size_t n_args, size_t n_kw,
                               const mp_obj_t *args) {

    // Allocate memory for the object
    adder_obj_t *self = m_new_obj(adder_obj_t);

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
static const mp_rom_map_elem_t adder_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_sum), MP_ROM_PTR(&adder_sum_obj) },
};

// Build the actual dict object
static MP_DEFINE_CONST_DICT(adder_locals_dict, adder_locals_dict_table);



/*********************************************************************
 *  The Adder type object
 *  This defines the Python class:
 *      name
 *      constructor
 *      method table
 *  Required for any class implemented in C.
 *********************************************************************/
MP_DEFINE_CONST_OBJ_TYPE(
    adder_type,
    MP_QSTR_Adder,
    MP_TYPE_FLAG_NONE,
    make_new, adder_make_new,
    locals_dict, &adder_locals_dict
    );


/*********************************************************************
 *  Module definition
 *  Exposes:
 *      Adder
 *  So Python can do:
 *      import ccls
 *      a = ccls.Adder()
 *********************************************************************/
static const mp_rom_map_elem_t cmodule_ccls_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_mymodule) },
    { MP_ROM_QSTR(MP_QSTR_Adder), MP_ROM_PTR(&adder_type) },
};

// Build module globals dict
static MP_DEFINE_CONST_DICT(cmodule_ccls_globals, cmodule_ccls_globals_table);

// Define module object
const mp_obj_module_t cmodule_ccls = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&cmodule_ccls_globals,
};


/*********************************************************************
 *  Module Registration
 *  Makes the module available to MicroPython under the name "ccls".
 *  Required for user C modules.
 *********************************************************************/
MP_REGISTER_MODULE(MP_QSTR_ccls, cmodule_ccls);

