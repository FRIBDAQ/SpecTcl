#include "ConfigCopySelector.h"
#include "SpectrumView.h"
#include "QRootCanvas.h"
#include "SelectableImage.h"

#include <QGridLayout>
#include <QLabel>
#include <QPixmap>
#include <QWizardPage>
#include <QSize>
#include <QCheckBox>

#include <TH1.h>

#include <iostream>

namespace Viewer {

ConfigCopySelector::ConfigCopySelector(SpectrumView& rView,
                                       QWidget *parent) :
    QWizard(parent),
    m_pView(&rView)
{
    addPage(createSelectSourcePage());
    addPage(createSelectDestinationPage());
    addPage(createSelectConfigOptionsPage());

    setWindowTitle(tr("Spectrum Attributes Copy Configuration Wizard"));
}

ConfigCopySelector::~ConfigCopySelector()
{
}


QWizardPage* ConfigCopySelector::createSelectSourcePage()
{
    auto pPage = new QWizardPage;

    auto pDirections = new QLabel(tr("Click on the histogram whose configuration you want to copy"),
    							  this);

    QVBoxLayout* pVertLayout = new QVBoxLayout;

    QGridLayout* pLayoutOfImages = createDummyDisplay();

    pVertLayout->addWidget(pDirections);
    pVertLayout->addLayout(pLayoutOfImages);

    pPage->setLayout(pVertLayout);

    return pPage;
}

QGridLayout* ConfigCopySelector::createDummyDisplay()
{
    auto pGridLayout = new QGridLayout;

    int nCols = m_pView->getColumnCount();
    int nRows = m_pView->getRowCount();

    for (int row=0; row<nRows; ++row) {
        for (int col=0; col<nCols; ++col) {
            QRootCanvas* pCanvas = m_pView->getCanvas(row, col);

            // skip rendering if there is nothing
            if (pCanvas == nullptr) continue;

            QPixmap pixmap(pCanvas->size());
            pCanvas->render(&pixmap);

            std::vector<TH1*> hists = SpectrumView::getAllHists(pCanvas);
            if (hists.size() > 0) {
            	auto pImage = new SelectableImage(pixmap,
            								      QString::fromAscii(hists.at(0)->GetName()),
            								      this);

            	pGridLayout->addWidget(pImage, row, col);
            }
        }
    }
    return pGridLayout;
}


QWizardPage* ConfigCopySelector::createSelectDestinationPage()
{
	auto pPage = new QWizardPage;

	auto pDirections = new QLabel(tr("Click on all of the spectra to copy attributes to:"), this);

	QVBoxLayout* pVertLayout = new QVBoxLayout;

	QGridLayout* pLayoutOfImages = createDummyDisplay();

	pVertLayout->addWidget(pDirections);
	pVertLayout->addLayout(pLayoutOfImages);

	pPage->setLayout(pVertLayout);

	return pPage;

}

QWizardPage* ConfigCopySelector::createSelectConfigOptionsPage()
{
	auto pPage = new QWizardPage;

	auto pDirections = new QLabel(tr("Choose the attributes you want to copy"), this);

	auto pVertLayout = new QVBoxLayout;
	pVertLayout->addWidget(pDirections);

	m_pXAxisOption = new QCheckBox("X axis range", this);
	pVertLayout->addWidget(m_pXAxisOption);
	m_pYAxisOption = new QCheckBox("Y axis range", this);
	pVertLayout->addWidget(m_pYAxisOption);

	m_pDrawOption = new QCheckBox("draw option", this);
	pVertLayout->addWidget(m_pDrawOption);


	pPage->setLayout(pVertLayout);

	return pPage;
}

} // end VIewer namespace
