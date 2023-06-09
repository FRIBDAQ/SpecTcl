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

/** @file:  pythonSpectrum.cpp
 *  @brief: Implement python encapsulation of a SpecTcl Spectrum.
 */
#include "pythonSpectrum.h"         // includes Python.h.
#include "SpecTcl.h"
#include "Spectrum.h"
#include "CAxis.h"
#include "Gamma2DD.h"
#include "GateContainer.h"
#include "DisplayInterface.h"
#include "Display.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>

#include <Exception.h>
#include <stdexcept>
#include <sstream>
#include <iostream>

class CGateContainer;

/**
 * The spectrum type provides an encapsulation of the spectrum
 * SpecTcl object. There are two ways to construct a spectrum:
 *
 *   spectrum(name) - looks up an existing spectrum and encapsulates it.
 *   spectrum(name, type, parameters, axis-specs)
 *        Creates a new spectrum 'name:
 *        -  type -is the spectrum type string - same string as used in
 *           Tcl
 *        - Parameters is the iterable object of parameter names.
 *            EXCEPTION _ for the type, gd (Gamma Deluxe), this  must be
 *            a two element iterable of iterables.  The first iterable
 *            contains the x parameters the second the y parameters.
 *        - axis-specs is the iterable object of axis specifications.
 *          each axis specification is a dict that has the keys
 *           *  low  - low limit of that axis.
 *           *  high - High limit of that axis.
 *           *  bins - Number of bins in that axis.
 *
 * Spectra have the following attributes:
 *    -   name
 *    -   type
 *    -   parameters - tuple of parameter names.
 *            EXCEPTION _ for the type, gd (Gamma Deluxe), this  must be
 *            a two element iterable of iterables.  The first iterable
 *            contains the x parameters the second the y parameters.
 *    -   axes       - tuple of axis definitions.
 *
 *  Spectra have the following methods:
 *     - clear - clear spectrum contents.
 *     - gate - Apply a gate to the spectrum.  The gate is specified by name.
 *     - ungate - Remove any gate applied to a spectrum.
 *     - bind   - Bind spectrum storage to  displayer shared memory.
 *     - unbind - Unbind spectrum stuorage from shared memory.
 *     - get    - Get a channel of the spectrum.
 *     - set    - Set a channel value in the spectrum.
 *
 * @note - memory is cheap all spectra are creqted as having longword
 *         channels. 
 */

// The object struct:

typedef struct {
    PyObject_HEAD
    SpectrumData  s_objData;
} SpectrumObject, *pSpectrumObject;

////////////////////////////////////////////////////////////////////
// Utility methods - note some of these may want to live in
// a common utility library.

/**
 * axisToDict
 *   Converts an axis definition into a dictionary.
 * @param def - references an axis definition.
 * @return PyObject* dict.
 */
static PyObject*
axisToDict(CAxis& def)
{
    PyObject* result = PyDict_New();
    
    PyDict_SetItemString(result, "low", PyFloat_FromDouble(def.getLow()));
    PyDict_SetItemString(result, "high", PyFloat_FromDouble(def.getHigh()));
    PyDict_SetItemString(result, "bins", PyLong_FromLong(def.getChannels()));
    
    return result;
}

/**
 * stringVecToTuple
 *    @param strings - strings to convert.
 *    @return Py_Object* Tuple whose contents are the objectified strings.
 */
static PyObject*
stringVecToTuple(std::vector<std::string>& strings)
{
    PyObject* result = PyTuple_New(strings.size());
    
    for (int i =0; i < strings.size(); i++) {
        PyObject* s = PyUnicode_FromString(strings[i].c_str());
        PyTuple_SetItem(result, i, s);
    }
    
    return result;
}

/**
 * getSpectrum
 *    @param self - pointer to object.
 *    @return CSpectrum* - pointer to the spectrum object.
 *    @retval NULL - the spectrum has been deleted.  In this case, an
 *                   exception will be raised.
 */
static CSpectrum*
getSpectrum(PyObject* self)
{
    pSpectrumObject me = (pSpectrumObject)(self);
    std::string name = *(me->s_objData.s_spectrumName);
    CSpectrum* pSpec = SpecTcl::getInstance()->FindSpectrum(name);
    if (!pSpec) {
        PyErr_SetString(PyExc_RuntimeError, "Spectrum no longer exists");
        return NULL;
    }
    return pSpec;     
}

