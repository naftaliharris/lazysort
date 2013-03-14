/* LazySorted objects */

#include <Python.h>
#include <time.h>
#include "params.h"

/* Definitions and functions for the binary search tree of pivot points.
 * The BST implementation is a Treap, selected because of its general speed,
 * especially when inserting and removing elements, which happens a lot in this
 * application. */

typedef struct PivotNode {
    Py_ssize_t index;
    int flags;
    int priority;
    struct PivotNode *left;
    struct PivotNode *right;
    struct PivotNode *parent;
} PivotNode;

/* SORTED_RIGHT means the pivot is to the right of a sorted region.
 * SORTED_LEFT means the pivot is the left of a sorted region */
#define SORTED_RIGHT 1
#define SORTED_LEFT 2
#define UNSORTED 0

/* A recursive function getting the consistency of a node.
 * Does not assume that the node is the root of the tree, and does NOT examine
 * the parentage of node. This is important, because it is often called on
 * nodes whose future parents don't know them yet, like in merge_trees(.) */
static void
assert_node(PivotNode *node)
{
    if (node->left != NULL) {
        assert(node->left->index < node->index);
        assert(node->left->priority <= node->priority);
        assert(node->left->parent == node);
        assert_node(node->left);
    }
    if (node->right != NULL) {
        assert(node->right->index > node->index);
        assert(node->right->priority <= node->priority);
        assert(node->right->parent == node);
        assert_node(node->right);
    }
}

/* A series of assert statements that the entire tree is consistent */
static void
assert_tree(PivotNode *root)
{
    assert(root != NULL);
    assert(root->parent == NULL);
    assert_node(root);
}

/* Inserts an index, returning a pointer to the node, or NULL on error.
 * *root is the root of the tree, while start is the node to insert from.
 */
static PivotNode *
insert_pivot(Py_ssize_t k, int flags, PivotNode **root, PivotNode *start)
{
    /* Build the node */
    PivotNode *node = (PivotNode *)PyMem_Malloc(sizeof(PivotNode));
    if (node == NULL)
        return (PivotNode *)PyErr_NoMemory();
    node->index = k;
    node->flags = flags;
    node->priority = rand(); /* XXX Security, thread-safety */
    node->left = NULL;
    node->right = NULL;

    /* Special case the empty tree */
    if (*root == NULL) {
        node->parent = NULL;
        *root = node;
        return node;
    }

    /* Put the node in it's sorted order */
    PivotNode *current = start;
    while (1) {
        if (current->index < k) {
            if (current->right == NULL) {
                current->right = node;
                node->parent = current;
                break;
            }
            current = current->right;
        }
        else if (current->index > k) {
            if (current->left == NULL) {
                current->left = node;
                node->parent = current;
                break;
            }
            current = current->left;
        }
        else {
            /* The pivot BST should always have unique pivots */
            PyErr_SetString(PyExc_SystemError, "All pivots must be unique");
            return NULL;
        }
    }

    /* Reestablish the treap invariant if necessary by tree rotations */
    PivotNode *child, *parent, *grandparent;
    while (node->priority > node->parent->priority) {
        /*          (parent)    (node)
         *            /              \
         *           /                \
         *          /                  \
         *     (node)        ->       (parent)
         *        \                     /
         *         \                   /
         *       (child)            (child)
         */
        if (node->index < node->parent->index) {
            child = node->right;
            parent = node->parent;
            grandparent = parent->parent;

            node->parent = grandparent;
            node->right = parent;
            parent->parent = node;
            parent->left = child;
            if (child != NULL)
                child->parent = parent;
        }
        /* (parent)                (node)
         *      \                   /
         *       \                 /
         *        \               /
         *        (node)  ->  (parent)
         *        /              \
         *       /                \
         *    (child)           (child)
         */
        else {
            child = node->left;
            parent = node->parent;
            grandparent = parent->parent;

            node->parent = grandparent;
            node->left = parent;
            parent->parent = node;
            parent->right = child;
            if (child != NULL)
                child->parent = parent;
        }

        /* Adjust node->parent's child pointer to point to node */
        if (node->parent != NULL) {
            if (k < node->parent->index) {
                node->parent->left = node;
            }
            else {
                node->parent->right = node;
            }
        }
        else {  /* The node has propogated up to the root */
            *root = node;
            break;
        }
    }

    assert_tree(*root);
    return node;
}

