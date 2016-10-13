#include "WinFileWriter.h"
#include "TabWorkspace.h"
#include "SpectrumView.h"
#include "QRootCanvas.h"
#include "dispwind.h"
#include "superpos.h"

#include <QRegExp>

#include <TObject.h>
#include <TList.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>

#include <cstdio>
#include <stdexcept>

namespace Viewer
{

WinFileWriter::WinFileWriter()
{
}


void WinFileWriter::writeTab(TabWorkspace &rWorkspace, const QString &path)
{

    win_db database;

    SpectrumView& rView = rWorkspace.getView();
    database.setx(rView.getColumnCount());
    database.sety(rView.getRowCount());
    database.settitle(rWorkspace.objectName().toUtf8().constData());

    for (int row=0; row<rView.getRowCount(); ++row) {
        for (int col=0; col<rView.getColumnCount(); ++col) {
            QRootCanvas* pCanvas = rView.getCanvas(row, col);
            if (pCanvas == nullptr) {
                std::string msg("WinFileWriter::writeTab() ");
                msg += "Attempted to access non-existent canvas ";
                msg += "at (" + std::to_string(row) + "," + std::to_string(col) + ")";
                throw std::runtime_error(msg);
            }

            int dim = getHistogramDimension(*pCanvas);
            if (dim == 1) {
                win_1d* pAttr = new win_1d;
                database.setdef(col, row, *pAttr);
                appendCanvasToWinDb(*pCanvas, *pAttr);
            } else if (dim == 2) {
                 win_2d* pAttr = new win_2d;
                database.setdef(col, row,*pAttr);
                appendCanvasToWinDb(*pCanvas, *pAttr);
            }
        }
    }

    database.write(path.toUtf8().constData());
}

void WinFileWriter::appendCanvasToWinDb(QRootCanvas &rCanvas, win_1d& dbAttr)
{
    dbAttr.setrend(histogram);

    std::vector<std::string> histNames = extractHistNamesFromCanvas(rCanvas);
    size_t nHists = histNames.size();
    for (size_t hIdx = 0; hIdx < nHists; ++hIdx) {
        if (hIdx == 0 ) {
            dbAttr.setSpectrumName(histNames[hIdx]);
        } else {
            dbAttr.GetSuperpositions().Add(histNames[hIdx]);
        }
    }
}

std::vector<std::string> WinFileWriter::extractHistNamesFromCanvas(QRootCanvas& rCanvas)
{
    std::vector<std::string> names;
    TObject* pObj;
    TIter next(rCanvas.getCanvas()->GetListOfPrimitives());
    while (( pObj = next()) ) {
        if (pObj->InheritsFrom(TH1::Class())) {
            QString clonedName = QString::fromUtf8(pObj->GetName());
            QString baseName = clonedName.mid(0, clonedName.lastIndexOf(QRegExp("_copy$")));
            names.push_back(baseName.toAscii().constData());
        }
    }

    return names;
}

void WinFileWriter::appendCanvasToWinDb(QRootCanvas &rCanvas, win_2d& dbAttr)
{
    dbAttr.setrend(color);
    std::vector<std::string> histNames = extractHistNamesFromCanvas(rCanvas);
    if (histNames.size() > 0) {
        std::cout << histNames[0] << std::endl;
        dbAttr.setSpectrumName(histNames[0]);
    } else {
        std::string msg("WinFileWriter::appendCanvasToWinDb() ");
        msg += "Failed to find hist in canvas.";
        throw std::runtime_error(msg);
    }
}

int WinFileWriter::getHistogramDimension(QRootCanvas &rCanvas)
{
    int dim = 0;
    TObject* pObj;
    TIter next(rCanvas.getCanvas()->GetListOfPrimitives());
    while (( pObj = next()) ) {
        if (pObj->InheritsFrom(TH2::Class())) {
            dim = 2;
        } else if (pObj->InheritsFrom(TH1::Class())) {
            dim = 1;
        }
    }

    return dim;
}

} // end Viewer namespace
