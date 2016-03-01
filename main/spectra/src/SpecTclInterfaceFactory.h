#ifndef SPECTCLINTERFACEFACTORY_H
#define SPECTCLINTERFACEFACTORY_H

#include <memory>

namespace Viewer {

// Forward declarations
class SpecTclInterface;


/*!
 * \brief Factory to produce SpecTclInterface instances
 *
 * This constructs only Hybrid and REST type interfaced. The C++ types that
 * pertain to this are:
 *  REST   -> SpecTclRESTInterface
 *  Hybrid -> SpecTclShMemInterface
 *
 */
class SpecTclInterfaceFactory
{
public:
    enum InterfaceType { REST, Hybrid };

public:
    // Canonicals
    SpecTclInterfaceFactory() = default;
    ~SpecTclInterfaceFactory() = default;
    SpecTclInterfaceFactory(const SpecTclInterfaceFactory& rhs) = default;

    /*!
     * \brief create a new SpecTclInterface
     *
     * \param type  type of interface to construct
     *
     * The caller clearly is passed ownership of the object that is created
     * by this.
     *
     * \return new SpecTclInterface
     */
    std::unique_ptr<SpecTclInterface> create(InterfaceType type);
};

} // end of namespace
#endif // SPECTCLINTERFACEFACTORY_H
