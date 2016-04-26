#include "SpectrumLayoutDialog.h"
#include "LayoutDialogChooser.h"
#include "BulkDrawChooser.h"
#include "SpecTclInterface.h"
#include "HistogramList.h"

#include <QHBoxLayout>

namespace Viewer
{

SpectrumLayoutDialog::SpectrumLayoutDialog(std::shared_ptr<SpecTclInterface> pSpecTcl,
                                           QWidget *parent) :
    QWidget(parent),
    m_pSpecTcl(pSpecTcl),
    m_pMainChooser(new LayoutDialogChooser(this)),
    m_pBulkChooser(new BulkDrawChooser(*pSpecTcl->getHistogramList(), this))
{
    m_pBulkChooser->hide();

    auto pLayout = new QHBoxLayout;
    pLayout->addWidget(m_pMainChooser);
    pLayout->addWidget(m_pBulkChooser);
    setLayout(pLayout);

    connect(m_pMainChooser, SIGNAL(bulkClicked()), this, SLOT(onBulkDrawSelected()));
    connect(m_pBulkChooser, SIGNAL(backClicked()),
            this, SLOT(onReturnToMainSelected()));
    connect(m_pBulkChooser, SIGNAL(draw(QStringList)),
            this, SLOT(onSelectionComplete(QStringList)));
    connect(m_pMainChooser, SIGNAL(sequentialClicked()),
            this, SLOT(onSequentialDrawSelected()));
    connect(m_pMainChooser, SIGNAL(loadFileClicked(QString)),
            this, SLOT(onLoadFileSelected(QString)));
}

void SpectrumLayoutDialog::onBulkDrawSelected()
{
    m_pMainChooser->hide();
    m_pBulkChooser->setHistogramList(m_pSpecTcl->getHistogramList()->histNames());
    m_pBulkChooser->show();
}

void SpectrumLayoutDialog::onSequentialDrawSelected()
{
    emit spectraChosenToDraw(QStringList());
}

void SpectrumLayoutDialog::onReturnToMainSelected()
{
    m_pBulkChooser->hide();
    m_pMainChooser->show();
}


void SpectrumLayoutDialog::onSelectionComplete(QStringList selection)
{
    emit spectraChosenToDraw(selection);
}

QString SpectrumLayoutDialog::getTabName() const {
    return m_pMainChooser->getTabName();
}

void SpectrumLayoutDialog::onLoadFileSelected(QString fileName)
{
    if (!fileName.isEmpty()) {
        emit loadFileChosen(fileName);
    }
}




} // end Viewer namespace
