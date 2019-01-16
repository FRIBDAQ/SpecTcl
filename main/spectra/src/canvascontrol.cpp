#include "canvascontrol.h"
#include "ui_canvascontrol.h"
#include <iostream>
#include "SpectrumView.h"
#include "SpecTclInterface.h"
#include "ControlPanel.h"

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

void CanvasControl::on_pClearSelected_clicked()
{
    std::cerr << "Clear selected\n";
    QRootCanvas* pCanvas = m_pView->getCurrentCanvas();
    m_pSpecTcl->clearSpectrum(pCanvas);
    
}

void CanvasControl::on_pClearVisible_clicked()
{
    std::cerr << "Clear spectra in visible canvases\n";
}

void CanvasControl::on_pClearAll_clicked()
{
    std::cerr << "Clear all spectra\n";
}

void CanvasControl::on_pEmptySelected_clicked()
{
    std::cerr << "Empty selected slot\n";
}

void CanvasControl::on_pEmptyVisible_clicked()
{
    std::cerr << "Empty slots in visible tab\n";
}
}