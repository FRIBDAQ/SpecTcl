#include "SaveAsDialog.h"
#include "ui_SaveAsDialog.h"
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

SaveAsDialog::SaveAsDialog(TabbedMultiSpectrumView& tabWidget,
                                   std::shared_ptr<SpecTclInterface> pSpecTcl,
                                   QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaveAsDialog),
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

SaveAsDialog::~SaveAsDialog()
{
    delete ui;
}

/*!
 * \brief Add the tab checkboxes to the widget
 *
 * The bulk of the GUI layout is defined in the SaveAsDialog.ui file as xml. However,
 * that layout does not add the tab check boxes because those are only knowable at
 * runtime.
 *
 */
void SaveAsDialog::setUpWidget()
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



void SaveAsDialog::onAccepted()
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

/*!
 * \brief SaveAsDialog::writeToRootFile
 *
 * The logic for the writing of ROOT files is defined mostly in
 * the RootFileWriter class. Here we simply create an instance of it
 * and then feed it all of the tabs.
 */
void SaveAsDialog::writeToRootFile()
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
                writer.writeTab(*pWorkspace);
            } else {
                QString msg ("The requested tab (%1) does not exist. Aborting save operation.");
                QMessageBox::warning(this, "Export failure",
                                     msg.arg(pCheckBox->text()));
                return;
            }
        }
    }
}

int SaveAsDialog::getTabSelectedCount()
{
    int count = 0;
    for (auto pCheckBox : m_checkBoxes) {
        if (pCheckBox->isChecked()) {
            count++;
        }
    }
    return count;
}

void SaveAsDialog::writeToWinFile()
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

void SaveAsDialog::onRejected()
{
    QDialog::reject();
}

void SaveAsDialog::onBrowse()
{
    QString path = QFileDialog::getSaveFileName(this,"Choose output file name",
                                                QDir::currentPath());

    if (!path.isEmpty()) {
        ui->pOutputPath->setText(path);
    }
}

void SaveAsDialog::onSelectAll()
{
    bool checked = m_pSelectAllCheckBox->isChecked();
    for (auto pCheckBox : m_checkBoxes) {
        pCheckBox->setChecked(checked);
    }
}

void SaveAsDialog::onPathEdited(const QString &)
{
    updateSaveButtonState();
}


QString SaveAsDialog::formOutputPath(const QString &user_path, const QString &tabName)
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

void SaveAsDialog::updateSaveButtonState()
{
    if ((ui->pWinSelector->isChecked() || ui->pROOTSelector->isChecked())
            && !ui->pOutputPath->text().isEmpty()) {
        ui->pSaveButton->setDisabled(false);
    } else {
        ui->pSaveButton->setDisabled(true);
    }
}

} // end Viewer namespace
