   

//! \class: CCalibratedParameterCommand           
//! \file:  .h
/*!
  \class CCalibratedParameterCommand
  \file  .h

  This file provides a command set for manipulating calibrated
  parameters.  A summary of the syntax supported is:
  
  calibparam name number raw fitname ?units?  - Creates a calibration
  calibparam -list ?pattern?   - Lists the patterns
  calibparam -delete name    - Destroys a calibration.
  calibparam -refresh ?pattern? - refreshes the fits of a parameter.
  
  

*/

// Author:
//   Ron Fox
//   NSCL
//   Michigan State University
//   East Lansing, MI 48824-1321
//   mailto:fox@nscl.msu.edu
//
// Copyright 

#ifndef __CCALIBRATEDPARAMETERCOMMAND_H  //Required for current class
#define __CCALIBRATEDPARAMETERCOMMAND_H

//
// Include files:
//
#ifndef __TCLPROCESSOR_H
#include <TCLProcessor.h>
#endif


#ifndef __CCALIBRATEDPARAMETERMANAGER_H
#include <CCalibratedParameterManager.h>
#endif

#ifndef __CFITFACTORY_H
#include <CFitFactory.h>
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif


// Forward definitions:

class CTCLInterpreter;
class CTCLResult;
class CHistogrammer;
class CCalibratedParameter;

class CCalibratedParameterCommand : public CTCLProcessor     
{
private:
	CHistogrammer* m_pHistogrammer;
public:
    //  Constructors and other canonical operations.


    CCalibratedParameterCommand (CTCLInterpreter* pInterp);		//!< Constructor.
    virtual  ~ CCalibratedParameterCommand ( ); //!< Destructor.

	// Copy like operations are not supported and therefore comparison
	// does not make sense.
private:
    CCalibratedParameterCommand (const CCalibratedParameterCommand& rSource ); //!< Copy construction.
    CCalibratedParameterCommand& operator= (const CCalibratedParameterCommand& rhs); //!< Assignment.
    int operator== (const CCalibratedParameterCommand& rhs) const; //!< == comparison.
    int operator!= (const CCalibratedParameterCommand& rhs) const; //!< != comparison.


// Class operations:

public:

	virtual   int operator() (CTCLInterpreter& rInterp, CTCLResult& rResult, 
  							   int argc, char** argv)   ; 
    int Create_parse (CTCLInterpreter& rInterp, CTCLResult& rResult, 
     				  int argc, char** argv)   ; 
	int List_parse (CTCLInterpreter& rInterp, CTCLResult& rResult, 
				    int  argc, char** argv)   ; 
	virtual   int Delete_parse (CTCLInterpreter& rInterp, CTCLResult& rResult, 
							    int argc, char** argv)   ; 
     int Update_parse (CTCLInterpreter& rInterp, CTCLResult& rResult, 
     				   int argc, char** argv)   ; 
     int Create (STD(string)& Result, STD(string) name, int id, STD(string) raw, 
     		     STD(string) fitname, STD(string) units)   ; 
     STD(string) List (STD(string) pattern)   ; 
     int Delete (STD(string)& Result, STD(string) parameter)   ; 
     void Update (STD(string) pattern)   ; 
     CFit* FindFit (STD(string) sName)   ; 
     STD(string) Usage ()   ; 

	// Utilities available to derived and helper classes:

    static STD(string) FormatParameter (STD(string)  name, CCalibratedParameter& rParam); 


};

#endif
