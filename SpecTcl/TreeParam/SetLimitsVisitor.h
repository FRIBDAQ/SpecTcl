///////////////////////////////////////////////////////////
//  SetLimitsVisitor.h
//  Implementation of the Class SetLimitsVisitor
//  Created on:      30-Mar-2005 11:03:54 AM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////

#if !defined(__SETLIMITSVISITOR_H)
#define __SETLIMITSVISITOR_H


#include "CMatchingVisitor.h"

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

// Forward Defs:

class CTreeParameter;

/**
 * Sets the upper/lower limits on a set of parameters that match a pattern.
 * @author Ron Fox
 * @version 1.0
 * @created 30-Mar-2005 11:03:54 AM
 */
class SetLimitsVisitor : public CMatchingVisitor
{
  
private:
  /**
   * New low limit
   */
  double m_fLow;
  /**
   * new high limit
   */
  double m_fHigh;

public:

  virtual ~SetLimitsVisitor();
  SetLimitsVisitor(STD(string) pattern, double low, double high);
  virtual void OnMatch(CTreeParameter* parameter);
  
};




#endif
