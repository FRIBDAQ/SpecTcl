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

#ifndef AUTOUPDATER_H
#define AUTOUPDATER_H

#include <QObject>

#include <memory>

class QTimer;

namespace Viewer
{

class SpecTclInterface;
class SpectrumView;

/*!
 * \brief The AutoUpdater class
 *
 * This provides facilities to periodically update a spectrum view automatically.
 * The user sets this up through a drop down menu. Currently, there is a one-to-one
 * relationship between a SpectrumView and an AutoUpdater class. The TabWorkspace
 * is what orchestrates this, as it owns both of them.
 *
 * At the moment, the AutoUpdate feature only supports the updating of all spectra
 * on a single tab at the same time.
 *
 */
class AutoUpdater : public QObject
{
    Q_OBJECT

public:

    //
    // For documentation, please see the doxygen generated docs or the source code
    //

    explicit AutoUpdater(std::shared_ptr<SpecTclInterface> pSpecTcl,
                         SpectrumView& rView,
                         QObject *parent = 0);

    void start(int nSeconds);
    void stop();

    int  getInterval() const;
    bool isActive() const;
    void updateAll();

public slots:
    void onTimeout();

private:
    std::shared_ptr<SpecTclInterface> m_pSpecTcl;
    SpectrumView*                     m_pView;
    QTimer*                           m_pTimer;
};

} // end Viewer namespace

#endif // AUTOUPDATER_H
