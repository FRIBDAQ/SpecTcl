/*
 * SelectableImage.h
 *
 *  Created on: Jun 1, 2016
 *      Author: tompkins
 */

#ifndef SELECTABLEIMAGE_H_
#define SELECTABLEIMAGE_H_

#include <QWidget>
#include <QPixmap>
#include <QString>
#include <QSize>

class QPaintEvent;

namespace Viewer {

class SelectableImage : public QWidget {
public:
	SelectableImage(const QPixmap& image, const QString& name, QWidget* pParent);
	virtual ~SelectableImage();

	virtual void paintEvent(QPaintEvent* pEvent);

	virtual QSize sizeHint() const;
private:
	QPixmap	m_pixmap;
	QString	m_name;
};

} /* namespace Viewer */
#endif /* SELECTABLEIMAGE_H_ */
