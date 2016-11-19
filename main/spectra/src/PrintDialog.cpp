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
#include <QFileDialog>
#include <QDir>

#include <cstdlib>
#include <iostream>

namespace Viewer
{

//
//
PrintDialog::PrintDialog(std::shared_ptr<SpecTclInterface> pSpecTcl,
                         TabbedMultiSpectrumView &rView, QWidget *parent) :
    QDialog(parent),
    m_pSpecTcl(pSpecTcl),
    m_pView(&rView)
{
    assembleWidgets();
    connectSignalsAndSlots();
}

//
//
QLayout* PrintDialog::assemblePrintControls()
{

    m_pPrinterLabel = new QLabel(tr("<h1>Select Printer</h1>"), this);
    m_pPrinterLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_pPrinterSelect = new QComboBox(this);
    m_pPrinterSelect->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_pPrinterLabel->setBuddy(m_pPrinterSelect);

    QStringList printers = PrintingSystem::instance().getAvailablePrinters();
    if (printers.count() == 0) {
        m_pPrinterSelect->addItem("Save to file (ps)", QVariant("ps"));
        m_pPrinterSelect->addItem("Save to file (pdf)", QVariant("pdf"));
    } else {
        m_pPrinterSelect->addItems(printers);
    }

    m_pCancelButton = new QPushButton(tr("&Cancel"), this);
    m_pOkButton = new QPushButton(tr("&Print"), this);
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

    return pPrinterInfoLayout;
}

//
//
void PrintDialog::assembleWidgets()
{

    auto pPrinterInfoLayout = assemblePrintControls();

    m_pPreviewLabel = new QLabel(tr("<h1>Preview</h1>"), this);
    m_pPreviewLabel->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,
                                               QSizePolicy::Fixed));

    RootFileWriter writer(m_pSpecTcl);
    auto& view = m_pView->getCurrentWorkspace().getView();
    int nRows = view.getRowCount();
    int nCols = view.getColumnCount();
    std::unique_ptr<QRootCanvas> upCanvas = writer.combineCanvases(view.getAllCanvases(),
                                                                   nRows, nCols, this);
    m_pCanvas = upCanvas.release();
    m_pCanvas->setParent(this);
    m_pCanvas->setMinimumWidth(400);
    m_pCanvas->setMinimumHeight(400*0.7727); // enforce US Letter aspect ratio

    auto pPreviewLayout = new QGridLayout;
    pPreviewLayout->addWidget(m_pPreviewLabel, 0, 1, 1, 3, Qt::AlignLeft);
    pPreviewLayout->addWidget(m_pCanvas, 2, 1);
    pPreviewLayout->setRowStretch(1, 1);
    pPreviewLayout->setRowStretch(3, 1);
    pPreviewLayout->setColumnStretch(0, 1);
    pPreviewLayout->setColumnStretch(2, 1);

    auto pVertSeparator = new QFrame(this);
    pVertSeparator->setFrameStyle(QFrame::VLine | QFrame::Plain);
    pVertSeparator->setLineWidth(2);

    auto pTopLayout = new QHBoxLayout;
    pTopLayout->addLayout(pPrinterInfoLayout);
    pTopLayout->addWidget(pVertSeparator);
    pTopLayout->addLayout(pPreviewLayout);

    setLayout(pTopLayout);
}

//
//
void PrintDialog::connectSignalsAndSlots()
{
    connect(m_pOkButton, SIGNAL(clicked()), this, SLOT(accepted()));
    connect(m_pCancelButton, SIGNAL(clicked()), this, SLOT(rejected()));
}

//
//
void PrintDialog::accepted()
{
    QVariant tag = m_pPrinterSelect->itemData(m_pPrinterSelect->currentIndex());
    if (tag == QVariant("ps")) {
        QString path = QFileDialog::getSaveFileName(this, tr("Save File"), QDir::currentPath(),
                                                    tr("Postscript files (*.ps)"));
        if (path.isEmpty()) {
            return;
        } else {
                m_pCanvas->getCanvas()->Print(path.toUtf8().constData(), "ps");
        }
    } else if (tag == QVariant("pdf")) {
        QString path = QFileDialog::getSaveFileName(this, tr("Save File"), QDir::currentPath(),
                                                    tr("Pdf files (*.pdf)"));
        if (path.isEmpty()) {
            return;
        } else {
            m_pCanvas->getCanvas()->Print(path.toUtf8().constData(), "pdf");
        }
    } else {
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

        // clean up the file
        std::remove("_81239azsdfaw__tmp_print__.ps");
    }

    QDialog::accept();
}

//
//
void PrintDialog::rejected()
{
    QDialog::reject();
}


} // end Viewer namespace
