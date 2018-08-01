/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


// Header files:

#ifndef FILTEREVENTPROCESSOR_H
#define FILTEREVENTPROCESSOR_H

#include <EventProcessor.h>
#include <histotypes.h>
#include <vector>
#include <string>

// Forward Definitions:

class CAnalyzer;
class CBufferDecoder;
class CEvent;

/*!
  This event processor is responsible for decoding filtered event
  data.  For the format of these events, see CFilterBufferDecoder.
  We need to handle two calls:
  - OnOther - builds a mapping array between filter parameter ids.
              and parameters that are defined in spectcl.
  - operator() - Extracts filtered parameters and stores them in the
                 event array.

*/
class CFilterEventProcessor : public CEventProcessor
{
private:
  // Member data:

  std::vector<Int_t>  m_ParameterMap; //!< Maps Filter parameters -> spectcl.
public:
  // Constructors and other cannonical operations.

  CFilterEventProcessor();	//!< Default constructor.
  CFilterEventProcessor(const CFilterEventProcessor& rhs); //!< Copy constructor
  CFilterEventProcessor& operator=(const CFilterEventProcessor& rhs);
  int operator==(const CFilterEventProcessor& rhs);
  int operator!=(const CFilterEventProcessor& rhs) {
    return !operator==(rhs);
  }

  // Member functions:

public:
  //!  Called for parameter description records.
  virtual Bool_t OnOther(UInt_t nType,
			 CAnalyzer& rAnalyzer,
			 CBufferDecoder& rDecoder);

  //! Called for each physics event.
  virtual Bool_t operator()(const Address_t pEvent,
			    CEvent& rEvent,
			    CAnalyzer& rAnalyzer,
			    CBufferDecoder& rDecoder); 

  // Utility functions:

    virtual Bool_t OnEventSourceOpen(std::string name);
  
public:				// These may be generally useful:
  static void* GetString(void* p, 
			 std::string& result); //!< retrieve string.
  static void* GetInt(void* p,
		      Int_t&  i);         //!< Retrieve an int.
  static void* GetFloat(void* p,
			DFloat_t& f);      //!<  Retrieve a float.

};

#endif
