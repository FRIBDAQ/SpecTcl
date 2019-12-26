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
#include <PointlistGate.h>
#include <CGammaCut.h>
#include <CGammaBand.h>
#include <CGammaContour.h>
#include <MaskAndGate.h>
#include <MaskEqualGate.h>
#include <MaskNotGate.h>
#include <ConstituentIterator.h>

#include <GateFactory.h>

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

/**
 * parameterName
 *    Turns a parameter id into a parameter name object.
 *
 * @param id - Id of the parameter.
 * @return PyObject* - either a unicode object containing the parameter name
 *                     or Py_None, if the parameter no longer exists.
 */
static PyObject*
parameterName(int id)
{
    CParameter* pParam = SpecTcl::getInstance()->FindParameter(id);
    if (pParam) {
        std::string name = pParam->getName();
        return PyUnicode_FromString(name.c_str());
    } else {
        Py_RETURN_NONE;
    }    
}
/**
 * parameterNameList
 *    Given a vector of UInt_t containing parameter ids, turns that into
 *    a Tuple containing the corresponding parameter names.
 *
 *  @param ids - the parameter list
 *  @return PyObject* tuple of names.
 */
static PyObject*
parameterNameList(const std::vector<UInt_t>& ids)
{
    PyObject* result = PyTuple_New(ids.size());
    for (int i =0; i < ids.size(); i++) {
        PyTuple_SetItem(result, i, parameterName(ids[i]));
    }
    return result;
}
/**
 * makePointDict
 *    Given and FPoint const reference, returns a PYthon Dict with x, and y
 *    keys that have values that are the x and y coords of the points.
 *
 * @param point - the point to convert.
 * @return PyObject* - the resulting dict.
 */
static PyObject*
makePointDict(const FPoint& point)
{
    Float_t x = point.X();
    PyObject* xObj = PyFloat_FromDouble(x);
    
    Float_t y = point.Y();
    PyObject* yObj = PyFloat_FromDouble(y);
    
    PyObject* result = PyDict_New();
    
    PyDict_SetItemString(result, "x", xObj);
    PyDict_SetItemString(result, "y", yObj);
    
    return result;
}
/**
 * tupleToStringVector
 *    Takes a python Tuple and turns it into a vector of strings
 *
 * @param tuple - pointer to the tuple object.
 * @return std::vector<std::string>  The resulting vector of strings.
 */
static std::vector<std::string>
tupleToStringVector(PyObject* tuple)
{
    std::vector<std::string> result;
    Py_ssize_t nItems = PyTuple_Size(tuple);
    for (int i =0; i < nItems; i++) {
        PyObject *pItem = PyTuple_GetItem(tuple, i);
        std::string text(PyUnicode_AsUTF8(pItem));
        result.push_back(text);
    }
    
    return result;
}
/**
 * tupleToPoints
 *    Take a points tuple and turn it into a vector of points.
 *    The tuple is assumed to contain dicts that have the keys x and y.
 *
 * @param tuple - pointer to the tuple.
 * @return std::vector<FPoint> the decoded points.
 * @throw std::invalid_argument - if the tuple isn't or if an element isn't a dict and
 *                                if the dict does not have the required keys.
 */
static std::vector<FPoint>
tupleToPoints(PyObject* tuple)
{
    std::vector<FPoint> result;
    if(!PyTuple_Check(tuple)) {
        throw std::invalid_argument("Points must be passed as a tuple");
    }
    Py_ssize_t nItems = PyTuple_Size(tuple);
    PyObject* xKey = PyUnicode_FromString("x");   // key for x coordinate.
    PyObject* yKey = PyUnicode_FromString("y");   // Key for y coordinate.
    
    
    for (int i =0; i < nItems; i++) {
        PyObject* pItem = PyTuple_GetItem(tuple, i);
        if (!PyDict_Check(pItem)) {
            throw std::invalid_argument("Elements of point tuples must be dicts");
        }
        PyObject* xobj = PyDict_GetItem(pItem, xKey);
        PyObject* yobj = PyDict_GetItem(pItem, yKey);
        if (xobj && yobj) {
            double x = PyFloat_AsDouble(xobj);
            double y = PyFloat_AsDouble(yobj);
            
            FPoint pt(x,y);
            result.push_back(pt);
            
        } else {
            throw std::invalid_argument(
                "Elements of point tuples must have 'x' and 'y' keys"
            );
        }
    }
    
    return result;
}
/**
 * createNewGate
 *    Called by init if there's no match to the parameter name.
 *    The assumption is that the user wants to create a new gate.
 *    This method tries to do that, creating a new gate and entering it into the
 *    histogrammer.
 *
 * @param pName - pointer to the C string that contains the gate name.
 * @param pType - Pointer to the C string that contains the gate type.
 * @param pParameters  - If parameters are needed, this a  pointer to a Python
 *                tuple that contains the parameter names.  If not supplied this
 *                is Null.
 * @param pGateNames -if dependent gates are required, points to a tuple of
 *                gate names.  Null if not suppplied.
 * @param pPoints - if points are needed, this points to a tuple of point dicts.
 *                each dict contains 'x', 'y' keys.   If not supplied, this is
 *                NULL.
 * @param low,high - for slice-like gates, these are the low and high slice limits.
 * @param mask    - For bit mask gates.
 *
 * @return CGate* nullptr for error with a Python exception raised or 0 for success.
 * 
 */
