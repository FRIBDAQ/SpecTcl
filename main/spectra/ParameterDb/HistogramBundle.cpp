#include "HistogramBundle.h"

namespace Viewer
{
namespace RootEmbed
{

HistogramBundle::HistogramBundle(const ::Viewer::HistogramBundle &histBundle)
    : m_pHist(histBundle.getHist()),
      m_cuts1d(),
      m_cuts2d(),
      m_histInfo(histBundle.)
{
}

} // end RootEmbed namespace
} // end Viewer
