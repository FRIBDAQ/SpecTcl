#include "SliceTableItem.h"
#include "GSlice.h"
#include <QString>

SliceTableItem::SliceTableItem(QListWidget *parent)
    : QListWidgetItem(parent),
      m_pSlice(nullptr)
{}

SliceTableItem::SliceTableItem(const QString& text,
                               QListWidget *parent,
                               int type,
                               GSlice* pSlice)
    : QListWidgetItem(text, parent, type),
   m_pSlice(pSlice)
{}

SliceTableItem::~SliceTableItem()
{
}


