/*
 * SelectableImage.cpp
 *
 *  Created on: Jun 1, 2016
 *      Author: tompkins
 */

#include "SelectableImage.h"
#include <QPaintEvent>
#include <QPainter>

#include <iostream>

namespace Viewer {

SelectableImage::SelectableImage(const QPixmap& pixmap, const QString& name, QWidget* pParent)
	: QWidget(pParent),
	  m_pixmap(pixmap),
	  m_name(name)
{

}

SelectableImage::~SelectableImage() {
	// TODO Auto-generated destructor stub
}

void SelectableImage::paintEvent(QPaintEvent* pEvent)
{
	QPainter brush(this);

	QRgb color;
	int value;

	QImage image = m_pixmap.toImage();
	QSize size = m_pixmap.size();
	for (int x=0; x<size.width(); ++x) {
		for (int y=0; y<size.height(); ++y) {
			color = image.pixel(x, y);
			// convert to grayscale
			int gray = 0.299*qRed(color) + 0.587*qGreen(color) + 0.114*qBlue(color);
			image.setPixel(x,y, qRgb(gray, gray, gray));
		}
	}

	brush.drawImage(QPoint(), image, rect());

	//brush.drawPixmap(QPoint(), m_pixmap, rect());
	QPen pen;
	pen.setColor(Qt::black);
	pen.setWidth(2);
	brush.setPen(pen);

	QRect frame = frameGeometry();
	brush.drawRect(0, 0, frame.width(), frame.height());

	brush.setPen(Qt::red);
	brush.setFont(QFont("Arial", 12));
	brush.drawText(rect(), Qt::AlignCenter, m_name);

	QWidget::paintEvent(pEvent);
}

QSize SelectableImage::sizeHint() const
{
	return QSize(50, 50);
}

} /* namespace Viewer */

