#ifndef SAVETOROOTDIALOG_H
#define SAVETOROOTDIALOG_H

#include <QString>
#include <QStringList>
#include <QDialog>

#include <memory>

namespace Ui {
class SaveToRootDialog;
}

class QCheckBox;

namespace Viewer {

class TabbedMultiSpectrumView;
class SpecTclInterface;

/*!
 * \brief The SaveToRootDialog class
 *
 * This class is slightly misnamed because it can also save win files.
 * The dialog this creates is the dialog reached through the "File > Save As"
 * menu.
 */
class SaveToRootDialog : public QDialog
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
    int getTabSelectedCount();
    QString formOutputPath(const QString& user_path, const QString& tabName);


public slots:
    void onAccepted();
    void onRejected();
    void onBrowse();

    void onSelectAll();

    void onPathEdited(const QString& value);
    void updateSaveButtonState();

private:
    Ui::SaveToRootDialog*       ui;
    TabbedMultiSpectrumView&    m_tabWidget;
    std::vector<QCheckBox*>     m_checkBoxes;
    QCheckBox*                  m_pSelectAllCheckBox;
    std::shared_ptr<SpecTclInterface> m_pSpecTcl;
};

} // end Viewer namespace

#endif // SAVETOROOTDIALOG_H
