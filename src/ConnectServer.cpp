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
// $Id: ConnectDialog.cpp 478 2009-10-29 12:26:09Z linev $
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

#include "ConnectServer.h"
#include "ui_ConnectServer.h"
#include "GlobalSettings.h"

ConnectDialog::ConnectDialog( QWidget* parent )
   : QDialog( parent ),
   ui(new Ui::ConnectDialog)
{
   setObjectName("Go4ConnectServer");
   ui->setupUi(this);

   ui->HostName->setText(GlobalSettings::getServerHost());
   ui->PortNumber->setValue(GlobalSettings::getServerPort());

   connect(ui->ConnectBtn,SIGNAL(pressed()), this, SLOT(cacheServerSettings()));
   connect(ui->ConnectBtn,SIGNAL(clicked()), this, SLOT(close()));
}

void ConnectDialog::cacheServerSettings() {
  GlobalSettings::setServerHost(ui->HostName->text());
  GlobalSettings::setServerPort(ui->PortNumber->value());
}
