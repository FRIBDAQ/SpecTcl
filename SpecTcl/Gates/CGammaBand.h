///////////////////////////////////////////////////////////
//  CGammaBand.h
//  Implementation of the Class CGammaBand
//  Created on:      22-Apr-2005 12:54:24 PM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////

#if !defined(__CGAMMABAND_H)
#define __CGAMMABAND_H

#ifndef __BAND_H
#include "Band.h"
#endif

#ifndef _POINT_H
#include "Point.h"
#endif

#ifdef __HISTOTYPES_H
#include <histotypes.h>
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

#ifndef __STL_VECTOR
#include <vector>
#ifndef __STL_VECTOR
#endif
#endif


// Forward definitions:

class CGate;
class CEvent;


/**
 * Implements a gamma band gate.  A gamma band tries all ordered pairs of
 * parameters against the band.  If any pair is in the gate, the gate is true.
 * @created 22-Apr-2005 12:54:24 PM
 * @author Ron Fox
 * @version 1.0
 * @updated 22-Apr-2005 02:40:24 PM
 */
class CGammaBand : public CBand
{
public:
  typedef STD(vector)<UInt_t> ParameterList;
  typedef ParameterList::iterator parameterIterator;
private:
  /**
   * List of parameter ids  of the parameters to create pairs for checking from.
   */
  ParameterList m_Parameters;
  
public:
  CGammaBand();
  virtual ~CGammaBand();
  CGammaBand(const STD(vector)<FPoint>& rPoints, const ParameterList& rParameters);
  CGammaBand(const CGammaBand& rhs);
  CGammaBand& operator=(const CGammaBand& rhs);
  int operator==(const CGammaBand& rhs) const;
  int operator!=(const CGammaBand& rhs) const;
  ParameterList getParameters() const;
  CGate* clone();
  STD(string) Type() const;
  virtual Bool_t inGate(CEvent& rEvent);
  CGammaBand::parameterIterator parametersBegin();
  CGammaBand::parameterIterator parametersEnd();
  size_t parameterSize();
  

};


#endif 
