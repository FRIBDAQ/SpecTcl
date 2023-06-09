#ifndef ONEDIMGATEEDIT_H
#define ONEDIMGATEEDIT_H

#include <QWidget>


#include "GSlice.h"
#include <utility>
#include <memory>
#include <vector>
#include <HistogramBundle.h>

class TPad;

class QLineEdit;
class QValidator;


namespace Ui {
class OneDimGateEdit;
}


namespace Viewer
{

class SpecTclInterface;
class QRootCanvas;



/*! Dialog for editing 1D gates (aka slices)
 *
 * This actually operates almost identically to the TwoDimGateEdit
 * except that it maintains a GSlice rather than a GGate. See the
 * TwoDimGateEdit docs for the working of this.
 *
 * The accept() and reject() methods decorate the standard QDialog::accept() and
 * QDialog::reject() methods.
 */
class OneDimGateEdit : public QWidget
{
    Q_OBJECT

public:
    explicit OneDimGateEdit(QRootCanvas& canvas,
                                 HistogramBundle& hist,
                                std::shared_ptr<SpecTclInterface> pSpecTcl,
                                 GSlice* pSlice = nullptr,
                                 QWidget *parent = nullptr);
    ~OneDimGateEdit();

    void registerSlice(GSlice* pSlice);
    void editSlice(GSlice* pSlice);

signals:
    /*! Emitted when a user accepts changes
     *
     * \param pSlice  pointer to slice that will be kept by MasterGateList
     */
    void completed(GSlice* pSlice);
    void accepted();
    void rejected();

public slots:
    virtual void accept();
    virtual void reject();
    virtual void onMousePress(QRootCanvas* pad);
    virtual void onMouseRelease(QRootCanvas* pad);
    virtual void onClick(QRootCanvas* pad);

    /*! Set the name and also update the accept button */
    void onNameChanged(QString name);
    void lowEditChanged();
    void highEditChanged();
    void onLowChanged(double x1, double y1, double x2, double y2);
    void onHighChanged(double x1, double y1, double x2, double y2);

private:
    // Projection gates demand special handling for creation
    // and editing:
    
    GSlice* duplicateSlice(
        const std::string& newName, const std::string& newParam,
        const GSlice& source 
    );
    std::vector<std::pair<std::string, GSlice*> >
    createProjectionComponents(
        SpJs::HistInfo& hInfo, GSlice* pSource
    );
    void createProjectionGate(SpJs::HistInfo& hInfo, GSlice* pSource);
    void editProjectionGate(SpJs::HistInfo& hInfo, GSlice* pSource);
    
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

    void connectSignals();

private:
    Ui::OneDimGateEdit *ui;
    QRootCanvas& m_canvas;
    HistogramBundle& m_histPkg;
    GSlice m_editSlice;
    GSlice* m_pOldSlice;
    QLineEdit* m_editFocus;
    QValidator* m_editValidator;
    std::shared_ptr<SpecTclInterface> m_pSpecTcl;
    std::pair<int, int> m_lastMousePressPos;
};

} // end of namespace

#endif // ONEDIMGATEEDIT_H
