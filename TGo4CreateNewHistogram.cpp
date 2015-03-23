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
#include <QtNetwork>
#include <QMessageBox>
#include <QFile>

TGo4CreateNewHistogram::TGo4CreateNewHistogram( QWidget* parent)
    : QDialog( parent)
{
    m_pNAM = new QNetworkAccessManager(this);

    auto go4sett = GlobalSettings::getInstance();
   setObjectName("Go4CreateNewHistogram");
   setupUi(this);
   histNameEdit->setPlaceholderText("MyHist");

   QVariant npoints = go4sett->value("hist/xaxis/nbins");
   QVariant min = go4sett->value("hist/xaxis/min");
   QVariant max = go4sett->value("hist/xaxis/max");
   XNoOfBins->setText(npoints.toString());
   Xmin->setText(min.toString());
   Xmax->setText(max.toString());

   npoints = go4sett->value("hist/yaxis/nbins");
   min = go4sett->value("hist/yaxis/min");
   max = go4sett->value("hist/yaxis/max");
   YNoOfBins->setText(npoints.toString());
   Ymin->setText(min.toString());
   Ymax->setText(max.toString());
   YAxisGroup->hide();

    adjustSize();

   connect(dimensionCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(showYAxisWidgets(int)));
   connect(m_pNAM,SIGNAL(finished(QNetworkReply*)),this,SLOT(onCreateDone(QNetworkReply*)));
   connect(createButton,SIGNAL(clicked()),this,SLOT(createRemoteHist()));

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

   auto gpSettings = GlobalSettings::getInstance();

   QString host = gpSettings->value("/server/hostname").toString();
   int port = gpSettings->value("/server/port").toInt();

   QString base = QString("http://%1:%2/spectcl/spectrum/create?name=%3").arg(host).arg(port).arg(bname);

   // http://daqdev-wheezy:8000/spectcl/spectrum/create?name=test&type=1&parameters=event.raw.00&axes={0%20100%2099}
   if (dimensionCombo->currentIndex()==0) {
       base += "&type=1";
       base += "&parameters=event.raw.00";
       base += QString("&axes={%1 %2 %3}").arg(xmin).arg(xmax).arg(nxbins);
   }

   QUrl url (base);
   QFile dbg("astat");
   dbg.open(QIODevice::WriteOnly);
   dbg.write(url.toEncoded());
   dbg.close();

   m_pNAM->get(QNetworkRequest(url));
}

void TGo4CreateNewHistogram::createRemoteHist()
{
    encodeRequest();
    close();
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

void TGo4CreateNewHistogram::onCreateDone(QNetworkReply *reply)
{
    auto  error = reply->error();
    if (error == QNetworkReply::NoError) {
        QByteArray bytes = reply->readAll();
        QString str = QString::fromUtf8(bytes.data(), bytes.size());

        QMessageBox::information(0,"",str);
    } else {
        QMessageBox::warning(0,"","Failed to read reply for request");
    }
}
