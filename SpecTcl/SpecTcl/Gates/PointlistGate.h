//  CPointListGate.h:
//
//    This file defines the CPointListGate class.
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

#ifndef __POINTLISTGATE_H  //Required for current class
#define __POINTLISTGATE_H
                               //Required for base classes
#ifndef __GATE_H
#include "Gate.h"
#endif                               
                               
                               //Required for 1:M associated classes
#ifndef __POINT_H
#include "Point.h"
#endif                                                               

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

#ifndef __STL_VECTOR
#include <vector>
#define __STL_VECTOR
#endif

#ifndef __STRING
#include <string>
#define __STRING
#endif

class CPointListGate  : public CGate        
{
  UInt_t m_nxId;  // Parameter Id of the gate's X parameter.
  UInt_t m_nyId;  // // Parameter id of the Y parameter of the gate.
  
  vector<CPoint> m_aConstituents;
  
public:
			// Constructor

  CPointListGate (UInt_t nXId, UInt_t nYId,
		  const vector<CPoint>& Points) :
    m_nxId(nXId),
    m_nyId(nYId),
    m_aConstituents(Points)
  {}
  CPointListGate(UInt_t nXId, UInt_t nYId, UInt_t nPts,
		 UInt_t *xCoords, UInt_t *yCoords);
  CPointListGate(UInt_t nXId, UInt_t nYId, UInt_t nPts,
		 CPoint* pPoints);
  ~ CPointListGate ( ) { }       //Destructor

	
			//Copy constructor

  CPointListGate (const CPointListGate& aCPointListGate )   : 
    CGate (aCPointListGate) 
  {   
    m_nxId          = aCPointListGate.m_nxId;
    m_nyId          = aCPointListGate.m_nyId;
    m_aConstituents = aCPointListGate.m_aConstituents;
                
  }                                     

			//Operator= Assignment Operator

  CPointListGate& operator= (const CPointListGate& aCPointListGate)
  { 
    if (this == &aCPointListGate) return *this;          

    CGate::operator= (aCPointListGate);
    m_nxId          = aCPointListGate.m_nxId;
    m_nyId          = aCPointListGate.m_nyId;
    m_aConstituents = aCPointListGate.m_aConstituents;
    return *this;                                                                                                 
  }                                     

			//Operator== Equality Operator
                        // Gates don't have equality relationships.
private:
  int operator== (const CPointListGate& aCPointListGate);
public:
  // Selectors.

public:
                       //Get accessor function for attribute
  UInt_t getxId() const
  {
    return m_nxId;
  }

                       //Get accessor function for attribute
  UInt_t getyId() const
  {
    return m_nyId;
  }
       
  vector<CPoint> getPoints() const
  {
    return m_aConstituents;
  }

  vector<CPoint>::iterator getBegin() {
    return m_aConstituents.begin();
  }
  vector<CPoint>::iterator getEnd() {
    return m_aConstituents.end();
  }

  virtual vector<string> getSpecs() const { };

  // Mutators:

protected:
  void setxId (UInt_t am_nxId)
  { 
    m_nxId = am_nxId;
  }

  void setyId (UInt_t am_nyId)
  { 
    m_nyId = am_nyId;
  }
  void setPoints(vector<CPoint>& rPoints) 
  {
    m_aConstituents = rPoints;
  }

  //  Operations on the class.

public:                       
  virtual   CConstituentIterator Begin ()  ;
  virtual   CConstituentIterator End ()  ;
  virtual   UInt_t Size ()  ;
  virtual   std::string GetConstituent (CConstituentIterator& rIterator)  ;
  virtual   Bool_t inGate(CEvent& rEvent, vector<UInt_t>& Params) { }
};

#endif
