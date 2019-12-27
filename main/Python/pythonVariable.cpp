/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  pythonVariable.cpp
 *  @brief: Implementation of the spectcl.variable class.
 */
#include "pythonVariable.h"
#include <TreeParameter.h>


/**
 * The variable type provides an encapsulation of the
 * Tree variable data type in SpecTcl.  This is a variable that has name,
 * value and units It can be manipulated via Tcl (and now Python) and, via
 * the CTreeVariable class can be made to look like a double variable
 * in computations performed by SpecTcl event processors.
 *
 * The object has the following attributes:
 *   -  name (readonly) the variable name.
 *   -  value (read/write) Double value of the variable.
 *   - units (read/write) textual units of measure of the variable.
 * Methods are:
 *    valchanged() - bool returns true if the value changed.
 *    defchanged() - bool returns true if the definition (units?) changed.
 *    reset()   - Resets the definition changed flag.
 *    fireTraces() - Fires  variable traces (Tcl) and reset the value changed flag
 *
 * @note - linking to a python variable is not now supported though it is in principle
 *         possible by setting a variable trace on the underlying Tcl variable and using
 *         that trace to bidirectionally update the Tcl/Python variable.
 */

/**
 * object instance storage
 */

typedef struct {
    PyObject_HEAD
    VariableData s_objData;
} VariableObject, *pVariableObject;

//////////////////////////////////////////////////////////////////////////////
// Utilities

/**
 * getProperties
 *    Given a pointer to an object that is ostensibly a  variable, returns
 *    its properties.
 *
 * @param self - pointer to the variable object.
 * @return CTreeVariableProperties*
 */
static CTreeVariableProperties*
getProperties(PyObject* self)
{
    pVariableObject me = (pVariableObject)(self);
    
    return me->s_objData.s_pProperties;
}

//////////////////////////////////////////////////////////////////////////////
// Object allocation/initialization and deletion.

/**
 * ObjNew
 *    Allocate storage for an instance.
 *
 *  @param type - pointer to our type object.
 *  @param args - Ingored positional parameters.
 *  @param kwds - Ignored keword parameters.
 *  @return PyObject* the nely allocated object.
 *  @note   The properties object  pointer is initialized to nullptr.
 */
static PyObject*
ObjNew(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
    pVariableObject self = (pVariableObject)(type->tp_alloc(type, 0));
    if (self != NULL) {
        self->s_objData.s_pProperties = nullptr;
    }
    return (PyObject*)self;
}
/**
 * initialize
 *    Initializes the storage for a newly created object.
 *    This takes a single parameter, the name of the tree variable.
 *    - Locate the properties object
 *    - Put a pointer to it in the object data.
 *    - IF the tree variable does not exist, raise a PyExc_KeyError
 *
 * @param self - pointer to our object data.
 * @param args - Positional parameters - we should only have a name parameter.
 * @param kwds - Keyword parameters - which we don't support.
 * @return int  - 0 means success, -1 means an exception is raised.
 */
static int
initialize(PyObject* self, PyObject* args, PyObject* kwds)
{
    pVariableObject me = (pVariableObject)(self);
    char* name;
    
    if(!PyArg_ParseTuple(args, "s", &name)) {
        return -1;
    }
    // Now find the variable:
    
    auto p = CTreeVariable::find(name);
    if (p == CTreeVariable::end()) {
        PyErr_SetString(PyExc_LookupError, "No such tree parameter");
        return -1;                          // not founfd.
    }
    me->s_objData.s_pProperties = p->second;
    return 0;                                          // Success.
}
/**
 * deallocate
 *     Free stroage - we don't delete the properties because the treevariable
 *     outlives us.
 *
 *  @param self - pointer to us.
 */
static void
deallocate(PyObject* self)
{
    Py_TYPE(self)->tp_free((PyObject*)self);
}
///////////////////////////////////////////////////////////////////////////////
// Instance methods;

/**
 * valchanged
 *   @param self - pointer to instance data.
 *   @return PyObject* bool - true if the value changed flag is set.
 */
static PyObject*
valchanged(PyObject* self, PyObject* Py_UNUSED(ignored))
{
    CTreeVariableProperties* props = getProperties(self);
    if (props->valueChanged() ) {   // ternary operator is unsafe given the below are macros.
        Py_RETURN_TRUE;
    } else {
        Py_RETURN_FALSE;
    }
}
/**
 * defchanged
 *   @param self - pointer to the object data.
 *   @return PyObject* bool that reflects the definition changed flag.
 */
static PyObject*
defchanged(PyObject* self, PyObject* Py_UNUSED(ignored))
{
    CTreeVariableProperties* props = getProperties(self);
    if (props->definitionChanged() ) {
        Py_RETURN_TRUE;
    } else {
        Py_RETURN_FALSE;
    }
}
/**
 * reset
 *    Reset the definition changed flag.  To reset the variable changed flag,
 *    fireTraces must be used.
 *
 * @param self - pointer to object data.
 * @return Py_None
 */
static PyObject*
reset(PyObject* self, PyObject* Py_UNUSED(ignored))
{
    CTreeVariableProperties* props = getProperties(self);
    props->resetDefinitionChanged();
    Py_RETURN_NONE;
}

