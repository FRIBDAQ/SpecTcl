
 
// Class: C2Bands                     //ANSI C++
//
//   // Models a contour which is formed from 2 bands.  The idea is
// that in many detector models, particle id is available in a de/e
// spectrum as hyperbollic bands of counts.  Placing a bunch of 
// hyperbollic band lines allows separation by treating the area
// between adjacent bands as 'in a contour gate formed from these
// bands.'
//  This gate therefore acts just like a contour *however*:
//  1.  The point list is initialized from two bands, the 'upper'
//       and 'lower' band.
//  2.  The Y coordinates in the 'upper' band are decreased by
//       1 in order to make the gate exclusive of points on the
//       upper band to prevent double counting if these are
//       stacked.
//
//   The point list is built up in a clockwise manner in that the
//    we start with the first point of the upper band and continue
//    with the last point of the lower band going in reverse order.
//  
//   A crude attempt is made to ensure that this clockwise ordering is correct:
//   If the X coordinate of each band's first point is less than that of its
//   last point, then the band points are reversed.
//
// Author:
//     Ron Fox
//     NSCL
//     Michigan State University
//     East Lansing, MI 48824-1321
//     mailto: fox@nscl.msu.edu
// 
// (c) Copyright NSCL 1999, All rights reserved .h
//

#ifndef __2BANDS_H  //Required for current class
#define __2BANDS_H

                               //Required for base classes
#ifndef __CONTOUR_H
#include "Contour.h"
#endif

#ifndef __BAND_H
#include "Band.h"
#endif
          
#ifndef __STL_STRING                     
#include <string>        //Required for include files
#define __STL_STRING
#endif

#ifndef __STL_VECTOR
#include <vector>
#define __STL_VECTOR
#endif
                               
class C2Bands  : public CContour        
{                       
			
protected:

public:

   //Default constructor alternative to compiler provided default constructor

  C2Bands (UInt_t nXId, UInt_t nYId,
	   vector<CPoint>& rLowBand, 
	   vector<CPoint>& rHiBand); 

  virtual  ~ C2Bands ( ) { }  //Destructor 

   //Copy constructor alternative to compiler provided default copy constructor

  C2Bands (const C2Bands& aC2Bands )   : CContour (aC2Bands) 
  { 
  }                                     

   // Operator= Assignment Operator alternative to compiler provided 
   // default operator= If base class make virtual
   //Assignment alternatives for association objects: 
   //  (1) initialize association object to nullAssociation Object
   //  (2) Shallow copy to copy pointers to association objects 
   //  (3) Deep copy to create new association objects
   //      and copy values of association objects

  C2Bands& operator= (const C2Bands& aC2Bands) {
    if(&aC2Bands != this)
      CContour::operator=(aC2Bands);

    return *this;
  }
 
   //Operator== 
   //    Illegal.
private:
  int operator== (const C2Bands& aC2Bands) const;
public:

  // Selectors.

public:

 virtual   std::string Type ()    ;
 
protected:

  std::vector<CPoint> MarshallPoints(vector<CPoint>& rLow,
				    vector<CPoint>& rHi);

};

#endif
