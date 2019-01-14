#ifndef MARKERDIALOG_H
#define MARKERDIALOG_H

#include <QDialog>
#include <QRootCanvas.h>
#include <Rtypes.h>

class TMarker;

namespace Ui {
class MarkerDialog;
}

namespace Viewer {
class HistogramBundle;
class SpecTclInterface;
    
class MarkerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MarkerDialog(QWidget *parent, SpecTclInterface* pSpecTcl);
    ~MarkerDialog();
    
    virtual void closeEvent(QCloseEvent* e);

private slots:
    void on_pMarkerDialogButtonBox_accepted();
    void on_pMarkerDialogButtonBox_rejected();
    void onDeleteSelected();


    void onMouseRelease(QWidget* pad);
signals:
    void onAccepted(QString name, HistogramBundle* pHis, Double_t x, Double_t y);
    void onRejected();
    void onDeleted();

private:
    HistogramBundle* getCurrentHistogram();
    void deleteTentativeMarker();
    void loadMarkerList();
    
private:
    Ui::MarkerDialog *ui;
    SpecTclInterface* m_pSpecTcl;
    QRootCanvas*     m_pCanvas;
    
    TMarker* m_pTentativeMarker;

};
}
#endif // MARKERDIALOG_H
