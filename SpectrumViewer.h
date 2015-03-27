#ifndef SPECTRUMVIEWER_H
#define SPECTRUMVIEWER_H

#include "ContentRequestHandler.h"
#include <QFrame>
#include <QPair>

// for HistogramBundle
#include <HistogramList.h>

// temporary
class TH1;
class QRootCanvas;
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

    QRootCanvas* getCurrentFocus() const;
    HistogramBundle* getCurrentHist() const;

public slots:
    void requestUpdate();

    void update(HistogramBundle* gHist);

    void onError(int errorCode, const QString& reason);

private:
    QUrl formUpdateRequest();

private:
    Ui::SpectrumViewer*   ui;
    HistogramBundle*       m_currentHist;
    QRootCanvas*          m_canvas;
    ContentRequestHandler m_reqHandler;
    QRootCanvas*          m_currentCanvas;
};

#endif // SPECTRUMVIEWER_H
