   

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

#ifndef CCALIBRATEDPARAMETERCOMMAND_H  //Required for current class
#define CCALIBRATEDPARAMETERCOMMAND_H

//
// Include files:
//
#include <TCLProcessor.h>
#include <CCalibratedParameterManager.h>
#include <CFitFactory.h>
#include <string>


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
     int Create (std::string& Result, std::string name, int id, std::string raw, 
     		     std::string fitname, std::string units)   ; 
     std::string List (std::string pattern)   ; 
     int Delete (std::string& Result, std::string parameter)   ; 
     void Update (std::string pattern)   ; 
     CFit* FindFit (std::string sName)   ; 
     std::string Usage ()   ; 

	// Utilities available to derived and helper classes:

    static std::string FormatParameter (std::string  name, CCalibratedParameter& rParam); 


};

#endif
