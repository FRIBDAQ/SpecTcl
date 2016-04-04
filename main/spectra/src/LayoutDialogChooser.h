#ifndef LAYOUTDIALOGCHOOSER_H
#define LAYOUTDIALOGCHOOSER_H

#include <QWidget>

class QHidEvent;
class QShowEvent;

namespace Ui {
class LayoutDialogChooser;
}

namespace Viewer
{


class LayoutDialogChooser : public QWidget
{
    Q_OBJECT
    
public:
    explicit LayoutDialogChooser(QWidget *parent = 0);
    ~LayoutDialogChooser();

    QString getTabName() const;

    void showEvent(QShowEvent* pEvt);
    void hideEvent(QHideEvent* pEvt);

signals:
    void bulkClicked();
    void sequentialClicked();

public slots:
    void onBulkClicked();
    void onSequentialClicked();
    
private:
    Ui::LayoutDialogChooser *ui;
};

} // end Viewer namespace

#endif // LAYOUTDIALOGCHOOSER_H
