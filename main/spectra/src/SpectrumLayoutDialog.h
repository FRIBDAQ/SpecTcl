#ifndef SPECTRUMLAYOUTDIALOG_H
#define SPECTRUMLAYOUTDIALOG_H

#include <QWidget>
#include <QStringList>

#include <memory>

namespace Viewer {

class TabLayoutDialog;
class BulkDrawChooser;
class SpecTclInterface;

/*!
 * \brief The SpectrumLayoutDialog class
 *
 * This is the orchestrator of a set dialogs that are used for setting up a
 * tab. It is the first dialog that a user is presented with. There are three
 * buttons that are displayed to set up with the bulk dialog, sequential selection,
 * or from a win file. These button in turn launch different dialogs. The bulk
 * select button launches the BulkDrawChooser, the sequential selection simply
 * puts the user into the tab workspace, and the "load from file" selection causes
 * the user to be presented with a file selection dialog. Loading from file relies on
 * the TabFromWinFileCompositor to set up the MultiSpectrumView with the appropriate
 * layout of spectra.
 *
 */
class SpectrumLayoutDialog : public QWidget
{
    Q_OBJECT
public:
    explicit SpectrumLayoutDialog(std::shared_ptr<SpecTclInterface> pSpecTcl,
                                  QWidget *parent = 0);
    
    QString getTabName() const;

signals:
    void spectraChosenToDraw(QStringList selected);
    void loadFileChosen(QString fileName);

public slots:
    void onBulkDrawSelected();
    void onSequentialDrawSelected();
    void onReturnToMainSelected();
    void onSelectionComplete(QStringList selection);
    void onLoadFileSelected(QString fileName);

private:
    std::shared_ptr<SpecTclInterface> m_pSpecTcl;
    TabLayoutDialog*    m_pMainChooser;
    BulkDrawChooser*        m_pBulkChooser;
};

} // end Viewer namespace

#endif // SPECTRUMLAYOUTDIALOG_H
