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

#ifndef __CRATELIST_H
#define __CRATELIST_H


#ifndef __STL_LIST
#include <list>
#ifndef __STL_LIST
#define __STL_LIST
#endif
#endif


// forward types:

class CRateProcessor;


/*!
   This class is a singleton that contains all of the
   rates objects.  There are really only a few things you can
   do to this list:
   - Add a rates object.
   - Remove a rates object.
   - Iterate through the rates objects (begin, end).
   - Mark an object for later deletion.
   - Get the singleton instance of this class.

   Rates objects would like to delete themselves however
   self deletion in C++ is risky at best.  Therefore,
   The rates list stores, along with each rate object pointer,
   a flag that can be set to indicate the object should at some time
   be deleted.  Some other external agency is then responsible for
   checking those flags and deleting the ones which require deletion.
*/
class CRateList {
  // Public data types:
public:
  typedef STD(pair)<CRateProcessor*, bool> Entry;
  typedef STD(list)<Entry>                 RateList;
  typedef RateList::iterator               RateListIterator;

  // data members:

private:
  RateList           m_rates;
  static CRateList*  m_pInstance;

  // The constructors are all private to control construction
  // (singleton pattern).  The rest of the canonicals are not needed.

private:
  CRateList();
  ~CRateList();
  CRateList(const CRateList& rhs);
  CRateList& operator=(const CRateList& rhs);
  int operator==(const CRateList& rhs) const;
  int operator!=(const CRateList& rhs) const;

  // Acquire the singleton instance:

public:
  static CRateList*   getInstance();

  // Operations on the list:

  void addProcessor(CRateProcessor& processor);
  void deleteProcessor(CRateProcessor& processor);
  void markForDeletion(CRateProcessor& processor);

  // List iteration:

  RateListIterator find(CRateProcessor& processor);
  RateListIterator begin();
  RateListIterator end();


};

#endif
