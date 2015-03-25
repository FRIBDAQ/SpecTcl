#include "GateList.h"
#include <algorithm>
#include <TCutG.h>

using namespace std;

GateList::GateList()
    : m_gates()
{
}


void GateList::addGate(std::unique_ptr<TCutG> cut)
{
    // the unique ptr give pretty strong assurance that there
    // is no copy in the list already (it is still possible but if
    // we managed to do that, we have major issues)
    m_gates.push_back(std::move(cut));
}

GateList::iterator GateList::getGate(const QString &name)
{
    return find_if(m_gates.begin(),
                   m_gates.end(),
                   [&](const unique_ptr<TCutG>& cut) -> bool {
                      QString cname = cut->GetName();
                      return (cname==name);
                   });
}