static CGate*
createNewGate(
    const char* pName, const char* pType, PyObject* pParameter,
    PyObject* pGateNames, PyObject*pPoints, double low, double high, int mask
)
{
    CGate* pGate;
    SpecTcl* pApi = SpecTcl::getInstance();
    
    try {
        // Convert the string type to a CGateFactory gate type enum:
        
        auto gateType =  CGateFactory::stringToGateType(pType);
        
        // Gates with dependent gates:
        
        if ((gateType == CGateFactory::And) ||
            (gateType == CGateFactory::Not) ||
            (gateType == CGateFactory::Or)
        )  {
            // We need some dependent gates:
            
            if (pGateNames) {
                std::vector<std::string> gates = tupleToStringVector(pGateNames);
                pGate = pApi->CreateGate(gateType, gates);
            } else {
                throw std::invalid_argument("Compound gates need a gates keyword parameter");
            }
        } else if ((gateType == CGateFactory::band) || (gateType == CGateFactory::contour)) {
            if (pParameter && pPoints) {
                std::vector<std::string> params = tupleToStringVector(pParameter);
                std::vector<FPoint>      pts    = tupleToPoints(pPoints);
                pGate = pApi->CreateGate(gateType, params, pts);
            } else {
                throw std::invalid_argument(
                    "Gates and bands need parametrs and points keyword parameters"
                );
            }
        } else if (gateType ==  CGateFactory::cut) {
            // limits are in the low/high values but we need parameters.
            if (!pParameter) {
                throw std::invalid_argument("Slice gates need a parameter");
            }
            std::vector<std::string> params = tupleToStringVector(pParameter);
            if (params.size() != 1) {
                throw std::invalid_argument("Slice gates can only have one parameter");
                
            } else {
                pGate = pApi->CreateCut(params[0], low, high);
            }
        } else if (
            (gateType == CGateFactory::gammaband) ||
            (gateType == CGateFactory::gammacontour)
        ) {
            // Need params and points:
            
            if (pParameter && pPoints) {
                std::vector<std::string> params = tupleToStringVector(pParameter);
                std::vector<FPoint>      pts    = tupleToPoints(pPoints);
                pGate = pApi->CreateGate(gateType, params, pts);
            } else {
                throw std::invalid_argument("Gamma 2d gates need parameters and points");
            }
        } else if (gateType == CGateFactory::gammacut) {
            // need any number of parameters and low/high
            
            if(pParameter) {
                std::vector<std::string> params = tupleToStringVector(pParameter);
                pGate = pApi->CreateGammaCut(low, high, params);
            } else {
                throw std::invalid_argument("Gamma slices need parameters");
            }
        } else if (
            (gateType == CGateFactory::em) || (gateType == CGateFactory::am) ||
            (gateType == CGateFactory::nm)
        ) {
            // Need one parameter and a mask
            
            if (pParameter) {
                std::vector<std::string> params = tupleToStringVector(pParameter);
                if (params.size() == 1) {
                    pGate = pApi->CreateGate(gateType, params, mask);
                } else {
                    throw std::invalid_argument("Mask gates must have exactly one parameter");
                }
            } else {
                throw std::invalid_argument("Mask gates require a parameter");
            }
        } else if (gateType == CGateFactory::trueg) {
            pGate = pApi->CreateTrueGate();
        } else if (gateType == CGateFactory::falseg) {
            pGate = pApi->CreateFalseGate();
        } else {
            // In theory, gateTypeToString has already thrown but this can't
            // hurt either.
            
            throw std::invalid_argument("Invalid gate type");
        }
        
        
    }
    // Failures result in a null pointer  return and a Python exception
    catch (CException& e) {
        std::string msg = e.ReasonText();
        PyErr_SetString(PyExc_SyntaxError, msg.c_str());
        return nullptr;
    }
    catch (std::exception& e) {
        std::string msg = e.what();
        PyErr_SetString(PyExc_SyntaxError, msg.c_str());
        return nullptr;
    }
    catch (std::string msg) {
        PyErr_SetString(PyExc_SyntaxError, msg.c_str());
        return nullptr;
    }
    catch (const char* msg) {
        PyErr_SetString(PyExc_SyntaxError, msg);
        return nullptr;
    }
    catch (...) {
        PyErr_SetString(PyExc_AssertionError,"un-anticipated C++ exception caught");
        return nullptr;
    }
    
    if(!pGate) {
        PyErr_SetString(
            PyExc_AssertionError,
            "No exception but no valid gate at the end of it all"
        );
    }
    
    return pGate;
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
        "name","type", "parameters", "gates", "points", "low", "high", "mask", 
        nullptr
    };
    const char* tupleconversions = "s";
    const char* kwdconversions  = "s|$sOOOddd";
    
    
    // The parameters:
    
    char* pName(nullptr);             // Required
    char* pType(nullptr);
    PyObject* pParameters(nullptr);
    PyObject* pGateNames(nullptr);
    PyObject* pPoints(nullptr);
    double low(0), high(0);
    int mask(0);
    
    
    
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
            &pGateNames, &pPoints, &low, &high, &mask
        )
    ){
        PyErr_Clear();   
        if (!pType) {
            PyErr_SetString(PyExc_TypeError, "Gate 'type' is a required keyword argument");
            return -1;
        }
        
        CGate* pGate = createNewGate(
            pName, pType, pParameters, pGateNames, pPoints, low, high, mask
        );
        // If we succeeded, find our gate container and set it in the object storage.
        if (pGate) {
            SpecTcl* pApi = SpecTcl::getInstance();
            pApi->AddGate(pName, pGate);
            me->s_pContainer = pApi->FindGate(pName);
        }
        return 0;
        
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
        PyObject* result = PyTuple_New(1);
        PyTuple_SetItem(result, 0, parameterName(paramId));
        return result;
    } else if ((type == "c") || (type == "b")) {
        // These are both point list gates 2 item ntuple containing the names:
        // X parameter is first.
        CPointListGate*  pGate = dynamic_cast<CPointListGate*>(pContainer->getGate());
        PyObject* result = PyTuple_New(2);
        int xid = pGate->getxId();
        int yid = pGate->getyId();
        
        PyTuple_SetItem(result, 0, parameterName(xid));
        PyTuple_SetItem(result, 1, parameterName(yid));
        return result;
    } else if (type == "gs") {
        
        // Tuple of parameters
        
        CGammaCut* pCut = dynamic_cast<CGammaCut*>(pContainer->getGate());
        CGammaCut::ParameterList params = pCut->getParameters();
        return parameterNameList(params);
        
    } else if (type == "gb") {
        CGammaBand* pBand = dynamic_cast<CGammaBand*>(pContainer->getGate());
        CGammaBand::ParameterList params = (pBand->getParameters());   // Todo gamma gate base class...
        return parameterNameList(params);
    } else if (type == "gc") {
        CGammaContour* pContour = dynamic_cast<CGammaContour*>(pContainer->getGate());
        CGammaContour::ParameterList params (pContour->getParameters());   // Todo gamma gate base class...
        return parameterNameList(params);
    } else if (type == "em") {
        CMaskEqualGate* pGate = dynamic_cast<CMaskEqualGate*>(pContainer->getGate());
        PyObject* result = PyTuple_New(1);
        PyTuple_SetItem(result, 0, parameterName(pGate->getId()));
        return result;
    } else if (type == "am") {
        CMaskAndGate* pGate = dynamic_cast<CMaskAndGate*>(pContainer->getGate());
        PyObject* result = PyTuple_New(1);
        PyTuple_SetItem(result, 0, parameterName(pGate->getId()));
        return result;
    } else if (type == "nm") {
        CMaskNotGate* pGate = dynamic_cast<CMaskNotGate*>(pContainer->getGate());
        PyObject* result = PyTuple_New(1);
        PyTuple_SetItem(result, 0, parameterName(pGate->getId()));
        return result;
        
    } else {
        Py_RETURN_NONE;                 // These gate types don't have parameters.
    }
}
/**
 * getGates
 *   Returns a tuple that contains the name sof the gates the compound gate
 *   depends on.  Note that if a gate is primitive or in some other way
 *   does not depend on other gates, Py_None is returned, not an empty tuple.
 *   Note as well that if there are multiple levels of dependency only the top
 *   level is returned, for example:
 * \verbatim
 *     gate cut c {....}
 *     gate not n c
 *     gate and * {n gate1 gate2}
 *
 * \endverbatim
 *
 *    this will return a tuple containing n, gate, and gate2, not cut, gate1, and gate2.
 *    This is necessary to allow gates to be reconstructed from previously saved
 *    information.
 *
 * @param self   - Pointer to the gate object data.
 * @param closure - pointer to something I don't really understand the necessity of.
 * @return PyObject* - tuple containing the gates or none if the gate does not
 *                 depend on anything.
 */
