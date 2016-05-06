#ifndef WINFILEWRITER_H
#define WINFILEWRITER_H


#include <vector>
#include <string>

#include <QString>

class win_1d;
class win_2d;

namespace Viewer
{

class TabWorkspace;
class QRootCanvas;

class WinFileWriter
{

public:
    WinFileWriter();

    void writeTab(TabWorkspace& rWorkspace, const QString& path);

private:
    void appendCanvasToWinDb(QRootCanvas& rCanvas, win_1d &dbAttr);
    void appendCanvasToWinDb(QRootCanvas& rCanvas, win_2d &dbAttr);

    std::vector<std::string> extractHistNamesFromCanvas(QRootCanvas& rCanvas);

    int getHistogramDimension(QRootCanvas& rCanvas);
};

} // end Viewer namespace

#endif // WINFILEWRITER_H