/* Takes two trees and merges them into one while preserving the treap 
 * invariant. left must have a smaller index than right. */
static PivotNode *
merge_trees(PivotNode *left, PivotNode *right)
{
    assert(left != NULL || right != NULL);
    
    if (left == NULL)
        return right;
    if (right == NULL)
        return left;

    assert(left->parent == right->parent);
    assert(left->index < right->index);
    assert_node(left);
    assert_node(right);

    if (left->priority > right->priority) {
        right->parent = left;
        left->right = merge_trees(left->right, right);

        assert_node(left);
        return left;
    }
    else {
        left->parent = right;
        right->left = merge_trees(left, right->left);

        assert_node(right);
        return right;
    }
}

static void
delete_node(PivotNode *node, PivotNode **root)
{
    assert_tree(*root);
    
    if (node->left == NULL) {
        /* node has at most one child in node->right, so we just have the 
         * grandparent adopt it, if node is not the root. If node is the root,
         * we promote the child to root. */
        if (node->parent != NULL) {
            if (node->parent->left == node) {
                node->parent->left = node->right;
            }
            else {
                node->parent->right = node->right;
            }
        }
        else {  /* Node is the root */
            *root = node->right;
        }

        if (node->right != NULL) {
            node->right->parent = node->parent;
        }

        PyMem_Free(node);
    }
    else {
        if (node->right == NULL) {
            /* node has a single child in node->left, so have grandparent
             * adopt it as above */
            if (node->parent != NULL) {
                if (node->parent->left == node) {
                    node->parent->left = node->left;
                }
                else {
                    node->parent->right = node->left;
                }
            }
            else {  /* Node is the root */
                *root = node->left;
            }

            /* node->left is not NULL because of the outer if-else statement */
            node->left->parent = node->parent;

            PyMem_Free(node);
        }
        else {
            /* The hard case: node has two children. We merge the two children
             * into one treap, and then replace node by this treap */
            PivotNode *children = merge_trees(node->left, node->right);
            
            if (node->parent != NULL) {
                if (node->parent->left == node) {
                    node->parent->left = children;
                }
                else {
                    node->parent->right = children;
                }
            }
            else {  /* Node is the root */
                *root = children;
            }

            /* children is not NULL since node has two children */
            children->parent = node->parent;

            PyMem_Free(node);
        }
    }

    assert_tree(*root);
}

/* If a sorted pivot is between two sorted section, removes the sorted pivot */
static void
depivot(PivotNode *left, PivotNode *right, PivotNode **root)
{
    assert_tree(*root);
    assert(left->flags & SORTED_LEFT);
    assert(right->flags & SORTED_RIGHT);
    
    if (left->flags & SORTED_RIGHT) {
        delete_node(left, root);
    }

    if (right->flags & SORTED_LEFT) {
        delete_node(right, root);
    }

    assert_tree(*root);
}

/* Finds PivotNodes left and right that bound the index */
/* Never returns k in right_node, only the left, if applicable */
static void
bound_index(Py_ssize_t k, PivotNode *root, PivotNode **left, PivotNode **right)
{
    assert_tree(root);

    *left = NULL;
    *right = NULL;
    PivotNode *current = root;
    while (current != NULL) {
        if (current->index < k) {
            *left = current;
            current = current->right;
        }
        else if (current->index > k) {
            *right = current;
            current = current->left;
        }
        else {
            *left = current;
            break;
        }
    }

    assert (*left != NULL && ((*left)->index == k || *right != NULL));
}