static PyObject*
getGates(PyObject* self, void*)
{
    CGateContainer* pContainer = getContainer(self);
    std::string type = (*pContainer)->Type();
    
    if ((type == "*") || (type =="+") || (type == "-")) {
        CGate* pGate =  pContainer->getGate();
        
        // The constituent iterators return the names of gates:
        
        PyObject* result = PyTuple_New(pGate->Size());
        CConstituentIterator p = pGate->Begin();
        for (int i =0; i < pGate->Size(); i++) {
            std::string name = pGate->GetConstituent(p);
            ++p;
            
            PyTuple_SetItem(result, i, PyUnicode_FromString(name.c_str()));
        }
        return result;
        
    } else {
         Py_RETURN_NONE;                // Primitive gate.
    }
}
/**
 * getLow
 *    Gets the low limit of a slice type gate.  Applies to gates of type
 *    s and gs.
 * @param self - pointer to object storage.
 * @param closure - not well documented parameter.
 * @return PyObject* floating point number.
 */
static PyObject*
getLow(PyObject* self, void* closure)
{
    CGateContainer* pContainer = getContainer(self);
    std::string type = (*pContainer)->Type();

    // Gamma slice derives from cut so it has the getLow method:
    
    if ((type == "s") || (type == "gs")) {
        CCut* pCut = dynamic_cast<CCut*>(pContainer->getGate());
        Float_t low = pCut->getLow();
        return PyFloat_FromDouble(low);
    } else  {
        Py_RETURN_NONE;
    }
}
/** getHigh
 *    Same as getLow but gets the cut/slice high limit:
 */
