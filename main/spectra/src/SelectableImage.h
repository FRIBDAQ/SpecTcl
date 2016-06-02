/*
 * SelectableImage.h
 *
 *  Created on: Jun 1, 2016
 *      Author: tompkins
 */

#ifndef SELECTABLEIMAGE_H_
#define SELECTABLEIMAGE_H_

#include <QAbstractButton>
#include <QPixmap>
#include <QString>
#include <QSize>

class QPaintEvent;
class QPoint;

namespace Viewer {

class SelectableImage : public QAbstractButton {
public:
	SelectableImage(const QPixmap& image, const QString& name, QWidget* pParent);
	virtual ~SelectableImage();

	virtual void paintEvent(QPaintEvent* pEvent);
	virtual void resizeEvent(QResizeEvent* pEvent);

	virtual QSize sizeHint() const;
	virtual bool hitButton(const QPoint& pos) const;

private:
	QPixmap	m_pixmap;
	QPixmap m_activePixmap;
};

} /* namespace Viewer */
#endif /* SELECTABLEIMAGE_H_ */
