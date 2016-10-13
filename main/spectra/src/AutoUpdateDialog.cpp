#include "AutoUpdateDialog.h"
#include "ui_AutoUpdateDialog.h"
#include "AutoUpdater.h"

#include <QButtonGroup>

namespace Viewer {

AutoUpdateDialog::AutoUpdateDialog(AutoUpdater& rUpdater, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AutoUpdateDialog),
    m_updater(rUpdater)
{
    ui->setupUi(this);

    m_pButtonGroup = new QButtonGroup(this);
    m_pButtonGroup->addButton(ui->pOnButton,  ON);
    m_pButtonGroup->addButton(ui->pOffButton, OFF);
    m_pButtonGroup->setExclusive(true);

    // set up the buttons to reflect the state of the updater
    if (m_updater.isActive()) {
        ui->pOnButton->setChecked(true);
    } else {
        ui->pOffButton->setChecked(true);
    }

    // convert interval from milliseconds to seconds
    ui->pPeriodSpinner->setValue(m_updater.getInterval()/1000);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(onAccepted()));
    connect(ui->buttonBox, SLOT(rejected()), this, SLOT(onRejected()));
}

AutoUpdateDialog::~AutoUpdateDialog()
{
    delete ui;
}


void AutoUpdateDialog::onAccepted()
{
    int buttonId = m_pButtonGroup->id(m_pButtonGroup->checkedButton());

    if (buttonId == ON) {
        int nSeconds = ui->pPeriodSpinner->value();
        m_updater.start(nSeconds);
    } else if (buttonId == OFF) {
        m_updater.stop();
    }
}

void AutoUpdateDialog::onRejected()
{
}

} // end Viewer namespace
