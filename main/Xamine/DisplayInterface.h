/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2015.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Jeromy Tompkins
         NSCL
         Michigan State University
         East Lansing, MI 48824-1321
*/


#ifndef DISPLAYINTERFACE_H
#define DISPLAYINTERFACE_H

#include <daqdatatypes.h>
#include <histotypes.h>
#include <xamineDataTypes.h>

#include <string>
#include <vector>
#include <utility>
#include <list>

class CHistogrammer;
class CDisplay;
class CSpectrum;
class CSpectrumFit;
class CDisplayGate;
class CGateContainer;


class CDisplayInterface
{

private:
    CHistogrammer*      m_pSorter;
    CDisplay*           m_pDisplay;

public:
    CDisplayInterface();
    CDisplayInterface(const CDisplayInterface&);
    virtual ~CDisplayInterface();


    CHistogrammer* getHistogrammer() {
        return m_pSorter;
    }

    void setHistogrammer(CHistogrammer* pSorter) {
        m_pSorter = pSorter;
    }

    CDisplay *getDisplay() const;
    void setDisplay(CDisplay *pDisplay);


};

#endif // DISPLAYINTERFACE_H
