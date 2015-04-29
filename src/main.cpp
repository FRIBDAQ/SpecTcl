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
#include "mainwindow.h"
#include <TQApplication.h>
#include <TQRootApplication.h>
#include "GlobalSettings.h"

#include "QHistInfo.h"

int main(int argc, char *argv[])
{
  Q_INIT_RESOURCE(resources);
  QApplication::setGraphicsSystem("native");

  TQApplication a("app", &argc, argv);
  TQRootApplication b(argc, argv, 0);

  Viewer::MainWindow w;
  w.show();

  Viewer::GlobalSettings::setServerHost("localhost");
  Viewer::GlobalSettings::setServerPort(8000);

//  qRegisterMetaType<SpJs::HistInfo>("SpJs::HistInfo");
//  qRegisterMetaType<SpJs::HistInfo>("HistInfo");
//  qRegisterMetaType<std::vector<SpJs::HistInfo> >("vector<SpJs::HistInfo>");
//  qRegisterMetaType<std::vector<SpJs::HistInfo> >("std::vector<SpJs::HistInfo>");
//  qRegisterMetaType<std::vector<SpJs::HistInfo> >("std::vector<HistInfo>");
//  qRegisterMetaType<std::vector<SpJs::HistInfo> >("vector<HistInfo>");


  return b.exec();
}
