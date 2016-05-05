
#ifndef VIEWER_ROOTEMBED_PARAMETERS_H
#define VIEWER_ROOTEMBED_PARAMETERS_H

#include <TObject.h>

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

} // end RootEmbed namespace
} // end Viewer namespace

#endif // GRAPHICOBJECTDB_H