/** IterableToStringVector
 *    Convert a Python iterable object to a vector of strings.
 * @param o - the object to convert.
 * @return std::vector<std::string> the strings in the iterable.
 * @throw std::string for errors.  The caller should catch and turn that into
 *        a python exception e.g.
*/
static std::vector<std::string>
IterableToStringVector(PyObject* o)
{
    PyObject* iter(nullptr);
    PyObject* item(nullptr);
    std::vector<std::string> result;
    try {
        iter = PyObject_GetIter(o);
        if (!iter) throw std::string("Object must be iterable and is not");
        
        while(item = PyIter_Next(iter)) {
            // Python strings are all unicode:
            
            if (!PyUnicode_Check(item)) {
                throw std::string("Object elements must be unicode strings.");
            }
            const char* value = PyUnicode_AsUTF8(item);
            if (!value) {
                throw std::string("One of the object's elements does not convert to UTF8");
            }
            result.push_back(std::string(value));
            
            Py_DECREF(item);
            item = nullptr;
        }
        Py_DECREF(iter);
        
    } catch (...) {
        if (iter) Py_DECREF(iter);
        if (item) Py_DECREF(item);
        throw;
    }
    return result;
}
/**
 * IterableToUInts
 *   @param  Coords -iterable of unsigned longs that contains the coordinates.
 *   @return std::vector<UInt_t>  - the extracted unsigned longs
 *   @throw std::string on errors.
 */
std::vector<UInt_t>
IterableToUInts(PyObject* coords)
{
    std::vector<UInt_t> result;
    PyObject* iter = PyObject_GetIter(coords);
    PyObject* item(nullptr);
    
    if (!iter) {
        throw std::string("The coordinates must be an iterable object");

    }
    try {
        while(item = PyIter_Next(iter)) {
            if(!PyNumber_Check(item)) {
                throw std::string("Coordinates must be numeric");
            }
            PyObject* i = PyNumber_Long(item);
            if (!i) {
                throw std::string("Coordinates must have an integer representation");
            }
            result.push_back(PyLong_AsUnsignedLong(i));
            
            Py_DECREF(i);
            Py_DECREF(item);
            item = NULL;
        }
        Py_DECREF(iter);
        iter = NULL;
    } catch(...) {
        if(iter) Py_DECREF(iter);
        if(item) Py_DECREF(item);
        throw;
    }
    return result;
}

/**
 * createSpectrum
 *    Create a new spectrum.
 *  @param self Pointer to our object struct which, if successful, will
 *         have the spectrum name constructed into the object data
 *  @param  args - name, type-string, parameters, and axis definitions.
 *  @return int - as expected to be returned by init - note that
 *                if nonzero (failure) and exception should have been raised.
 */
