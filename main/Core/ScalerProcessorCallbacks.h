
/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

*/
/*
  Author:
      Ron Fox
      NSCL
      Michigan State University
      East Lansing, MI 48824-1321
*/


#ifndef __SCALERPROCESSORCALLBACKS_H
#define __SCALERPROCESSORCALLBACKS_H
#include <CBufferCallback.h>


// Forward definitions:

class CTCLInterpreter;		// TCL Interpreter encapsulated as an object.
class CBufferProcessor;		// The buffer processor.

/*!
    The callback class below should be created and registered
    for begin, pause, end, and resume run buffers.  It is legitimate
    to create only  a single object and register this on all four of 
    those types.

    Once registered, this class will interact with a TCL interpreter in 
    a manner identical to what the sclclient does to a tcl server
    for control buffers.  This is used within SpecTcl to communicate
    scaler data out to the user etc.

    \note  The implementation of this class assumes an NSCL buffer structure.
           This is implied in two ways:
	   - This is a callback for  the libBufferAnalysis library which 
	     assumes an NSCL buffer structure.
           - The analysis performed within the implementation assumes an NSCL
	     buffer structure.

   
*/
class CSpecTclControlBufferCallback : public CBufferCallback
{
private:
  CBufferProcessor*  m_pProcessor;    //!< The processor that invokes us.
  CTCLInterpreter*   m_pInterpreter;  //!< The TCL interpreter we interact with.
public:
  // Constructors and other canonical operations:

  CSpecTclControlBufferCallback(CBufferProcessor* pProcessor,
				CTCLInterpreter*  pInterpreter);
  virtual ~CSpecTclControlBufferCallback();

  CSpecTclControlBufferCallback(const CSpecTclControlBufferCallback& rhs);
  CSpecTclControlBufferCallback& operator=(const CSpecTclControlBufferCallback& rhs);
  int operator==(const CSpecTclControlBufferCallback& rhs) const;
  int operator!=(const CSpecTclControlBufferCallback& rhs) const;

  // class overrides:

  virtual void operator()(unsigned int nType, const void* pBuffer);

};

///////////////////////////////////////////////////////////////

/*!  
   The callback class below should be created and registered for scaler buffers.
   Once registered it will mimic the operation of the sclclient on a tclserver for
   scaler buffers.  This is the mechanism that SpecTcl uses to communicate
   scaler information to the user.

   \note   The implementation of this class assumes an NSCL buffer structure.
           This is implied in two ways:
	   - This is a callback for  the libBufferAnalysis library which 
	     assumes an NSCL buffer structure.
           - The analysis performed within the implementation assumes an NSCL
	     buffer structure.
*/
class CSpecTclScalerBufferCallback : public CBufferCallback
{
private:
  CBufferProcessor* m_pProcessor;   //!< The buffer processor that invokes us.
  CTCLInterpreter*  m_pInterpreter; //!< The TCL Interpreter we interact with.
public:
  // Constructors and other canonical operations.

  CSpecTclScalerBufferCallback(CBufferProcessor* pProcesor,
			       CTCLInterpreter*  pInterpreter);
  virtual ~CSpecTclScalerBufferCallback();


  CSpecTclScalerBufferCallback(const CSpecTclScalerBufferCallback& rhs);
  CSpecTclScalerBufferCallback& operator=(const CSpecTclScalerBufferCallback& rhs);
  int operator==(const CSpecTclScalerBufferCallback& rhs) const;
  int operator!=(const CSpecTclScalerBufferCallback& rhs) const;

  // class overrides.

  virtual void operator()(unsigned int nType, const void* pBuffer); 
 

};

#endif
