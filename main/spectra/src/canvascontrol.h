#ifndef CANVASCONTROL_H
#define CANVASCONTROL_H

#include <QWidget>

namespace Ui {
class canvascontrol;
}
namespace Viewer {
class SpectrumView;
class SpecTclInterface;
class ControlPanel;

class CanvasControl : public QWidget
{
    Q_OBJECT

public:
    explicit CanvasControl(
        SpectrumView* pView, SpecTclInterface* pInterface, ControlPanel* pPanel,
        QWidget *parent = 0
    );
    ~CanvasControl();

private slots:
    void on_pClearSelected_clicked();

    void on_pClearVisible_clicked();

    void on_pClearAll_clicked();

    void on_pEmptySelected_clicked();

    void on_pEmptyVisible_clicked();

private:
    Ui::canvascontrol *ui;
    SpectrumView*      m_pView;
    SpecTclInterface*  m_pSpecTcl;
    ControlPanel*      m_pControlPanel;
};

}
#endif // CANVASCONTROL_H
