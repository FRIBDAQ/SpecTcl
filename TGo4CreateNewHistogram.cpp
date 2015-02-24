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

#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TDirectory.h"
#include "TROOT.h"
#include <QButtonGroup>
#include "GlobalSettings.h"
#include "HistogramList.h"

TGo4CreateNewHistogram::TGo4CreateNewHistogram( QWidget* parent)
    : QDialog( parent)
{
    auto go4sett = GlobalSettings::getInstance();
   setObjectName("Go4CreateNewHistogram");
   setupUi(this);
   HisName->setPlaceholderText("MyHist");
   HisTitle->setText("My Title");

   HisClassGrp = new QButtonGroup(this);
   HisClassGrp->addButton(TH1_b, 1);
   HisClassGrp->addButton(TH2_b, 2);
   HisClassGrp->addButton(TH3_b, 3);

   HisTypeGrp = new QButtonGroup(this);
   HisTypeGrp->addButton(S_b, 1);
   HisTypeGrp->addButton(D_b, 2);
   HisTypeGrp->addButton(F_b, 3);
   HisTypeGrp->addButton(I_b, 4);
   HisTypeGrp->addButton(C_b, 5);

   //int htype = go4sett->value("/hist/defaultType").toInt();
   int htype=11;
   HisClassGrp->button(htype / 10)->setChecked(true);
   HisTypeGrp->button(htype % 10)->setChecked(true);

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

   npoints = go4sett->value("hist/zaxis/nbins");
   min = go4sett->value("hist/zaxis/min");
   max = go4sett->value("hist/zaxis/max");
   ZNoOfBins->setText(npoints.toString());
   Zmin->setText(min.toString());
   Zmax->setText(max.toString());

   fSelectedCmd = 0;
}

int TGo4CreateNewHistogram::GetSelectedCmd()
{
   return fSelectedCmd;
}

void TGo4CreateNewHistogram::MakeHistogram()
{
   QByteArray bname = HisName->text().toAscii();
   QByteArray btitle = HisTitle->text().toAscii();

   const char* hname = bname.constData();
   const char* htitle = btitle.constData();

   int htype = HisClassGrp->checkedId()*10 + HisTypeGrp->checkedId();

   int nxbins = XNoOfBins->text().toInt();
   int nybins = YNoOfBins->text().toInt();
   int nzbins = ZNoOfBins->text().toInt();

   double xmin = Xmin->text().toDouble();
   double xmax = Xmax->text().toDouble();
   double ymin = Ymin->text().toDouble();
   double ymax = Ymax->text().toDouble();
   double zmin = Zmin->text().toDouble();
   double zmax = Zmax->text().toDouble();


   TH1* h = 0;
   switch(htype) {
      case 11: h = new TH1S(hname, htitle, nxbins, xmin, xmax); break;
      case 12: h = new TH1D(hname, htitle, nxbins, xmin, xmax); break;
      case 13: h = new TH1F(hname, htitle, nxbins, xmin, xmax); break;
      case 14: h = new TH1I(hname, htitle, nxbins, xmin, xmax); break;
      case 15: h = new TH1C(hname, htitle, nxbins, xmin, xmax); break;
      case 21: h = new TH2S(hname, htitle, nxbins, xmin, xmax, nybins, ymin, ymax); break;
      case 22: h = new TH2D(hname, htitle, nxbins, xmin, xmax, nybins, ymin, ymax); break;
      case 23: h = new TH2F(hname, htitle, nxbins, xmin, xmax, nybins, ymin, ymax); break;
      case 24: h = new TH2I(hname, htitle, nxbins, xmin, xmax, nybins, ymin, ymax); break;
      case 25: h = new TH2C(hname, htitle, nxbins, xmin, xmax, nybins, ymin, ymax); break;
      case 31: h = new TH3S(hname, htitle, nxbins, xmin, xmax, nybins, ymin, ymax, nzbins, zmin, zmax); break;
      case 32: h = new TH3D(hname, htitle, nxbins, xmin, xmax, nybins, ymin, ymax, nzbins, zmin, zmax); break;
      case 33: h = new TH3F(hname, htitle, nxbins, xmin, xmax, nybins, ymin, ymax, nzbins, zmin, zmax); break;
      case 34: h = new TH3I(hname, htitle, nxbins, xmin, xmax, nybins, ymin, ymax, nzbins, zmin, zmax); break;
      case 35: h = new TH3C(hname, htitle, nxbins, xmin, xmax, nybins, ymin, ymax, nzbins, zmin, zmax); break;
   }
   h->SetDirectory(0);

   if (h) {
     HistogramList::getInstance()->addHist(*h);
   }
}



void TGo4CreateNewHistogram::CreateRemoteHist()
{
   fSelectedCmd = 2;
   accept();
}


