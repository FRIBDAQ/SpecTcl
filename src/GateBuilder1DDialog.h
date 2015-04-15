#ifndef GATEBUILDER1DDIALOG_H
#define GATEBUILDER1DDIALOG_H

#include "GSlice.h"
#include <QDialog>

class QRootCanvas;
class HistogramBundle;

class TPad;

class QLineEdit;
class QValidator;

namespace Ui {
class GateBuilder1DDialog;
}



/*! Dialog for editing 1D gates (aka slices)
 *
 * This actually operates almost identically to the GateBuilderDialog
 * except that it maintains a GSlice rather than a GGate. See the 
 * GateBuilderDialog docs for the working of this.
 *
 * The accept() and reject() methods decorate the standard QDialog::accept() and 
 * QDialog::reject() methods.
 */
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
    /*! Emitted when a user accepts changes 
     *
     * \param pSlice  pointer to slice that will be kept by DockableGateManager
     */
    void completed(GSlice* pSlice);

public slots:
    virtual void accept();
    virtual void reject();
    virtual void onClick(TPad* pad);

    /*! Set the name and also update the accept button */
    void onNameChanged(QString name);
    void lowEditChanged();
    void highEditChanged();

private:

    // Update lines and text
    void updateLow(double x);
    void updateHigh(double x);

    // Update text edits
    void updateLowEdit(double x);
    void updateHighEdit(double x);

    // utility to determine if user is focused on low entry
    bool focusIsLow();

    // Hides the original lines
    void removeOldLines(GSlice& pSlice);


private:
    Ui::GateBuilder1DDialog *ui;
    QRootCanvas& m_canvas;
    HistogramBundle& m_histPkg;
    GSlice m_editSlice;
    GSlice* m_pOldSlice;
    QLineEdit* m_editFocus;
    QValidator* m_editValidator;
};
#endif // GATEBUILDER1DDIALOG_H
