#include "ConfigCopySelector.h"
#include "SpectrumView.h"
#include "QRootCanvas.h"
#include "SelectableImage.h"

#include <QGridLayout>
#include <QLabel>
#include <QPixmap>
#include <QImage>
#include <QWizardPage>
#include <QSize>
#include <QCheckBox>
#include <QApplication>
#include <QDesktopWidget>

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

    auto pDirections = new QLabel(tr("<h1>Click on the histogram whose configuration you want to copy</h2>"),
    							  this);

    QVBoxLayout* pVertLayout = new QVBoxLayout;

    QGridLayout* pLayoutOfImages = createDummyDisplay(m_sourceGroup, true);

    pVertLayout->addWidget(pDirections);
    pVertLayout->addLayout(pLayoutOfImages);

    pPage->setLayout(pVertLayout);

    return pPage;
}

QGridLayout* ConfigCopySelector::createDummyDisplay(QButtonGroup& group, bool autoExclusive)
{
    auto pGridLayout = new QGridLayout;

    int nCols = m_pView->getColumnCount();
    int nRows = m_pView->getRowCount();

    int id = 0;
    for (int row=0; row<nRows; ++row) {
        for (int col=0; col<nCols; ++col) {
            QRootCanvas* pCanvas = m_pView->getCanvas(row, col);

            // skip rendering if there is nothing
            if (pCanvas == nullptr) continue;

            QPixmap pixmap(pCanvas->size());
            pixmap.fill(Qt::gray);
            std::vector<TH1*> hists = SpectrumView::getAllHists(pCanvas);
            if (hists.size() > 0) {
            	auto pImage = new SelectableImage(pixmap,
            								      QString::fromAscii(hists.at(0)->GetName()),
            								      this);
            	pImage->setAutoExclusive(autoExclusive);
            	group.addButton(pImage, id);
            	++id; // increment button id count

            	pGridLayout->addWidget(pImage, row, col);
            }
        }
    }

    return pGridLayout;
}


QWizardPage* ConfigCopySelector::createSelectDestinationPage()
{
	auto pPage = new QWizardPage;

	auto pDirections = new QLabel(tr("<h2>Click on all of the spectra to copy attributes to:</h2>"), this);

	QVBoxLayout* pVertLayout = new QVBoxLayout;

	m_destinationGroup.setExclusive(false);

	QGridLayout* pLayoutOfImages = createDummyDisplay(m_destinationGroup, false);

	auto pSelectAll = new QCheckBox("Select all", pPage);
	connect(pSelectAll, SIGNAL(clicked(bool)), this, SLOT(setDestinationsChecked(bool)));

	pVertLayout->addWidget(pDirections);
	pVertLayout->addLayout(pLayoutOfImages);
	pVertLayout->addWidget(pSelectAll);

	pPage->setLayout(pVertLayout);

	return pPage;

}

void ConfigCopySelector::setDestinationsChecked(bool checked)
{
	std::cout << "setDestinationsChecked " << checked << std::endl;
	QList<QAbstractButton*> buttons = m_destinationGroup.buttons();
	int nButtons = buttons.count();
	for (int id=0; id<nButtons; ++id) {
		buttons[id]->setChecked(checked);
	}

}

QWizardPage* ConfigCopySelector::createSelectConfigOptionsPage()
{
	auto pPage = new QWizardPage;

	auto pDirections = new QLabel(tr("<h2>Choose the attributes you want to copy</h2>"), this);

	auto pVertLayout = new QVBoxLayout;
	pVertLayout->addWidget(pDirections);

	m_pXAxisOption = new QCheckBox("X axis range", this);
	pVertLayout->addWidget(m_pXAxisOption);
	m_pYAxisOption = new QCheckBox("Y axis range", this);
	pVertLayout->addWidget(m_pYAxisOption);

	m_pDrawOption = new QCheckBox("Default draw option", this);
	pVertLayout->addWidget(m_pDrawOption);


	auto pSelectAll = new QCheckBox("Select all", this);
	connect(pSelectAll, SIGNAL(clicked(bool)), this, SLOT(setCheckedOptions(bool)));

	pVertLayout->addWidget(pSelectAll);
	pPage->setLayout(pVertLayout);

	return pPage;
}

void ConfigCopySelector::setCheckedOptions(bool checked)
{
	std::cout << "setCheckedOptions " << checked << std::endl;
	m_pXAxisOption->setChecked(checked);
	m_pYAxisOption->setChecked(checked);
	m_pDrawOption->setChecked(checked);
}

void ConfigCopySelector::accept()
{
	QWizard::accept();
}

ConfigCopySelection ConfigCopySelector::getSelection() const
{
	ConfigCopySelection selection;

	QAbstractButton* pButton = m_sourceGroup.checkedButton();
	if (pButton) selection.s_sourceHist = pButton->text();
	else return selection;

	QList<QAbstractButton*> destButtons = m_destinationGroup.buttons();
	for (int id=0; id<destButtons.size(); ++id) {
		if (destButtons[id]->isChecked()) {
			selection.s_destinationHists.push_back(destButtons[id]->text());
		}
	}

	selection.s_copyXAxis 	   = m_pXAxisOption->isChecked();
	selection.s_copyYAxis      = m_pYAxisOption->isChecked();
	selection.s_copyDrawOption = m_pDrawOption->isChecked();

	return selection;
}


} // end VIewer namespace