static int
createSpectrum(pSpectrumObject self, PyObject* args)
{
    char*     name;
    char*     type;
    PyObject* parameters;
    PyObject* axisDefs;
    if (!PyArg_ParseTuple(args, "ssOO", &name, &type, &parameters, &axisDefs)) {
        return -1;
    }
    // Duplicate names are bad:
    
    if (SpecTcl::getInstance()->FindSpectrum(name))  {
        PyErr_SetString(PyExc_RuntimeError, "Duplicate spectrum name");
        return -1;
    }
    // Failures from here on in toss exceptions:
    
    int stat = 0;            // hope for the best.
    PyObject* iter(nullptr);
    PyObject* item(nullptr);
    PyObject* xpars(nullptr);
    PyObject* ypars(nullptr);
    CSpectrum* pSpec(nullptr);
    try {
        // We can easily create the axes specs from the axisDefs iterable.
        
        std::vector<Float_t> lows;
        std::vector<Float_t> highs;
        std::vector<UInt_t>   binsvec;
        
       
        iter = PyObject_GetIter(axisDefs);
        if(!iter) throw std::string("Axis definitions must be an iterable");
        while(item = PyIter_Next(iter)) {
            // Item must be a dict -- or subclass
            
            if (!PyDict_Check(item)) {
                throw std::string("Axes must be specified with a dictionary");
            }
            PyObject* low = PyDict_GetItemString(item, "low");
            PyObject* high= PyDict_GetItemString(item, "high");
            PyObject* bins= PyDict_GetItemString(item, "bins");
            if ((!low) || (!high) || (!bins)) {
                throw std::string("Axis dictionaries must have low, high and bins keys");
            }
            // Require that low, high, bins be numbers:
            
            if ((!PyNumber_Check(low)) || (!PyNumber_Check(high)) || (!PyNumber_Check(bins))) {
                throw std::string("Low, high and bins for axis specs must be numbers");
            }
            lows.push_back(PyFloat_AsDouble(PyNumber_Float(low)));
            highs.push_back(PyFloat_AsDouble(PyNumber_Float(high)));
            binsvec.push_back(PyLong_AsLong(PyNumber_Long(bins)));
            
            // I think low, high, bins and their conversions are all borrowed
            // and hence don't need decrefs(?).
            
            
            Py_DECREF(item);
            item = nullptr;              // So we don't double decr.
        }
        Py_DECREF(iter);
        iter = nullptr;
        
        // With the exception of "gd" spectrum types, the
        // parameters object should be an iterable containing parameter names.
        
        std::string stype(type);          // Makes comparisons easier.
        std::stringstream typeStream;
        typeStream << stype;
        SpectrumType_t spectrumType;
        typeStream >> spectrumType;
        if (spectrumType != keG2DD) {             // single vector of parameter names.
            std::vector<std::string> parameterNames;
            
            parameterNames = IterableToStringVector(parameters);
            
            // At this point we can try to create the histogram:
            
            pSpec = SpecTcl::getInstance()->CreateSpectrum(
                name, spectrumType, keLong, parameterNames, binsvec, &lows, &highs
            );
            if (!pSpec) throw std::string("Failed to create spectrum");
            
        } else {
            // There are x and y parameters.  The parameters
            // argument must be iterable with exactly two elements, no more.
            // No less.
            std::vector<std::string> xparNames;
            std::vector<std::string> yparNames;
            
            
            PyObject* iter = PyObject_GetIter(parameters);
            if (!iter) throw std::string("parameters must support iteration");
            xpars          = PyIter_Next(iter);
            ypars          = PyIter_Next(iter);
            
            PyObject* dummy = PyIter_Next(iter);
            
            // Count checking
            
            if(dummy) {
                Py_DECREF(dummy);
                throw std::string("gd spectra only should have two parameter lists.");
            }
            if ((!xpars) || (!ypars)) {
                throw std::string("gd spectra must have exatly two parameter lists.");
            }
            xparNames = IterableToStringVector(xpars);
            yparNames = IterableToStringVector(ypars);
            
            Py_DECREF(xpars);
            xpars = nullptr;
            
            Py_DECREF(ypars);
            ypars = nullptr;
            
            Py_DECREF(iter);
            iter = nullptr;
        }
        self->s_objData.s_spectrumName = new std::string(name);  // save spectrum name.
    } catch (CException& e) {
        PyErr_SetString(PyExc_RuntimeError, e.ReasonText());
        stat = -1;
    }
    catch (std::exception& e) {
        PyErr_SetString(PyExc_RuntimeError, e.what());
        stat = -1;
    } catch (std::string msg) {
        PyErr_SetString(PyExc_RuntimeError, msg.c_str());
        stat = -1;
    } catch (const char* msg) {
        PyErr_SetString(PyExc_RuntimeError, msg);
        stat = -1;
    } catch (...) {
        PyErr_SetString(
            PyExc_RuntimeError,
            "Unanticipated exception type caught creating a spectrum"
        );
        stat = -1;
    }
    
    if(iter)  Py_DECREF(iter);
    if(item)  Py_DECREF(item);
    if(xpars) Py_DECREF(xpars);
    if(ypars) Py_DECREF(ypars);
    if(pSpec) SpecTcl::getInstance()->AddSpectrum(*pSpec);    
    return stat;
}

////////////////////////////////////////////////////////////////////
// Object allocation and deletion.

/**
 * ObjNew
 *    Allocate storage for the object.  The string we carry
 *    along will be initialized to a null pointer.
 *
 * @param type - pointer to our type object.
 * @param args - unused.
 * @param kwargs -unused.
 * @return PyObject* - pointer to the newly created SpectrumObject.
 */
