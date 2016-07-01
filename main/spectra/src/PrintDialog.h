#ifndef PRINTDIALOG_H
#define PRINTDIALOG_H

#include <QDialog>
#include <memory>

class QPushButton;
class QLabel;
class QComboBox;


namespace Viewer
{

class SpecTclInterface;
class TabbedMultiSpectrumView;
class QRootCanvas;

class PrintDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PrintDialog(std::shared_ptr<SpecTclInterface> pSpecTcl,
                         TabbedMultiSpectrumView& rView, QWidget *parent = 0);
    
signals:
    
public slots:
    void accepted();
    void rejected();

private:
    void assembleWidgets();
    void connectSignalsAndSlots();
    void populatePrinterOptions();

    // data members
private:
    std::shared_ptr<SpecTclInterface> m_pSpecTcl;
    TabbedMultiSpectrumView* m_pView;

    QComboBox*              m_pPrinterSelect;
    QPushButton*            m_pOkButton;
    QPushButton*            m_pCancelButton;
    QLabel*                 m_pPrinterLabel;
    QLabel*                 m_pPreviewLabel;
    QRootCanvas*            m_pCanvas;
};

} // end Viewer namespace

#endif // PRINTDIALOG_H
