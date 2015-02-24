#include "SpectrumViewer.h"
#include "ui_SpectrumViewer.h"
#include "QRootCanvas.h"
#include <TH1.h>
#include <TCanvas.h>
#include <HistogramList.h>
#include <RequestHandler.h>

SpectrumViewer::SpectrumViewer(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::SpectrumViewer),
    m_reqHandler(new RequestHandler(this))
{

    ui->setupUi(this);
    setFrameShadow(QFrame::Plain);
    setFrameShape(QFrame::NoFrame);
    m_canvas = new QRootCanvas(this);
    ui->gridLayout->addWidget(m_canvas,0,0);

    m_canvas->getCanvas()->Resize();
    m_canvas->getCanvas()->cd();

//    m_hist = new TH1D("raw00","The TEST", 4096, 0, 4096);
//    m_hist->SetBinContent(2,3);
//    m_hist->SetBinContent(5,1);
//    m_hist->SetBinContent(8,6);
//    m_hist->SetDirectory(0);

//    m_hist->Draw();
    m_canvas->show();

//    HistogramList::getInstance()->addHist(*m_hist);


    // set up the connections of signals/slots
    connect(ui->updateButton,SIGNAL(pressed()),m_reqHandler,SLOT(updateRequest()));
}

SpectrumViewer::~SpectrumViewer()
{
    delete ui;
}


void SpectrumViewer::update(TH1* hist)
{
    if (hist) {
        m_canvas->cd();
        hist->Draw();
    }
    m_canvas->Modified();
    m_canvas->update();
}
