#ifndef __CSPECTRUMBYPARAMETER_H
#define __CSPECTRUMBYPARAMETER_H


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
#ifndef __HISTOGRAMMER_H
#include <Histogrammer.h>
#endif

#ifndef __DYNAMICLIST_H
#include "DynamicList.h"
#endif

#ifndef __STL_VECTOR
#include <vector>
#ifndef __STL_VECTOR
#define __STL_VECTOR
#endif
#endif

// Forward type/class definitions:


class CSpectrum;
class CEvent;

/*!
  This class is the heart of SpecTcl histogramming.
  It observes the spectrum dictionary and stores it organized
  in a way that is more efficiently histogrammed in the 
  presence of sparse events. 

  There are basically two types of spectra.  Those that have a 
  requirement for their parameters, and those that work on a soup
  of parameters..where none are actually required.

  Those that require their parameters (in normal analysis the bulk of
  the spectra), are organized in lists that are indexed by one of the
  parameters they require.  The rest are stored in a separate
  miscellaneous list.

  When an event arrives, the parameters that are present are used
  to traverse the lists of spectra that require them and then, finally,
  the miscellaneous list is traversed.  Thus if the number of spectra
  that don't need to be incremented are large, it is possible to avoid
  the overhead of checking the presence of spectra whose parameters are just
  absent in the parameter list.

*/
class CSpectrumByParameter : public SpectrumDictionaryObserver
{
private:
  typedef DynamicList<CSpectrum> SpectrumList;
private:
  std::vector<SpectrumList*>      m_spectraByParameter;
  SpectrumList                    m_remainingSpectra;

  // Canonicals:

public:
  CSpectrumByParameter();
  virtual ~CSpectrumByParameter();
  CSpectrumByParameter(const CSpectrumByParameter& rhs);
  CSpectrumByParameter& operator=(const CSpectrumByParameter& rhs);

  // Comparisons just don't make sense:

private:
  int operator==(const CSpectrumByParameter& rhs) const;
  int operator!=(const CSpectrumByParameter& rhs) const;

  // operations on the objects:


public:
  // Get lists.

  CSpectrum** getSpectrumList(int parameterId); //!< List for parameterId.
  CSpectrum** getSpectrumList();                //!< List for misc. spectra.

  void operator()(const CEvent& event);	        //!< Increment the event.

  // The interface for observers:

  virtual void onAdd(std::string name, CSpectrum*& spectrum);
  virtual void onRemove(std::string name, CSpectrum*& spectrum);


  // utilities:

private:
  void cloneIn(const CSpectrumByParameter& rhs);
  void addToParameter(CSpectrum* spectrum, unsigned int parameterId);
  static void incrementList(const CEvent& event, CSpectrum** spectra);
  
};

#endif
