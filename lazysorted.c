/* LazySorted objects */

#include "Python.h"
#include <search.h>

/* Definitions for the binary search tree of pivot points */
typedef struct {
    Py_ssize_t index;
    int flags;
} PivotNode;

int
compareNodes(const void *pa, const void *pb)
{
    if (((PivotNode *)pa)->index < ((PivotNode *)pb)->index)
        return -1;
    if (((PivotNode *)pa)->index > ((PivotNode *)pb)->index)
        return 1;
    return 0;
}

typedef struct {
    PyObject_HEAD
    PyListObject        *xs;            /* Partially sorted list */
    PivotNode           *root;          /* Root of the pivot BST */
} LSObject;

static PyTypeObject LS_Type;

#define LSObject_Check(v)      (Py_TYPE(v) == &LS_Type)

static PyObject *
newLSObject(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LSObject *self;
    self = (LSObject *)type->tp_alloc(type, 0);
    if (self == NULL)
        return NULL;

    self->xs = (PyListObject *)PyList_Type.tp_new(&PyList_Type, args, kwds);
    if (self->xs == NULL)
        return NULL;

    self->root = NULL;

    return (PyObject *)self;
}

/* Private helper functions for partial sorting */

/* These macros are basically taken from list.c */
#define ISLT(X, Y) PyObject_RichCompareBool(X, Y, Py_LT)

#define IFLT(X, Y) if ((ltflag = ISLT(X, Y)) < 0) goto fail;  \
            if(ltflag)

/* N.B: No semicolon at the end, so that you can include one yourself */
#define SWAP(i, j) tmp = ob_item[i];  \
                   ob_item[i] = ob_item[j];  \
                   ob_item[j] = tmp

/* Picks a pivot point among the indices left <= i < right */
static Py_ssize_t
pick_pivot(PyObject **ob_item, Py_ssize_t left, Py_ssize_t right)
{
    /* XXX */
    return left;
}

/* Partitions the data between left and right and returns the pivot index,
 * or -1 on error */
static Py_ssize_t
partition(PyObject **ob_item, Py_ssize_t left, Py_ssize_t right)
{
    PyObject *tmp;
    PyObject *pivot;
    Py_ssize_t ltflag;

    Py_ssize_t pivot_index = pick_pivot(ob_item, left, right);
    pivot = ob_item[pivot_index];

    SWAP(left, pivot_index);
    Py_ssize_t last_less = left;

    /* Invariant: last_less and everything to its left is less than
     * pivot or the pivot itself */

    Py_ssize_t i;
    for (i = left + 1; i < right; i++) {
        IFLT(ob_item[i], pivot) {
            last_less++;
            SWAP(i, last_less);
        }
    }

    SWAP(left, last_less);
    return last_less;

fail:
    return -1;
}

/* LazySorted methods */

static PyObject *indexerr = NULL;

static PyObject *
list_item(LSObject *ls, Py_ssize_t i)
{
    if (i < 0 || i >= Py_SIZE(ls->xs)) {
        if (indexerr == NULL) {
            indexerr = PyString_FromString(
                "list index out of range");
            if (indexerr == NULL)
                return NULL;
        }
        PyErr_SetObject(PyExc_IndexError, indexerr);
        return NULL;
    }
    Py_INCREF(ls->xs->ob_item[i]);
    return ls->xs->ob_item[i];
}

static PyObject *
ls_subscript(LSObject* self, PyObject* item)
{
    if (PyIndex_Check(item)) {
        Py_ssize_t i;
        i = PyNumber_AsSsize_t(item, PyExc_IndexError);
        if (i == -1 && PyErr_Occurred())
            return NULL;
        if (i < 0)
            i += PyList_GET_SIZE(self->xs);
        return list_item(self, i);
    }
    /*
    else if (PySlice_Check(item)) {
        Py_ssize_t start, stop, step, slicelength, cur, i;
        PyObject* result;
        PyObject* it;
        PyObject **src, **dest;

        if (PySlice_GetIndicesEx((PySliceObject*)item, Py_SIZE(self),
                         &start, &stop, &step, &slicelength) < 0) {
            return NULL;
        }

        if (slicelength <= 0) {
            return PyList_New(0);
        }
        else if (step == 1) {
            return list_slice(self, start, stop);
        }
        else {
            result = PyList_New(slicelength);
            if (!result) return NULL;

            src = self->ob_item;
            dest = ((PyListObject *)result)->ob_item;
            for (cur = start, i = 0; i < slicelength;
                 cur += step, i++) {
                it = src[cur];
                Py_INCREF(it);
                dest[i] = it;
            }

            return result;
        }
    }
    */
    else {
        PyErr_Format(PyExc_TypeError,
                     "list indices must be integers, not %.200s",
                     item->ob_type->tp_name);
        return NULL;
    }
}

