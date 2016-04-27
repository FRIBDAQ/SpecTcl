#include "TabFromWinFileCompositor.h"
#include "SpectrumView.h"
#include "dispwind.h"
#include "TabWorkspace.h"
#include "HistogramList.h"
#include "SpecTclInterface.h"
#include "QRootCanvas.h"
#include "ViewDrawPanel.h"
#include "GeometrySelector.h"

#include <QString>
#include <QList>
#include <QMessageBox>

#include <TCanvas.h>

#include <iostream>
#include <algorithm>
#include <exception>

namespace Viewer {

TabFromWinFileCompositor::TabFromWinFileCompositor(std::shared_ptr<SpecTclInterface> pSpecTcl)
    : m_pSpecTcl(pSpecTcl)
{
}

void TabFromWinFileCompositor::compose(TabWorkspace &rWorkSpace, const QString &fileName)
{
    win_db layoutDb;
    try {
    layoutDb.read(fileName.toUtf8().constData());
    } catch (std::exception& exc) {
        QMessageBox::warning(nullptr, "Configuration error",
                             exc.what());
        return;
    }

    int nCols = layoutDb.nx();
    int nRows = layoutDb.ny();


    // First set the geometry controls to the correct values
    GeometrySelector& geoControl = rWorkSpace.getDrawPanel().getGeometrySelector();
    geoControl.setGeometry(nRows, nCols);

    // Set the geometry of the view next
    SpectrumView& view = rWorkSpace.getView();
    view.onGeometryChanged(nRows, nCols);

    // Draw all of the histograms that we are told exist.
    for (int col=0; col<layoutDb.nx(); ++col) {
        for (int row=0; row<layoutDb.ny(); ++row) {
            win_attributed* pAttributes = layoutDb.getdef(col, row);
            if (pAttributes) {

                QRootCanvas* pCanvas = view.getCanvas(row, col);

                assert(pCanvas != nullptr);

                setUpCanvas(*pCanvas, *pAttributes);
            }
        }
    }


}

void TabFromWinFileCompositor::setUpCanvas(QRootCanvas& rCanvas, win_attributed &rAttributes)
{
    QString specName;

    if (rAttributes.is1d()) {
        auto& attr = dynamic_cast<win_1d&>(rAttributes);
        specName = QString::fromStdString(attr.getSpectrumName());
    } else {
        auto& attr = dynamic_cast<win_2d&>(rAttributes);
        specName = QString::fromStdString(attr.getSpectrumName());
    }

    HistogramList* pHistList = m_pSpecTcl->getHistogramList();
    QList<QString> names = pHistList->histNames();

    auto itResult = std::find_if(names.begin(), names.end(),
                            [&specName](const QString& lhs) {
                                return (QString::compare(specName, lhs, Qt::CaseInsensitive) == 0);
                            });
    if (itResult == names.end()) {
        QString msg("Unable to find spectrum named %1 using case insensitive search.\n");
        msg += "This implies the spectrum does not exist and cannot be plotted.";
        QMessageBox::warning(nullptr, QString("Configuration error"), msg.arg(specName) );
        return;
    }

    HistogramBundle* pHistPkg = pHistList->getHist(*itResult);
    if (pHistPkg) {
        rCanvas.cd();
        pHistPkg->draw();
//        rCanvas.Modified(1);
//        rCanvas.Update();
    }
}

} // end Viewer namespace
