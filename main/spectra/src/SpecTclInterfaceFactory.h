#ifndef SPECTCLINTERFACEFACTORY_H
#define SPECTCLINTERFACEFACTORY_H

#include <memory>

namespace Viewer {

class SpecTclInterface;

class SpecTclInterfaceFactory
{
public:
    enum InterfaceType { REST, Hybrid };

public:
    SpecTclInterfaceFactory() = default;
    ~SpecTclInterfaceFactory() = default;
    SpecTclInterfaceFactory(const SpecTclInterfaceFactory& rhs) = default;

    std::unique_ptr<SpecTclInterface> create(InterfaceType type);
};

} // end of namespace
#endif // SPECTCLINTERFACEFACTORY_H
