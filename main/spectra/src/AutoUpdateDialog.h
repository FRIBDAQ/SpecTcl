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
