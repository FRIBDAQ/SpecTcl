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

#ifndef ConnectServer_H
#define ConnectServer_H


#include <QDialog>
#include <QButtonGroup>
#include <ui_ConnectServer.h>

#include <memory>

namespace Viewer
{

class SpecTclInterfaceControl;


/*!
 * \brief The ConnectDialog class
 *
 * This is in charge of configuring the connection to the REST server. It
 * allows the user to set the mode of connectivity (local or remote). It also
 * allows the user to manipulate the settings for the server. This dialog is
 * modal.
 */
class ConnectDialog : public QDialog
{
    Q_OBJECT

public:
    /*!
     * \brief ConnectDialog Constructor
     * \param rInterface    accessor for the SpecTclInterface
     * \param parent        owner of this
     *
     * This method launches the dialog in a modal fashion.
     */
    ConnectDialog( SpecTclInterfaceControl& rInterface, QWidget* parent = 0 );


    ///////////////////////////////////////////////////////////////////////////
public slots:
    void onAccept();

    ///////////////////////////////////////////////////////////////////////////
private:
    /*!
     * \brief Assemble widgets into the megawidget
     */
    void assembleWidgets();

    /*!
     * \brief Commit the settings to the GlobalSettings
     */
    void cacheServerSettings();

    ///////////////////////////////////////////////////////////////////////////
private:
    std::unique_ptr<Ui::ConnectDialog>   m_pUI;
    SpecTclInterfaceControl             *m_pInterfaceControl;
    QButtonGroup                        *m_pButtonGroup;
};

} // end of namespace

#endif
