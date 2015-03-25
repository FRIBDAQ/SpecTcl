#ifndef GATEBUILDERDIALOG_H
#define GATEBUILDERDIALOG_H

#include <QDialog>
#include <QString>
#include <TCutG.h>
#include <memory>

class TPad;
class QRootCanvas;

namespace Ui {
class GateBuilderDialog;
}

class GateBuilderDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit GateBuilderDialog(QRootCanvas& viewer, QWidget *parent = 0);
    ~GateBuilderDialog();

public slots:
    virtual void accept();
    void newPoint(TPad* pad);
    void nameChanged(const QString& name);

signals:
    void completed(TCutG* pCut);

private:
    void encodeRequest(const TCutG& cut);

private:
    Ui::GateBuilderDialog *ui;
    QRootCanvas& m_canvas;
    QString m_name;
    std::unique_ptr<TCutG> m_pCut;
};

#endif // GATEBUILDERDIALOG_H
