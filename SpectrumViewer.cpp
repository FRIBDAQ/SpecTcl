#include "SpectrumViewer.h"
#include "HistogramList.h"
#include "ContentRequestHandler.h"
#include "GlobalSettings.h"
#include "HistogramList.h"
#include "ui_SpectrumViewer.h"
#include "QRootCanvas.h"
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <iostream>
#include <QMessageBox>

SpectrumViewer::SpectrumViewer(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::SpectrumViewer),
    m_currentHist(nullptr),
    m_canvas(nullptr),
    m_reqHandler()
{

    ui->setupUi(this);
    setFrameShadow(QFrame::Plain);
    setFrameShape(QFrame::NoFrame);

    m_canvas = new QRootCanvas(this);
    ui->gridLayout->addWidget(m_canvas,0,0);

    m_canvas->getCanvas()->Resize();
    m_canvas->getCanvas()->cd();

    m_canvas->show();

    // set up the connections of signals/slots
    connect(ui->updateButton, SIGNAL(pressed()), this, SLOT(requestUpdate()));

    if ( !connect(&m_reqHandler, SIGNAL(parsingComplete(const GuardedHist*)), this, SLOT(update(const GuardedHist*))) ) {
        std::cout << "Failed to connect parsingComplete --> update" << std::endl;
    }
    if ( !connect(&m_reqHandler, SIGNAL(error(int,const QString&)), this, SLOT(onError(int, const QString&))) ) {
        std::cout << "Failed to connect error --> onError" << std::endl;
    }
}

SpectrumViewer::~SpectrumViewer()
{
    delete ui;
}

void SpectrumViewer::requestUpdate()
{
    m_reqHandler.get(formUpdateRequest());
}

void SpectrumViewer::update(const GuardedHist* pgHist)
{
    m_canvas->cd();

    GuardedHist gHist = *pgHist;

    LockGuard<GuardedHist> lock(gHist);
    m_currentHist = gHist.hist();
    TH1& hist = *(gHist.hist());

    if (hist.InheritsFrom(TH2::Class())) {
        hist.Draw("colz");
    } else {
        hist.Draw();
    }

    m_canvas->Modified();
    m_canvas->update();
}

QUrl SpectrumViewer::formUpdateRequest()
{
  if (m_currentHist) {
    QString name = m_currentHist->GetName();
    auto host = GlobalSettings::getServerHost();
    auto port = GlobalSettings::getServerPort();

    QString reqUrl("http://%1:%2/spectcl/spectrum/contents?name=%3");
    reqUrl = reqUrl.arg(host).arg(port).arg(name);
    return QUrl(reqUrl);
  }
}

void SpectrumViewer::onError(int errorCode, const QString& reason)
{
    QMessageBox::warning(nullptr,"Error during update",reason);
}
