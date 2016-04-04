#ifndef BULKDRAWCHOOSER_H
#define BULKDRAWCHOOSER_H

#include <QWidget>
#include <QString>
#include <QList>

#include <vector>

class QShowEvent;
class QHideEvent;

namespace Ui {
class BulkDrawChooser;
}

namespace Viewer
{

class HistogramList;

class BulkDrawChooser : public QWidget
{
    Q_OBJECT
    
public:
    explicit BulkDrawChooser(HistogramList& rHistList,
                              QWidget *parent = 0);
    ~BulkDrawChooser();

    void setHistogramList(const QList<QString>& rHistNames);
    void grabKeyboardForFilter();
    void releaseKeyboardFromFilter();

    void showEvent(QShowEvent* pEvent);
    void hideEvent(QHideEvent* pEvent);

signals:
    void backClicked();
    void draw(QStringList selection);

public slots:
    void onBackClicked();
    void onDrawClicked();

    void reapplyFilter(const QString& filter);

private:
    void clearList();

private:
    Ui::BulkDrawChooser    *ui;
    HistogramList&          m_rHistList;
    bool                    m_selectAll;
};


} // end Viewer namespace

#endif // BULKDRAWCHOOSER_H
