/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


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


/*
  Change Log:
  $Log$
  Revision 5.3  2005/12/19 16:28:09  ron-fox
  Remove illegal default parameter from CPointlistGate::InGate in the header`

  Revision 5.2  2005/06/03 15:19:20  ron-fox
  Part of breaking off /merging branch to start 3.1 development

  Revision 5.1.2.3  2005/05/27 17:47:37  ron-fox
  Re-do of Gamma gates also merged with Tim's prior changes with respect to
  glob patterns.  Gamma gates:
  - Now have true/false values and can therefore be applied to spectra or
    take part in compound gates.
  - Folds are added (fold command); and these perform the prior function
      of gamma gates.

  Revision 5.1.2.1  2004/12/21 17:51:22  ron-fox
  Port to gcc 3.x compilers.

  Revision 5.1  2004/11/29 16:56:03  ron-fox
  Begin port to 3.x compilers calling this 3.0

  Revision 4.5  2003/08/25 16:25:32  ron-fox
  Initial starting point for merge with filtering -- this probably does not
  generate a goo spectcl build.

  Revision 4.4  2003/04/15 19:15:42  ron-fox
  To support real valued parameters, primitive gates must be internally stored as real valued coordinate pairs.

*/
#ifndef POINTLISTGATE_H  //Required for current class
#define POINTLISTGATE_H
                               //Required for base classes
#include "Gate.h"
#include "Point.h"
#include <histotypes.h>
#include <vector>
#include <string>


/*!
   This class is a base class for gates that consist of a list
   of points.  Examples are bands and countours.  The boundaries
   of each of these gates can be expressed as an ordered set of 
   points.
   There's an explicit assumption that point list gates are 2-d 
   gates.
*/
class CPointListGate  : public CGate        
{
  UInt_t m_nxId;  //!< Parameter Id of the gate's X parameter.
  UInt_t m_nyId;  //!< Parameter id of the Y parameter of the gate.
  
  std::vector<FPoint> m_aConstituents; //!< The points themselves.
  
public:

  //!< Constructor with std::vector of points.
  CPointListGate (UInt_t nXId, UInt_t nYId,
		  const std::vector<FPoint>& Points) :
    m_nxId(nXId),
    m_nyId(nYId),
    m_aConstituents(Points)
  {}
  //!< Constructor with arrays of coordinates.
  CPointListGate(UInt_t nXId, UInt_t nYId, UInt_t nPts,
		 Float_t *xCoords, Float_t *yCoords);
  //!< Constructor with array of Points.
  CPointListGate(UInt_t nXId, UInt_t nYId, UInt_t nPts,
		 FPoint* pPoints);
  virtual ~ CPointListGate ( ) { }       //Destructor

	
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

  //!< Equality comparison

  int operator== (const CPointListGate& aCPointListGate) const;
  // Selectors.

public:
  UInt_t getxId() const
  {
    return m_nxId;
  }

  UInt_t getyId() const
  {
    return m_nyId;
  }
       
  std::vector<FPoint> getPoints() const
  {
    return m_aConstituents;
  }

  std::vector<FPoint>::iterator getBegin() {
    return m_aConstituents.begin();
  }
  std::vector<FPoint>::iterator getEnd() {
    return m_aConstituents.end();
  }

  virtual std::vector<std::string> getSpecs() const { 
    std::vector<std::string> empty;
    return empty;
  };

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
  void setPoints(std::vector<FPoint>& rPoints) 
  {
    m_aConstituents = rPoints;
  }

  //  Operations on the class.

public:                       
  virtual   CConstituentIterator Begin ()  ;
  virtual   CConstituentIterator End ()  ;
  virtual   UInt_t Size ()  ;
  virtual   std::string GetConstituent (CConstituentIterator& rIterator)  ;
  virtual  Bool_t inGate(CEvent& rEvent);
  virtual   Bool_t inGate(CEvent& rEvent, const std::vector<UInt_t>& Params) 
    {return inGate(rEvent); }
  
  virtual  Bool_t Inside(Float_t x, Float_t y) = 0;
protected:
  int       Crosses(Float_t x, Float_t y, 
		    std::vector<FPoint>::iterator f,
		    std::vector<FPoint>::iterator s); 

};

#endif
