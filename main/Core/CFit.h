// Author:
//   Ron Fox
//   NSCL
//   Michigan State University
//   East Lansing, MI 48824-1321
//   mailto:fox@nscl.msu.edu
//
/*!
  \class CFit

  Abstract base class defining the interface for fits.

*/

//
// Include files:
//

#ifndef __CFIT_H  //Required for current class
#define __CFIT_H

#ifndef __NAMEDITEM_H
#include <NamedItem.h>
#endif

#ifndef __STL_VECTOR
#include <vector>
#ifndef __STL_VECTOR
#define __STL_VECTOR
#endif
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

#ifndef __STL_UTILITY
#include <utility>
#ifndef __STL_UTILITY
#define __STL_UTILITY
#endif
#endif



class CFit : public  CNamedItem     
{
  // Public data types.
public:
  struct Point {		//!< A point for the fit.

    double x;
    double y;

    int operator==(const Point& rhs) const {
      return (x == rhs.x) && (y == rhs.y);
    }
    int operator!=(const Point& rhs) const {
      return !(operator==(rhs));
    }
  };
  typedef std::vector<Point> PointArray; //!< Point array is a std::vector of points.
  typedef PointArray::iterator PointIterator;

  typedef std::pair<std::string, double> FitParameter;
  typedef std::vector<FitParameter> FitParameterList;
  typedef FitParameterList::iterator FitParameterIterator;

  typedef enum {
    Accepting,
    Performed
  } FitState;			//!< State of the fit.
private:
  
  // Private Member data:

  std::vector<Point> m_afpPoints;  //!<  //<! Input points.  
  FitState m_eState;  //!<  //!< accumulating | performed  
   


public:
    //  Constructors and other canonical operations.
    //  You may need to adjust the parameters
    //  and the visibility esp. if you cannot
    // implement assignment/copy construction.
    // safely.
  CFit (std::string name, int id = 0);		//!< Constructor.
  virtual  ~ CFit ( ); //!< Destructor.
  CFit (const CFit& rSource ); //!< Copy construction.
  CFit& operator= (const CFit& rhs); //!< Assignment.
  int operator== (const CFit& rhs) const; //!< == comparison.
  int operator!= (const CFit& rhs) const { //!< != comparison.
    return !(operator==(rhs));
  }
    

// Insert any mutators you wish to export here.
// visibility should be protected.

protected:
  void SetFitState(FitState state) {	//!< set the state of the fit.
    m_eState = state;
  }

// Class operations:

public:
  
  

  void AddPoint (Point p)   ; 
  void ClearPoints();
  PointIterator begin ()   ; 
  PointIterator end ()   ; 
  size_t size ()   ; 
  FitState GetState ()  const ; 

 
  
 
  // Pure virtual functions must be implemented by
  // subclasses.

  virtual CFit* clone() = 0;
  virtual   void Perform ()   = 0 ; 
  virtual  double operator() (double x)  =0 ; 
  virtual  FitParameterList GetParameters () =0  ; 
  virtual  std::string Type() const  = 0;
  virtual  std::string makeTclFitScript() = 0;
 

private:
  void CopyIn(const CFit& rhs);	//!< Isolate copy in operation.

};

#endif
