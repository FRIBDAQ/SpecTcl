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


#include "Main.h"

#include "MainWindow.h"
#include "QHistInfo.h"
#include "GlobalSettings.h"

#include <TQApplication.h>
#include <TQRootApplication.h>
#include <TEnv.h>
#include <TStyle.h>
#include <TROOT.h>

#include <QStringList>
#include <QCoreApplication>
#include <QHostInfo>
#include <QHostAddress>

#include <iostream>
#include <algorithm>
#include <iterator>
#include <list>

using namespace std;

namespace Viewer {

Main::Main()
{
}

int Main::operator()(int argc, char* argv[])
{
    m_opts.parse(argc, argv);

    QApplication::setGraphicsSystem("native");

    // Set some default values for ROOT
    gEnv->SetValue("Unix.*.Root.UseTTFonts",true);
    gStyle->SetOptStat(0); // this is not useful at the moment. I can do it better using Qt widgets

    if (m_opts.disableTrueTypeFonts()) {
        gEnv->SetValue("Unix.*.Root.UseTTFonts",false);
    }

    if (serverIsOnThisMachine(m_opts.getHost())) {
        std::cout << "Starting local session" << std::endl;
        GlobalSettings::setSessionMode(GlobalSettings::LOCAL);
    } else {
        std::cout << "Starting remote session" << std::endl;
        GlobalSettings::setSessionMode(GlobalSettings::REMOTE);
    }
    GlobalSettings::setServerHost(m_opts.getHost());
    GlobalSettings::setServerPort(m_opts.getPort());
    GlobalSettings::setPollInterval(5000);

    // start Qt AND ROOT event loops... yes we need both
    TQApplication qtEventLoop("app", &argc, argv);
    TQRootApplication rootEventLoop(argc, argv, 0);

    MainWindow w;
    w.show();

    int status = rootEventLoop.exec();

    // we have a static object destruction order problem that we need to get
    // around. ROOT does not create more than 1 instance of TROOT in the life
    // of the program. Once it is deleted, it never can be created again. That
    // causes problems with the destruction of our TCutG objects that try to
    // access gROOT->GetListOfSpecials() after it has gROOT has been turned back
    // to nullptr. Calling exit() here sweeps a seg fault caused by this under the
    // rug so our users don't see it. There is no issue being caused here. In the
    // future, this might bite us if we are doing more I/O type stuff in ROOT or
    // using their threading system.
    exit(status);

    // we never get here.
    return status;
}

/*!
     * \brief Check to see if REST server is local
     *
     * The basic premise is that we compare all of the IP addresses associated
     * with the local machine with those of the machine specified. If there is
     * overlap between these lists of IP addresses, then the REST server is
     * local. Otherwise, it is remote.
     *
     * \param serverName    name of machine REST server is running
     *
     * \return boolean indicating whether REST server is on localhost
     */
bool Main::serverIsOnThisMachine(QString serverName)
{

    // get all IP addresses of localhost machine
    QHostInfo thisMachine = QHostInfo::fromName("localhost");
    QList<QHostAddress> thisMachineAddresses = thisMachine.addresses();

    // localhost will likely return different ip addresses than resolving
    // the ip addresses associated with "hostname".
    thisMachine = QHostInfo::fromName(QHostInfo::localHostName());
    QList<QHostAddress> moreThisAddresses = thisMachine.addresses();
    thisMachineAddresses.append(moreThisAddresses);

    // get all IP addresses of server
    QHostInfo thatMachine = QHostInfo::fromName(serverName);
    QList<QHostAddress> thatMachineAddresses = thatMachine.addresses();

    return listsShareContent(thisMachineAddresses, thatMachineAddresses);
}

bool Main::listsShareContent(QList<QHostAddress> list0, QList<QHostAddress> list1)
{

    // if these two sets of addresses overlap, then we have the same machine
    auto comparison = [](const QHostAddress& h0, const QHostAddress& h1) {
        return (h0.toString() < h1.toString());
    };

    // set_intersection requires that the lists are ordered.
    sort(list0.begin(), list0.end(), comparison);
    sort(list1.begin(), list1.end(), comparison);

    list<QHostAddress> intersection;
    auto last = set_intersection(list0.begin(), list0.end(),
                                 list1.begin(), list1.end(),
                                 back_inserter(intersection), comparison);

    return (intersection.size()>0);
}

} // end of Viewer namespace