static PyObject*
ObjNew(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
    pSpectrumObject p;
    p = (pSpectrumObject)(type->tp_alloc(type, 0));
    if (p!=NULL) {
        p->s_objData.s_spectrumName = nullptr;
    }
    return (PyObject*) p;
}
/**
 * init
 *    Initializes a spectrum object.  This can mean looking up the
 *    spectrum in the spectrum dictionary.  If it's found that's cool.
 *    This can also mean attempting to create a spectrum and put it in the
 *    spectrum dictionary which can be a lugubrious process that we'll
 *    delegate to the createSpectrum function.
 *
 *  @param self - pointer to the object to initialize (pSpectrumObject)
 *  @param args - Positional parameters.
 *  @param kwds - Keyword parameters/dict.
 *  @return int   - 0 on success, -1 if failure (in which case an exception
 *                 is assumed to have been thrown)
 */
static int
init(PyObject* self, PyObject* args, PyObject* kwds)
{
    pSpectrumObject me = (pSpectrumObject)(self);
    Py_ssize_t   nargs = PyTuple_Size(args);
    
    // There can be one or four parameters:
    
    if ((nargs != 1) && (nargs != 4)) {
        PyErr_SetString(PyExc_SyntaxError, "Incorrect number of parameters");
        return -1;
    }
    if (nargs == 1) {
        // Lookup.  If we can find it it's cool:
        
        char *name;
        if (!PyArg_ParseTuple(args, "s", &name)) {
            return -1;
        }
        if (SpecTcl::getInstance()->FindSpectrum(name)) {
            delete me->s_objData.s_spectrumName; // Harmless if nil.
             me->s_objData.s_spectrumName = new std::string(name);
             return 0;
        } else {
            // not found
            
            PyErr_SetString(PyExc_LookupError, "No such spectrum");
            return -1;
        }
    } else {
        // Create:
        
        return createSpectrum(me, args);
    }
}
/**
 * deallocate
 *    Free storage associated with a spectrum object.
 *
 * @param self - object to free.
 */
static void
deallocate(PyObject* self)
{
    pSpectrumObject me = (pSpectrumObject)(self);
    delete me->s_objData.s_spectrumName;
    Py_TYPE(self)->tp_free(self);
}
////////////////////////////////////////////////////////////////////////////
// Methods

/**
 * clear
 *    Clears the contents of the spectrum.
 *
 * @param self - pointer to what's axtually a pSpectrumObject that.
 * @return Py_None.
 * @note if the spectrum encapsulated  by this object has been deleted,
 *       we raise a runtime error.
 */
static PyObject*
clear(PyObject* self, PyObject* Py_UNUSED(ignored))
{
    
    CSpectrum* pSpec = getSpectrum(self);
    if (pSpec) {
        pSpec->Clear();
        pSpec->clearStatArrays();
        
        Py_RETURN_NONE;
    } else {
        return NULL;
    }
}

/**
 * ungate
 *    Remove any gate set on the encapsulated spectrum spectrum.
 *
 * @param self - pointer to the pSpectrumObject object.
 * @return PyNone
 * @note raises an exception if the spectrum could not be found.  If the
 *       spectrum is ungated, this just is a no-op.
 */
static PyObject*
ungate(PyObject* self, PyObject* Py_UNUSED(ignored))
{
    CSpectrum* pSpec = getSpectrum(self);
    if (pSpec) {
        pSpec->Ungate();
        Py_RETURN_NONE;
    } else {
        return NULL;
    }
}
/**
 * gate
 *    Apply a gate to this spectrum.
 *
 * @param self - pointer to the object data.
 * @param args - Pointer to a tuple that must contain only a single parameter,
 *               the name of the gate.
 * @return Py_None though a runtime error exception will be thrown if either
 *              the spectrum has been deleted out from under us or the gate
 *              is not found.
 */
static PyObject*
gate(PyObject* self, PyObject* args)
{
    CSpectrum* pSpec = getSpectrum(self);
    if (pSpec) {
        char* gateName;
        if (!PyArg_ParseTuple(args, "s", &gateName)) {
            return NULL;
        }
        CGateContainer* pGate = SpecTcl::getInstance()->FindGate(gateName);
        if (pGate) {
            pSpec->ApplyGate(pGate);
            Py_RETURN_NONE;
        } else {
            PyErr_SetString(PyExc_RuntimeError, "No such gate");
            return NULL;
        }
    } else {
        return NULL;
    }
}
/**
 * bind
 *    Binds the spectrum to the displayer.
 *  @param self - pointer to this object.
 *  @return Py_None if successful.
 */
