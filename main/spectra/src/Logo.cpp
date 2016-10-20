//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2016.
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
