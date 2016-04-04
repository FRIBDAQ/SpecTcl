#ifndef SPECTRUMLAYOUTDIALOG_H
#define SPECTRUMLAYOUTDIALOG_H

#include <QWidget>
#include <QStringList>

#include <memory>

namespace Viewer {

class LayoutDialogChooser;
class BulkDrawChooser;
class SpecTclInterface;

class SpectrumLayoutDialog : public QWidget
{
    Q_OBJECT
public:
    explicit SpectrumLayoutDialog(std::shared_ptr<SpecTclInterface> pSpecTcl,
                                  QWidget *parent = 0);
    
    QString getTabName() const;

signals:
    void spectraChosenToDraw(QStringList selected);

public slots:
    void onBulkDrawSelected();
    void onSequentialDrawSelected();
    void onReturnToMainSelected();
    void onSelectionComplete(QStringList selection);

private:
    std::shared_ptr<SpecTclInterface> m_pSpecTcl;
    LayoutDialogChooser*    m_pMainChooser;
    BulkDrawChooser*        m_pBulkChooser;
};

} // end Viewer namespace

#endif // SPECTRUMLAYOUTDIALOG_H
