#ifndef GATEBUILDER1DDIALOG_H
#define GATEBUILDER1DDIALOG_H

#include "GSlice.h"
#include <QDialog>

class QRootCanvas;
class HistogramBundle;

class TPad;

class QLineEdit;

namespace Ui {
class GateBuilder1DDialog;
}

class GateBuilder1DDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit GateBuilder1DDialog(QRootCanvas& canvas,
                                 HistogramBundle& hist,
                                 GSlice* pSlice = nullptr,
                                 QWidget *parent = nullptr);
    ~GateBuilder1DDialog();
    
signals:
    void completed(GSlice* pSlice);

public slots:
    virtual void accept();
    virtual void reject();
    virtual void onClick(TPad* pad);

private:
    void updateLow(double x);
    void updateHigh(double x);
    void updateLowEdit(double x);
    void updateHighEdit(double x);
    bool focusIsLow();
    void removeOldLines(GSlice& pSlice);


private:
    Ui::GateBuilder1DDialog *ui;
    QRootCanvas& m_canvas;
    HistogramBundle& m_histPkg;
    GSlice m_editSlice;
    GSlice* m_pOldSlice;
    QLineEdit* m_editFocus;

};
#endif // GATEBUILDER1DDIALOG_H
