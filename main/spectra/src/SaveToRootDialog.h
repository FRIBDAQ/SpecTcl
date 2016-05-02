#ifndef SAVETOROOTDIALOG_H
#define SAVETOROOTDIALOG_H

#include <QString>
#include <QStringList>
#include <QWidget>


namespace Ui {
class SaveToRootDialog;
}

class QCheckBox;

namespace Viewer {

class TabbedMultiSpectrumView;

class SaveToRootDialog : public QWidget
{
    Q_OBJECT
    
public:
    explicit SaveToRootDialog(TabbedMultiSpectrumView& tabWidget, QWidget *parent = 0);

    ~SaveToRootDialog();

protected:
    void setUpWidget();

public slots:
    void onAccepted();
    void onRejected();
    void onBrowse();

    void onSelectAll();

signals:
    void accepted();
    void rejected();

private:
    Ui::SaveToRootDialog *ui;
    TabbedMultiSpectrumView&    m_tabWidget;
    std::vector<QCheckBox*>     m_checkBoxes;
    QCheckBox*                  m_pSelectAllCheckBox;
};

} // end Viewer namespace

#endif // SAVETOROOTDIALOG_H
