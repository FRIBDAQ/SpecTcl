#ifndef SPECTRUMVIEWER_H
#define SPECTRUMVIEWER_H

#include <QFrame>

// temporary
class TH1;
class QRootCanvas;
class RequestHandler;
class QString;

namespace Ui {
class SpectrumViewer;
}

class SpectrumViewer : public QFrame
{
    Q_OBJECT
    
public:
    explicit SpectrumViewer(QWidget *parent = 0);
    ~SpectrumViewer();

public slots:
    void update(TH1* hist);

private:
    Ui::SpectrumViewer *ui;
    TH1* m_hist;
    QRootCanvas* m_canvas;
    RequestHandler* m_reqHandler;
};

#endif // SPECTRUMVIEWER_H
