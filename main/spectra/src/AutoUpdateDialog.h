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

#ifndef AUTOUPDATEDIALOG_H
#define AUTOUPDATEDIALOG_H

#include <QDialog>

class QButtonGroup;

namespace Ui {
class AutoUpdateDialog;
}

namespace Viewer {

class AutoUpdater;

/*! \brief AutoUpdateDialog class
 *
 * The AutoUpdateDialog class defines the dialog that the user interfaces with
 * to set up auto update for the currently selected tab. It relies on an auto updater
 * that is passed in as a reference. It does not own the updater, rather it manipulates
 * it according to the user's needs.
 *
 * No state is written to the AutoUpdater until the user accepts the new changes.
 *
 * The UI for this is defined in a .ui file called AutoUpdateDialog.ui.
 *
 * \todo In the future, it may be worth making this a general configuration for all tabs
 *       and maybe even for the panes in each tab.
 */
class AutoUpdateDialog : public QDialog
{

public:
    enum State { ON=1, OFF=0 };

    Q_OBJECT
    
public:
    explicit AutoUpdateDialog(AutoUpdater& rUpdater,
                              QWidget *parent = 0);
    ~AutoUpdateDialog();
    
public slots:
    void onAccepted();
    void onRejected();

private:
    Ui::AutoUpdateDialog *ui;
    QButtonGroup*          m_pButtonGroup;
    AutoUpdater&          m_updater;
};

} // end Viewer namespace

#endif // AUTOUPDATEDIALOG_H
