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

/** @file:  pythonParameterObjects.h
 *  @brief: Defines the parameter objects.
 */

#include "pythonParameters.h"
#include <string>
#include <CTreeParameter.h>

/**
 * The parameter type provides an encapsulation of the
 * treeparameter objects.  There are two ways
 * to construct an object:
 *
 *  parameter(name)   - Looks up a tree parameter and encapsulates it
 *                        in the object.
 *  parameter(name, low, high, bins, units) - Creates a new CTree parameter object.
 *
 *  @note Destruction of the tree parameter destroys the encapsulating
 *        python object but does not destroy the parameter itself in keeping
 *        with SpecTcl's "parameters are forever" policy.
 *
 * Parameters when constructed have the following methods:
 *    get/setbins  - gets/sets the number of bins recommended for the parameter.
 *    get/setlow   - gets/sets the recommended spectrum low limit.
 *    get/sethigh  - gets/sets the recommend spectrum high limit.
 *    get/setunits - gets/sets the parameter units of measure.
 *    name         - returns the parameter name.
 *    check        - checks for modification
 *    uncheck      - unsets the modification flag.
 *
 */

typedef struct {
    PyObject_HEAD
    ParameterData s_objData;
} ParameterObject, *pParameterObject;
////////////////////////////////////////////////////////////////////////////
// Object creation and deletion:

/**
 * deallocate
 * 
 * deallocate the object
 *    This just does a tp_free
 *
 * @param self - pointer to us.
 */
static void
deallocate(pParameterObject self)
{
    Py_TYPE(self)->tp_free((PyObject*)self);
}
/**
 * ObjNew
 *   Allocate storage for the object and initialize it as empty.
 *
 * @param type - pointer to our type object
 * @param args - unused(?)
 * @param kwds - unused(?)
 * @return PyObject*  - pointer to the newly created object.
 */
static PyObject*
ObjNew(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
    pParameterObject p;
    p  = (pParameterObject)(type->tp_alloc(type, 0));
    if (p != NULL) {
        p->s_objData.s_pParameter = NULL;
    }
    
    return (PyObject*)p;
}
/**
 * init
 *    Initialize the object data.
 *    This could have a single parameter the name of the parameter to lookup.
 *    Or it could have name, low, high, units to create a new object.
 *
 *  @param self - pointer to the object we're initializing.
 *  @param args - Positional arguments (the only type accepted).
 *  @param kwds - unused in this vcase.
 *  @return int - -1 on failure 0 for success.
 */
static int
init(PyObject* self, PyObject* args, PyObject* kwds)
{
    Py_ssize_t nargs = PyTuple_Size(args);
    pParameterObject me = (pParameterObject)(self);
    if (nargs == 1) {
        const char* pName;
        if (!PyArg_ParseTuple(args, "s", &pName)) {
            return -1;                      // Parse failed.
        }
        std::multimap<std::string, CTreeParameter*>::iterator p =
            CTreeParameter::find(std::string(pName));
        if (p == CTreeParameter::end()) {
            PyErr_SetString(PyExc_LookupError, "No such parameter");
            return -1;
        } else {
            me->s_objData.s_pParameter = p->second;
            return 0;
        }
    } else if (nargs == 5) {
        char* pName;
        char* pUnits;
        double low, high;
        int    bins;
        if (!PyArg_ParseTuple(args, "sddis", &pName, &low, &high, &bins,&pUnits))  {
            return -1;
        }
        // Create the new tree parameter:
        
        me->s_objData.s_pParameter = new CTreeParameter(pName, bins, low, high, pUnits);
        me->s_objData.s_pParameter->Bind();
        
        return 0;
        
    } else {
        // wrong number.
        
        PyErr_SetString(PyExc_IndexError, "Should be either 1 or 5 parameters only");
        return -1;
    }
}
//////////////////////////////////////////////////////////////////////////////
// Object method implementations.

/**
 * name
 *    Return the name of the tree parameter.
 * @return PyObject*
 */
static PyObject*
name(pParameterObject  self, PyObject* Py_UNUSED(ignored))
{
    return PyUnicode_FromString(self->s_objData.s_pParameter->getName().c_str());
}
/**
 * getbins
 *    Return the number of bins in a tree parameter.
 */
static PyObject*
getbins(pParameterObject  self, PyObject* Py_UNUSED(ignored))
{
    return PyLong_FromUnsignedLong(
        self->s_objData.s_pParameter->getBins()
    );
}
/**
 *  getlow
 *     Return the parameter's low limit.
 */
static PyObject*
getlow(pParameterObject  self, PyObject* Py_UNUSED(ignored))
{
    return PyFloat_FromDouble(
        self->s_objData.s_pParameter->getStart()
    );
}
/**
 * gethigh
 *   Return the parameters's high 'limit'.
 */
static PyObject*
gethigh(pParameterObject  self, PyObject* Py_UNUSED(ignored))
{
    return PyFloat_FromDouble(
        self->s_objData.s_pParameter->getStop()
    );
}
/**
 * getunits
 *   return the parameter's units.
 */
