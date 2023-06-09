///////////////////////////////////////////////////////////
//  CGammaCut.h
//  Implementation of the Class CGammaCut
//  Created on:      22-Apr-2005 12:54:51 PM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////

#ifndef CGAMMACUT_H
#define CGAMMACUT_H

#include "Cut.h"
#include "Gate.h"
#include <histotypes.h>
#include <vector>
#include <string>

/// Forward deefinitions.

class CEvent;


/**
 * This is a new gamma cut that is true whenever any single parameter is inside
 * the gamma gate.   Gates accepted from Xamine on gamma spectra will be gamma
 * gates.
 * @created 22-Apr-2005 12:54:51 PM
 * @author Ron Fox
 * @version 1.0
 * @updated 22-Apr-2005 02:40:43 PM
 */
class CGammaCut : public CCut
{
public:
  typedef std::vector<UInt_t> ParameterList;
  typedef ParameterList::iterator parameterIterator;
  
private:
  /**
   * Vector of parameter id's that are checked against the slice defined by m_nLow,
   * m_nHigh in the parent class.
   */
  ParameterList m_Parameters;
  
public:
  CGammaCut();
  virtual ~CGammaCut();
  CGammaCut(Float_t low, Float_t high, const std::vector<UInt_t> rParameters);
  CGammaCut(const CGammaCut& rhs);
  const CGammaCut& operator=(const CGammaCut& rhs);
  int operator==(const CGammaCut& rhs) const;
  int operator!=(const CGammaCut& rhs) const;
  std::vector<UInt_t> getParameters() const;
  virtual Bool_t inGate(CEvent& rEvent);
  virtual Bool_t inGate(Float_t x) {
    return CCut::inGate(x);
  }
  virtual CGate* clone();
  virtual std::string Type() const;
  CGammaCut::parameterIterator parameterBegin();
  CGammaCut::parameterIterator parameterEnd();
  size_t parameterSize();
  ParameterList getParameters() {
    return m_Parameters;
  }
};


#endif 
