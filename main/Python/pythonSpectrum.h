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

/** @file:  pythonSpectra.h
 *  @brief: spectcl.spoectrum type.
 */
#ifndef PYTHONSPECTRA_H
#define PYTHONSPECTRA_H
#define PY_SSIZE_T_CLEAN         // from the examples ...
#include <Python.h>
#ifdef HAVE_STAT                // dueling features.
#undef HAVE_STAT
#endif

#include <string>

extern void setupPythonSpectrumObjects(PyObject* module);

// typedefs that define the extensions to the python type struct for
// spectrum:


/**
 * @note Spectra can be ephemeral.  Therefore we need to deal with
 *       the possibility that a spectrum that our type represents
 *       has been destroyed out from underneath us.  We do
 *       this by holding the spectrum name and, yes, looking it up
 *       each time we need it. Not terribly fast but necessary.
 *     @note the spectrum name must be a pointer to std::string since the
 *           python allocation does not do a new and therefore does not
 *           properly constructor destruct the string.  We have to take care
 *           of that our self in type handling code.
 */

class CSpectrum;
typedef struct _SpectrumData {
    std::string* s_spectrumName;  
} SpectrumData, *pSpectrumData;

#endif