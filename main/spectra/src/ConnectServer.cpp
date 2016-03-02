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
#include "SpecTclInterfaceFactory.h"
#include "SpecTclInterfaceControl.h"
#include "SpecTclInterface.h"

#include <QButtonGroup>


namespace Viewer
{


//
//
ConnectDialog::ConnectDialog( SpecTclInterfaceControl& rInterface, QWidget* parent )
   : QDialog( parent ),
   m_pUI(new Ui::ConnectDialog),
   m_pInterfaceControl(&rInterface)
{

   assembleWidgets();

   connect(m_pUI->ConnectBtn,SIGNAL(pressed()), this, SLOT(onAccept()));
   connect(m_pUI->ConnectBtn,SIGNAL(clicked()), this, SLOT(close()));
}

//
//
void ConnectDialog::assembleWidgets()
{
    m_pUI->setupUi(this);

    // group the radiobuttons to work together
    m_pButtonGroup = new QButtonGroup(this);
    m_pButtonGroup->addButton(m_pUI->remoteRadioBtn, 0);
    m_pButtonGroup->addButton(m_pUI->localRadioBtn, 1);
    m_pButtonGroup->setExclusive(true);

    int mode = GlobalSettings::getSessionMode();
    if (mode == 0) {
        m_pUI->remoteRadioBtn->setChecked(true);
    } else {
        m_pUI->localRadioBtn->setChecked(true);
    }

    m_pUI->HostName->setText(GlobalSettings::getServerHost());
    m_pUI->PortNumber->setValue(GlobalSettings::getServerPort());
}


//
//
void ConnectDialog::onAccept() {
    cacheServerSettings();

    int selectedMode = m_pButtonGroup->checkedId();

    int currentMode = GlobalSettings::getSessionMode();

    if (currentMode != selectedMode) {
        SpecTclInterfaceFactory factory;
        if (selectedMode == 0) {
            m_pInterfaceControl->setInterface(factory.create(SpecTclInterfaceFactory::REST));
        } else {
            m_pInterfaceControl->setInterface(factory.create(SpecTclInterfaceFactory::Hybrid));
        }
    }

    GlobalSettings::setSessionMode(selectedMode);

}

//
//
void ConnectDialog::cacheServerSettings() {
  GlobalSettings::setServerHost(m_pUI->HostName->text());
  GlobalSettings::setServerPort(m_pUI->PortNumber->value());
}

} // end of namepsace
