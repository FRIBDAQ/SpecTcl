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

#include <config.h>
#include "CFitDictionary.h"
#include "CSpectrumFit.h"
#include "DictionaryException.h"
#include "CFitFactory.h"
#include "CLinearFitCreator.h"
#include <tcl.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

// Static member data:

CFitDictionary*   m_pTheInstance(0);



/*!
   Construct the fit dictionary... This registers the 'known' 
   fit creators.

*/
CFitDictionary::CFitDictionary()
{
  CFitFactory::AddFitType("linear", new CLinearFitCreator);
}
CFitDictionary::~CFitDictionary() 
{}

/*!
   Get the singleton instance:
*/
CFitDictionary&
CFitDictionary::getInstance()
{
  if (!m_pTheInstance) {
    m_pTheInstance = new CFitDictionary;
  }
  return *m_pTheInstance;
}

/*!

   Add a fit to the dictionary.. if the fit exists throw a duplicate
   key exception.
   \param fit : CSpectrumFit&
      Reference to the fit to add.
*/
void
CFitDictionary::add(CSpectrumFit& fit)
{
  if (find(fit.fitName()) != end()) {
    // Duplicate:

    throw CDictionaryException(static_cast<UInt_t>(CDictionaryException::knDuplicateKey),
			       "Adding a spectrum fit to the fit dictionary",
			       fit.fitName());
  }
  addOrReplace(fit);
}
/*!
   Add or replace a fit... that is we don't check for duplication.. just
   overwrite an existing entry.
   \param fit : CSpectrumFit& 
      reference to the fit.
*/
void
CFitDictionary::addOrReplace(CSpectrumFit& fit)
{
  m_Fits[fit.fitName()] = &fit;
}

/*!
   Delete a fit from the dictionary.
   If the fit cannot be found a dictionary exception is thrown.
   If the fit is found, any observers will hve their Delete member
   invoked.
   \param name : std::string
      Name of the fit to delete (not the spectrum).
*/
void
CFitDictionary::Delete(string name)
{
  iterator i = find(name);
  if (i == end()) {
    throw CDictionaryException(static_cast<UInt_t>(CDictionaryException::knNoSuchKey),
			       "Deleting a spectrum from the fit dictionary",
			       name);
  }
  erase(i);

  
}
/*!
  Returns a begin of iteration iterator tothe fit map.
  Note that itertors point to a pair<std::string, CSpectrumFit*>.
  Thus fit names are iterator->first, and the fit itself is iterator->second
*/
CFitDictionary::iterator
CFitDictionary::begin()
{
  return m_Fits.begin();
}
/*!
  Returns an end of iteration iterator.
  This iterator should never be dereferned as it is 'points' off the end
  of the map.   It should only be compared to other iterators to
  know when it is time to stop iterating.
*/
CFitDictionary::iterator
CFitDictionary::end()
{
  return m_Fits.end();
}
/*!
  Returns the number of elements in the fit map.
*/
size_t
CFitDictionary::size()
{
  return m_Fits.size();
}
/*!
   Attempt to locate a fit in the map by name.
   \param name : std::string
      Name of the fit we are searching for.
   \return CFitDictionary::iterator
   \retval end() the fit does not exist.
   \retval other - an iterator that 'points' to the pair<std::string CSpectrumFit*>
           that matches.
*/
CFitDictionary::iterator
CFitDictionary::find(string name)
{
  return m_Fits.find(name);
}
/*!
  Removes a single item from the map.  
  Observers Delete member will be called.
  \param here : CFitDictionary::iterator
    Iterator pointing to the element to delete.
*/
void
CFitDictionary::erase(CFitDictionary::iterator here)
{
  observeDelete(*(here->second));
  m_Fits.erase(here);
}
/*!
  Same as above but for a range of iterators.
  observers will be triggered for all elements in the range.
  The range deleted is [first,last)... so that end() can be specified.
*/
void
CFitDictionary::erase(CFitDictionary::iterator first,
		      CFitDictionary::iterator last)
{
  // Since deletion invalidates iterators. we must first
  // trigger all observers, then and only then we can delete
  // the range... its' not ok just to iterate through single erases.

  for (CFitDictionary::iterator i = first; i != last; i++) {
    observeDelete(*(i->second));
  }
  // Remove the elements.

  m_Fits.erase(first,last);
}
//////////////////////////////////////////////////////////////////////////
/////////////////////////// Observer functions ///////////////////////////
//////////////////////////////////////////////////////////////////////////

/*!
   Add an observer to the list of objects monitoring our operations.
   observers are objects that come from classes derived from
   CFitDictionary::CObserver  When registered, they are called when
   interesting things happen inside the dictionary. 
   - Add is called whenever a fit is added to the dictionary.
   - Delete is called whenever a fit is about to be removed from the dictionary.
   - Update is called whenever a fit has been asked to update itself.

   \param obs : CFitDictionary::CObserver&
      Reference to an observer to add to the observation list.
*/
void
CFitDictionary::addObserver(CFitDictionary::CObserver& obs)
{
  m_Observers.push_back(&obs);
}
/*!
   Remove an observer from the list. The address of the observer passed
   in is used to find a match.  At this time it is a no-op to attempt to
   delete an observer that does not exist.  This is mostly laziness but
   justified by my belief that the set of observers is likely to be quite
   static.
   \param obs  : CFitDictionary::CObserver&
      Reference to the observer to remove.
*/
void
CFitDictionary::removeObserver(CFitDictionary::CObserver& obs)
{
  ObserverList::iterator i = m_Observers.begin();
  while (i != m_Observers.end()) {
    if (*i == &obs) {
      m_Observers.erase(i);
      return;
    }
    i++;
  }
}
/*!
   Trigger the Add member of all observers.
*/
void
CFitDictionary::observeAdd(CSpectrumFit& fit)
{
  ObserverList::iterator i = m_Observers.begin();
  while (i != m_Observers.end()) {
    CObserver* p = *i;
    p->Add(fit);
    i++;
  }
}
/*!
   Triger the delete member of all observers.
*/
void
CFitDictionary::observeDelete(CSpectrumFit& fit)
{
  ObserverList::iterator i=  m_Observers.begin();
  while (i != m_Observers.end()) {
    CObserver* p = *i;
    p->Delete(fit);
    i++;
  }
}
/*!  

   Trigger update notification.
*/
void
CFitDictionary::observeUpdate(CSpectrumFit& fit)
{
  ObserverList::iterator i = m_Observers.begin();
  while (i != m_Observers.end()) {
    CObserver* p = *i;
    p->Update(fit);
    i++;
  }
}
/*!
    Update all the fits that match a specific name string. Observers
    Update member is triggered.

    \param pattern : std::string ["*"]
      The pattern to match. If omitted, this defaults to * which matches
      all fits.

*/
void
CFitDictionary::updateFits(string pattern)
{
  CFitDictionary::iterator i = begin();
  while (i != end()) {
    if (Tcl_StringMatch(i->first.c_str(), pattern.c_str())) {
      i->second->update();
      observeUpdate(*(i->second));
    }
    i++;
  }
}
