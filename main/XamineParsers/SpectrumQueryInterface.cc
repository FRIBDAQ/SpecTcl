
#include <SpectrumQueryInterface.h>

namespace Win {

  SpectrumQueryInterface::SpectrumQueryInterface() : m_pQuerier() {}

  void SpectrumQueryInterface::setQueryEntity(std::shared_ptr<SpectrumQuerier> pQueryObj)
  {
    m_pQuerier = pQueryObj;
  }

} // end win Namespace