static PyObject*
bind(PyObject* self, PyObject* Py_UNUSED(ignored))
{
    CSpectrum* pSpectrum = getSpectrum(self);
    if (pSpectrum) {
        SpecTcl* api = SpecTcl::getInstance();
        CDisplayInterface* pDisplayApi = api->GetDisplayInterface();
        CDisplay*          pDisplay    = pDisplayApi->getCurrentDisplay();
        if (!pDisplay->spectrumBound(pSpectrum)) {
            pDisplay->addSpectrum(*pSpectrum, *(api->GetHistogrammer()));
        }
        Py_RETURN_NONE;
    } else {
        return NULL;
    }
}
/**
 * unbind
 *   Unbind the spectrum from the display.
 * @param selfg - pointer to this object.
 * @return Py_None if successful.
 */
static PyObject*
unbind(PyObject* self, PyObject* Py_UNUSED(ignored))
{
    CSpectrum* pSpectrum = getSpectrum(self);
    if (pSpectrum) {
        SpecTcl* api = SpecTcl::getInstance();
        CDisplayInterface* pDisplayApi = api->GetDisplayInterface();
        CDisplay*          pDisplay    = pDisplayApi->getCurrentDisplay();
        if (pDisplay->spectrumBound(pSpectrum)) {
            pDisplay->removeSpectrum(*pSpectrum, *(api->GetHistogrammer()));
        }
        Py_RETURN_NONE;
    } else {
        return NULL;
    }    
}

/**
 * Get
 *    Return the value of a channel from a  spectrum.
 *    The single parameter is an interable that contains the spectrum
 *    channel coordinates.
 *
 * @param self - pointer to the storage associated with the object whose method is
 *               being called.
 * @param args - Pointer to the coordinate iterable argument.
 * @return PyObject* - the channelvalue (an integer).
 */

static PyObject*
Get(PyObject* self, PyObject* args)
{
    CSpectrum*  pSpectrum = getSpectrum(self);
    if (!pSpectrum) {
        return NULL;                     // Spectrum was yanked out from underneath.
    }
    
    PyObject* coordObj(0);
    
    if(!PyArg_ParseTuple(args, "O", &coordObj)) {
        return NULL;
    }
    //  Coords must be able to give us an iterable:
    // All this is done in a try catch block to ensure that
    // we decref what we need in case of error.
    // We set appropriately decrefed objects to null to prevent over decrefing.
    
    
    
    try {

        std::vector<UInt_t> coords = IterableToUInts(coordObj);
        
        // At this point we've marshalled the coords.. there can be at most 2:
        
        if (coords.size() > 2) {
            throw std::string("SpecTcl Spectra have at most two coordinates.");
        }
    
        ULong_t iResult =    (*pSpectrum)[coords.data()];   // Can throw.
        return PyLong_FromUnsignedLong(iResult);
    }
    catch (CException& e) {
        PyErr_SetString(PyExc_RuntimeError, e.ReasonText());
        return NULL;
    }
    catch (std::string msg) {
        PyErr_SetString(PyExc_RuntimeError, msg.c_str());
        return NULL;
    }
    catch (...) {
        PyErr_SetString(PyExc_RuntimeError, "Unanticipated exception type");
        return NULL;
    }
   
    PyErr_SetString(PyExc_RuntimeError, "*** Control flow bug!! ****");
    return NULL;
}
/**
 * Set
 *    Sets a spectrum channel.  There should be two arguments.  The
 *    first is an iterable that contains the integer channel coordinates.
 *    The second is the value to give to that coordinate.
 *
 * @param self - pointer to the storage of the object that is having its method
 *               called.
 * @param args - positional parameters (see above for what's expected).
 * @return Py_None
 * 
 */
