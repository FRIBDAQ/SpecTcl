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

   connect(ui->ConnectBtn,SIGNAL(pressed()), this, SLOT(cacheServerSettings));
   connect(ui->ConnectBtn,SIGNAL(clicked()), this, SLOT(close()));
}

void ConnectDialog::cacheServerSettings() {
  GlobalSettings::setServerHost(ui->HostName->text());
  GlobalSettings::setServerPort(ui->PortNumber->value());
}
