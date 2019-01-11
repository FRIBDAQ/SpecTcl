#ifndef MARKERDIALOG_H
#define MARKERDIALOG_H

#include <QDialog>

namespace Ui {
class MarkerDialog;
}

class MarkerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MarkerDialog(QWidget *parent = 0);
    ~MarkerDialog();

private slots:
    void on_pMarkerDialogButtonBox_accepted();

    void on_pMarkerDialogButtonBox_rejected();

    void on_pMarkerDialogButtonBox_destroyed();
signals:
    void onAccepted(QString name, int x, int y);
    void onRejected();
    void onDeleted();

private:
    Ui::MarkerDialog *ui;
};

#endif // MARKERDIALOG_H
