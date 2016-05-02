#include "SaveToRootDialog.h"
#include "ui_SaveToRootDialog.h"
#include "RootFileWriter.h"
#include "TabbedMultiSpectrumView.h"

#include <QFileDialog>
#include <QCheckBox>
#include <QDir>
#include <QMessageBox>

#include <iostream>

namespace Viewer
{

SaveToRootDialog::SaveToRootDialog(TabbedMultiSpectrumView& tabWidget, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SaveToRootDialog),
    m_tabWidget(tabWidget),
    m_checkBoxes()
{
    ui->setupUi(this);

    setUpWidget();

    connect(ui->pSaveButton, SIGNAL(clicked()), this, SLOT(onAccepted()));
    connect(ui->pCancelButton, SIGNAL(clicked()), this, SLOT(onRejected()));
    connect(ui->pBrowseButton, SIGNAL(clicked()), this, SLOT(onBrowse()));
}

SaveToRootDialog::~SaveToRootDialog()
{
    delete ui;
}

void SaveToRootDialog::setUpWidget()
{
    int insertionRow = 1;
    QStringList tabNames = m_tabWidget.getTabNames();
    for (size_t row =0; row<tabNames.size(); ++row) {
        auto pCheckBox = new QCheckBox(tabNames.at(row), this);
        // b/c we are only constructing this once, the column count should reflect the
        // number of actual visible columns
        ui->verticalLayout->insertWidget(insertionRow, pCheckBox);

        m_checkBoxes.push_back(pCheckBox);

        insertionRow++;
    }

    m_pSelectAllCheckBox = new QCheckBox("Select all", this);
    ui->verticalLayout->insertWidget(insertionRow, m_pSelectAllCheckBox);

    connect(m_pSelectAllCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onSelectAll()));
}

void SaveToRootDialog::onAccepted()
{

    QString outputPath = ui->pOutputPath->text();

    RootFileWriter writer;
    writer.openFile(outputPath, QString("UPDATE"));

    for (auto pCheckBox : m_checkBoxes) {
        if (pCheckBox->isChecked()) {
            TabWorkspace* pWorkspace = m_tabWidget.getTabWorkspace(pCheckBox->text());
            if (pWorkspace) {
                writer.writeTab(*pWorkspace, true);
            } else {
                QString msg ("The requested tab (%1) does not exist. Aborting save operation.");
                QMessageBox::warning(this, "Export failure",
                                     msg.arg(pCheckBox->text()));
                return;
            }
        }
    }

    emit accepted();
}

void SaveToRootDialog::onRejected()
{
    emit rejected();
}

void SaveToRootDialog::onBrowse()
{
    QString path = QFileDialog::getSaveFileName(this,"Choose output file name",
                                                QDir::currentPath());

    if (!path.isEmpty()) {
        ui->pOutputPath->setText(path);
    }
}

void SaveToRootDialog::onSelectAll()
{
    bool checked = m_pSelectAllCheckBox->isChecked();
    for (auto pCheckBox : m_checkBoxes) {
        pCheckBox->setChecked(checked);
    }
}

} // end Viewer namespace
