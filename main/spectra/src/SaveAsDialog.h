#ifndef SaveAsDialog_H
#define SaveAsDialog_H

#include <QString>
#include <QStringList>
#include <QDialog>

#include <memory>

namespace Ui {
class SaveAsDialog;
}

class QCheckBox;

namespace Viewer {

class TabbedMultiSpectrumView;
class SpecTclInterface;

/*!
 * \brief The SaveAsDialog class
 *
 * This class is the dialog that is presented when Save As is selected from the
 * file drop-down menu. Through it, the user has access to saving as a ROOT or a
 * win file.
 *
 */
class SaveAsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SaveAsDialog(TabbedMultiSpectrumView& tabWidget,
                              std::shared_ptr<SpecTclInterface> pSpecTcl,
                              QWidget *parent = 0);

    ~SaveAsDialog();


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
    Ui::SaveAsDialog*       ui;
    TabbedMultiSpectrumView&    m_tabWidget;
    std::vector<QCheckBox*>     m_checkBoxes;
    QCheckBox*                  m_pSelectAllCheckBox;
    std::shared_ptr<SpecTclInterface> m_pSpecTcl;
};

} // end Viewer namespace

#endif // SaveAsDialog_H
