#include "TabFromWinFileCompositor.h"
#include "SpectrumView.h"
#include "dispwind.h"
#include "TabWorkspace.h"
#include "HistogramList.h"
#include "SpecTclInterface.h"
#include "QRootCanvas.h"

#include <QString>
#include <TCanvas.h>

#include <iostream>
#include <thread>
#include <chrono>

namespace Viewer {

TabFromWinFileCompositor::TabFromWinFileCompositor(std::shared_ptr<SpecTclInterface> pSpecTcl)
    : m_pSpecTcl(pSpecTcl)
{
}

void TabFromWinFileCompositor::compose(TabWorkspace &rWorkSpace, const QString &fileName)
{
    win_db layoutDb;
    layoutDb.read(fileName.toUtf8().constData());

    int nCols = layoutDb.nx();
    int nRows = layoutDb.ny();

    SpectrumView& view = rWorkSpace.getView();

    view.onGeometryChanged(nRows, nCols);

    for (int col=0; col<layoutDb.nx(); ++col) {
        for (int row=0; row<layoutDb.ny(); ++row) {
            win_attributed* pAttributes = layoutDb.getdef(col, row);
            if (pAttributes) {

                std::cout << row << " " << col << std::endl;
                QRootCanvas* pCanvas = view.getCanvas(row, col);

                assert(pCanvas != nullptr);

                std::cout << "found : " << (void*)pCanvas << std::endl;
                setUpCanvas(*pCanvas, *pAttributes);
            }
        }
    }


}

void TabFromWinFileCompositor::setUpCanvas(QRootCanvas& rCanvas, win_attributed &rAttributes)
{
    QString name;

    if (rAttributes.is1d()) {
        auto& attr = dynamic_cast<win_1d&>(rAttributes);
        name = QString::fromStdString(attr.getSpectrumName());
    } else {
        auto& attr = dynamic_cast<win_2d&>(rAttributes);
        name = QString::fromStdString(attr.getSpectrumName());
    }

    HistogramList* pHistList = m_pSpecTcl->getHistogramList();
    std::cout << "Composing histogram " << name.toStdString() << std::endl;
    HistogramBundle* pHistPkg = pHistList->getHist(name);
    if (pHistPkg) {
        std::cout << name.toStdString() << " found" << std::endl;
        rCanvas.cd();
        std::cout << "gPad = " << gPad << std::endl;
        std::cout << "Pad = " << rCanvas.getCanvas() << std::endl;
        pHistPkg->draw();
        rCanvas.Modified(1);
        rCanvas.Update();
    } else {
        std::cout << name.toStdString() << " NOT found" << std::endl;
    }
}

} // end Viewer namespace
