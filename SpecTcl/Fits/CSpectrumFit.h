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

#ifndef __CSPECTRUMFIT_H
#define __CSpECTRUMFIT_H


#ifndef __NAMEDITEM_H
#include <NamedItem.h>		// base class
#endif

#ifndef __CFIT_H
#include "CFit.h"		// Need this for CFitParameterList.
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif



// Forward class definitions:

class CSpectrum;

/*!
  This class defines a fit on a region of interest of a spectrum
  The spectrum must be a 1-d spectrum.
  The idea is that you can create one of these on a spectrum,
  it will take care of loading the spectrum channels/counts into
  the fit and perform the fit.  The fit can be updated (which  means
  clearing and reloading the points).
  - Item name is the spectrum name on which this is defined.
  - Id will be a unique id in the namespace of gate ids. if used in SpecTcl.

*/
class CSpectrumFit : public CNamedItem
{
  // Member data:
private:
  int     m_low;		// Channel coordinates of left side of ROI.
  int     m_high;		// Channel coordinates of right side of ROI.
  bool    m_ownFit;		// true if we dynamically created the fit.
  CFit*   m_pFit;		// Pointer to the fit object.

  // Canonicals.. note that since fits can clone, we can support copy construction.

public:
  CSpectrumFit(STD(string) spectrumName, int myId,
	       int low, int high, 
	       CFit&    fit);
  CSpectrumFit(const CSpectrumFit& rhs);
  virtual ~CSpectrumFit();
  CSpectrumFit& operator=(const CSpectrumFit& rhs);
  int operator==(const CSpectrumFit& rhs) const;
  int operator!=(const CSpectrumFit& rhs) const;

  // Delegations to the fit:

  CFit::PointIterator begin();
  CFit::PointIterator end();
  size_t              size();
  CFit::FitState      GetState(); // although this should always be CFit::Performed.
  double              operator()(double x);
  CFit::FitParameterList getParameters();
  STD(string)         fitType();
  STD(string)         makeTclFitScript();
  STD(string)         fitName();

  // Functions that are implemented (not as delegations to m_pFit) in this
  // class:

  CSpectrumFit*       clone();	// Clone self.
  void                update();

  int                 low() const;
  int                 high() const;

  // Private utilities.
private:
  CSpectrum* findSpectrum();

  
};

#endif
