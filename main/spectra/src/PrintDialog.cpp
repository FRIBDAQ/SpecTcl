#include "PrintDialog.h"
#include "RootFileWriter.h"
#include "SpecTclInterface.h"
#include "TabWorkspace.h"
#include "TabbedMultiSpectrumView.h"
#include "QRootCanvas.h"
#include "TCanvas.h"

#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QFile>
#include <QTextStream>

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

void PrintDialog::assembleWidgets()
{
    m_pPrinterLabel = new QLabel(tr("Printer"), this);
    m_pPrinterLabel->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
    m_pPrinterSelect = new QComboBox(this);
    m_pPrinterSelect->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));

    // Create a file that contains all of the printers we know about
    std::system("lpstat -a | awk '{print $1}' > .__temp_printers.txt");

    // Read in the file
    QFile file(".__temp_printers.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_pPrinterSelect->addItem(tr("lp"));
    } else {
        QTextStream stream(&file);
        while (1) {
            QString printerName = stream.readLine();

            if (stream.atEnd()) break;

            m_pPrinterSelect->addItem(printerName);
        }
    }

    std::remove(".__temp_printers.txt");

    auto pPrinterInfoLayout = new QHBoxLayout;
    pPrinterInfoLayout->addWidget(m_pPrinterLabel);
    pPrinterInfoLayout->addWidget(m_pPrinterSelect);

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
    m_pCanvas->setMinimumWidth(100);
    m_pCanvas->setMinimumHeight(120);

    m_pCancelButton = new QPushButton(tr("&Cancel"), this);
    m_pOkButton = new QPushButton(tr("&Ok"), this);
    m_pOkButton->setAutoDefault(true);
    m_pOkButton->setDefault(true);

    auto pButtonLayout = new QHBoxLayout;
    pButtonLayout->addSpacerItem(new QSpacerItem(30,20));
    pButtonLayout->addWidget(m_pCancelButton);
    pButtonLayout->addWidget(m_pOkButton);

    auto pVLayout = new QVBoxLayout;
    pVLayout->addLayout(pPrinterInfoLayout);
    pVLayout->addWidget(m_pPreviewLabel);
    pVLayout->addWidget(m_pCanvas);
    pVLayout->addLayout(pButtonLayout);

    setLayout(pVLayout);
}

void PrintDialog::connectSignalsAndSlots()
{
    connect(m_pOkButton, SIGNAL(clicked()), this, SLOT(accepted()));
    connect(m_pCancelButton, SIGNAL(clicked()), this, SLOT(rejected()));
}

void PrintDialog::accepted()
{

    m_pCanvas->getCanvas()->Print("_81239azsdfaw__tmp_print__.ps", "ps");

    QString printer = m_pPrinterSelect->itemText(m_pPrinterSelect->currentIndex());
    QString printCmdTemplate("lp -d %1 _81239azsdfaw__tmp_print__.ps");
    QString printCmd = printCmdTemplate.arg(printer);

    std::cout << "Printing command = '" << printCmd.toStdString() << "'" << std::endl;
    std::system(printCmd.toUtf8().constData());

    emit accept();
}

void PrintDialog::rejected()
{
    emit reject();
}


} // end Viewer namespace
