#include "LayoutDialogChooser.h"
#include "ui_LayoutDialogChooser.h"
#include "Logo.h"

#include <QButtonGroup>
#include <QFileDialog>
#include <QDir>
#include <QKeyEvent>

namespace Viewer {

LayoutDialogChooser::LayoutDialogChooser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LayoutDialogChooser),
    m_pButtons(new QButtonGroup(this))
{
    ui->setupUi(this);

    auto hLogoLayout = new QHBoxLayout(this);
    hLogoLayout->addStretch();
    hLogoLayout->addWidget(new Logo(this));
    hLogoLayout->addStretch();

    ui->pVerticalLayout->insertLayout(0, hLogoLayout);

    m_pButtons->addButton(ui->pBulkButton, BULK);
    m_pButtons->addButton(ui->pManualButton, MANUAL);
    m_pButtons->addButton(ui->pLoadFileButton, FILE);

    ui->pContinueButton->setEnabled(false);
    ui->pTabNameEdit->setFocus();

    connect(ui->pContinueButton, SIGNAL(clicked()), this, SLOT(onContinue()));
    connect(ui->pTabNameEdit, SIGNAL(textChanged(const QString&)),
            this, SLOT(onTabNameChanged(const QString&)));
}

LayoutDialogChooser::~LayoutDialogChooser()
{
    delete ui;
}

void LayoutDialogChooser::onContinue()
{
    int selection = m_pButtons->checkedId();
    switch (selection) {
    case BULK:
        emit bulkClicked();
        break;
    case MANUAL:
        emit sequentialClicked();
        break;
    case FILE:
    {
        QString fileName
                = QFileDialog::getOpenFileName(this,
                                               tr("Open File"),
                                               QDir::currentPath(),
                                               tr("Image Files (*.root *.win)"));

        emit loadFileClicked( fileName );
        break;
    }
    }
}

void LayoutDialogChooser::onTabNameChanged(const QString &newText)
{
    ui->pContinueButton->setEnabled( ! newText.isEmpty() );
}

QString LayoutDialogChooser::getTabName() const
{
    return ui->pTabNameEdit->text();
}

void LayoutDialogChooser::keyPressEvent(QKeyEvent *pEvent)
{
    if (pEvent->key()==Qt::Key_Return || pEvent->key()==Qt::Key_Enter) {
        onContinue();
    }
}


} // end Viewer namespace
