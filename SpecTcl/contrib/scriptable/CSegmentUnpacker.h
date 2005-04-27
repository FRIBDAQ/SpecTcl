/*!
  \class CSegmentUnpacker
  \file  .h

  This is an Abstract Base Class that is the base class
  of all objects that can unpack event chunks.  
  We define:
  - An interface for the unpacking functionality.
  - Management of our 'owner'.
  All segment unpackers except a top-level 
  unpacking object are owned by some CPacket
  object. The functions that manage this ownership
  are:
  - Own - Specifies who owns the unpacker.
  - Disown - Specifies the unpacker is no longer owned.
  - isOwned - Query if the unpacker is owned already.
  - OnDelete does whatever is needed to the 
    our owner.

*/

// Author:
//   Ron Fox
//   NSCL
//   Michigan State University
//   East Lansing, MI 48824-1321
//   mailto:fox@nscl.msu.edu
//
// Copyright 

#ifndef __CSEGMENTUNPACKER_H  //Required for current class
#define __CSEGMENTUNPACKER_H

//
// Include files:
//

                               //Required for base classes
#ifndef __CCONFIGURABLEOBJECT_H     //CConfigurableObject
#include "CConfigurableObject.h"
#endif
 

#ifndef __TRANSLATORPOINTER_H
#include <TranslatorPointer.h>
#endif


#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif
                               //Required for 1:1 association classes

// Forward class definitions (convert to includes if required):

class CPacket;
class CEvent;
class CAnalyzer;
class CBufferDecoder;
class CHistogrammer;
class CTCLInterpreter;

class CSegmentUnpacker  : public CConfigurableObject        
{
private:
  

  CPacket* m_pOwner;  //!  Our owner if not null.  
   
public:
    //  Constructors and other canonical operations.

  CSegmentUnpacker (const STD(string)& rName,
		    CTCLInterpreter& rInterp);		//!< Constructor.
  ~ CSegmentUnpacker ( ); //!< Destructor.

  // Copy construction of objects derived from command
  // processors is not allowed.  This means us too:
private:
  CSegmentUnpacker (const CSegmentUnpacker& rSource ); //!< Copy construction.
  CSegmentUnpacker& operator= (const CSegmentUnpacker& rhs); //!< Assignment.
  int operator== (const CSegmentUnpacker& rhs) const; //!< == comparison.
  int operator!= (const CSegmentUnpacker& rhs) const; //!< != comparison.

public:

  // Virtual members (interface specifications, default implementations or
  // base class overrides).

  //! Recognize and unpack.
  virtual   TranslatorPointer<UShort_t> Unpack (TranslatorPointer<UShort_t> pEvent, 
						CEvent& rEvent, 
						CAnalyzer& rAnalyzer, 
						CBufferDecoder& rDecoder)   = 0 ; 
                                    
  virtual   void Setup (CAnalyzer& 			rAnalyzer, 
			CHistogrammer& rHistogrammer)   = 0 ; //!< Setup config
  virtual STD(string) getType() const = 0;   //!< Return the module type.

  virtual   void OnDelete ()   ;        //!< About to be destroyed.

  // Functions that are not virtual.

  void Own (CPacket* pNewOwner)   ; //!< Specify our owner.
  void Disown ()   ; 		    //!< Remove ownership. 
  bool isOwned ()  const ;	    //!< true if owned
  CPacket* getOwner ()   ; 	    //!< Return owner. 
protected:

private:
  
};

#endif
