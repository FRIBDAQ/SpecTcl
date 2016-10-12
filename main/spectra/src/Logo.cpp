#include "Logo.h"

#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>

#include <iostream>

namespace Viewer {

Logo::Logo(QWidget *parent) :
    QWidget(parent),
    m_logo(":/icons/logo_with_words.png"),
    m_activeLogo(m_logo)
{}

void Logo::paintEvent(QPaintEvent *pEvent)
{
    QPainter painter(this);
    painter.drawImage(QPoint(), m_activeLogo);
}

void Logo::resizeEvent(QResizeEvent *pEvent)
{
    m_activeLogo = m_logo.scaled(pEvent->size());
}

QSize Logo::sizeHint() const
{
    return m_activeLogo.size();
}

} // end Viewer namespace
