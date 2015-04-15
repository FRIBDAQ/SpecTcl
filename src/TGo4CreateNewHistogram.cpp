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
// $Id: TGo4CreateNewHistogram.cpp 707 2011-01-24 13:09:24Z linev $
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum für Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TGo4CreateNewHistogram.h"


#include "GlobalSettings.h"
#include "HistogramList.h"
#include "ParameterCmdParser.h"
#include "ParameterInfo.h"
#include <QtNetwork>
#include <QMessageBox>
#include <QFile>
#include <json/json.h>
#include <stdexcept>
#include <iostream>
#include <memory>

TGo4CreateNewHistogram::TGo4CreateNewHistogram( QWidget* parent)
: QDialog( parent)
{
  m_pNAM = new QNetworkAccessManager(this);

  auto host = GlobalSettings::getServerHost();
  auto port = GlobalSettings::getServerPort();
  m_server = QString("http://%1:%2").arg(host).arg(port);

  setObjectName("Go4CreateNewHistogram");
  setupUi(this);
  histNameEdit->setPlaceholderText("MyHist");

  auto axisInfo = GlobalSettings::getAxisInfo(Vwr::XAxis);
  XNoOfBins->setText(axisInfo[0]);
  Xmin->setText(axisInfo[1]);
  Xmax->setText(axisInfo[2]);

  axisInfo = GlobalSettings::getAxisInfo(Vwr::YAxis);
  YNoOfBins->setText(axisInfo[0]);
  Ymin->setText(axisInfo[1]);
  Ymax->setText(axisInfo[2]);
  YAxisGroup->hide();

  adjustSize();

  //  get list of parameters from SpecTcl and fill the gui with them
  m_params = updateParameterList();
  loadKnownParameters();

  connect(dimensionCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(showYAxisWidgets(int)));
  connect(m_pNAM,SIGNAL(finished(QNetworkReply*)),this,SLOT(onCreateDone(QNetworkReply*)));
  connect(createButton,SIGNAL(clicked()),this,SLOT(createRemoteHist()));
  connect(XParamCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(updateXParamData(int)));
  connect(YParamCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(updateYParamData(int)));
}


void TGo4CreateNewHistogram::encodeRequest()
{
   QString bname = histNameEdit->text();

   int nxbins = XNoOfBins->text().toInt();
   int nybins = YNoOfBins->text().toInt();

   double xmin = Xmin->text().toDouble();
   double xmax = Xmax->text().toDouble();

   double ymin = Ymin->text().toDouble();
   double ymax = Ymax->text().toDouble();

   QString base = QString("%1/spectcl/spectrum/create?name=%3").arg(m_server).arg(bname);

   if (dimensionCombo->currentIndex()==0) {
       base += "&type=1";
       base += "&parameters=";
       base += XParamCombo->currentText();
       base += QString("&axes={%1 %2 %3}").arg(xmin).arg(xmax).arg(nxbins);
   } else {
       base += "&type=2";
       base += "&parameters=";
       base += XParamCombo->currentText() + " ";
       base += YParamCombo->currentText();
       base += QString("&axes={%1 %2 %3}").arg(xmin).arg(xmax).arg(nxbins);
       base += QString(" {%1 %2 %3}").arg(ymin).arg(ymax).arg(nybins);
   }
   std::cout << base.toStdString() << std::endl;
   QUrl url (base);
   m_pNAM->get(QNetworkRequest(url));
}

void TGo4CreateNewHistogram::createRemoteHist()
{
    encodeRequest();

}

void TGo4CreateNewHistogram::showYAxisWidgets(int index)
{
    if (index==1) {
        YAxisGroup->show();
    } else {
        YAxisGroup->hide();
    }
    adjustSize();
}

// Called when SpecTcl has completed its actions
void TGo4CreateNewHistogram::onCreateDone(QNetworkReply *reply)
{
    auto  error = reply->error();
    if (error == QNetworkReply::NoError) {
        QByteArray bytes = reply->readAll();
        QString str = QString::fromUtf8(bytes.data(), bytes.size());

//        QMessageBox::information(0,"",str);
    } else {
        QMessageBox::warning(0,"","Failed to read reply for request");
    }
    close();
}

std::vector<SpJs::ParameterInfo> TGo4CreateNewHistogram::updateParameterList()
{
  QString updateUrl = m_server + ("/spectcl/parameter/list");

  // Request the list of parameters and block until SpecTcl gives them to you.
  QEventLoop loop;
  QObject::connect(m_pNAM, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
  std::unique_ptr<QNetworkReply> reply(m_pNAM->get(QNetworkRequest(QUrl(updateUrl))));
  loop.exec();

  if (reply->error() == QNetworkReply::NoError) {
    // Read the response in the json
    Json::Reader reader;
    Json::Value value;

    QByteArray bytes = reply->readAll();
    bool ok = reader.parse(bytes.constData(),value);
    if (!ok) {
      throw std::runtime_error ("Failed to parse json");
    }

    // parse the content... this will throw if the json
    // specifies a status other than "ok"
    return SpJs::ParameterCmdParser().parseList(value);

  } else {

    throw std::runtime_error ("Network error while updating parameters");
  }
}




void TGo4CreateNewHistogram::loadKnownParameters()
{

    auto iter = m_params.begin();
    auto iterend = m_params.end();

    while (iter!=iterend) {
        XParamCombo->addItem(QString::fromStdString(iter->s_name));
        YParamCombo->addItem(QString::fromStdString(iter->s_name));
        ++iter;
    }

}

void TGo4CreateNewHistogram::updateXParamData(int paramIndex)
{
    auto iter = m_params.begin()+paramIndex;

    XNoOfBins->setText(QString("%1").arg(iter->s_nBins));
    Xmin->setText(QString("%1").arg(QString::number(iter->s_low,'f',2)));
    Xmax->setText(QString("%1").arg(QString::number(iter->s_high,'f',2)));

}
void TGo4CreateNewHistogram::updateYParamData(int paramIndex)
{
    auto iter = m_params.begin()+paramIndex;

    YNoOfBins->setText(QString("%1").arg(iter->s_nBins));
    Ymin->setText(QString("%1").arg(QString::number(iter->s_low,'f',2)));
    Ymax->setText(QString("%1").arg(QString::number(iter->s_high,'f',2)));

}
