///////////////////////////////////////////////////////////
//  CGammaContour.h
//  Implementation of the Class CGammaContour
//  Created on:      22-Apr-2005 12:54:38 PM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////

#ifndef CGAMMACONTOUR_H
#define CGAMMACONTOUR_H

#include "Contour.h"
#include "Point.h"
#include <histotypes.h>
#include <string>
#include <vector>


// Forward definitions:

class CGate;
class CEvent;

/**
 * A gamma contour is a contour with more than 2 parameters.  Each ordered pair of
 * parameters is passed to the gate evaluation function.  If any pair is true, the
 * gate is true.
 * @created 22-Apr-2005 12:54:38 PM
 * @author Ron Fox
 * @version 1.0
 * @updated 22-Apr-2005 02:40:37 PM
 */
class CGammaContour : public CContour
{
public:
  typedef std::vector<UInt_t> ParameterList;
  typedef ParameterList::iterator parameterIterator;
private:
  /**
   * The ids of the parameter ids that are defined on the spectrum.
   */
  ParameterList m_Parameters;
public:
  CGammaContour();
  virtual ~CGammaContour();
  CGammaContour(const std::vector<FPoint>& points, const ParameterList& parameters);
  CGammaContour(const CGammaContour& rhs);
  CGammaContour& operator=(const CGammaContour& rhs);
  int operator==(const CGammaContour& rhs) const;
  int operator!=(const CGammaContour& rhs) const;
  ParameterList getParameters() const;
  virtual CGate* clone();
  std::string Type() const;
  virtual Bool_t inGate(CEvent& rEvent);
  bool UsesParameter(UInt_t nParam);
  CGammaContour::parameterIterator parametersBegin();
  CGammaContour::parameterIterator parametersEnd();
  size_t parameterSize();
  
  
};


#endif
