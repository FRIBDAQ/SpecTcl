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

#ifndef SPECTCLDISPLAYINTERFACE_H
#define SPECTCLDISPLAYINTERFACE_H

#include "DisplayInterface.h"

/*!
 * \brief A specialized display manager for spectcl
 *
 * The only difference between this and the CDisplayInterface is that
 * this is constructed non-empty. On construction, it adds creators for the
 * null, xamine, and spectra displays. It also constructs an instance
 * of the null display.
 */
class CSpecTclDisplayInterface : public CDisplayInterface
{

public:
    CSpecTclDisplayInterface();
};

#endif // SPECTCLDISPLAYINTERFACE_H
