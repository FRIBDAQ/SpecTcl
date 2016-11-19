#ifndef __CFLATTENEDGATELIST_H
#define __CFLATTENEDGATELIST_H
/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/
#ifndef __DICTIONARY_H
#include <Dictionary.h>
#endif


#ifndef __HISTOGRAMMER_H
#include <Histogrammer.h>
#endif

#ifndef __DYNAMICLIST_H
#include "DynamicList.h"
#endif

class CHistogrammer;


/*!
  The Flattened Gate list class is a gate observer that
  maintains a flat list of gates (rather than the
  tree used for dictionary lookups.  The list is maintained
  in a DynamicList<CGateContainer*>  This list can be
  fetched by a histogrammer, and is maintained by the observer 
  callback members so that it is gaurenteed to be correct.

  The only gates stored here are gates that can cache, as the
  list is intended to be used by the historammer to clear the cached
  gates prior to processing events.

*/
class CFlattenedGateList : public CGateObserver
{
  // Types:
private:


  typedef DynamicList<CGateContainer>  GateList;

  // Member data:

private:
  GateList        m_Gates;
  CHistogrammer*  m_pHistogrammer;
public:
  CFlattenedGateList(CHistogrammer* histogrammer);
  virtual ~CFlattenedGateList();

  CFlattenedGateList(const CFlattenedGateList& rhs);
  CFlattenedGateList& operator=(const CFlattenedGateList& rhs);

  // comparisons are meaningless..

private:
  int operator==(const CFlattenedGateList& rhs) const;
  int operator!=(const CFlattenedGateList& rhs) const;

  // MemberFunctions:
public:
  void clearCache();
  CGateContainer** getList();

  // The gate observer interface:
public:
  virtual void onAdd(std::string name, CGateContainer& container);
  virtual void onRemove(std::string name, CGateContainer& item);
  virtual void onChange(std::string name, CGateContainer& item);

};



#endif
