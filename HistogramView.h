#ifndef HISTOGRAMVIEW_H
#define HISTOGRAMVIEW_H

#include <QDockWidget>
#include <QList>
#include <QListWidget>
#include <HistInfo.h>
#include <vector>

class TH1;
class ListRequestHandler;

namespace Ui {
class HistogramView;
}

class HistogramView : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit HistogramView(QWidget *parent = 0);
    ~HistogramView();

signals:
    void histSelected(TH1* hist);

public slots:
    void onUpdate();
    void setList(std::vector<SpJs::HistInfo> list);
    void onDoubleClick(QModelIndex index);

private:
    bool histExists(const QString& name);
    void deleteHists();

private:
    Ui::HistogramView *ui;
    ListRequestHandler* m_req;
};

#endif // HISTOGRAMVIEW_H
