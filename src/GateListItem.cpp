#include "GateListItem.h"

namespace Viewer
{

GateListItem::GateListItem(const QString& name,
                             QListWidget* parent,
                             int type,
                             GGate* pGate)
    :
    QListWidgetItem(name, parent, type),
    m_pGate(pGate)
{
}

GateListItem::~GateListItem()
{
}


} // end of namespace