static void
free_tree(PivotNode *root)
{
    assert_node(root);  /* root might not actually be the root of the tree */

    if (root->left != NULL)
        free_tree(root->left);
    if (root->right != NULL)
        free_tree(root->right);

    PyMem_Free(root);
}

/* The LazySorted object */
 
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
    if (insert_pivot(-1, UNSORTED, &self->root, self->root) == NULL)
        return NULL;

    return (PyObject *)self;
}

/* Private helper functions for partial sorting */

/* These macros are basically taken from list.c
 * Returns 1 if x < y, 0 if x >= y, and -1 on error */
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
    /* XXX Better pivot selection */
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

/* Runs insertion sort on the items between left and right */
static void
insertion_sort(PyObject **ob_item, Py_ssize_t left, Py_ssize_t right)
{
    PyObject *tmp;
    Py_ssize_t i, j;

    for (i = left; i < right; i++) {
        tmp = ob_item[i];
        for (j = i; j > 0 && ISLT(tmp, ob_item[j - 1]); j--)
            ob_item[j] = ob_item[j - 1];
        ob_item[j] = tmp;
    }
}

/* LazySorted methods */

static PyObject *indexerr = NULL;

static PyObject *
ls_item(LSObject *ls, Py_ssize_t k)
{
    Py_ssize_t xs_len = Py_SIZE(ls->xs);

    if (k < 0 || k >= xs_len) {
        if (indexerr == NULL) {
            indexerr = PyString_FromString(
                "list index out of range");
            if (indexerr == NULL)
                return NULL;
        }
        PyErr_SetObject(PyExc_IndexError, indexerr);
        return NULL;
    }

    /* Find the best possible bounds */
    PivotNode *left, *right;
    bound_index(k, ls->root, &left, &right);
    /* bound_index never returns k in right, but right might be NULL if
     * left->index == k, so check left->index first. */
    if (left->index == k || right->flags & SORTED_RIGHT) {
        Py_INCREF(ls->xs->ob_item[k]);
        return ls->xs->ob_item[k];
    }

    /* Run quickselect */
    Py_ssize_t pivot_index;

    while (left->index + 1 + SORT_THRESH <= right->index) {
        pivot_index = partition(ls->xs->ob_item,
                                left->index + 1,
                                right->index);
        if (pivot_index < k) {
            if (left->right == NULL) {
                left = insert_pivot(pivot_index, UNSORTED, &ls->root, left);
            }
            else {
                left = insert_pivot(pivot_index, UNSORTED, &ls->root, right);
            }
            if (left == NULL)
                return NULL;
        }
        else if (pivot_index > k) {
            if (left->right == NULL) {
                right = insert_pivot(pivot_index, UNSORTED, &ls->root, left);
            }
            else {
                right = insert_pivot(pivot_index, UNSORTED, &ls->root, right);
            }
            if (right == NULL)
                return NULL;
        }
        else {
            if (left->right == NULL) {
                left = insert_pivot(pivot_index, UNSORTED, &ls->root, left);
            }
            else {
                left = insert_pivot(pivot_index, UNSORTED, &ls->root, right);
            }
            if (left == NULL) /* We're just using left as a variable */
                return NULL;

            Py_INCREF(ls->xs->ob_item[k]);
            return ls->xs->ob_item[k];
        }
    }

    insertion_sort(ls->xs->ob_item, left->index + 1, right->index);
    left->flags |= SORTED_LEFT;
    right->flags |= SORTED_RIGHT;
    depivot(left, right, &ls->root);

    Py_INCREF(ls->xs->ob_item[k]);
    return ls->xs->ob_item[k];
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
        return ls_item(self, i);
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
    free_tree(self->root);
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

    if (insert_pivot(Py_SIZE(self->xs), UNSORTED, &self->root, self->root) ==
            NULL)
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
    srand(time(NULL)); /* XXX Worry about security, thread-safety etc */

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
