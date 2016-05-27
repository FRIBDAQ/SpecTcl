
#ifndef SPECTRUMQUERY_H
#define SPECTRUMQUERY_H

#include <memory>
#include <string>
#include <stdexcept>

namespace Win {

struct SpectrumQueryResults
{
  bool s_exists;
  int  s_dimension;
  int  s_nBinsX;
  int  s_nBinsY;
  std::string  s_name;
};

class SpectrumQuerier {

  public:
    virtual SpectrumQueryResults getSpectrumInfo(int slot) = 0;
    virtual SpectrumQueryResults getSpectrumInfo(const std::string& slot) = 0;
};

class SpectrumQueryInterface {
  private:
    std::shared_ptr<SpectrumQuerier> m_pQuerier;

  public:
    SpectrumQueryInterface();

    void setQueryEntity(std::shared_ptr<SpectrumQuerier> pQueryObj);

    template<class T>
    SpectrumQueryResults getSpectrumInfo(T specId) {
      if (!m_pQuerier) {
        std::string msg ("SpectrumQueryInterface::getSpectrumInfo() ");
        msg += "Cannot query spectrum if no query object exits.";
        throw std::runtime_error(msg);
      }

      return m_pQuerier->getSpectrumInfo(specId);

    }

};

} // end Win namespace

#endif 