static PyObject*
Set(PyObject* self, PyObject* args)
{
    CSpectrum* pSpectrum  = getSpectrum(self);
    if (!pSpectrum) {
        return NULL;
    }
    
    ULong_t   value;
    PyObject* coordObj;
    
    if(!PyArg_ParseTuple(args, "Ok", &coordObj, &value)) {
        return NULL;
    }
    try {
        std::vector<UInt_t> coords = IterableToUInts(coordObj);
        pSpectrum->set(coords.data(), value);
        Py_RETURN_NONE;
    }
    catch (CException e) {
        PyErr_SetString(PyExc_RuntimeError, e.ReasonText());
        return NULL;
    }
    catch (std::string msg) {
        PyErr_SetString(PyExc_RuntimeError, msg.c_str());
        return NULL;
    }
    catch (...) {
        PyErr_SetString(PyExc_RuntimeError, "Unanticipated exception type");
        return NULL;
    }
    
    PyErr_SetString(PyExc_RuntimeError, "*** Logic Flow error");
    return NULL;

    
}
/**
 * project
 *
 *     Create a projection of this spectrum.  Note that only 2-d spectrum types
 *     can ge projected.  The actual logic is complex enough and entertwined
 *     (alas) with the Tcl project command that we're just going to
 *     construct and execute an appropriate Tcl command.
 *
 *  @param self - This is a pointer to our object data.
 *  @param args - Positiona arguments that are, in order:
 *                - Name of the new spectrum.
 *                - Direction ("x" or "y") in which the projection will be done.
 *  @param kwds   - Optional arguments identified by keywords as follows:
 *                 *  snapshot = bool - if true the result is a snapshot spectrum
 *                    otherwise it's live.  The default is false.
 *                 *  contour - Name of a contour gate within which
 *                    the projection is done.  If snapshot is false,
 *                    this gate will be applied to the new spectrum.
 *                    
 * @return PyObject* On success, this is the name of the new spectrum.  A common
 *                 idiom might be:
 * \verbatim
 *    s = spectrum('twod')
 *    p = spectrum(s.project('projected', x, contour='some_contoure'))
 * \endverbatim
 *    Which creates the new projected spectrum and an object wrapping it.
 */
static PyObject*
project(PyObject* self, PyObject* args, PyObject* kwds)
{
    CSpectrum* pSpec = getSpectrum(self);
    if (pSpec) {
        std::string originalName = pSpec->getName();
        
        char* newName(nullptr);
        char* direction;
        int   snapshot(0);
        char* gateName(nullptr);
        const char* keywords[] = {"name", "direction", "contour", "snapshot", NULL};
        
        if(!PyArg_ParseTupleAndKeywords(
            args, kwds, "ss|$sp", const_cast<char**>(keywords), 
            &newName, &direction, &gateName, &snapshot
        )) {
            return NULL;
        }
        
        try {
            CTCLInterpreter* pI = SpecTcl::getInstance()->getInterpreter();
            CTCLObject cmd;                    // Build up the command here:
            cmd.Bind(pI);
            cmd = "project";
            if (!snapshot) {
                cmd += "-nosnapshot";
            }
            cmd += originalName;
            cmd += newName;
            cmd += direction;
            if (gateName) {
                cmd += gateName;
            }
            
            cmd();
            
            // If we got here it worked:
            
            return PyUnicode_FromString(newName);
            
        }
        catch (CException& e) {
            PyErr_SetString(PyExc_RuntimeError,  e.ReasonText());
            return NULL;
        }
        catch (std::string msg) {
            PyErr_SetString(PyExc_RuntimeError, msg.c_str());
            return NULL;
        }
        catch (const char* msg) {
            PyErr_SetString(PyExc_RuntimeError, msg);
            return NULL;
        }
        catch (...) {
            PyErr_SetString(
                PyExc_RuntimeError, "Unanticipated exception type caught"
            );
            return NULL;
        }
        
    } else {
        return NULL;
    }
}
//////////////////////////////////////////////////////////////////////////
// Attribute access. Note that the attributes are actually in the
// CSpectrum object that we're going to look up at each turn.

/**
 * getName
 *    @param self - pointer to our object data.
 *    @param closure - A value in the getter setter table entry for
 *                     this method.
 *    @return PyObject*  - unicode name.
 *    @note if the spectrum no longer exists we raise a runtime rror.
 */
