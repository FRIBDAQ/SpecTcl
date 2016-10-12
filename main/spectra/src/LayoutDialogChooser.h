#ifndef LAYOUTDIALOGCHOOSER_H
#define LAYOUTDIALOGCHOOSER_H

#include <QWidget>

class QKeyEvent;
class QButtonGroup;

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
    Ui::LayoutDialogChooser *ui;
    QButtonGroup*           m_pButtons;
};

} // end Viewer namespace

#endif // LAYOUTDIALOGCHOOSER_H
