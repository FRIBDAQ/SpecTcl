#ifndef GRAPHICOBJECTDB_H
#define GRAPHICOBJECTDB_H

#include <TObject.h>

#include <map>
#include <string>

namespace Viewer {

namespace RootEmbed {

/*!
 * \brief Container for two parameter names
 *
 * There is a parameter name for an x- and y-axis. That is the most that we need
 * to know for any of the graphical objects.
 */
class Parameters : public TObject
{
private:
    std::string m_xparam;
    std::string m_yparam;

public:
    Parameters(const std::string& xparam=std::string(),
               const std::string& yparam = std::string());
    Parameters(const Parameters&);
    Parameters& operator=(const Parameters&);
    ~Parameters();

    void setXParameter(const std::string& param);
    std::string getXParameter() const;

    void setYParameter(const std::string& param);
    std::string getYParameter() const;

    ClassDef(Viewer::RootEmbed::Parameters, 1);
};


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
