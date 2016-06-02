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
	  m_activePixmap(pixmap)
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
//	QBrush brush;
//
//	brush.setColor(QColor(Qt::gray));
//	painter.fillRect(rect(), brush);

	if (isChecked()) {
//		std::cout << "button '" << text().toStdString() << "' is checked" << std::endl;
		pen.setColor(Qt::red);
		pen.setWidth(4);
	} else {
//		std::cout << "button '" << text().toStdString() << "' is NOT checked" << std::endl;
		pen.setColor(Qt::black);
		pen.setWidth(2);
	}
	painter.setPen(pen);

	QRect frame = frameGeometry();
	painter.drawRect(0, 0, frame.width(), frame.height());
	//	std::cout << "frame (x y w h) = " << frame.x() << " " << frame.y()
	//			  << " " << frame.width() << " " << frame.height() << std::endl;

	if (isChecked()) {
		painter.setPen(Qt::red);
		painter.setFont(QFont("Arial", 16));
	} else {
		painter.setPen(Qt::black);
		painter.setFont(QFont("Arial", 12));
	}
	painter.drawText(rect(), Qt::AlignCenter, text());

}

QSize SelectableImage::sizeHint() const
{
	return QSize(50, 50);
}

bool SelectableImage::hitButton(const QPoint& pos) const
{
//	std::cout << "\nhit button : " << std::endl;
//	std::cout << "point (x y) = " << pos.x() << " " << pos.y() << std::endl;
	QRect frame = frameGeometry();
//	std::cout << "box (x y w h) = " << frame.x() << " " << frame.y() << " "
//			<< frame.width() << " " << frame.height() << std::endl;

	return QAbstractButton::hitButton(pos);
}

void SelectableImage::resizeEvent(QResizeEvent* pEvent)
{
	std::cout << "resize Event" << std::endl;
	m_activePixmap = m_pixmap.scaled(pEvent->size());
}

} /* namespace Viewer */

