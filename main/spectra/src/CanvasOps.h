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
