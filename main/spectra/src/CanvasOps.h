#ifndef CANVASOPS_H
#define CANVASOPS_H

#include <QString>

#include <vector>

class TPad;

namespace Viewer {

class QRootCanvas;

namespace CanvasOps {

std::vector<QString> extractAllHistNames(QRootCanvas& rCanvas);
std::vector<QString> extractAllHistNames(TPad& rPad);

} // end CanvasOps namespace
} // end Viewer namespace

#endif // CANVASOPS_H
