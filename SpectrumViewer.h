#ifndef SPECTRUMVIEWER_H
#define SPECTRUMVIEWER_H

#include "ContentRequestHandler.h"
#include <QFrame>
#include <QPair>

// temporary
class TH1;
class QRootCanvas;
class QString;
class GuardedHist;

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

public slots:
    void requestUpdate();

    void update(const GuardedHist& gHist);

    void onError(int errorCode, const QString& reason);

private:
    QUrl formUpdateRequest();

private:
    Ui::SpectrumViewer*   ui;
    TH1*                  m_currentHist;
    QRootCanvas*          m_canvas;
    ContentRequestHandler m_reqHandler;
    QRootCanvas*          m_currentCanvas;
};

#endif // SPECTRUMVIEWER_H
