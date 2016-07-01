#ifndef PRINTDIALOG_H
#define PRINTDIALOG_H

#include <QDialog>
#include <memory>

class QPushButton;
class QLabel;
class QComboBox;
class QLayout;

namespace Viewer
{

class SpecTclInterface;
class TabbedMultiSpectrumView;
class QRootCanvas;

/*!
 * \brief Printing Dialog
 *
 * Because ROOT and Qt's paint mechanism are completely disjoint of each other,
 * the Qt printing subsystem had to be circumvented to get high quality printed
 * documents. Furthermore, it was not possible to produce meaningful printed documents
 * using ROOT printing methods if the QRootCanvas was not actually visible. For this
 * reason, this custom dialog exists. It provides the user the ability to select which
 * printer to print to and displays the canvas to be printed. There are many more options
 * that can be added here but this base functionality probably accomplishes most of
 * the use cases. The user is able to print the current tab with this at the moment.
 */
class PrintDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PrintDialog(std::shared_ptr<SpecTclInterface> pSpecTcl,
                         TabbedMultiSpectrumView& rView, QWidget *parent = 0);
    
signals:

public slots:
    /*! \brief Triggered when user presses Print */
    void accepted();

    /*! \brief Triggered when user presses Cancel */
    void rejected();


private:
    QLayout* assemblePrintControls();
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