static PyObject*
getHigh(PyObject* self, void* closure)
{
    CGateContainer* pContainer = getContainer(self);
    std::string type = (*pContainer)->Type();

    // Gamma slice derives from cut so it has the getLow method:
    
    if ((type == "s") || (type == "gs")) {
        CCut* pCut = dynamic_cast<CCut*>(pContainer->getGate());
        Float_t high = pCut->getHigh();
        return PyFloat_FromDouble(high);
    } else  {
        Py_RETURN_NONE;
    }
}
/**
 * getPoints
 *    Returns the set of points that make up the gate boundary for a 2d
 *    primitive gate (e.g. b, c, gb, gc).
 *    The points are returned as a Tuple of dicts where each dict contains
 *    an 'x' and 'y' key.
 *
 * @param self - pointer to object storage.
 * @param closure
 * @return PyObject*  - see above.  Non applicable gates will return none.
 */
static PyObject*
getPoints(PyObject* self, void* closure)
{
    CGateContainer* pContainer = getContainer(self);
    std::string type = (*pContainer)->Type();
    
    if ((type == "b") || (type == "c") || (type == "gb")  || (type == "gc")) {
        CPointListGate* pGate = dynamic_cast<CPointListGate*>(pContainer->getGate());
        std::vector<FPoint> points = pGate->getPoints();
        
        PyObject* result = PyTuple_New(points.size());
        for (int i =0; i < points.size(); i++) {
            PyTuple_SetItem(result, i, makePointDict(points[i]));
        }
        return result;
    } else {
        Py_RETURN_NONE;
    }
}
/**
 * mask
 *    Returns the mask value for a mask gate.
 * @param self - pointer to to object storage.
 * @param closure
 * @return PyObject* - integer mask.
 */
