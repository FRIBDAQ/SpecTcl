#include "SaveToRootDialog.h"
#include "ui_SaveToRootDialog.h"
#include "RootFileWriter.h"
#include "WinFileWriter.h"
#include "TabbedMultiSpectrumView.h"
#include "SpecTclInterface.h"
#include "TabWorkspace.h"

#include <QFileDialog>
#include <QCheckBox>
#include <QDir>
#include <QMessageBox>
#include <QDebug>
#include <QCommonStyle>
#include <QRegExp>

#include <iostream>

namespace Viewer
{

SaveToRootDialog::SaveToRootDialog(TabbedMultiSpectrumView& tabWidget,
                                   std::shared_ptr<SpecTclInterface> pSpecTcl,
                                   QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaveToRootDialog),
    m_tabWidget(tabWidget),
    m_checkBoxes(),
    m_pSpecTcl(pSpecTcl)
{
    ui->setupUi(this);

    setUpWidget();

    connect(ui->pSaveButton, SIGNAL(clicked()), this, SLOT(onAccepted()));
    connect(ui->pCancelButton, SIGNAL(clicked()), this, SLOT(onRejected()));
    connect(ui->pBrowseButton, SIGNAL(clicked()), this, SLOT(onBrowse()));
    connect(ui->pOutputPath, SIGNAL(textChanged(const QString&)),
            this, SLOT(onPathEdited(const QString&)));
    connect(ui->pWinSelector, SIGNAL(stateChanged(int)), this, SLOT(updateSaveButtonState()));
    connect(ui->pROOTSelector, SIGNAL(stateChanged(int)), this, SLOT(updateSaveButtonState()));
}

SaveToRootDialog::~SaveToRootDialog()
{
    delete ui;
}

void SaveToRootDialog::setUpWidget()
{
    QCommonStyle style;
    ui->pSaveButton->setEnabled(false);
    ui->pBrowseButton->setIcon(style.standardIcon(QStyle::SP_DirIcon));

    int insertionRow = 0;
    QStringList tabNames = m_tabWidget.getTabNames();
    for (size_t row =0; row<tabNames.size(); ++row) {
        auto pCheckBox = new QCheckBox(tabNames.at(row), this);
        // b/c we are only constructing this once, the column count should reflect the
        // number of actual visible columns
        if (insertionRow == 0) {
            ui->formLayout->insertRow(insertionRow, QString("<h2>Select tabs to save:</h2>"), pCheckBox);
        } else {
            ui->formLayout->insertRow(insertionRow, nullptr, pCheckBox);
        }

        m_checkBoxes.push_back(pCheckBox);

        insertionRow++;
    }

    m_pSelectAllCheckBox = new QCheckBox("Select all", this);
    ui->formLayout->insertRow(insertionRow, nullptr, m_pSelectAllCheckBox);

    connect(m_pSelectAllCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onSelectAll()));

}



void SaveToRootDialog::onAccepted()
{


    if (ui->pROOTSelector->isChecked()) {
        writeToRootFile();
    }

    if (ui->pWinSelector->isChecked()) {
        writeToWinFile();
    }

    if ( !ui->pROOTSelector->isChecked() && !ui->pWinSelector->isChecked()) {
        QMessageBox::warning(this, "Save to file error", "Please select at least one file type before accepting");
        QDialog::reject();
    } else {
        QDialog::accept();
    }
}

void SaveToRootDialog::writeToRootFile()
{
    QString outputPath = ui->pOutputPath->text();
    if (outputPath.lastIndexOf(QRegExp(".root$")) == -1) {
        outputPath.append(".root");
    }

    RootFileWriter writer(m_pSpecTcl);
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
}

int SaveToRootDialog::getTabSelectedCount()
{
    int count = 0;
    for (auto pCheckBox : m_checkBoxes) {
        if (pCheckBox->isChecked()) {
            count++;
        }
    }
    return count;
}

void SaveToRootDialog::writeToWinFile()
{

    WinFileWriter writer;

    QString path = ui->pOutputPath->text();

    if (path.lastIndexOf(QRegExp(".win$")) == -1) {
        path.append(".win");
    }

    bool appendIndices = false;
    int nTabsToWrite = getTabSelectedCount();
    if ( nTabsToWrite > 1 ) {
        appendIndices = true;
    }


    for (auto pCheckBox : m_checkBoxes) {
        if (pCheckBox->isChecked()) {
            TabWorkspace* pWorkspace = m_tabWidget.getTabWorkspace(pCheckBox->text());
            if (pWorkspace) {
                QString outputPath = formOutputPath(path, pWorkspace->objectName());
                writer.writeTab(*pWorkspace, outputPath);
            } else {
                QString msg ("The requested tab (%1) does not exist. Aborting save operation.");
                QMessageBox::warning(this, "Export failure",
                                     msg.arg(pCheckBox->text()));
                return;
            }
        }
    }
}

void SaveToRootDialog::onRejected()
{
    QDialog::reject();
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

void SaveToRootDialog::onPathEdited(const QString &)
{
    updateSaveButtonState();
}


QString SaveToRootDialog::formOutputPath(const QString &user_path, const QString &tabName)
{
    QString path = user_path;

    int periodIndex = path.lastIndexOf(".");
    if (periodIndex != -1) {
        path.insert(periodIndex, QString("_") + tabName);
    } else {
        path += QString("_") + tabName + ".win";
    }

    return path;
}

void SaveToRootDialog::updateSaveButtonState()
{
    if ((ui->pWinSelector->isChecked() || ui->pROOTSelector->isChecked())
            && !ui->pOutputPath->text().isEmpty()) {
        ui->pSaveButton->setDisabled(false);
    } else {
        ui->pSaveButton->setDisabled(true);
    }
}

} // end Viewer namespace
