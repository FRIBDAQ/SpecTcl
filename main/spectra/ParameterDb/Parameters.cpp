
#include "Parameters.h"

ClassImp(Viewer::RootEmbed::Parameters)

namespace Viewer
{

namespace RootEmbed {


//
//
Parameters::Parameters(const std::string &xparam, const std::string &yparam)
    : m_xparam(xparam), m_yparam(yparam)
{}

//
//
Parameters::Parameters(const Parameters & rhs) :
    m_xparam(rhs.m_xparam),
    m_yparam(rhs.m_yparam)
{}


//
//
Parameters& Parameters::operator=(const Parameters& rhs)
{
    if (this!=&rhs) {
        m_xparam = rhs.m_xparam;
        m_yparam = rhs.m_yparam;
    }

    return *this;
}

//
//
Parameters::~Parameters() {}


//
//
void Parameters::setXParameter(const std::string &param)
{
    m_xparam = param;
}

//
//
std::string Parameters::getXParameter() const
{
    return m_xparam;
}

//
//
void Parameters::setYParameter(const std::string &param)
{
    m_yparam = param;
}

//
//
std::string Parameters::getYParameter() const
{
    return m_yparam;
}


} // end RootEmbed namespace
} // end Viewer namespace