static PyObject*
getunits(pParameterObject  self, PyObject* Py_UNUSED(ignored))
{
    return PyUnicode_FromString(
        self->s_objData.s_pParameter->getUnit().c_str()
    );
}
/**
 * check
 *    Returns the modified flag for the parameter.
 */
static PyObject*
check(pParameterObject  self, PyObject* Py_UNUSED(ignored))
{
    if (self->s_objData.s_pParameter->hasChanged()) {
        Py_RETURN_TRUE;
    } else {
        Py_RETURN_FALSE;
    }
}
/**
 * reset
 *    Reset the changed flag.
 */
static PyObject*
reset(pParameterObject  self, PyObject* Py_UNUSED(ignored))
{
    self->s_objData.s_pParameter->resetChanged();
    Py_RETURN_NONE;
}

/**
 * markchanged
 *    Set the changed flag.
 */
static PyObject*
markchanged(pParameterObject  self, PyObject* Py_UNUSED(ignored))
{
    self->s_objData.s_pParameter->setChanged();
    Py_RETURN_NONE;
}

/**
 *setbins
 *   Set the number of bins in the parameter
 * @param self - this object.
 * @param args - method parameters.
 * @return PyObject* (self to support chaining).
 */
static PyObject*
setbins(pParameterObject self, PyObject* args)
{
    int value;
    if(!PyArg_ParseTuple(args, "i", &value)) {
        return NULL;
    }
    self->s_objData.s_pParameter->setBins(value);
    self->s_objData.s_pParameter->setChanged();
    
    return (PyObject*)(self);
}
/**
 * setLow
 *    Set the low limit for a parameter
 */
static PyObject*
setlow(pParameterObject self, PyObject* args)
{
    double value;
    if(!PyArg_ParseTuple(args, "d", &value)) {
        return NULL;
    }
    self->s_objData.s_pParameter->setStart(value);
    self->s_objData.s_pParameter->setChanged();
    
    return (PyObject*)(self);
}
/**
 * sethigh
 *   Same as above but set the high limit.
 */
static PyObject*
sethigh(pParameterObject self, PyObject* args)
{
    double value;
    if(!PyArg_ParseTuple(args, "d", &value)) {
        return NULL;
    }
    self->s_objData.s_pParameter->setStop(value);
    self->s_objData.s_pParameter->setChanged();
    
    return (PyObject*)(self);
}
/**
 * setunits
 *   Sets new units.
 */
static PyObject*
setunits(pParameterObject self, PyObject* args)
{
    char* value;
    if(!PyArg_ParseTuple(args, "s", &value)) {
        return NULL;
    }
    self->s_objData.s_pParameter->setUnit(value);
    self->s_objData.s_pParameter->setChanged();
    
    return (PyObject*)(self);
}
/**
 *
/** Method table
 */
static PyMethodDef parameterMethods[] = {
    // pretty much getters:
    {"name", (PyCFunction)(name), METH_NOARGS, "Return parameter name"},
    {"getbins", (PyCFunction)(getbins), METH_NOARGS, "Return bin count"},
    {"getlow",  (PyCFunction)(getlow), METH_NOARGS, "Return low limit"},
    {"gethigh", (PyCFunction)(gethigh), METH_NOARGS, "Return high limit"},
    {"getunits", (PyCFunction)(getunits), METH_NOARGS, "return units string"},
    {"check",  (PyCFunction)(check), METH_NOARGS, "Check for modification"},
    
    //Mutators - these all return None
    
    {"reset",  (PyCFunction)(reset), METH_NOARGS, "Reset changed flag"},
    {"markchanged", (PyCFunction)(markchanged), METH_NOARGS, "Set the changed flag"},
    
    // These return self to support chaining
    
    {"setbins", (PyCFunction)(setbins), METH_VARARGS, "Set bin count"},
    {"setlow",  (PyCFunction)(setlow), METH_VARARGS, "set low limit"},
    {"sethigh", (PyCFunction)(sethigh), METH_VARARGS, "set high limit"},
    {"setunits", (PyCFunction)(setunits), METH_VARARGS, "set units"},
    
    {NULL, NULL, 0, NULL}
};

static PyTypeObject parameterType = {        // Note this makes warnings unless we use c++20q
    PyVarObject_HEAD_INIT(NULL, 0)         
    
    
    

};

void setupPythonParameterObjects(PyObject* module)
{
    parameterType.tp_name = "spectcl.parameter";
    parameterType.tp_basicsize = sizeof(ParameterObject);
    parameterType.tp_itemsize = 0;
    
    parameterType.tp_dealloc = (destructor) deallocate;
    parameterType.tp_flags =  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
    parameterType.tp_doc = "Parameter object";
    parameterType.tp_methods = parameterMethods;
    parameterType.tp_members = NULL;
    parameterType.tp_init = (initproc) init;
    parameterType. tp_new = ObjNew;
    
    PyType_Ready(&parameterType);
    Py_INCREF(&parameterType);
    PyModule_AddObject(module, "parameter", (PyObject*)(&parameterType));
}
