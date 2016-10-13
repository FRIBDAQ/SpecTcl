
#ifndef SPECTRASPECTRUMINTERFACE_H
#define SPECTRASPECTRUMINTERFACE_H

#include <SpectrumQueryInterface.h>
#include <memory>

namespace Viewer {

class SpecTclInterface;

class SpectraSpectrumInterface : public Win::SpectrumQuerier
{
    
public:
    SpectraSpectrumInterface(std::shared_ptr<SpecTclInterface> pSpecTcl);
    virtual Win::SpectrumQueryResults getSpectrumInfo(int slot);
    virtual Win::SpectrumQueryResults getSpectrumInfo(const std::string& name);
    
private:
    std::shared_ptr<SpecTclInterface> m_pSpecTcl;

};

}
#endif
