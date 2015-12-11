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

#ifndef __CHISTOGRAMMERFITOBSERVER_H
#define __CHISTOGRAMMERFITOBSERVER_H


#ifndef __CFITDICTIONARY_H
#include <CFitDictionary.h>
#endif


class CSpectrumFit;
class CDisplay;

/*!
   The CHistogrammerFitObserver class is an observer that the
   CHistogrammer class/object registers to monitor changes in the
   fit dictionary.  As this class is a subclass of CFitDictionary::CObserver,
   the key members called are:
   - Add  - Called when a new fit is added to the dictionary.
   - Delete - Called when an existing fit is about to be deleted from the dictionary
   - Update - Called when  a fit has been asked to update its parameters based
              on the current spectrum values.

   CHistgrammer uses this to update fits on spectra bound to Xamine.
   Ensuring the appropriate fits are visible on Xamine's bound spectra.
*/
class CHistogrammerFitObserver : public CFitDictionary::CObserver
{
  CDisplay*  m_pDisplay; // We are monitoring on behalf of this.
public:
  CHistogrammerFitObserver(CDisplay &histogrammer);
  virtual ~CHistogrammerFitObserver();
  // 
  // Since observers only have purpose when registered on the fit dictionary,
  // no purpose is served by allowing the other canonicals:
  //
private:
  CHistogrammerFitObserver(const CHistogrammerFitObserver& rhs);
  CHistogrammerFitObserver& operator=(const CHistogrammerFitObserver& rhs);
  int operator==(const CHistogrammerFitObserver& rhs) const;
  int operator!=(const CHistogrammerFitObserver& rhs) const;
public:

  // These are the functions that implement the observation and are called
  // by the fit dictionary's observer support functions:
  //

  virtual void Add(CSpectrumFit& fit);
  virtual void Delete(CSpectrumFit& fit);
  virtual void Update(CSpectrumFit& fit);

};
   

#endif
