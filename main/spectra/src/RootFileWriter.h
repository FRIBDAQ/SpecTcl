#ifndef ROOTFILEWRITER_H
#define ROOTFILEWRITER_H

#include <QString>

#include <memory>

class TFile;
class TVirtualPad;
class TCanvas;

namespace Viewer
{

class QRootCanvas;
class TabWorkspace;

class RootFileWriter
{
public:
    RootFileWriter();
    ~RootFileWriter();

    void openFile(const QString& path, const QString &options);
    void closeFile();

    void writeCanvas(QRootCanvas &rCanvas);

    void writeTab(TabWorkspace &rWorkspace, bool combine);

    std::unique_ptr<QRootCanvas> combineCanvases(std::vector<QRootCanvas*>& canvases,
                                                 int nRows, int nCols);

    void copyCanvasIntoPad(TCanvas& rCanvas, TVirtualPad& rPad);

    int convertToPadIndex(size_t canvasIndex, int nRows, int nCols);

private:
    TFile* m_pFile;
};

} // end Viewer namespace

#endif // ROOTFILEWRITER_H
