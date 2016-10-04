//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2016.
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

#include "CmdLineOptions.h"
#include "MainWindow.h"
#include "QHistInfo.h"
#include "GlobalSettings.h"

#include <TQApplication.h>
#include <TQRootApplication.h>
#include <TEnv.h>
#include <TStyle.h>

#include <QStringList>
#include <QCoreApplication>

#include <iostream>


int main(int argc, char *argv[])
{

  Viewer::CmdLineOptions opts;
  opts.parse(argc, argv);

  Q_INIT_RESOURCE(resources);

  QApplication::setGraphicsSystem("native");

  // Set some default values for ROOT
  gEnv->SetValue("Unix.*.Root.UseTTFonts",true);
  gStyle->SetOptStat(0); // this is not useful at the moment. I can do it better using Qt widgets

  if (opts.disableTrueTypeFonts()) {
      gEnv->SetValue("Unix.*.Root.UseTTFonts",false);
  }

  TQApplication a("app", &argc, argv);
  TQRootApplication b(argc, argv, 0);

  Viewer::GlobalSettings::setSessionMode(1);  
  Viewer::GlobalSettings::setServerHost(opts.getHost());
  Viewer::GlobalSettings::setServerPort(opts.getPort());
  Viewer::GlobalSettings::setPollInterval(5000);

  Viewer::MainWindow w;
  w.show();


  return b.exec();
}
