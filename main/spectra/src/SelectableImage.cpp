/*
 * SelectableImage.cpp
 *
 *  Created on: Jun 1, 2016
 *      Author: tompkins
 */

#include "SelectableImage.h"
#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>

#include <iostream>

namespace Viewer {

SelectableImage::SelectableImage(const QPixmap& pixmap, const QString& name, QWidget* pParent)
	: QAbstractButton(pParent),
	  m_pixmap(pixmap),
      m_activePixmap(pixmap),
      m_isWhitened(false)
{
	setText(name);
	setCheckable(true);
	setChecked(false);
	setAutoExclusive(true);

}

SelectableImage::~SelectableImage() {
	// TODO Auto-generated destructor stub
}

void SelectableImage::paintEvent(QPaintEvent* pEvent)
{
	QPainter painter(this);

	int value;

	painter.drawPixmap(QPoint(), m_activePixmap, rect());

	QPen pen;

    if (isChecked()) {
		pen.setColor(Qt::red);
		pen.setWidth(4);
	} else {
		pen.setColor(Qt::black);
		pen.setWidth(2);
	}
	painter.setPen(pen);

	QRect frame = frameGeometry();
    if (!m_isWhitened) {
        painter.drawRect(0, 0, frame.width(), frame.height());
    }

	if (isChecked()) {
		painter.setPen(Qt::red);
		painter.setFont(QFont("Arial", 16));
	} else {
        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 12));
    }

    if (!m_isWhitened) {
       painter.drawText(rect(), Qt::AlignCenter, text());
    }
}

QSize SelectableImage::sizeHint() const
{
	return QSize(50, 50);
}

bool SelectableImage::hitButton(const QPoint& pos) const
{
	QRect frame = frameGeometry();

	return QAbstractButton::hitButton(pos);
}

void SelectableImage::resizeEvent(QResizeEvent* pEvent)
{
    m_activePixmap = m_activePixmap.scaled(pEvent->size());
    m_pixmap = m_pixmap.scaled(pEvent->size());
}


void SelectableImage::whitenOut(bool whiten)
{
    if (m_isWhitened == whiten) return;

    m_isWhitened = whiten;

    if (whiten) {
        QPixmap newPm = m_pixmap;
        newPm.fill(QColor(255,255,255));
        m_activePixmap = newPm;
        setEnabled(false);
    } else {
        m_activePixmap = m_pixmap;
        setEnabled(true);
    }
}

} /* namespace Viewer */

