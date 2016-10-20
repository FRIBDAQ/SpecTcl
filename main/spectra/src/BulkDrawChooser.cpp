//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2016.
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    Authors:
//    Jeromy Tompkins
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321

#include "BulkDrawChooser.h"
#include "ui_BulkDrawChooser.h"
#include "HistogramList.h"

#include <QRegExp>
#include <QListWidget>
#include <QListWidgetItem>

#include <iostream>

namespace Viewer
{

BulkDrawChooser::BulkDrawChooser(HistogramList &rHistList, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BulkDrawChooser),
    m_rHistList(rHistList),
    m_selectAll(false)
{
    ui->setupUi(this);

    // populate the histogram list
    setHistogramList(m_rHistList.histNames());

    ui->pHistList->setFocusProxy(ui->pFilterEdit);
    ui->pFilterEdit->setFocus();


    // connect all of the signals and slots
    connect(ui->pBackButton, SIGNAL(clicked()), this, SLOT(onBackClicked()));
    connect(ui->pDrawButton, SIGNAL(clicked()), this, SLOT(onDrawClicked()));
    connect(ui->pFilterEdit, SIGNAL(textEdited(const QString&)),
            this, SLOT(reapplyFilter(const QString&)));
    connect(ui->pFilterEdit, SIGNAL(returnPressed()), this, SLOT(onDrawClicked()));
}

BulkDrawChooser::~BulkDrawChooser()
{
    delete ui;
}

void BulkDrawChooser::onBackClicked()
{
    emit backClicked();
}


void BulkDrawChooser::hideEvent(QHideEvent *pEvent) {
    releaseKeyboardFromFilter();
}


void BulkDrawChooser::showEvent(QShowEvent *pEvent) {
    grabKeyboardForFilter();
}

void BulkDrawChooser::grabKeyboardForFilter() {
    ui->pFilterEdit->grabKeyboard();
}

void BulkDrawChooser::releaseKeyboardFromFilter() {
    ui->pFilterEdit->releaseKeyboard();
}

void BulkDrawChooser::onDrawClicked()
{
    QList<QListWidgetItem*> selection = ui->pHistList->selectedItems();

    QStringList histsToDraw;
    for (auto& pItem : selection) {
        histsToDraw.push_back(pItem->text());
    }

    emit draw(histsToDraw);
}

void BulkDrawChooser::setHistogramList(const QList<QString>& histNames)
{
    QString filter = ui->pFilterEdit->text();

    clearList();

    QRegExp matchPattern;
    matchPattern.setPatternSyntax(QRegExp::Wildcard);
    if (filter.isEmpty()) {
        filter = "*";
    } else {
        if (filter.at(filter.count()-1) != QChar('*')) {
            filter += "*";
        }
    }
    matchPattern.setPattern(filter);

    for (auto& name : histNames) {

        if (matchPattern.exactMatch(name)) {
            auto pItem = new QListWidgetItem(name, ui->pHistList);
            pItem->setSelected(m_selectAll);
            ui->pHistList->addItem(pItem);

        }
    }
}

void BulkDrawChooser::clearList()
{
    QListWidgetItem* pItem;
    while (( pItem = ui->pHistList->takeItem(0) )) {
        delete pItem;
    }
}

void BulkDrawChooser::reapplyFilter(const QString &filter)
{
    m_selectAll = true;
    setHistogramList(m_rHistList.histNames());
}

} // end of Viewer namespace
