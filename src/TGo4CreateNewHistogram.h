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

// $Id: TGo4CreateNewHistogram.h 489 2009-11-04 12:20:26Z linev $
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

#ifndef TGO4CREATENEWHISTOGRAM_H
#define TGO4CREATENEWHISTOGRAM_H


#include <ParameterInfo.h>
#include <QDialog>
#include "ui_TGo4CreateNewHistogram.h"
#include <vector>

class QNetworkAccessManager;
class QNetworkReply;

class TGo4CreateNewHistogram : public QDialog, public Ui::TGo4CreateNewHistogram
 {
     Q_OBJECT

 public:
   TGo4CreateNewHistogram( QWidget* parent = 0);

 public slots:

   virtual void createRemoteHist();
   void showYAxisWidgets(int);
   void onCreateDone(QNetworkReply*);
   void updateXParamData(int paramIndex);
   void updateYParamData(int paramIndex);

private:
   void encodeRequest();
   std::vector<SpJs::ParameterInfo> updateParameterList();
   void loadKnownParameters();


private:
   QNetworkAccessManager *m_pNAM;
   QString m_server;
   std::vector<SpJs::ParameterInfo> m_params;

 };
#endif
