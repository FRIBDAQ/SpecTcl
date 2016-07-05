#include "ConfigCopySelector.h"
#include "SpectrumView.h"
#include "QRootCanvas.h"
#include "SelectableImage.h"
#include "SpecTclInterface.h"
#include "HistogramList.h"
#include "HistogramBundle.h"

#include <QGridLayout>
#include <QLabel>
#include <QPixmap>
#include <QImage>
#include <QWizardPage>
#include <QSize>
#include <QCheckBox>

#include <TH1.h>
#include <TClass.h>

#include <iostream>

namespace Viewer {

ConfigCopySelector::ConfigCopySelector(SpectrumView& rView, std::shared_ptr<SpecTclInterface> pSpecTcl,
                                       QWidget *parent) :
    QWizard(parent),
    m_pView(&rView),
    m_pSpecTcl(pSpecTcl)
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

    // Check the first button
    QList<QAbstractButton*> buttons = m_sourceGroup.buttons();
    if (buttons.count() > 0) {
        buttons[0]->setChecked(true);
    }

    pVertLayout->addWidget(pDirections);
    pVertLayout->addLayout(pLayoutOfImages);

    pPage->setLayout(pVertLayout);

    return pPage;
}

QGridLayout* ConfigCopySelector::createDummyDisplay(QButtonGroup& group,
                                                    bool autoExclusive)
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
	QList<QAbstractButton*> buttons = m_destinationGroup.buttons();

	int nButtons = buttons.count();
	for (int id=0; id<nButtons; ++id) {
        if (buttons[id]->isVisible()) {
            // the button is visible, so it is compatible

            buttons[id]->setChecked(checked);
        }
        // otherwise, the button is hidden because it is not compatible.
        // we don't want to check the hidden buttons.
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

/*!
 * \brief Disable destination canvases that don't match source spectrum
 * \param id    index of the page
 */
void ConfigCopySelector::showAllDestinationButtons()
{
    QList<QAbstractButton*> destButtons = m_destinationGroup.buttons();
    for (int id=0; id<destButtons.size(); ++id) {
        destButtons[id]->show();
    }
}

void ConfigCopySelector::initializePage(int id)
{
    if (id == 1) {
        QMap<QAbstractButton*,bool> compatibilityMap = getDestinationCompatibility();
        if (compatibilityMap.count() == 0) {
            showAllDestinationButtons();
        } else {
            QMapIterator<QAbstractButton*, bool> it(compatibilityMap);

            while (it.hasNext()) {
                it.next();
                QAbstractButton* pButton = it.key();

                if (it.value()) {
                    pButton->show();
                } else {
                    pButton->hide();
                }
            }
        }
    }
    QWizard::initializePage(id);
}


/*!
 * \brief Determine if histograms are compatible for attribute copy
 *
 * This is actually very restrictive. The user is not allowed to copy attributes unless
 * the source and destination have the same axis definitions. They must have the smae number
 * of bins as well as full range.
 *
 * \param pSourceBundle  source of attributes
 * \param pDestBundle    dest of attributes
 */
bool ConfigCopySelector::compatibleHists(HistogramBundle* pSourceBundle, HistogramBundle* pDestBundle)
{
    QString destType(pDestBundle->getHist().ClassName());
    QString sourceType(pSourceBundle->getHist().ClassName());

    int nSourceXBins = pSourceBundle->getHist().GetXaxis()->GetNbins();
    int nDestXBins = pDestBundle->getHist().GetXaxis()->GetNbins();

    int nSourceYBins = pSourceBundle->getHist().GetYaxis()->GetNbins();
    int nDestYBins = pDestBundle->getHist().GetYaxis()->GetNbins();

    double sourceXLow = pSourceBundle->getHist().GetXaxis()->GetXmin();
    double destXLow = pSourceBundle->getHist().GetXaxis()->GetXmin();

    double sourceXHigh = pSourceBundle->getHist().GetXaxis()->GetXmax();
    double destXHigh = pSourceBundle->getHist().GetXaxis()->GetXmax();

    double sourceYLow = pSourceBundle->getHist().GetYaxis()->GetXmin();
    double destYLow = pSourceBundle->getHist().GetYaxis()->GetXmin();

    double sourceYHigh = pSourceBundle->getHist().GetYaxis()->GetXmax();
    double destYHigh = pSourceBundle->getHist().GetYaxis()->GetXmax();

    return ((nSourceXBins == nDestXBins) && (nSourceYBins == nDestYBins)
            && (sourceXLow == destXLow) && (sourceXHigh == destXHigh)
            && (sourceYLow == destYLow) && (sourceYHigh == destYHigh));
}

QMap<QAbstractButton*, bool> ConfigCopySelector::getDestinationCompatibility()
{
    QMap<QAbstractButton*, bool> compatibilityMap;

    QAbstractButton* pButton = m_sourceGroup.checkedButton();
    QString sourceHistName = pButton->text();

    HistogramList* pList = m_pSpecTcl->getHistogramList();
    HistogramBundle* pSourceBundle = pList->getHist(sourceHistName);
    if (pSourceBundle == nullptr) {
        return compatibilityMap;
    }


    QList<QAbstractButton*> destButtons = m_destinationGroup.buttons();
    for (int id=0; id<destButtons.size(); ++id) {
        HistogramBundle* pDestBundle = pList->getHist(destButtons[id]->text());
        if (pDestBundle == nullptr) continue;

        compatibilityMap[destButtons[id]] = compatibleHists(pSourceBundle, pDestBundle);
    }

    return compatibilityMap;
}


} // end VIewer namespace
