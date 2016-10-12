#include "MultiInfoPanel.h"
#include "ui_MultiInfoPanel.h"

#include "QRootCanvas.h"
#include "TabWorkspace.h"
#include "SpecTclInterface.h"
#include "HistogramList.h"
#include "HistogramBundle.h"
#include "InformationPanel.h"
#include "SpectrumView.h"

#include <QFrame>
#include <QString>

namespace Viewer
{

MultiInfoPanel::MultiInfoPanel(TabWorkspace& rView,
                               std::shared_ptr<SpecTclInterface> pSpecTcl,
                               QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MultiInfoPanel),
    m_workspace(rView),
    m_pSpecTcl(pSpecTcl),
    m_nValidTabs(0)
{
    ui->setupUi(this);
}

MultiInfoPanel::~MultiInfoPanel()
{
    delete ui;
}

void MultiInfoPanel::setSpecTclInterface(std::shared_ptr<SpecTclInterface> pSpecTcl)
{
    m_pSpecTcl = pSpecTcl;

    for (int i=0; i<ui->pTabWidget->count(); ++i) {
        auto pPanel = dynamic_cast<InformationPanel*>(ui->pTabWidget->widget(i));
        if (pPanel) {
            pPanel->setSpecTclInterface(m_pSpecTcl);
        }

    }
}

void MultiInfoPanel::currentCanvasChanged(QRootCanvas &rCanvas)
{

    auto hists = SpectrumView::getAllHists(&rCanvas);

    removeAllTabs();

    if (hists.size() == 0) {
        addEmptyTab();
    } else {
        HistogramList* pHistList = m_pSpecTcl->getHistogramList();
        for (auto& pHist : hists) {
            auto pHistPkg = pHistList->getHistFromClone(pHist);
            if (pHistPkg) {
                addTab(*pHistPkg);
            }
        }
    }
}

void MultiInfoPanel::updateContent(QRootCanvas &rCanvas)
{

    std::cout << "MultiInfoPanel::updateContent" << std::endl;

    auto hists = SpectrumView::getAllHists(&rCanvas);

    HistogramList* pHistList = m_pSpecTcl->getHistogramList();
    for (auto& pHist : hists) {
        auto pHistPkg = pHistList->getHistFromClone(pHist);
        if (pHistPkg) {
            int index = findTab(pHistPkg->getName());
            if (index >= 0) {
                auto pPanel = dynamic_cast<InformationPanel*>(ui->pTabWidget->widget(index));
                if (pPanel) {
                    pPanel->onHistogramChanged(*pHistPkg);
                }
            }
        }
    }
}


int MultiInfoPanel::findTab(const QString& name) {

    int nTabs = ui->pTabWidget->count();
    int index = 0;
    while (index < nTabs) {
        if (name == ui->pTabWidget->tabText(index)) {
            break;
        }
        index++;
    }

    if (index == nTabs) {
        index = -1;
    }

    return index;

}

void MultiInfoPanel::addEmptyTab()
{
    QFrame* pFrame = new QFrame(this);
    ui->pTabWidget->addTab(pFrame, QString());
}


void MultiInfoPanel::addTab(HistogramBundle& rBundle)
{
    InformationPanel* pPanel = new InformationPanel(m_workspace, m_pSpecTcl,
                                                    rBundle.getName(), this);
    pPanel->onHistogramChanged(rBundle);
    ui->pTabWidget->addTab(pPanel, rBundle.getName());
    m_nValidTabs++;
}

void MultiInfoPanel::removeAllTabs()
{
    int nTabs = ui->pTabWidget->count();
    for (int i=0; i<nTabs; ++i) {
        ui->pTabWidget->removeTab(0);
    }

    m_nValidTabs = 0;
}

} // end Viewer namespace
