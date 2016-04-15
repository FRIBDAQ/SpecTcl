#ifndef AUTOUPDATEDIALOG_H
#define AUTOUPDATEDIALOG_H

#include <QDialog>

class QButtonGroup;

namespace Ui {
class AutoUpdateDialog;
}

namespace Viewer {

class AutoUpdater;

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
