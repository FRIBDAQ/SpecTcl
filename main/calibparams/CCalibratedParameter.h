

// Author:
//   Ron Fox
//   NSCL
//   Michigan State University
//   East Lansing, MI 48824-1321
//   mailto:fox@nscl.msu.edu
//
// Copyright 
//! \class: CCalibratedParameter           
//! \file:  .h
/*!
  \class CCalibratedParameter

  A calibrated parameter is a parameter that is 
  related to a raw parameter via  a fit.  For each
  event, the calibrated parameter is computed by
  evaluating the calibrated parameter's fit with respect 
  to a raw parameter. 

*/


#ifndef CCALIBRATEDPARAMETER_H  //Required for current class
#define CCALIBRATEDPARAMETER_H

//
// Include files:
//

#include <string>        //Required for include files 
#include <histotypes.h>        //Required for include files  


// Forward class definitions (convert to includes if required):

class CCalibFit;
class CEvent;

// The class interface

class CCalibratedParameter      
{
private:
  
  // Private Member data:
    int    m_nParameterId;        //!<  Id of parameter raw parameter we calibrate..  
    int    m_nTargetParameterId;  //!<  Id of resulting parameter.  
    STD(string) m_sFitName;            //!<  Name of fit that's set on us.  
   
    CCalibFit*  m_pFit;                //!< 1:1 association object data member 

public:
    //  Constructors and other canonical operations.
    CCalibratedParameter (int nTargetId,
    					  int nRawId,
    					  STD(string) sFitName,
    					  CCalibFit*  pFit);		//!< Constructor.
    virtual  ~ CCalibratedParameter ( ); //!< Destructor.
    CCalibratedParameter (const CCalibratedParameter& rSource ); //!< Copy construction.
    CCalibratedParameter& 
               operator= (const CCalibratedParameter& rhs); //!< Assignment.
    int operator== (const CCalibratedParameter& rhs) const; //!< == comparison.
    int operator!= (const CCalibratedParameter& rhs) const; //!< != comparison.



public:
	int getTargetId() const {
		return m_nTargetParameterId;
	}
	int getRawId() const {
		return m_nParameterId;
	}
	STD(string) getFitName() const {
		return m_sFitName;
	}
	const CCalibFit* getFit() const {
		return m_pFit;
	}

public:

	void operator() (CEvent& rEvent) const   ; //!< Evaluate the calibration
	CCalibFit* ReplaceFit (CCalibFit& rFit)   ;          //!< Set a new fit (name is fixed).
};

#endif
