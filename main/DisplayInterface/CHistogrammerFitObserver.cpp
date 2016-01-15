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
#include "CHistogrammerFitObserver.h"
#include "Display.h"
#include <CSpectrumFit.h>


///////////////////////////////////////////////////////////////////////////
/////////////////////// Implementation of canonicals. /////////////////////
///////////////////////////////////////////////////////////////////////////

/*!
   Construct a histogrammer fit observer.
   We're going to help insulate the histogrammer from the fit dictionary
   by automatically registering ourselves with the dictionary (similarly
   destruction can unregister.).
*/
CHistogrammerFitObserver::CHistogrammerFitObserver(CDisplay& pDisplay) :
  m_pDisplay(&pDisplay)
{
}
/*!
  Destroy  ourselves... we automatically unregister us from the 
  dictionary. Note that we really don't anticipate this will be called
  as our observation is likely to last the lifetime of the program.
*/
CHistogrammerFitObserver::~CHistogrammerFitObserver()
{
  CFitDictionary& Dict(CFitDictionary::getInstance());
  Dict.removeObserver(*this);
}

/////////////////////////////////////////////////////////////////////////
///////////////// Implementation of observation functions ///////////////
/////////////////////////////////////////////////////////////////////////

/*!
   Add  : called when a new fit is added to the fit dicationary.
   We will really delegate this to the histogrammer's addFit function.
   The purpose of the observer is to loosen the coupling between fit
   actions and the histogrammer.
   \param fit : CSpectrumFit&
      The new fit being added to the dictionary.
*/
void
CHistogrammerFitObserver::Add(CSpectrumFit& fit)
{
  m_pDisplay->addFit(fit);
}

/*!
 Delete : Called when an existing fit is being deleted from the fit dictionary.
 This is delegated to the Histogrammer's deleteFit function, which is supposed
 to remove the fit from appropriate Xamine spectrum slots.
 \param fit : CSpectrumFit&
    The fit being deleted.
*/
void
CHistogrammerFitObserver::Delete(CSpectrumFit& fit)
{
  m_pDisplay->deleteFit(fit);
}
/*!
  Update : Called when an existing fit has been asked to update itself based on 
  the new values of the underlying spetrum (after increments e.g.).
  From the point of view of the histogrammer, this is a delete followed by
  a re-add of the updated fit.
  \param fit : CSpectrumFit&
      The fit being updated.
*/
void
CHistogrammerFitObserver::Update(CSpectrumFit& fit)
{
  Delete(fit);
  Add(fit);
}
