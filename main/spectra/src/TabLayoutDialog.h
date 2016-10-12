#ifndef TABLAYOUTDIALOG_H
#define TABLAYOUTDIALOG_H

#include <QWidget>

class QKeyEvent;
class QButtonGroup;

namespace Ui {
class TabLayoutDialog;
}

namespace Viewer
{

/*!
 * \brief The Tab layout dialog class
 *
 * The TabLayoutDialog is the first widget that all users
 * interact with when they open up the Spectra program. The TabLayoutDialog
 * shows the Logo, has a line edit for the tab name, some radio buttons,
 * and a continue button.
 *
 */
class TabLayoutDialog : public QWidget
{
    Q_OBJECT
    
public:
    explicit TabLayoutDialog(QWidget *parent = 0);
    ~TabLayoutDialog();

    QString getTabName() const;
    void keyPressEvent(QKeyEvent* pEvent);

signals:
    void bulkClicked();
    void sequentialClicked();
    void loadFileClicked(QString filename);

public slots:
    void onContinue();
    void onTabNameChanged(const QString& newText);
    
private:
    enum Selection { BULK, MANUAL, FILE};
    Ui::TabLayoutDialog *ui;
    QButtonGroup*           m_pButtons;
};

} // end Viewer namespace

#endif // TabLayoutDialog_H
