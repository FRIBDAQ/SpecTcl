#ifndef VIEWER_ROOTEMBED_HISTOGRAMBUNDLE_H
#define VIEWER_ROOTEMBED_HISTOGRAMBUNDLE_H

#include "HistInfo.h"

#include <TObject.h>
#include <TCutG.h>
#include <TCut.h>

#include <map>
#include <string>

class TH1;

namespace Viewer
{

class HistogramBundle;

namespace RootEmbed
{

class HistogramBundle : public TObject
{
private:
    TH1*                          m_pHist;
    std::map<std::string, TCut>   m_cuts1d;
    std::map<std::string, TCutG>  m_cuts2d;
    SpJs::HistInfo                m_histInfo;

public:
    HistogramBundle(const ::Viewer::HistogramBundle& histBundle);


    ClassDef(Viewer::RootEmbed::HistogramBundle, 1);

};

} // end RootEmbed namespace
} // end Viewer namespace

#endif // HISTOGRAMBUNDLE_H