static PyObject*
getName(PyObject* self, void* closure)
{
    CSpectrum* pSpectrum = getSpectrum(self);
    if (pSpectrum) {
        return PyUnicode_FromString(pSpectrum->getName().c_str());
    } else {
        return NULL;
    }
}
/**
 * getType
 *    @param self - pointer to our object data.
 *    @param closure - additional data from the dispatch table.
 *    @return PyObjectd*  unicode stringified Spectrum type.
 */
static PyObject*
getType(PyObject* self, void* closure)
{
    CSpectrum* pSpectrum = getSpectrum(self);
    if (pSpectrum) {
        SpectrumType_t eType = pSpectrum->getSpectrumType();
        std::stringstream s;
        s << eType;
        return PyUnicode_FromString(s.str().c_str());
    } else {
        return NULL;
    }
}
/**
 * getParams
 *   @param self - pointer to the object data.
 *   @param closure - from the dispatch table.
 *   @return PyObject*  - For all but keG2DD spectra, this is a single tuple
 *                      containing the parameters for the spectrum. For
 *                      keG2DD spectra, this is a  pair of tuples, the first
 *                      parameters on the x the second parameters on the y
 *  @note - when I say parameters - I mean parameter names.
 *  @note - if a parameter has been deleted (not likely) it's name will be
 *          "--Deleted--"  I believe this matches what spectrum -list does.
 */
static PyObject*
getParams(PyObject* self, void* closure)
{
    CSpectrum* pSpectrum = getSpectrum(self);
    SpecTcl* pApi = SpecTcl::getInstance();
    if (pSpectrum) {
        if (pSpectrum->getSpectrumType() != keG2DD) {
            // Build a vector of parameter names:
            
            std::vector<std::string> names;
            std::vector<UInt_t> ids;
            pSpectrum->GetParameterIds(ids);
            for (int i =0; i < ids.size(); i++) {
                CParameter* pDef = pApi->FindParameter(ids[i]);
                if (pDef) {
                    names.push_back(pDef->getName());
                } else {
                    names.push_back("--Deleted--");
                }
            }
            return stringVecToTuple(names);
            
        } else {
            CGamma2DDL* pG = reinterpret_cast<CGamma2DDL*>(pSpectrum);
            std::vector<CParameter> xpars = pG->getXParameters();
            std::vector<CParameter> ypars = pG->getYParameters();
            
            std::vector<std::string> xNames;
            std::vector<std::string> yNames;
            for (int i =0; i < xpars.size(); i++) {
                xNames.push_back(xpars[i].getName());
            }
            for (int i = 0; i < ypars.size(); i++) {
                yNames. push_back(ypars[i].getName());
            }
            
            PyObject* x = stringVecToTuple(xNames);
            PyObject* y = stringVecToTuple(yNames);
            
            PyObject* result = PyTuple_New(2);
            PyTuple_SetItem(result, 0, x);
            PyTuple_SetItem(result, 1, y);
            
            return result;
        }
    } else {
        return NULL;
    }
}

/**
 *  getAxes
 *    @param self - pointer to our object data.
 *    @param closure - data from the dispatch table.
 *    @return PyObject* - Tuple of dictionaries that contain the axis definitions.
 *                       each axis definition contains the keys
 *                       - low - axis low limit
 *                       - high - axis high limit.
 *                       - bins - number of bins on the axis.
 */
static PyObject*
getAxes(PyObject* self, void* closure)
{
    CSpectrum* pSpectrum = getSpectrum(self);
    if (pSpectrum) {
        CSpectrum::Axes defs = pSpectrum->getAxisMaps();
        PyObject* result = PyTuple_New(defs.size());
        
        for (int i = 0; i < defs.size(); i++) {
            PyTuple_SetItem(result, i, axisToDict(defs[i]));
        }
        
        return result;
    } else {
        return NULL;
    }
}
/**
 * getGate
 *   @param self - pointer to the object storage.
 *   @param closure - unused.
 *   @return PyObject - unicode name of the gate applied to the spectrum.
 */
