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
#include <SpecTcl.h>

#include <stdlib.h>
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
    const char* conversions = "s$s$O$O$O$d$d";
    
    // The parameters:
    
    char* pName(nullptr);             // Required
    char* pType(nullptr);
    PyObject* pParameters(nullptr);
    PyObject* pGateNames(nullptr);
    PyObject* pPoints(nullptr);
    int low(0), high(0);
    
    if (
        PyArg_ParseTupleAndKeywords(
            args, kwds, conversions, keywords,
            pName, pType, pParameters, pGateNames, pPoints, &low, &high
        )
    ) {
        return -1;                      // Exception already raised.
    }
    // If there's a name but pType, pParameters, pGateNames, pPoints are all null,
    // this is a wrapping.
    
    if (pName && (!pType && !pParameters && !pGateNames && !pPoints)) {
        
        // Wrap existing gate:
        
        SpecTcl* pApi = SpecTcl::getInstance();
        me->s_pContainer = pApi->FindGate(pName);
        if (!me->s_pContainer) {
            PyErr_SetString(PyExc_LookupError, "No such gate name");
            return -1;
        }
        return 0;
        
    } else {
        
        PyErr_SetString(PyExc_TypeError, "Gate creation not yet supported");
        // Create/replace gate.
        
    } 
}

// Methods on the object:

static PyMethodDef gateMethods[] {    
    
    // end of of method sentinel.
    
    {NULL, NULL, 0, NULL}
};

//The type struct:

static PyTypeObject gateType {
    PyVarObject_HEAD_INIT(NULL, 0)
};

void setupPythonGateObjects(PyObject* module)
{
    gateType.tp_name      = "spectcl.gate";
    gateType.tp_basicsize = sizeof(GateObject);
    gateType.tp_itemsize  = 0;
    gateType.tp_dealloc   = reinterpret_cast<destructor>(deallocate);
    gateType.tp_flags     = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
    gateType.tp_doc       = "Gate object";
    gateType.tp_methods   = gateMethods;
    gateType.tp_members   = NULL;
    gateType.tp_init      = reinterpret_cast<initproc>(init);
    gateType.tp_new       = ObjNew;
    
    PyType_Ready(&gateType);
    Py_INCREF(&gateType);
    PyModule_AddObject(module, "gate", reinterpret_cast<PyObject*>(&gateType));
}