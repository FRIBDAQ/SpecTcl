#include "canvascontrol.h"
#include "ui_canvascontrol.h"
#include <iostream>
#include "SpectrumView.h"
#include "SpecTclInterface.h"
#include "ControlPanel.h"
#include "HistogramList.h"
#include <QList>


namespace Viewer {
    
CanvasControl::CanvasControl(
    SpectrumView* pView, SpecTclInterface* pInterface, ControlPanel* pPanel,
    QWidget *parent
) :
    QWidget(parent),
    ui(new Ui::canvascontrol),
    m_pView(pView), m_pSpecTcl(pInterface),
    m_pControlPanel(pPanel)
{
    ui->setupUi(this);
    

}

CanvasControl::~CanvasControl()
{
    delete ui;
}
/**
 *  Clear the contents in the currently selected spectrum
 */
void CanvasControl::on_pClearSelected_clicked()
{
    
    QRootCanvas* pCanvas = m_pView->getCurrentCanvas();
    m_pSpecTcl->clearSpectrum(pCanvas);
    
}
/**
 *  Clear the ccontents of the spectra in the visible view.
 */
void CanvasControl::on_pClearVisible_clicked()
{
    
    std::cerr << "Clear spectra in visible canvases\n";
    std::vector<QRootCanvas*> canvases = m_pView->getAllCanvases();
    for (int i = 0; i < canvases.size(); i++) {
        m_pSpecTcl->clearSpectrum(canvases[i]);
    }
}

void CanvasControl::on_pClearAll_clicked()
{
    
    std::cerr << "Clear all spectra\n";
    QList<QString> names = m_pSpecTcl->getHistogramList()->histNames();
    for (auto p = names.begin(); p != names.end(); p++) {
        QString name = *p;
        m_pSpecTcl->clearSpectrum(&name);
    }
}

void CanvasControl::on_pEmptySelected_clicked()
{
    std::cerr << "Empty selected slot\n";
    m_pView->emptyCurrentCanvas();
}

void CanvasControl::on_pEmptyVisible_clicked()
{
    std::cerr << "Empty slots in visible tab\n";

}
}