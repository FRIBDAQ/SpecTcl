#include "ParameterList.h"
#include "ParameterInfo.h"

ParameterList* ParameterList::m_instance = nullptr;


ParameterList::~ParameterList()
{
    delete m_instance;
}

void ParameterList::addParameter(const SpJs::ParameterInfo& info)
{
}


SpJs::ParameterInfo ParameterList::getParameter(const std::string &name)
{
    return SpJs::ParameterInfo();
}

SpJs::ParameterInfo ParameterList::getParameter(int id)
{
    return SpJs::ParameterInfo();
}
