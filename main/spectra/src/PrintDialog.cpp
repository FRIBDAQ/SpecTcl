#include "PrintDialog.h"
#include "RootFileWriter.h"
#include "SpecTclInterface.h"
#include "TabWorkspace.h"
#include "TabbedMultiSpectrumView.h"
#include "QRootCanvas.h"
#include "PrintingSystem.h"

#include <TCanvas.h>

#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QFile>
#include <QTextStream>
#include <QStringList>

#include <cstdlib>
#include <iostream>

namespace Viewer
{

PrintDialog::PrintDialog(std::shared_ptr<SpecTclInterface> pSpecTcl, TabbedMultiSpectrumView &rView, QWidget *parent) :
    QDialog(parent),
    m_pSpecTcl(pSpecTcl),
    m_pView(&rView)
{
    assembleWidgets();
    connectSignalsAndSlots();
}

void PrintDialog::populatePrinterOptions()
{
    QStringList printers = PrintingSystem::instance().getAvailablePrinters();

    m_pPrinterSelect->addItems(printers);
}

void PrintDialog::assembleWidgets()
{
    m_pPrinterLabel = new QLabel(tr("Printer"), this);
    m_pPrinterLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_pPrinterSelect = new QComboBox(this);
    m_pPrinterSelect->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    populatePrinterOptions();

    m_pCancelButton = new QPushButton(tr("&Cancel"), this);
    m_pOkButton = new QPushButton(tr("&Ok"), this);
    m_pOkButton->setAutoDefault(true);
    m_pOkButton->setDefault(true);

    m_pOkButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_pCancelButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    auto pPrinterInfoLayout = new QVBoxLayout;
    pPrinterInfoLayout->addWidget(m_pPrinterLabel);
    pPrinterInfoLayout->addWidget(m_pPrinterSelect);
    pPrinterInfoLayout->addSpacing(30);
    pPrinterInfoLayout->addWidget(m_pOkButton);
    pPrinterInfoLayout->addWidget(m_pCancelButton);
    pPrinterInfoLayout->addStretch();

    m_pPreviewLabel = new QLabel(tr("Preview"), this);
    m_pPreviewLabel->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));

    RootFileWriter writer(m_pSpecTcl);
    auto& view = m_pView->getCurrentWorkspace().getView();
    int nRows = view.getRowCount();
    int nCols = view.getColumnCount();
    std::unique_ptr<QRootCanvas> upCanvas = writer.combineCanvases(view.getAllCanvases(),
                                                                   nRows, nCols, this);
    m_pCanvas = upCanvas.release();
    m_pCanvas->setParent(this);
    m_pCanvas->setMinimumWidth(300);
    m_pCanvas->setMinimumHeight(400);

    auto pPreviewLayout = new QVBoxLayout;
    pPreviewLayout->addWidget(m_pPreviewLabel);
    pPreviewLayout->addWidget(m_pCanvas);

    auto pTopLayout = new QHBoxLayout;
    pTopLayout->addLayout(pPrinterInfoLayout);
    pTopLayout->addLayout(pPreviewLayout);

    setLayout(pTopLayout);
}

void PrintDialog::connectSignalsAndSlots()
{
    connect(m_pOkButton, SIGNAL(clicked()), this, SLOT(accepted()));
    connect(m_pCancelButton, SIGNAL(clicked()), this, SLOT(rejected()));
}

void PrintDialog::accepted()
{
    // Print to a file that is highly unlikely to exist
    m_pCanvas->getCanvas()->Print("_81239azsdfaw__tmp_print__.ps", "ps");

    QString printer = m_pPrinterSelect->itemText(m_pPrinterSelect->currentIndex());
    QString printerCmd;
    if (printer == "lp") {
        // in case we have a printer that
        printerCmd = "lp _81239azsdfaw__tmp_print__.ps";
    } else {
        QString printCmdTemplate("lp -d %1 _81239azsdfaw__tmp_print__.ps");
        printerCmd = printCmdTemplate.arg(printer);
    }

    std::system(printerCmd.toUtf8().constData());

    // clean up
    std::remove("_81239azsdfaw__tmp_print__.ps");

    emit accept();
}

void PrintDialog::rejected()
{
    emit reject();
}


} // end Viewer namespace
