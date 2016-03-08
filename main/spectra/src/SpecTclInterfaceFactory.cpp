//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2016.
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    Authors:
//    Jeromy Tompkins
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321


#include "SpecTclInterfaceFactory.h"
#include "SpecTclRESTInterface.h"
#include "SpecTclShMemInterface.h"
#include "ShMemKeyRequestHandler.h"
#include "GlobalSettings.h"

#include <stdexcept>

#include <unistd.h>

namespace Viewer {

std::unique_ptr<SpecTclInterface>
SpecTclInterfaceFactory::create(InterfaceType type) {

    switch(type) {
    case REST:
        return std::unique_ptr<SpecTclInterface>(new SpecTclRESTInterface);
        break;
    case Hybrid:
        return createShMemInterface();
        break;
    default:
        throw std::runtime_error("SpecTclInterfaceFactory::create() Invalid type specified.");
    }
}

/*!
 * \brief SpecTclInterfaceFactory::setUpShMemEnv
 *
 * The Xamine_initspectra command relies on having certain environment variables set.
 *
 */
void SpecTclInterfaceFactory::setUpShMemEnv()
{
    const char* pEnv = ::getenv("XAMINE_SHMEM");
    if (pEnv == nullptr) {
        ShMemKeyRequestHandler requester;
        requester.get();

        QString envValue = GlobalSettings::getSharedMemoryKey();

        ::setenv("XAMINE_SHMEM", envValue.toLocal8Bit().data(), 1);
    }
}

std::unique_ptr<SpecTclInterface> SpecTclInterfaceFactory::createShMemInterface()
{

    setUpShMemEnv();

    return std::unique_ptr<SpecTclInterface>(new SpecTclShMemInterface);
}

} // end of namespace
