//  CEvent.h:
//
//    This file defines the CEvent class.
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:fox@nscl.msu.edu
//
//  Copyright 1999 NSCL, All Rights Reserved.
//
/////////////////////////////////////////////////////////////

#ifndef __EVENT_H  //Required for current class
#define __EVENT_H

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif


#ifndef __STL_VECTOR
#include <vector>
#define __STL_VECTOR
#endif

#ifndef __VALIDVALUE_H
#include <ValidValue.h>
#endif

typedef std::vector<CValidValue<Int_t> >        CIntegerVector;
typedef CValidValue<Int_t>                     CParameterValue;
typedef CIntegerVector::iterator               CEventIterator;
                               
class CEvent      
{
  
private:
  CIntegerVector m_rvParameters;   // The data parameters.
  Int_t          m_nSize;	// Current vector size (optimization).
public:

	// Canonical functions, construction, destruction,
	// assignment and comparison.

			//Ensure initial values entered
  CEvent ();
  virtual~ CEvent ( ) { } //Destructor

			//Constructor with arguments
  CEvent(UInt_t nInitialSize);       
 	
			//Copy constructor

  CEvent (const CEvent& aEvent );
 
			//Operator= Assignment Operator

  CEvent& operator= (const CEvent& aEvent);

			//Operator== Equality Operator

  int operator== (const CEvent& aEvent);
  int operator!= (const CEvent& anEvent) {
	return !(operator==(anEvent));
  }
 
  //  Exported operations:

  //    Int_t& operator[] (UInt_t nParam)  ;
    CParameterValue& operator[](UInt_t nParam);
  
  CEventIterator begin ();
  CEventIterator end ();
  UInt_t size ()  const {
    return m_nSize;
  }
  void clear () {
    CEventIterator e = end();
    for(CEventIterator i = begin(); i != e; i++) {
      i->clear();
    }
    
  }
 
  // Utility protected functions:

protected:
  void DoAssign(const CEvent& anEvent);
  void Resize(UInt_t nElements);        // Resizes to hold at least nElements
  void ExpandGranule();		        // Expands vector by knExpandSize.
};

#endif
