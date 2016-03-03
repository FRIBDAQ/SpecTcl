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

#include "MainWindow.h"
#include <TQApplication.h>
#include <TQRootApplication.h>
#include "GlobalSettings.h"
#include <TEnv.h>
#include <TStyle.h>

#include "QHistInfo.h"

int main(int argc, char *argv[])
{
  Q_INIT_RESOURCE(resources);
  QApplication::setGraphicsSystem("native");

  gEnv->SetValue("Unix.*.Root.UseTTFonts", false); // TTF are SLOW!
  gStyle->SetOptStat(0); // this is not useful at the moment. I can do it better using Qt widgets

  TQApplication a("app", &argc, argv);
  TQRootApplication b(argc, argv, 0);

  Viewer::GlobalSettings::setSessionMode(1);

  Viewer::MainWindow w;
  w.show();


  Viewer::GlobalSettings::setServerHost("localhost");
  Viewer::GlobalSettings::setServerPort(8080);
  Viewer::GlobalSettings::setPollInterval(5000);

  return b.exec();
}