static Py_ssize_t
ls_length(LSObject *ls)
{
    return Py_SIZE(ls->xs);
}

static void
LS_dealloc(LSObject *self)
{
    Py_DECREF(self->xs);
    while (tdelete(self->root, (void **)&self->root, compareNodes) != NULL) {
        /* do nothing */
    }
    PyObject_Del(self);
}

static PyObject *
LS_xs(LSObject *self)
{
    Py_INCREF(self->xs);
    return (PyObject *)self->xs;
}

static PyMethodDef LS_methods[] = {
    {"__getitem__",     (PyCFunction)ls_subscript, METH_O|METH_COEXIST,
        PyDoc_STR("__getitem__ ADD Documentation")},
    {"get_xs",          (PyCFunction)LS_xs, METH_NOARGS,
        PyDoc_STR("get_xs() -> List")},
    {NULL,              NULL}           /* sentinel */
};

static PyMappingMethods ls_as_mapping = {
    (lenfunc)ls_length,
    (binaryfunc)ls_subscript,
    NULL,
};

static int
LS_init(LSObject *self, PyObject *args, PyObject *kw)
{
    PyObject *arg = NULL;
    static char *kwlist[] = {"sequence", 0};

    if (!PyArg_ParseTupleAndKeywords(args, kw, "|O:list", kwlist, &arg))
        return -1;

    if (PyList_Type.tp_init((PyObject *)self->xs, args, kw))
        return -1;

    return 0;
}

static PyTypeObject LS_Type = {
    /* The ob_type field must be initialized in the module init function
     * to be portable to Windows without using C++. */
    PyVarObject_HEAD_INIT(NULL, 0)
    "lazysorted.LazySorted",/*tp_name*/
    sizeof(LSObject),       /*tp_basicsize*/
    0,                      /*tp_itemsize*/
    /* methods */
    (destructor)LS_dealloc, /*tp_dealloc*/
    0,                      /*tp_print*/
    0,                      /*tp_getattr*/
    0,                      /*tp_setattr*/
    0,                      /*tp_compare*/
    0,                      /*tp_repr*/
    0,                      /*tp_as_number*/
    0,                      /*tp_as_sequence*/
    &ls_as_mapping,         /*tp_as_mapping*/
    0,                      /*tp_hash*/
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
    LS_methods,             /*tp_methods*/
    0,                      /*tp_members*/
    0,                      /*tp_getset*/
    0,                      /*tp_base*/
    0,                      /*tp_dict*/
    0,                      /*tp_descr_get*/
    0,                      /*tp_descr_set*/
    0,                      /*tp_dictoffset*/
    (initproc)LS_init,      /*tp_init*/
    0,                      /*tp_alloc*/
    newLSObject,            /*tp_new*/
    0,                      /*tp_free*/
    0,                      /*tp_is_gc*/
};
/* --------------------------------------------------------------------- */


/* List of functions defined in the module */

static PyMethodDef ls_methods[] = {
    {NULL,              NULL}           /* sentinel */
};

PyDoc_STRVAR(module_doc,
"This is a template module just for instruction.");

/* Initialization function for the module */

PyMODINIT_FUNC
initlazysorted(void)
{
    PyObject *m;

    /* Finalize the type object including setting type of the new type
     * object; doing it here is required for portability, too. */

    if (PyType_Ready(&LS_Type) < 0)
        return;

    /* Create the module and add the functions */
    m = Py_InitModule3("lazysorted", ls_methods, module_doc);
    if (m == NULL)
        return;

    PyModule_AddObject(m, "LazySorted", (PyObject *)&LS_Type);
}
