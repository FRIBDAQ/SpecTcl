#ifndef VIEWER_ROOTEMBED_PARAMETERDB_H
#define VIEWER_ROOTEMBED_PARAMETERDB_H

#include <TObject.h>

#include <map>
#include <string>

namespace Viewer {

namespace RootEmbed {

class Parameters;

/*!
 * \brief A database for embedding in ROOT files
 *
 * There must be a way for Spectra to know the parameters that are associated with
 * the various histograms and cuts in a TFile. This is designed to provide that
 * information. It is no more than a look up table of parameters for a specific name.
 *
 */
class ParameterDb : public TObject
{
private:
    std::map<std::string, ::Viewer::RootEmbed::Parameters> m_db;

public:
    ParameterDb();

    const std::map<std::string, Parameters>& getDatabase() const;

    void setObject(const std::string& name, const Parameters& params);


    ClassDef(Viewer::RootEmbed::ParameterDb, 1);
};

} // end RootEmbed namespace
} // end Viewer namespace

#endif // GRAPHICOBJECTDB_H
