#include "MarkerDialog.h"
#include "ui_MarkerDialog.h"
#include "MainWindow.h"
#include "TabbedMultiSpectrumView.h"
#include <QRootCanvas.h>
#include "HistogramBundle.h"
#include "TabWorkspace.h"
#include "SpectrumView.h"
#include "HistogramList.h"
#include "SpecTclInterface.h"
#include "CanvasOps.h"
#include <TPad.h>
#include <TMarker.h>
#include <QMessageBox>
#include <iostream>
#include "Marker.h"
#include <sstream>
#include <QTableWidgetSelectionRange>

static const int ROOT_TENTATIVE_MARKER = 4;   // Open circle rendition.


namespace Viewer {

MarkerDialog::MarkerDialog(QWidget *parent, SpecTclInterface* pSpecTcl) :
    QDialog(parent),
    ui(new Ui::MarkerDialog),
    m_pSpecTcl(pSpecTcl),
    m_pCanvas(nullptr),
    m_pTentativeMarker(nullptr)
{
    ui->setupUi(this);
    
    // We need to go through all this BS in order to establish a connection
    // beteen mouse clicks on the current canvas and us.  We use mouse release
    // events to accept the tentative marker position.
    
    MainWindow*              pMain      = MainWindow::getInstance();
    TabbedMultiSpectrumView* pView      = pMain->getView();
    TabWorkspace&            pWorkspace(pView->getCurrentWorkspace());
    m_pCanvas  =  pWorkspace.getView().getCurrentCanvas();

    connect(ui->pDelete, SIGNAL(clicked()), this, SLOT(onDeleteSelected()));
    
    connect(
        m_pCanvas, SIGNAL(mousePressed(QWidget*)),
        this, SLOT(onMouseRelease(QWidget*))
    );
    // Load the marker list box:
    
    loadMarkerList();
    
}

MarkerDialog::~MarkerDialog()
{
    delete ui;
    delete m_pTentativeMarker;
}

// Slot implementations:

void MarkerDialog::on_pMarkerDialogButtonBox_accepted()
{
    // No-op if there's no tentative marker or there's no
    // marker name.. but tell them that:
    
    
    QString markerName = ui->lineEdit->text();
    markerName = markerName.trimmed();
    
    if (! m_pTentativeMarker || markerName.isEmpty()) {
        QString msgTxt = "You must provide a marker position and a marker name";
        QString titleTxt = "Can't accept marker";
        QMessageBox msg(
            QMessageBox::Warning, titleTxt, msgTxt, QMessageBox::Ok, this
        );
        msg.exec();
        
        deleteTentativeMarker();
        emit onRejected();
        return;
    }
    // We have a hit.
    // - Figure out the histogram it's accepted on.
    // - Figure out the coordinates and fire off the
    //   onAccepted signal:
    //
    Double_t x = m_pTentativeMarker->GetX();
    Double_t y = m_pTentativeMarker->GetY();
    
    
    HistogramBundle* pBundle = getCurrentHistogram();
    deleteTentativeMarker();
    
    emit onAccepted(markerName, pBundle, x, y);
}

void MarkerDialog::on_pMarkerDialogButtonBox_rejected()
{
    deleteTentativeMarker();
    emit onRejected();
}

void MarkerDialog::closeEvent(QCloseEvent* e)
{
    deleteTentativeMarker();
    emit onDeleted();
    QDialog::closeEvent(e);
}
/**
 * onMouseRelease
 *    Fired from the canvas when a mouse click ends.
 *    - Capture the mouse position.
 *    - If necessary, create a new tentative marker, if not move the
 *      existing one.
 *    - Do the magic to get the new marker actually displayed in the
 *      mouse hit position.
 *
 *  @param pad - the root canvas on which the click occured.
 */
void
MarkerDialog::onMouseRelease(QWidget* ppad)
{

    
    QRootCanvas* pad = reinterpret_cast<QRootCanvas*>(ppad);
    // The root canvas produces the raw pixel coords:
    
    int px = pad->GetEventX();
    int py = pad->GetEventY();
    
    // These must be converted to spectrum coordinates:
    
    Double_t x = pad->AbsPixeltoX(px);
    Double_t y = pad->AbsPixeltoY(py);
    
    // Now do what needs to be done for the marker:
    
    if (!m_pTentativeMarker) {
        m_pTentativeMarker = new TMarker(0.0, 0.0, ROOT_TENTATIVE_MARKER);
    }
    // Regardless, set the position:
    
    m_pTentativeMarker->SetX(x);
    m_pTentativeMarker->SetY(y);
    
    m_pTentativeMarker->Draw();
    
    // This kludge was necessary at the time we wrote this hopefully
    // no longer?
    
    gPad->SetLogx(gPad->GetLogx());
    
}

/**
 * getCurrentHistogram
 *    Get the histogram in the current pad.  Note that with superpositions,
 *    a pad can have several histograms.  The first of these is the
 *    'primary histogra' and is what we're going to use.
 */
HistogramBundle*
MarkerDialog::getCurrentHistogram()
{
    std::vector<QString> histNames = CanvasOps::extractAllHistNames(*gPad);
    if (histNames.empty()) {
        throw std::runtime_error(
            "Accepting marker : There's no histogram on the current pad!!"
        );
    }
    QString histName = histNames[0];
    HistogramList* pHistograms = m_pSpecTcl->getHistogramList();
    HistogramBundle* pResult   = pHistograms->getHistFromClone(histName);
    
    if (!pResult) {
        throw std::runtime_error(
            "Accepting marker:  Can't find the histogram bundle for the histogram"
        );
    }
    return pResult;
}
/**
 * deleteTentativeMarker
 *    Get rid of the tentative marker and remove it from the display.
 */
void
MarkerDialog::deleteTentativeMarker()
{
    delete m_pTentativeMarker;
    m_pTentativeMarker = nullptr;
    m_pSpecTcl->requestHistContentUpdate(m_pCanvas);
}


/**
 * loadMarkerList
 *
 *    Load the marker listbox with the set of markers, and their coords.
 *    for this spectrm.
 *    
 */
void
MarkerDialog::loadMarkerList()
{
    HistogramBundle* b = getCurrentHistogram();
    std::vector<const GraphicalObject*> objects = b->getGrobjs();
    ui->pMarkerTable->setSortingEnabled(false);
    
    int row = 0;                  // Headers are a row.
    for (int i = 0; i < objects.size(); i++) {
        const Marker* pMarker = dynamic_cast<const Marker*>(objects[i]);
        if (pMarker) {
            ui->pMarkerTable->setRowCount(row+1);    // New row and titles.
            QString name = pMarker->getName();
            Double_t   x = pMarker->getX();
            Double_t   y = pMarker->getY();
            std::stringstream s;
            s << "(" << x << ", " << y << ")";
            QString coords(s.str().c_str());
            
            QTableWidgetItem* pName = new QTableWidgetItem(name);
            QTableWidgetItem*  pCoords = new QTableWidgetItem(coords);
            
            ui->pMarkerTable->setItem(row, 0, pName);
            ui->pMarkerTable->setItem(row, 1, pCoords);
            std::cerr << "Filled row " << row << std::endl;
            row++;
        }
    }
    ui->pMarkerTable->setSortingEnabled(true);
}

/**
 * Delete the seleted marker
 */
void
MarkerDialog::onDeleteSelected()
{
    std::cerr << "Delete clicked\n";
    QList<QTableWidgetSelectionRange> selection =
        ui->pMarkerTable->selectedRanges();

    for(int i =0; i < selection.length(); i++) {
        const QTableWidgetSelectionRange& range(selection.at(i));
        deleteMarkerRange(range.topRow(), range.bottomRow());
    }
    clearMarkerList();
    loadMarkerList();
    
    m_pSpecTcl->requestHistContentUpdate(m_pCanvas);
}
/**
 *  deleteMarkerRange
 *     Destroy the set of markers on the current spectrum
 *     that match the list of markers in the given selection range of the
 *     marker table.  Note that the marker table is not modified by this action.
 *
 *  @param topRow - The top row of the selection.
 *  @#param bottomRow - The  bottom row of the selection (inclusive).
 */
void
MarkerDialog::deleteMarkerRange(int topRow, int bottomRow)
{
    HistogramBundle* pBundle = getCurrentHistogram();
    for (int i = topRow; i <= bottomRow; i++) {
        QTableWidgetItem* nameItem = ui->pMarkerTable->item(i, 0);
        QString name = nameItem->text();
        pBundle->removeGrob(name);
    }
}
/**
 * clearMarkerList
 *    Clear the marker list table.
 */
void
MarkerDialog::clearMarkerList()
{
    ui->pMarkerTable->setRowCount(0);    // This deletes the data too.
}
}