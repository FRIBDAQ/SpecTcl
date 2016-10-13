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

/*!
 * \brief The SelectableImage class
 *
 * This is really just a checkbutton that is represented by an image
 *
 * When the image is clicked on, a red border is drawn around the outside
 * of it and the text is changed to red. This is used in the ConfigCopySelector.
 *
 * It maintains two pixmaps. The first pixmap is a copy of the pixmap that is
 * passed into it at construction. The second pixmap is the actual pixmap that
 * is displayed. This latter pixmap can be whitened out or refreshed to look
 * like the original pixmap passed in.
 */
class SelectableImage : public QAbstractButton {
public:
	SelectableImage(const QPixmap& image, const QString& name, QWidget* pParent);
	virtual ~SelectableImage();

	virtual void paintEvent(QPaintEvent* pEvent);
	virtual void resizeEvent(QResizeEvent* pEvent);

	virtual QSize sizeHint() const;
	virtual bool hitButton(const QPoint& pos) const;

    /*!
     * \brief Replace the active pixmap with the appropriate version
     * \param whiten    whether to whiten out the image or not
     *
     * If the pixmap will be whitened out, then replace the active pixmap with white.
     * If it will not be whitenend out, then reaplace the active pixmap with m_pixmap
     */
    virtual void whitenOut(bool whiten);

private:
	QPixmap	m_pixmap;
	QPixmap m_activePixmap;
    bool    m_isWhitened;
};

} /* namespace Viewer */
#endif /* SELECTABLEIMAGE_H_ */
