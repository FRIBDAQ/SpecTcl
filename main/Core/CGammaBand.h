///////////////////////////////////////////////////////////
//  CGammaBand.h
//  Implementation of the Class CGammaBand
//  Created on:      22-Apr-2005 12:54:24 PM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////

#ifndef CGAMMABAND_H
#define CGAMMABAND_H

#include "Band.h"
#include "Point.h"
#include <histotypes.h>
#include <string>
#include <vector>


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
  typedef std::vector<UInt_t> ParameterList;
  typedef ParameterList::iterator parameterIterator;
private:
  /**
   * List of parameter ids  of the parameters to create pairs for checking from.
   */
  ParameterList m_Parameters;
  
public:
  CGammaBand();
  virtual ~CGammaBand();
  CGammaBand(const std::vector<FPoint>& rPoints, const ParameterList& rParameters);
  CGammaBand(const CGammaBand& rhs);
  CGammaBand& operator=(const CGammaBand& rhs);
  int operator==(const CGammaBand& rhs) const;
  int operator!=(const CGammaBand& rhs) const;
  ParameterList getParameters() const;
  CGate* clone();
  std::string Type() const;
  virtual Bool_t inGate(CEvent& rEvent);
  CGammaBand::parameterIterator parametersBegin();
  CGammaBand::parameterIterator parametersEnd();
  size_t parameterSize();
  

};


#endif 
