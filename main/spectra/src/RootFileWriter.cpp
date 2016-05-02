#include "RootFileWriter.h"

#include "QRootCanvas.h"
#include "TabWorkspace.h"
#include "SpectrumView.h"

#include <TFile.h>
#include <TCollection.h>
#include <TObject.h>
#include <TPad.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TH2.h>

#include <iostream>

namespace Viewer
{

RootFileWriter::RootFileWriter()
    : m_pFile(nullptr)
{
}

RootFileWriter::~RootFileWriter()
{
    if (m_pFile) {
        if (m_pFile->IsOpen()) closeFile();
        delete m_pFile;
    }
}

void RootFileWriter::openFile(const QString &path, const QString& options)
{
    if (m_pFile) {
        closeFile();
    }
    m_pFile = new TFile(path.toUtf8().constData(), options.toUtf8().constData());
}

void RootFileWriter::closeFile()
{
    m_pFile->Close();
}

void RootFileWriter::writeCanvas(QRootCanvas &rCanvas)
{
    m_pFile->cd();
    // by default the menu bar is hidden, that makes sense in Spectra, but not in ROOT.
    rCanvas.getCanvas()->SetBit(TCanvas::kMenuBar, 1);
    rCanvas.getCanvas()->Write();
}

void RootFileWriter::writeTab(TabWorkspace &rWorkspace, bool combine)
{
   SpectrumView& rView = rWorkspace.getView();

   std::vector<QRootCanvas*> canvases = rView.getAllCanvases();

   if (combine) {
       std::unique_ptr<QRootCanvas> pCanvas = combineCanvases(canvases,
                                                              rView.getRowCount(),
                                                              rView.getColumnCount());
       pCanvas->getCanvas()->SetName(rWorkspace.objectName().toUtf8().constData());
       std::cout << "Writing workspace : " << rWorkspace.objectName().toUtf8().constData() << std::endl;
       writeCanvas(*pCanvas);
//   } else {

//       for (auto& pCanvas : canvases) {
//           writeCanvas(*pCanvas, );
//       }
   }

}

std::unique_ptr<QRootCanvas>
RootFileWriter::combineCanvases(std::vector<QRootCanvas*> &canvases,
                                int nRows, int nCols)
{
    // the canvases are expected to be ordered in the vector as
    // (0,0),(1,0),(2,0),...(1,0),(1,1),(1,2),...(2,0),(2,1),... where the elements are (row, col)
    // in other words,  we loop over row first, then column.

    std::unique_ptr<QRootCanvas> pQCanvas(new QRootCanvas(nullptr));
    TPad* pPad = pQCanvas->getCanvas();

    pPad->Divide(nCols, nRows, 0.001, 0.001);

    std::cout << "rows,cols = " << nRows << "," << nCols << std::endl;

    // this is a bit annoying becuase the pad id is numbered opposite to the order of
    // canvases in the vector. It expects us to iterate over columns first and then rows.
    for (size_t canvasIndex = 0; canvasIndex<canvases.size(); ++canvasIndex) {
        int padIndex = convertToPadIndex(canvasIndex, nRows, nCols);
        std::cout << "Cloning into pad " << padIndex << std::endl;
        TVirtualPad* pVPad = pPad->cd(padIndex);

        TCanvas* pCanvas = canvases[canvasIndex]->getCanvas();
        if (pCanvas) {
            std::cout << "\tcopying canvas into pad" << std::endl;
            copyCanvasIntoPad(*pCanvas, *pVPad);
        }
    }

    return pQCanvas;
}

void RootFileWriter::copyCanvasIntoPad(TCanvas& rCanvas, TVirtualPad& rPad)
{
    TVirtualPad* pOldPad = gPad;
    rPad.cd();

    TObject* pObj;
    TIter next(rCanvas.GetListOfPrimitives());
    while (( pObj = next() )) {
        gROOT->SetSelectedPad(&rPad);
        std::cout << "\t\t" << pObj->GetName() << std::endl;

        TObject* pCopyObj = pObj->Clone();
        if (pCopyObj->InheritsFrom(TH2::Class())) {
            pCopyObj->AppendPad("col2");
        } else {
            pCopyObj->AppendPad("");
        }
    }

    pOldPad->cd();
}


int RootFileWriter::convertToPadIndex(size_t canvasIndex, int nRows, int nCols)
{
    int rowIndex = canvasIndex / nCols;
    int colIndex = canvasIndex % nCols;

    return colIndex * nRows + rowIndex + 1;
}

} // end Viewer namespace
