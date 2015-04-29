#include "GeometrySelector.h"
#include <QTableWidgetItem>
#include <QString>
#include <QMoveEvent>
#include <QPoint>
#include <QEvent>

#include <iostream>
#include <iomanip>

using namespace std;

GeometrySelector::GeometrySelector(QWidget *parent) :
    QWidget(parent),
    m_button(QString("Geometry"), this),
    m_nRows(4),
    m_nColumns(4),
    m_table(m_nRows, m_nColumns, this)
{
    connect(&m_button, SIGNAL(pressed()), this, SLOT(onButtonPress()));

    m_button.show();

    fillTableWithSquares();
}


void GeometrySelector::onButtonPress()
{
    m_button.hide();
    m_table.show();
}


void GeometrySelector::fillTableWithSquares()
{
    for (int row=0; row<m_nRows; ++row) {
        for (int col=0; col<m_nColumns; ++col) {
            auto pItem = new QTableWidgetItem();
            pItem->setSizeHint(QSize(8, 8));
        }
    }
}

ostream& operator<<(ostream& stream, const QPoint& point)
{
    stream << "(" << setw(4) << point.x() << ", " << setw(4) << point.y() << ")";
    return stream;
}


HighlightableBox::HighlightableBox(QWidget *parent)
    : QWidget(parent)
{
}

void HighlightableBox::enterEvent(QEvent *pEvent)
{
    auto geo = geometry();

    cout << "Enter" << endl;
    QWidget::enterEvent(pEvent);
}

void HighlightableBox::leaveEvent(QEvent *pEvent)
{
    cout << "Leave" << endl;
    QWidget::leaveEvent(pEvent);
}

void HighlightableBox::moveEvent(QMoveEvent *pEvent)
{
    cout << "Move " << pEvent->oldPos() << " -> " << pEvent->pos() << endl;
}

