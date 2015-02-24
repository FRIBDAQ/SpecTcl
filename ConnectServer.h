// $Id: ConnectDialog.h 491 2009-11-04 12:41:22Z linev $
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

#ifndef ConnectDialog_H
#define ConnectDialog_H

#include <QDialog>

// forward declare
namespace Ui {
    class ConnectDialog;
}

class ConnectDialog : public QDialog
{
    Q_OBJECT

public:
    ConnectDialog( QWidget* parent = 0 );

public slots:
    void cacheServerSettings();

private:
    Ui::ConnectDialog* ui;
};


#endif