static PyObject*
getGate(PyObject* self, void* closure)
{
    CSpectrum* pSpectrum = getSpectrum(self);
    if (pSpectrum) {
        const CGateContainer* g = pSpectrum->getGate();
        std::string name = g->getName();    // The container holds the name.
        return PyUnicode_FromString(name.c_str());
    } else {
        return NULL;
    }
}
/**
 * getRangeErrors
 *    (range_errors) gets the range error counters.  This is a dict
 *    that contains the keys
 *    - overflows  -number of overflows.
 *    - underflows -number of underflows.
 *
 *    Each value in the dictionary is a tuple.  The tuple has one elemement
 *    for 1d spectra and 2 (x, y in that order) for 2d spectra.
 *
 * @param self  - Pointer to my object data.
 * @param closure - unused.
 * @return PyObject* that is a dictionary - see above.
 */
static PyObject*
getRangeErrors(PyObject* self, void* closure)
{
    CSpectrum* pSpectrum = getSpectrum(self);
    if (pSpectrum) {
        std::vector<unsigned> unders  = pSpectrum->getUnderflows();
        std::vector<unsigned> overs   = pSpectrum->getOverflows();
        
        // Make under/overflow tuples  we don't assume the sizes are the same.
        
        PyObject* underTuple = PyTuple_New(unders.size());
        for (int i =0; i < unders.size(); i++) {
            PyTuple_SetItem(underTuple, i, PyLong_FromUnsignedLong(unders[i]));
        }
        
        PyObject* overTuple = PyTuple_New(overs.size());
        for (int i = 0; i < overs.size(); i++) {
            PyTuple_SetItem(overTuple, i, PyLong_FromUnsignedLong(overs[i]));
        }
        
        // Now create and set the dictionary:
        
        PyObject* result = PyDict_New();
        PyDict_SetItemString(result, "underflows", underTuple);
        PyDict_SetItemString(result, "overflows",  overTuple);
        
        return result;
        
    } else {
        return NULL;
    }
}
//////////////////////////////////////////////////////////////////////////
// Tables of methods and getters/setters (we don't actually store
// data in the object but look it up each time)

// Object methods:

static PyMethodDef spectrumMethods[] = {
    {"clear", (PyCFunction)(clear), METH_NOARGS, "Clear  spectrum"},
    {"ungate", (PyCFunction)(ungate), METH_NOARGS, "Remove conditions on a spectrum"},
    {"gate", (PyCFunction)(gate), METH_VARARGS, "Apply a gate to a spectrum"},
    {"bind", (PyCFunction)(bind), METH_NOARGS, "Bind spectrum to displayer"},
    {"unbind", (PyCFunction)(unbind), METH_NOARGS, "Unbind spectrum from displayer"},
    {"get",    (PyCFunction)(Get),   METH_VARARGS, "Get channel value from spectrum"},
    {"set",    (PyCFunction)(Set),   METH_VARARGS, "Set channel value in a spectrum."},
    {"project", (PyCFunction)(project), METH_VARARGS | METH_KEYWORDS, "Create a projection spectrum"},
    
    // End sentinell
    
    {NULL, NULL, 0, NULL}
};

// Getters for the object attributes:

static PyGetSetDef gettersAndSetters[] = {
    {"name", (getter)getName, NULL, "Spectrum name"},
    {"type", (getter)getType, NULL, "Spectrum type string"},
    {"parameters", (getter)getParams, NULL, "Spectrum parameter names"},
    {"axes", (getter)getAxes, NULL, "Spectrum axis definitions"},
    {"gatename", (getter)getGate, NULL, "Name of gate"},
    {"range_errors", (getter)getRangeErrors, NULL, "Range errors" },
    {NULL}
};

// Object type

static PyTypeObject SpectrumType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "spectcl.spectcrum",             /* tp_name */
    sizeof(SpectrumObject),             /* tp_basicsize */
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
    "SpecTcl Spectra",           /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    spectrumMethods,             /* tp_methods */
    0,                         /* tp_members */
    gettersAndSetters,           /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)init,      /* tp_init */
    0,                         /* tp_alloc */
    ObjNew,                 /* tp_new */
};



/**
 * setupPythonSpectrumObjects
 *    Setup specrum type in spectcl module
 *  @param module - pointer to the module type:
 */
void setupPythonSpectrumObjects(PyObject* module)
{
    PyType_Ready(&SpectrumType);
    Py_INCREF(&SpectrumType);
    
    PyModule_AddObject(module, "spectrum", (PyObject*)(&SpectrumType));
}