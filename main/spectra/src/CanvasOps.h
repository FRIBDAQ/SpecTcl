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
class TVirtualPad;
class TObject;
class TObjLink;

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
std::vector<QString> extractAllHistNames(TVirtualPad& rPad);


/*!
 * \brief getDrawOption
 *
 * The draw option for objects is contained in a link of the pad's linked
 * list of primitive objects. This function searches for the link containing
 * the TObject, and then returns the associated option. If the link is not found,
 * an exception is thrown.
 *
 * \param pPad  the pad containing the drawn object
 * \param pObj  the drawn object
 * \return  the option string
 *
 * \throws std::runtime_error if the pPad is a nullptr
 * \throws std::runtime_error if the pObj is a nullptr
 * \throws std::runtime_error if the pObj is not drawn in pPad
 */
QString getDrawOption(TVirtualPad* pPad, TObject* pObj);

/*!
 * \brief setDrawOption
 *
 * Sets the draw option for a drawn object in a canvas.
 *
 * \param pPad  the pad containing the drawn object
 * \param pObj  the drawn object
 * \param opt   the option string
 *
 * \throws std::runtime_error if the pPad is a nullptr
 * \throws std::runtime_error if the pObj is a nullptr
 * \throws std::runtime_error if the pObj is not drawn in pPad
 */
void setDrawOption(TVirtualPad* pPad, TObject* pObj, const QString& opt);

/*!
 * \brief findOptionLink
 *
 * Draw options are stored in the links of the linked list of primitives
 * maintained by the pad. These are searched linearly until a link is found
 * containing the object of interest of the end of the list is encountered.
 *
 * \param pPad  the pad on which the object is supposed to be drawn
 * \param pObj  the object that is drawn
 *
 * \retval pointer to the link if found
 * \retval nullptr if not found
 *
 */
TObjLink* findOptionLink(TVirtualPad& pad, TObject& obj);

} // end CanvasOps namespace
} // end Viewer namespace

#endif // CANVASOPS_H