static PyObject*
getMask(PyObject* self, void* closure)
{
    CGateContainer* pContainer = getContainer(self);
    std::string type = (*pContainer)->Type();
    
    if ((type == "em") || (type == "am") || (type == "nm")) {
        CMaskGate* pGate = dynamic_cast<CMaskGate*>(pContainer->getGate());
        long mask = pGate->getCompare();
        return PyLong_FromLong(mask);
    } else {
        Py_RETURN_NONE;
    }
}
//////////////////////////////////////////////////////////////////////////////
// Object methods

/**
 * changeGate
 *    Changes the gate object.  The parameters accepted are esssentially the
 *    same as for the branch of init that creates a new gate, however
 *    the gate created will replace the gate by the name of self.
 *
 * @param self - Pointer to the object's storage.
 * @param args - Positional arguments, in this case there are none.
 * @params kwds - Keyword parameters.
 * @note - we initialize pretty much anything from the current gate.
 *         This allows for minimal modifications.  For example if all you want
 *         to do is change the set of points in an contour you just have to
 *         specify the 'points' keyword argument.
 * @return PyObject*  - None.
 * @retval nullptr - in case of error.
 */
static PyObject*
changeGate(PyObject* self, PyObject* args, PyObject* kwds)
{
    CGateContainer* pContainer = getContainer(self);
    std::string     type       = (*pContainer)->Type();
    std::string     name       = pContainer->getName();
    
    // Now initialize defaults from the object:
    
    const char* pName     = name.c_str();
    const char* pType     = type.c_str();
    PyObject* pParameters = getParameters(self, nullptr);
    PyObject* pGates      = getGates(self, nullptr);
    PyObject* pPoints     = getPoints(self, nullptr);
    
    // The remainder are integers that have to be pulled out of their objects.
    
    double low(0), high(0);
    int mask(0);
    
    PyObject* pobj       = getLow(self, nullptr);
    if (pobj != Py_None) low = PyFloat_AsDouble(pobj);
    
    pobj = getHigh(self, nullptr);
    if (pobj != Py_None) high = PyFloat_AsDouble(pobj);

    pobj = getMask(self,nullptr);
    if (pobj != Py_None) mask = PyLong_AsLong(pobj);
    
    // Now we're ready to parse the parameters we do have:
    
    const char* kwdconversions  = "|$sOOOddd";              // No name accepted.
    char* keywords[] = {
        "type", "parameters", "gates", "points", "low", "high", "mask", 
        nullptr
    };
    if (
        PyArg_ParseTupleAndKeywords(
            args, kwds, kwdconversions, keywords,  &pType, &pParameters,
            &pGates, &pPoints, &low, &high, &mask
        )
    ) {
        CGate* pGate = createNewGate(
            pName, pType, pParameters, pGates, pPoints, low, high, mask
        );
        if (pGate) {
            SpecTcl* pApi = SpecTcl::getInstance();
            pApi->ReplaceGate(pName, *pGate);    // Gate container is still valid.
            Py_INCREF(self);
            return self;
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}

// Methods on the object:

static PyMethodDef gateMethods[] {    
    {"change", (PyCFunction)changeGate, METH_VARARGS | METH_KEYWORDS},
    
    // end of of method sentinel.
    
    {NULL, NULL, 0, NULL}
};
// Getters for gate objects:

static PyGetSetDef gettersAndSetters[] = {
    // field   getter          setter    documnetation
    
    {"name", (getter)getName, NULL, "Gate Name"},
    {"type", (getter)getType, NULL, "Gate Type"},
    {"parameters", (getter)getParameters, NULL, "Parameters gate is defined on"},
    {"gates", (getter)getGates, NULL, "Component Gates"},
    {"low", (getter)getLow, NULL, "Get low limit of slices"},
    {"high", (getter)getHigh, NULL, "Get high limit of slices"},
    {"points", (getter)getPoints, NULL, "Get points from 2d gates"},
    {"mask", (getter)getMask, NULL, "Get mask for bitmask gate"},
    
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