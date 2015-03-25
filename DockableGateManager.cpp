#include "DockableGateManager.h"
#include "ui_DockableGateManager.h"
#include "GateBuilderDialog.h"
#include "SpectrumViewer.h"
#include "QRootCanvas.h"
#include <QListWidget>
#include "TCutG.h"

DockableGateManager::DockableGateManager(const SpectrumViewer& viewer, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DockableGateManager),
    m_view(viewer)
{
    ui->setupUi(this);
    connect(ui->addButton,SIGNAL(clicked()),this,SLOT(launchAddGateDialog()));
}

DockableGateManager::~DockableGateManager()
{
    delete ui;
}

void DockableGateManager::launchAddGateDialog()
{
    auto pCanvas = m_view.getCurrentFocus();
    GateBuilderDialog* dialog = new GateBuilderDialog(*pCanvas);
    dialog->setAttribute(Qt::WA_DeleteOnClose);

    connect(dialog,SIGNAL(completed(TCutG*)),this,SLOT(registerGate(TCutG*)));
    connect(this, SIGNAL(finished(int)), this, SLOT(close()));

    dialog->show();
    dialog->raise();

}

void DockableGateManager::registerGate(TCutG* pCut)
{
    Q_ASSERT(pCut!=nullptr);

    QListWidgetItem* pItem = new QListWidgetItem(QString(pCut->GetName()),
                                                 ui->gateList,Qt::UserRole);
    QVariant var = QVariant::fromValue(reinterpret_cast<void*>(pCut));
    pItem->setData(Qt::UserRole, var);

    ui->gateList->addItem(pItem);
}


