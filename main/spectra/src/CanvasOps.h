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

#ifndef CANVASOPS_H
#define CANVASOPS_H

#include <QString>

#include <vector>

class TPad;

namespace Viewer {

class QRootCanvas;

/*!
 * Functions to extract information about canvases
 */
namespace CanvasOps {

/*!
 * \brief extractAllHistNames
 *
 * \param rCanvas   the canvas to search
 *
 * This is recursive and can handle subpads.
 *
 * \return list of histogram names that are contained in canvas
 */
std::vector<QString> extractAllHistNames(QRootCanvas& rCanvas);

/*!
 * \brief extractAllHistNames
 * \param rPad  the pad to search
 *
 * This is the same as the above method except for the argument type.
 */
std::vector<QString> extractAllHistNames(TPad& rPad);

} // end CanvasOps namespace
} // end Viewer namespace

#endif // CANVASOPS_H
