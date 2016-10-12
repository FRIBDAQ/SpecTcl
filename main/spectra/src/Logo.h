#ifndef LOGO_H
#define LOGO_H

#include <QWidget>
#include <QImage>

namespace Viewer {

class Logo : public QWidget
{
    Q_OBJECT
public:
    explicit Logo(QWidget *parent = 0);

    void paintEvent(QPaintEvent* pEvent);
    void resizeEvent(QResizeEvent* pEvent);

    virtual QSize sizeHint() const;

private:
    QImage m_logo;
    QImage m_activeLogo;
};

} // end Viewer namespace

#endif // LOGO_H
