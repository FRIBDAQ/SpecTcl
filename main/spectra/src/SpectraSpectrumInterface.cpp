
#include <SpectraSpectrumInterface.h>

#include <SpecTclInterface.h>
#include <HistogramList.h>
#include <HistogramBundle.h>

#include <TH2.h>
#include <QString>

#include <stdexcept>

namespace Viewer {

  SpectraSpectrumInterface::SpectraSpectrumInterface(std::shared_ptr<SpecTclInterface> pSpecTcl)
      : m_pSpecTcl(pSpecTcl)
  {}

  Win::SpectrumQueryResults SpectraSpectrumInterface::getSpectrumInfo(int slot)
  {
      throw std::runtime_error("SpectraSpectrumInterface::getSpectrumInfo(int) Spectrum lookup by slot number not supported");
  }

  Win::SpectrumQueryResults SpectraSpectrumInterface::getSpectrumInfo(const std::string& name) {

    HistogramList* pList = m_pSpecTcl->getHistogramList();
    if (pList == nullptr) {
        std::string msg("SpectraSpectrumInterface::getSpectrumInfo(int) ");
        msg += "SpecTclInterface returned null instead of HistogramList";
        throw std::runtime_error(msg);
    }

    HistogramBundle* pBundle = pList->getHist(QString::fromStdString(name));

    Win::SpectrumQueryResults results;
    if (pBundle) {
        TH1& hist = pBundle->getHist();
      results.s_exists = true;
      results.s_dimension = hist.InheritsFrom(TH2::Class()) ? 2 : 1;
      results.s_nBinsX = hist.GetNbinsX();
      results.s_nBinsY = hist.GetNbinsY();
      results.s_name = name;
    } else {
      results.s_exists = false;
    }

    return results;
  }

}
