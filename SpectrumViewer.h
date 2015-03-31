//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2015.
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    Authors:
//    Jeromy Tompkins 
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321

#ifndef SPECTRUMVIEWER_H
#define SPECTRUMVIEWER_H

#include "ContentRequestHandler.h"
#include <QFrame>
#include <QPair>

class HistogramBundle;
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
    HistogramBundle*      m_currentHist;
    QRootCanvas*          m_canvas;
    ContentRequestHandler m_reqHandler;
    QRootCanvas*          m_currentCanvas;
};

#endif // SPECTRUMVIEWER_H
