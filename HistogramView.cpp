//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2015.
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

static const char* Copyright = "(C) Copyright Michigan State University 2015, All rights reserved";
#include "HistogramView.h"
#include "ui_HistogramView.h"
#include <ListRequestHandler.h>
#include <TH1.h>
#include <QList>
#include <vector>
#include <HistInfo.h>
#include <HistFactory.h>
#include <HistogramList.h>
#include <HistogramBundle.h>
#include <QTimer>

HistogramView::HistogramView(QWidget *parent) :
    QDockWidget(tr("Histograms"),parent),
    ui(new Ui::HistogramView),
    m_req(new ListRequestHandler(this))
{
  m_req->setHistogramView(this);
    ui->setupUi(this);

    QTimer::singleShot(1000,this,SLOT(onUpdate()));
    connect(m_req,SIGNAL(parseCompleted(std::vector<SpJs::HistInfo>)),
            this,SLOT(setList(std::vector<SpJs::HistInfo>)));

    connect(ui->histList,SIGNAL(doubleClicked(QModelIndex)),
            this,SLOT(onDoubleClick(QModelIndex)));
}

HistogramView::~HistogramView()
{
    deleteHists();
    delete ui;
}

void HistogramView::onUpdate()
{
    m_req->get();
}

void HistogramView::setList(std::vector<SpJs::HistInfo> names)
{
    SpJs::HistFactory factory;
//    ui->histList->clear();
    auto iter = names.begin();
    auto end = names.end();

    while (iter!=end) {

        SpJs::HistInfo& info = (*iter);
        QString name = QString::fromStdString(info.s_name);
        if (!histExists(name)) {

            // this should  get a unique ptr, we want to get the raw pointer
            // and strip the unique_ptr of its ownership (i.e. release())
            auto upHist = factory.create(info);
            TH1* pHist = upHist.get();
            upHist.release();
            HistogramList::getInstance()->addHist(*pHist);

            // Histograms are uniquely named, so we can use the name as the key
            QString name = QString::fromStdString((*iter).s_name);
            auto item = new QListWidgetItem(name, ui->histList, 
                                            QListWidgetItem::UserType);

            item->setData(Qt::UserRole,QVariant(name));

            QSize geo = ui->histList->size();
            ui->histList->insertItem(geo.height(), item);

        }

      ++iter;
    }

    QTimer::singleShot(1000,this,SLOT(onUpdate()));
}

void HistogramView::onDoubleClick(QModelIndex index)
{
    QString hname = index.data(Qt::UserRole).toString();
    HistogramBundle* gHist = HistogramList::getInstance()->getHist(hname);
    emit histSelected(gHist);
}


bool HistogramView::histExists(const QString& name)
{
    size_t nRows = ui->histList->count();
    for(size_t  entry=0; entry<nRows; ++entry) {
        QListWidgetItem* item = ui->histList->item(entry);
        if (item->text() == name) {
            return true;
        }
    }
    return false;
}

void HistogramView::deleteHists()
{
    size_t nEntries = ui->histList->count();
    for(size_t  entry=0; entry<nEntries; ++entry) {
        QListWidgetItem* item = ui->histList->item(entry);
        TH1* hist = (TH1*)(item->data(Qt::UserRole).value<void*>());
        delete hist;
    }
}

