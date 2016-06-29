#ifndef ROOTFILEWRITER_H
#define ROOTFILEWRITER_H

#include <QString>

#include <memory>
#include <functional>

class TFile;
class TVirtualPad;
class TCanvas;
class TPad;
class TDirectory;
class TObject;

class QWidget;

namespace Viewer
{

class QRootCanvas;
class TabWorkspace;
class SpecTclInterface;
class GSlice;
class GGate;
class HistogramBundle;


/*!
 * \brief A class to write canvases into a root file with a standard structure
 *
 * The structure of data stored by this will be:
 *
 * spectra
 * +-- hists
 * |    +-- histograms from the tabs in Spectra
 * +-- cuts
 * |    +-- converted (slices, bands, contours) from Spectra
 * +-- canvases
 *      +-- converted
 *
 */
class RootFileWriter
{
public:
    RootFileWriter(std::shared_ptr<SpecTclInterface> pSpecTcl);
    ~RootFileWriter();

    void openFile(const QString& path, const QString &options);
    void closeFile();

    void writeCanvas(QRootCanvas &rCanvas);

    void writeTab(TabWorkspace &rWorkspace, bool combine);

    std::unique_ptr<QRootCanvas> combineCanvases(const std::vector<QRootCanvas*>& canvases,
                                                 int nRows,
                                                 int nCols,
                                                 QWidget* pParent=nullptr);

    void copyCanvasIntoPad(TCanvas& rCanvas, TVirtualPad& rPad);
    void copyObjectsIntoDirectories(TPad &rCanvas);

    void createDirectory(const std::string& path);
private:
    int convertToPadIndex(size_t canvasIndex, int nRows, int nCols);

private:
    TFile*                              m_pFile;
    TDirectory*                         m_pDirectory;
    std::shared_ptr<SpecTclInterface>   m_pSpecTcl;
};

} // end Viewer namespace

#endif // ROOTFILEWRITER_H
