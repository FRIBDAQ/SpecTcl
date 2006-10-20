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

#ifndef __CFITDICTIONARY_H
#define __CFITDICTIONARY_H

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_SRING
#endif
#endif


#ifndef __STL_MAP
#include <map>
#ifndef __STL_MAP
#define __STL_MAP
#endif
#endif


#ifndef __STL_LIST
#include <list>
#ifndef __STL_LIST
#define __STL_LIST
#endif
#endif

#ifndef __CRT_UNISTD_H
#include <unistd.h>
#ifndef __CRT_UNISTD_H
#define __CRT_UNISTD_H
#endif
#endif

class CSpectrumFit;


/*!
   This class keeps track of all the fits that have been defined.
   This is a singleton class.
*/
class CFitDictionary {
  // Data Types:
public:
  typedef STD(map)<STD(string), CSpectrumFit*> FitMap;
  typedef FitMap::iterator      iterator;


  // classes derived from this can be used to monitor
  // adds and deletions to the dictionary.

  class CObserver {
  public:
    virtual void Add(CSpectrumFit& fit) =0;
    virtual void Delete(CSpectrumFit& fit) = 0;
    virtual void Update(CSpectrumFit& fit) = 0;

  };

  typedef STD(list)<CObserver*> ObserverList;

  // Data:
private:
  static CFitDictionary* m_pTheInstance;
  FitMap                 m_Fits;
  ObserverList           m_Observers;

  // canonicals are all private because we can only get one of us:

private:
  CFitDictionary();
  CFitDictionary(const CFitDictionary& rhs);
  virtual ~CFitDictionary();

  CFitDictionary& operator=(const CFitDictionary& rhs);
  int operator==(const CFitDictionary& rhs) const;
  int operator!=(const CFitDictionary& rhs) const;

public:
  static CFitDictionary& getInstance();

  // Operations on the dictionary;

public:
  void add(CSpectrumFit& fit);
  void addOrReplace(CSpectrumFit& fit);
  void Delete(STD(string) name);

  // iteration etc. through the fit dictionary:

  iterator begin();
  iterator end();
  size_t   size();
  iterator find(STD(string) name);
  void     erase(iterator here);
  void     erase(iterator first, iterator last);

  void updateFits(STD(string) name=STD(string)("*")); 


  // functions operating on the observers:

  void addObserver(CObserver& obs);
  void removeObserver(CObserver& obs);
protected:
  void observeAdd(CSpectrumFit& fit);
  void observeDelete(CSpectrumFit& fit);
  void observeUpdate(CSpectrumFit& fit);
public:
 
};

#endif
