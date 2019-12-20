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

/** @file:  pythonGates.cpp
 *  @brief: Implementation of the gate object for Spectcl's python package.
 */

#include "pythonGates.h"

#include <Gate.h>
#include <Cut.h>

#include <SpecTcl.h>

#include <stdlib.h>
#include <iostream>

////////////////////////////////////////////////////////////////////////
// Utilities:

/**
 * getContainer
 *    @param self -pointer to the object
 *    @return CGateContainer* - pointer to the gate container for the gate.
 */
static CGateContainer*
getContainer(PyObject* self)
{
    pGateObject pObj = reinterpret_cast<pGateObject>(self);
    return pObj->s_pContainer;
}

///////////////////////////////////////////////////////////////////////
// Methods required for object allocation, construction and destruction.

/**
 * deallocate
 *    Deallocate the object
 * @param self - pointer to the type - We don't destroy the gate container because
 *               we want to keep the gate alive etc.
 */
static void
deallocate(pGateObject self)
{
    Py_TYPE(self)->tp_free((PyObject*)self);
}
/**
 * ObjNew
 *   Allocate storage for object
 *   @param type    - Pointer to our type object.
 *   @param args    - unused positional parameters
 *   @param kwds    - keyword parameters
 *   @return PyObject* pointer to the new object.
 *   @retval NULL   - alllocation failed.
 */
static PyObject*
ObjNew(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
    pGateObject p;
    p = reinterpret_cast<pGateObject>(type->tp_alloc(type, 0));
    if (p != NULL) {
        p->s_pContainer = nullptr;
    }
    return reinterpret_cast<PyObject*>(p);

}
/**
 * init
 *    Initialize the gate object:
 *    There are two cases:
 *    -  There's only a positional parameter - in that case it's the name of
 *       an existing gate that will be wrapped.  If the gate does not exist,
 *       that's an error and an exception is thrown.
 *    -  If there are keyword parameters, the following keywords are supported
 *       and this is the creation of a new gate.  If that's successful, the new
 *       gate is wrappe, if not, again an error is thrown.
 *       The following keyword arguments are supported:
 *       *  type (required) the gate type string (e.g. "c" for a contour).
 *       *  parameters (depends on gate type) - iterable object containing the
 *                      parameters the gate depends on. (e.g. for gc).
 *       *  gates (depends on gate type) - iterable object containing the names of
 *                      gates this gate depends on  (e.g. for *).
 *       *  points (depends on gate type) - iterable object containing pairs
 *                      that are x/y points defining the acceptance region for the gate.
 *       * low,high (depends on gate type) low and high limits of a 1-d gate like
 *                       e.g. for "s"
 *  @param self - pointer to our object storage.
 *  @param args - positional parameters.
 *  @param kwds - Keyword parameters.
 *  @return int - status, 0 means success, -1 means error.
 *  
 */
static int 
init(PyObject* self, PyObject* args, PyObject* kwds)
{
    pGateObject me = reinterpret_cast<pGateObject>(self); 
    
    char* keywords[] = {
        "name","type", "parameters", "gates", "points", "low", "high",
        nullptr
    };
    const char* tupleconversions = "s";
    const char* kwdconversions  = "s|$sOOOdd";
    
    // The parameters:
    
    char* pName(nullptr);             // Required
    char* pType(nullptr);
    PyObject* pParameters(nullptr);
    PyObject* pGateNames(nullptr);
    PyObject* pPoints(nullptr);
    double low(0), high(0);
    
    
    
    if (((PyDict_Size(kwds) == -1) || (PyDict_Size(kwds) == 0)) &&
        PyArg_ParseTuple(
            args,  tupleconversions,
            &pName
        )
    ) {
        PyErr_Clear();                     // PyDict_Size might have set the error.
       
    // If there's a name but pType, pParameters, pGateNames, pPoints are all null,
    // this is a wrapping.
        
        // Wrap existing gate:
    
        SpecTcl* pApi = SpecTcl::getInstance();
        me->s_pContainer = pApi->FindGate(pName);
        if (!me->s_pContainer) {
            PyErr_SetString(PyExc_LookupError, "No such gate name");
            return -1;
        }
        return 0;
    
    } else if (
                          // PyDict_Size might have set the error.
        PyArg_ParseTupleAndKeywords(
            args, kwds, kwdconversions, keywords, &pName, &pType, &pParameters,
            &pGateNames, &pPoints, &low, &high
        )
    ){
        PyErr_Clear();   
        if (!pType) {
            PyErr_SetString(PyExc_TypeError, "Gate 'type' is a required keyword argument");
            return -1;
        }
        std::cerr << pName << " " << pType << " " << low << " " << high  << std::endl;
        
        // We need to try the keyword parse to see if he's trying to create a gate
        
        PyErr_SetString(PyExc_TypeError, "Gate creation not yet supported");
        return -1;
        
        
        // Create/replace gate.
        
    } else {
        return -1;
    }
}
///////////////////////////////////////////////////////////////////////////
// Getter implementations:

