#include "TabFromWinFileCompositor.h"
#include "SpectrumView.h"
#include "dispwind.h"
#include "TabWorkspace.h"
#include "HistogramBundle.h"
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
#include <Utility.h>

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

    QString tabName = QString::fromStdString(layoutDb.getTitle());
    rWorkSpace.setObjectName(tabName);

    // First set the geometry controls to the correct values
    GeometrySelector& geoControl = rWorkSpace.getDrawPanel().getGeometrySelector();
    geoControl.setGeometry(nRows, nCols);

    // Set the geometry of the view next
    SpectrumView& view = rWorkSpace.getView();
    view.setGeometry(nRows, nCols);

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

    bool isoned;
    HistogramBundle* pHistPkg = pHistList->getHist(*itResult);
    if (pHistPkg) {
        rCanvas.cd();
        pHistPkg->draw();
        isoned = rootHistogramIs1d(pHistPkg->getHist());
//        rCanvas.Modified(1);
//        rCanvas.Update();
    } else  return;      // No base spectrum means draw nothing:
    


    try {
        // handle superimposed histograms - for now we just suppress
        // drawing incopmatible spectra.
        
        auto& rAttr1D = dynamic_cast<win_1d&>(rAttributes);

        SuperpositionList& supers = rAttr1D.GetSuperpositions();
        SuperpositionListIterator iter(supers);

        
        while (!iter.Last()) {

            QString specName = QString::fromStdString(iter->SpectrumName());
            HistogramBundle* pBundle = pHistList->getHist(specName);
            if (pBundle) {
                // Histogrammust be compatible to draw:
                
                if (isoned == rootHistogramIs1d(pBundle->getHist())) {
                    pBundle->draw("same");    
                }
                
                
            } else {
                QString msg = "Request to superimpose histogram failed because histogram, '%1', was not found.";
                QMessageBox::warning(0, "Failed to locate histogram",
                                     msg.arg(specName));
            }
            iter.Next();
        }

    } catch (std::bad_cast exc) {
        // do nothing... this will happen if we have a win_2d. No problem.
    }

}

} // end Viewer namespace
