#ifndef SAVETOROOTDIALOG_H
#define SAVETOROOTDIALOG_H

#include <QString>
#include <QStringList>
#include <QWidget>

#include <memory>

namespace Ui {
class SaveToRootDialog;
}

class QCheckBox;

namespace Viewer {

class TabbedMultiSpectrumView;
class SpecTclInterface;

class SaveToRootDialog : public QWidget
{
    Q_OBJECT
    
public:
    explicit SaveToRootDialog(TabbedMultiSpectrumView& tabWidget,
                              std::shared_ptr<SpecTclInterface> pSpecTcl,
                              QWidget *parent = 0);

    ~SaveToRootDialog();

protected:
    void setUpWidget();
    void writeToRootFile();
    void writeToWinFile();

public slots:
    void onAccepted();
    void onRejected();
    void onBrowse();

    void onSelectAll();

    void onPathEdited(const QString& value);

signals:
    void accepted();
    void rejected();

private:
    Ui::SaveToRootDialog *ui;
    TabbedMultiSpectrumView&    m_tabWidget;
    std::vector<QCheckBox*>     m_checkBoxes;
    QCheckBox*                  m_pSelectAllCheckBox;
    std::shared_ptr<SpecTclInterface> m_pSpecTcl;
};

} // end Viewer namespace

#endif // SAVETOROOTDIALOG_H
