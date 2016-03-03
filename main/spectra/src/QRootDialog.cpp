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

// $Id: QRootDialog.cpp 478 2009-10-29 12:26:09Z linev $
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fr Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

//Author : Denis Bertini 01.11.2000

/**************************************************************************
* Copyright (C) 2000- Gesellschaft f. Schwerionenforschung, GSI           *
*                     Planckstr. 1, 64291 Darmstadt, Germany              *
*                     All rights reserved.                                *
* Contact:            http://go4.gsi.de                                   *
*                                                                         *
* This software can be used under the license agreements as stated in     *
* Go4License.txt file which is part of the distribution.                  *
***************************************************************************/

#include "QRootDialog.h"

#include <QtGui/QGridLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QLineEdit>

namespace Viewer
{

QRootDialog::QRootDialog() : QDialog()
{
   QGridLayout *gridLayout = new QGridLayout(this);
   gridLayout->setSpacing(1);
   gridLayout->setMargin(1);

   QHBoxLayout *buttLayout = new QHBoxLayout();

   QPushButton *bOk = new QPushButton(this);
   bOk->setText("Apply");
   connect(bOk, SIGNAL( clicked() ), this, SLOT( accept() ));
   buttLayout->addWidget(bOk);

   QPushButton *bCancel = new QPushButton(this);
   bCancel->setText("Cancel");
   connect(bCancel, SIGNAL( clicked() ), this, SLOT( reject() ));
   buttLayout->addWidget(bCancel);

   argLayout = new QVBoxLayout();

   setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                            QSizePolicy::Expanding));

   gridLayout->addLayout(argLayout, 0, 0);
   gridLayout->addLayout(buttLayout, 1, 0, Qt::AlignBottom);
}

void QRootDialog::addArg(const char* argname, const char* value, const char*)
{
   QLabel* lbl = new QLabel(argname);
   argLayout->addWidget(lbl);

   QLineEdit* le = new QLineEdit();
   le->setGeometry(10,10, 130, 30);
   le->setFocus();
   le->setText(value);
   argLayout->addWidget(le);

   fArgs.push_back(le);
}

QString QRootDialog::getArg(int n)
{
   if ((n<0) || (n>=fArgs.size())) return QString("");
   return fArgs[n]->text();
}

} // end of namespace
