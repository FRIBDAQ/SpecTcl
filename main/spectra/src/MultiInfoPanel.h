#ifndef MULTIINFOPANEL_H
#define MULTIINFOPANEL_H

#include <QWidget>

#include <memory>

namespace Ui {
class MultiInfoPanel;
}

namespace Viewer {

class QRootCanvas;
class SpecTclInterface;
class HistogramBundle;
class SpectrumView;

class MultiInfoPanel : public QWidget
{
    Q_OBJECT
    
public:
    explicit MultiInfoPanel(SpectrumView& rView,
                            std::shared_ptr<SpecTclInterface> pSpecTcl,
                            QWidget *parent = 0);
    ~MultiInfoPanel();
    
public slots:
    void currentCanvasChanged(QRootCanvas& rCanvas);

private:
    void addEmptyTab();
    void addTab(HistogramBundle& rBundle);
    void removeAllTabs();

private:
    Ui::MultiInfoPanel *ui;
    SpectrumView&                     m_view;
    std::shared_ptr<SpecTclInterface> m_pSpecTcl;
};

} // end Viewer namespace

#endif // MULTIINFOPANEL_H
