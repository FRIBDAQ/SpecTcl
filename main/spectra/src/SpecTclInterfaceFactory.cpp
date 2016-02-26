#include "SpecTclInterfaceFactory.h"
#include "SpecTclRESTInterface.h"
#include "SpecTclShMemInterface.h"

#include <stdexcept>

namespace Viewer {

std::unique_ptr<SpecTclInterface>
SpecTclInterfaceFactory::create(InterfaceType type) {

    switch(type) {
    case REST:
        return std::unique_ptr<SpecTclInterface>(new SpecTclRESTInterface);
        break;
    case Hybrid:
        return std::unique_ptr<SpecTclInterface>(new SpecTclShMemInterface);
        break;
    default:
        throw std::runtime_error("SpecTclInterfaceFactory::create() Invalid type specified.");
    }
}

} // end of namespace
