#include "RootFileWriter.h"

#include "QRootCanvas.h"
#include "TabWorkspace.h"
#include "SpectrumView.h"
#include "SpecTclInterface.h"
#include "HistogramList.h"
#include "MasterGateList.h"

#include <TFile.h>
#include <TCollection.h>
#include <TObject.h>
#include <TPad.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TH2.h>

#include <QDebug>

#include <iostream>
#include <stdexcept>

namespace Viewer
{

RootFileWriter::RootFileWriter(std::shared_ptr<SpecTclInterface> pSpecTcl)
    : m_pFile(nullptr),
      m_pDirectory(nullptr),
      m_pSpecTcl(pSpecTcl)
{
}

RootFileWriter::~RootFileWriter()
{
    if (m_pFile) {
        if (m_pFile->IsOpen()) closeFile();
        delete m_pFile;
    }
}

void RootFileWriter::createDirectory(const std::string &path)
{
    m_pDirectory = m_pFile->mkdir(path.c_str());
    if (m_pDirectory == nullptr) {
        // the directory already exists or the was an error!
        m_pFile->GetObject(path.c_str(), m_pDirectory);

        if (!m_pDirectory) {
            std::string msg("RootFileWriter::createDirectory() Failed to create ");
            msg += path + " directory.";
            throw std::runtime_error(msg);
        }
    }

    m_pDirectory->cd();

}

void RootFileWriter::openFile(const QString &path, const QString& options)
{
    QString outputPath = path;
    // ensure that the user provided the right suffix
    if (!outputPath.endsWith(".root")) {
        outputPath += ".root";
    }

    // in case we already have an open file. we only support dealing with a
    // single file at a time
    if (m_pFile) {
        closeFile();
    }

    m_pFile = new TFile(outputPath.toUtf8().constData(), options.toUtf8().constData());

    if (!m_pFile->IsOpen()) {
        throw std::runtime_error("RootFileWriter::openFile() Failed to open file.");
    }

    // create a spectra directory for histograms and such
    createDirectory("spectra");
    createDirectory("spectra/canvases");
    createDirectory("spectra/hists");
    createDirectory("spectra/cuts");

}

void RootFileWriter::closeFile()
{
    TDirectory* pSpecTclDir;
    m_pFile->GetObject("spectra", pSpecTclDir);

    pSpecTclDir->cd();

    if (pSpecTclDir) {
        pSpecTclDir->Write();
    }

    m_pFile->Close();
}




//
//
void RootFileWriter::writeCanvas(QRootCanvas &rCanvas)
{
    m_pFile->cd("spectra/canvases");
    // In spectra, the menu bar is hidden, that makes sense in Spectra, but not in ROOT.
    // ensure that the menu bar is enabled
    rCanvas.getCanvas()->SetBit(TCanvas::kMenuBar, 1);
    rCanvas.getCanvas()->Write();
}


/*!
 * \brief RootFileWriter::writeTab
 * \param rWorkspace    the workspace to write to file
 * \param combine       whether separate canvases should be saved or not
 *
 *  This is the entry point used by the SaveAsDialog.
 */
void RootFileWriter::writeTab(TabWorkspace &rWorkspace)
{
   SpectrumView& rView = rWorkspace.getView();

   std::vector<QRootCanvas*> canvases = rView.getAllCanvases();

   std::unique_ptr<QRootCanvas> pCanvas = combineCanvases(canvases,
                                                          rView.getRowCount(),
                                                          rView.getColumnCount());
   pCanvas->getCanvas()->SetName(rWorkspace.objectName().toUtf8().constData());
   //       std::cout << "Writing workspace : " << rWorkspace.objectName().toUtf8().constData() << std::endl;
   writeCanvas(*pCanvas);
   copyObjectsIntoDirectories(*pCanvas->getCanvas());
}




//
//
std::unique_ptr<QRootCanvas>
RootFileWriter::combineCanvases(const std::vector<QRootCanvas *> &canvases,
                                int nRows, int nCols, QWidget *pParent)
{
    // the canvases are expected to be ordered in the vector as
    // (0,0),(1,0),(2,0),...(1,0),(1,1),(1,2),...(2,0),(2,1),... where the elements are (row, col)
    // in other words,  we loop over row first, then column.

    std::unique_ptr<QRootCanvas> pQCanvas(new QRootCanvas(pParent));
    TPad* pPad = pQCanvas->getCanvas();

    pPad->Divide(nCols, nRows, 0.001, 0.001);

//    std::cout << "rows,cols = " << nRows << "," << nCols << std::endl;

    // this is a bit annoying becuase the pad id is numbered opposite to the order of
    // canvases in the vector. It expects us to iterate over columns first and then rows.
    for (size_t canvasIndex = 0; canvasIndex<canvases.size(); ++canvasIndex) {
        int padIndex = convertToPadIndex(canvasIndex, nRows, nCols);
        std::cout << "Cloning into pad " << padIndex << std::endl;
        TVirtualPad* pVPad = pPad->cd(padIndex);

        TCanvas* pCanvas = canvases[canvasIndex]->getCanvas();
        if (pCanvas) {
//            std::cout << "\tcopying canvas into pad" << std::endl;
            copyCanvasIntoPad(*pCanvas, *pVPad);
        }
    }

    return pQCanvas;
}


//
//
void RootFileWriter::copyCanvasIntoPad(TCanvas& rCanvas, TVirtualPad& rPad)
{
    TVirtualPad* pOldPad = gPad;
    rPad.cd();

    TObject* pObj;
    TIter next(rCanvas.GetListOfPrimitives());
    while (( pObj = next() )) {
        gROOT->SetSelectedPad(&rPad);
//        std::cout << "\t\t" << pObj->GetName() << std::endl;

        if (pObj->InheritsFrom(TH2::Class())) {
            pObj->Clone()->AppendPad("col");
        } else {
            pObj->Clone()->AppendPad("");
        }
    }

    pOldPad->cd();
}


//
//
void RootFileWriter::copyObjectsIntoDirectories(TPad& rCanvas)
{

    TObject* pObj;
    TIter next(rCanvas.GetListOfPrimitives());
    while (( pObj = next() )) {
 //       std::cout << "\t\t" << pObj->GetName() << std::endl;

        if (pObj->InheritsFrom(TH1::Class())) {
            m_pFile->cd("spectra/hists");

//            std::cout << pObj->GetName() << std::endl;
            if (gDirectory->GetList()->FindObject(pObj->GetName()) == nullptr) {
 //               std::cout << "adding to spectra/hists" << std::endl;
                gDirectory->Add(pObj->Clone());
            }
        } else if (pObj->InheritsFrom(TCutG::Class())) {
            m_pFile->cd("spectra/cuts");
            if (gDirectory->GetList()->FindObject(pObj->GetName()) == nullptr) {
                gDirectory->Add(pObj->Clone());
            }
        } else if (pObj->InheritsFrom(TPad::Class())) {
            copyObjectsIntoDirectories(dynamic_cast<TPad&>(*pObj));
        }
    }
}

//
//
int RootFileWriter::convertToPadIndex(size_t canvasIndex, int nRows, int nCols)
{
    int rowIndex = canvasIndex % nRows;
    int colIndex = canvasIndex / nRows;

    return rowIndex * nCols + colIndex + 1;
}

} // end Viewer namespace
