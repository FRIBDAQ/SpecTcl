
#include "ParameterDb.h"
#include "Parameters.h"

ClassImp(Viewer::RootEmbed::ParameterDb)

namespace Viewer
{
namespace RootEmbed 
{

//
//
ParameterDb::ParameterDb() : m_db()
{}


//
//
const std::map<std::string, Parameters>& ParameterDb::getDatabase() const
{
    return m_db;
}


//
//
void ParameterDb::setObject(const std::string &name, const Parameters &params)
{
    m_db[name] = params;
}


} // end RootEmbed namespace
} // end Viewer namespace
