
/* Use this file as a template to start implementing a module that
   also declares object types. All occurrences of 'Xxo' should be changed
   to something reasonable for your objects. After that, all other
   occurrences of 'xx' should be changed to something reasonable for your
   module. If your module is named foo your sourcefile should be named
   foomodule.c.

   You will probably want to delete all references to 'x_attr' and add
   your own types of attributes instead.  Maybe you want to name your
   local variables other than 'self'.  If your object type is needed in
   other files, you'll have to create a file "foobarobject.h"; see
   intobject.h for an example. */

/* LazySorted objects */

#include "Python.h"

typedef struct {
    PyObject_HEAD
    PyObject            *xs;            /* Partially sorted list */
} LazySortedObject;

static PyTypeObject LazySorted_Type;

#define LazySortedObject_Check(v)      (Py_TYPE(v) == &LazySorted_Type)

static LazySortedObject *
newLazySortedObject(PyObject *arg)
{
    LazySortedObject *self;
    self = PyObject_New(LazySortedObject, &LazySorted_Type);
    if (self == NULL)
        return NULL;
    self->xs = NULL;
    return self;
}

/* LazySorted methods */

static void
LazySorted_dealloc(LazySortedObject *self)
{
    Py_XDECREF(self->xs);
    PyObject_Del(self);
}

static PyObject *
LazySorted_demo(LazySortedObject *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":demo"))
        return NULL;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef LazySorted_methods[] = {
    {"demo",            (PyCFunction)LazySorted_demo,  METH_VARARGS,
        PyDoc_STR("demo() -> None")},
    {NULL,              NULL}           /* sentinel */
};

static PyObject *
LazySorted_getattr(LazySortedObject *self, char *name)
{
    if (self->xs != NULL) {
        PyObject *v = PyDict_GetItemString(self->xs, name);
        if (v != NULL) {
            Py_INCREF(v);
            return v;
        }
    }
    return Py_FindMethod(LazySorted_methods, (PyObject *)self, name);
}

static int
LazySorted_setattr(LazySortedObject *self, char *name, PyObject *v)
{
    if (self->xs == NULL) {
        self->xs = PyDict_New();
        if (self->xs == NULL)
            return -1;
    }
    if (v == NULL) {
        int rv = PyDict_DelItemString(self->xs, name);
        if (rv < 0)
            PyErr_SetString(PyExc_AttributeError,
                "delete non-existing LazySorted attribute");
        return rv;
    }
    else
        return PyDict_SetItemString(self->xs, name, v);
}

static PyTypeObject LazySorted_Type = {
    /* The ob_type field must be initialized in the module init function
     * to be portable to Windows without using C++. */
    PyVarObject_HEAD_INIT(NULL, 0)
    "xxmodule.LazySorted",             /*tp_name*/
    sizeof(LazySortedObject),          /*tp_basicsize*/
    0,                          /*tp_itemsize*/
    /* methods */
    (destructor)LazySorted_dealloc, /*tp_dealloc*/
    0,                          /*tp_print*/
    (getattrfunc)LazySorted_getattr, /*tp_getattr*/
    (setattrfunc)LazySorted_setattr, /*tp_setattr*/
    0,                          /*tp_compare*/
    0,                          /*tp_repr*/
    0,                          /*tp_as_number*/
    0,                          /*tp_as_sequence*/
    0,                          /*tp_as_mapping*/
    0,                          /*tp_hash*/
    0,                      /*tp_call*/
    0,                      /*tp_str*/
    0,                      /*tp_getattro*/
    0,                      /*tp_setattro*/
    0,                      /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,     /*tp_flags*/
    0,                      /*tp_doc*/
    0,                      /*tp_traverse*/
    0,                      /*tp_clear*/
    0,                      /*tp_richcompare*/
    0,                      /*tp_weaklistoffset*/
    0,                      /*tp_iter*/
    0,                      /*tp_iternext*/
    0,                      /*tp_methods*/
    0,                      /*tp_members*/
    0,                      /*tp_getset*/
    0,                      /*tp_base*/
    0,                      /*tp_dict*/
    0,                      /*tp_descr_get*/
    0,                      /*tp_descr_set*/
    0,                      /*tp_dictoffset*/
    0,                      /*tp_init*/
    0,                      /*tp_alloc*/
    0,                      /*tp_new*/
    0,                      /*tp_free*/
    0,                      /*tp_is_gc*/
};
/* --------------------------------------------------------------------- */


/* Function of no arguments returning new LazySorted object */

static PyObject *
xx_new(PyObject *self, PyObject *args)
{
    LazySortedObject *rv;

    if (!PyArg_ParseTuple(args, ":new"))
        return NULL;
    rv = newLazySortedObject(args);
    if (rv == NULL)
        return NULL;
    return (PyObject *)rv;
}

/* List of functions defined in the module */

static PyMethodDef xx_methods[] = {
    {"new",             xx_new,         METH_VARARGS,
        PyDoc_STR("new() -> new Xx object")},
    {NULL,              NULL}           /* sentinel */
};

PyDoc_STRVAR(module_doc,
"This is a template module just for instruction.");

/* Initialization function for the module (*must* be called initxx) */

PyMODINIT_FUNC
initlazysorted(void)
{
    PyObject *m;

    /* Finalize the type object including setting type of the new type
     * object; doing it here is required for portability, too. */
    if (PyType_Ready(&LazySorted_Type) < 0)
        return;

    /* Create the module and add the functions */
    m = Py_InitModule3("lazysorted", xx_methods, module_doc);
    if (m == NULL)
        return;

    PyModule_AddObject(m, "LazySorted", (PyObject *)&LazySorted_Type);
}