/**
 * getName
 *    Return the name of the gate.
 *
 *  @param self - pointer to the object.
 *  @param closure - value in the type table for this - unused.
 *  @return PyObject* Unicode string gate name.
 */
static PyObject*
getName(PyObject* self, void* closure)
{
   CGateContainer* pContainer = getContainer(self);
   std::string name = pContainer->getName();
   
   return PyUnicode_FromString(name.c_str());
}
/**
 * getType
 *    Getter for gate type (type attribute).
 *
 *  @param self - pointer to object.
 *  @param closure - unused.
 *  @return PyObject* (unicode object containing type).
 */
static PyObject*
getType(PyObject* self, void* closure)
{
    CGateContainer* pContainer = getContainer(self);
    std::string type = (*pContainer)->Type();
    return PyUnicode_FromString(type.c_str());
}

/**
 * getParameters
 *   Returns the parameters on which the gate is defined.  This works for
 *   gates of type:  s, b, c, gs, gb, gc, em am, nm
 *   All other gates don't depend on parameters:
 *
 * @param self - pointer to this object.
 * @param closure - unused.
 * @return PyObject*
 * @retval PyTuple - if the gate depends on parameters, this is the set of parameters.
 * @retval PyNone  - If the gate does not depend on parameters.
 */
static PyObject*
getParameters(PyObject* self, void* closure)
{
    CGateContainer* pContainer = getContainer(self);
    std::string type = (*pContainer)->Type();
    
    if (type == "s") {
        CCut* pCut = dynamic_cast<CCut*>(pContainer->getGate());
        int   paramId = pCut->getId();
        CParameter* pParam = SpecTcl::getInstance()->FindParameter(paramId);
        if (pParam) {
            std::string name = pParam->getName();
            return PyUnicode_FromString(name.c_str());
        } else {
            Py_RETURN_NONE;
        }
    } else {
        Py_RETURN_NONE;
    }
}

// Methods on the object:

static PyMethodDef gateMethods[] {    
    
    // end of of method sentinel.
    
    {NULL, NULL, 0, NULL}
};
// Getters for gate objects:

static PyGetSetDef gettersAndSetters[] = {
    // field   getter          setter    documnetation
    
    {"name", (getter)getName, NULL, "Gate Name"},
    {"type", (getter)getType, NULL, "Gate Type"},
    {"parameters", (getter)getParameters, NULL, "Parameters gate is defined on"},
    
    {NULL}                            // End sentinell.
};

//The type struct:

static PyTypeObject gateType {
    PyVarObject_HEAD_INIT(NULL, 0)
};

void setupPythonGates(PyObject* module)
{
    gateType.tp_name      = "spectcl.gate";
    gateType.tp_basicsize = sizeof(GateObject);
    gateType.tp_itemsize  = 0;
    gateType.tp_dealloc   = reinterpret_cast<destructor>(deallocate);
    gateType.tp_flags     = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
    gateType.tp_doc       = "Gate object";
    gateType.tp_methods   = gateMethods;
    gateType.tp_members   = NULL;
    gateType.tp_getset    = gettersAndSetters;
    gateType.tp_init      = reinterpret_cast<initproc>(init);
    gateType.tp_new       = ObjNew;
    
    PyType_Ready(&gateType);
    Py_INCREF(&gateType);
    PyModule_AddObject(module, "gate", reinterpret_cast<PyObject*>(&gateType));
}