/**
 * fireTraces
 *   Fires Tcl traces -- also sets the value changed flag.
 *   
 * @param self - pointer to object data.
 * @return Py_None
 */
static PyObject*
firetraces(PyObject* self, PyObject* Py_UNUSED(ignored))
{
    CTreeVariableProperties* props = getProperties(self);
    props->fireTraces();
}
//////////////////////////////////////////////////////////////////////////////
// Attribute getters and setters.
//

/**
 * getName
 *    Return the name attribute of the object - this is the name of the
 *    tree variable.
 * @param self - pointer to our data storage.
 * @param closure - unused parameter from the dispatch table.
 * @return PyObject* actually  a unicode object.
 */
static PyObject*
getName(PyObject* self, void* closure)
{
    CTreeVariableProperties* p = getProperties(self);
    return PyUnicode_FromString(p->getName().c_str());
}

/**
 * getValue
 *    @param self - pointer to the object storage.
 *    @param closure - unused parameter from the dispatch table.
 *    @return PyObject - the double value.
 */
static PyObject*
getValue(PyObject* self, void* closure)
{
    CTreeVariableProperties* p = getProperties(self);
    return PyFloat_FromDouble((double)(*p));
}
/**
 * setValue
 *    Assign  the value to a new double
 * @param self - pointer to object data.
 * @param  value - the new value for the object  must be a number.
 * @param  closure, unused.
 * @return int 0 - if success. -1 if an exception has been thrown.
 */
static int
setValue(PyObject* self, PyObject* value, void* closure)
{
    CTreeVariableProperties* p = getProperties(self);
    
    // value must be a float:
    
    if (!PyFloat_Check(value)) {
        PyErr_SetString(PyExc_ValueError, "Treevariable value must be a float");
        return -1;
    }
    *p = PyFloat_AsDouble(value);
    
    return 0;
}
/**
 * getUnits
 *    Return the variables unit string
 * @param self - pointer to object storage.
 * @param closure - ignored (null).
 * @return PyObject* unicode units string.
 */
static PyObject*
getUnits(PyObject* self, void* closure)
{
    CTreeVariableProperties* p = getProperties(self);
    std::string units = p->getUnits();
    return PyUnicode_FromString(units.c_str());
}
/**
 * setUnits
 *   @param self - pointer to object.
 *   @param value = unicode string value.
 *   @param closure  - unused.
 *   @return int 0 for success.
 */
static int
setUnits(PyObject* self, PyObject* value, void* closure)
{
    if(!PyUnicode_Check(value)) {
        PyErr_SetString(PyExc_ValueError, "Units must be a string");
        return -1;
    }
    CTreeVariableProperties* p = getProperties(self);
    p->setUnits(std::string(PyUnicode_AsUTF8(value)));
    
    return 0;
}
/////////////////////////////////////////////////////////////////////////////
// Dispatch tables and type struct:

/**
 *  variable methods:
 *    valchanged() - bool returns true if the value changed.
 *    defchanged() - bool returns true if the definition (units?) changed.
 *    reset()   - Resets the definition changed flag.
 *    fireTraces() - Fires  variable traces (Tcl) and reset the value changed flag

 */

static PyMethodDef variableMethods[] {
    {"valchanged", (PyCFunction)(valchanged), METH_NOARGS, "check for value changed"},
    {"defchanged", (PyCFunction)(defchanged), METH_NOARGS, "check for definition changed"},
    {"reset",      (PyCFunction)(reset),      METH_NOARGS, "Reset definition changed flag"},
    {"fireTraces", (PyCFunction)(firetraces), METH_NOARGS, "Fire variable Tcl traces"},
    
    {NULL, NULL, 0, NULL}
};

/*
 *  Attribute access dispatch table:
 *
 * The object has the following attributes:
 *   -  name (readonly) the variable name.
 *   -  value (read/write) Double value of the variable.
 *   - units (read/write) textual units of measure of the variable.
*/

static PyGetSetDef gettersAndSetters[] = {
    {"name", (getter)getName, NULL, "Variable name", NULL},
    {"value",(getter)getValue, (setter)setValue, "Variable value", NULL},
    {"units", (getter)getUnits, (setter)setUnits, "Units of measure", NULL},
    
    {NULL}
};

// Object type

static PyTypeObject VariableType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "spectcl.variable",             /* tp_name */
    sizeof(VariableObject),             /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)deallocate, /* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_reserved */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash  */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,   /* tp_flags */
    "SpecTcl Tree variables",           /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    variableMethods,             /* tp_methods */
    0,                         /* tp_members */
    gettersAndSetters,           /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)initialize,      /* tp_init */
    0,                         /* tp_alloc */
    ObjNew,                 /* tp_new */
};
/////////////////////////////////////////////////////////////////////////////////
// Module initialization.

void setupPythonVariableObjects(PyObject* module)
{
    PyType_Ready(&VariableType);
    Py_INCREF(&VariableType);
    PyModule_AddObject(module, "variable", (PyObject*)(&VariableType));
}