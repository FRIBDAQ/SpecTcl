#ifndef GATELISTITEM_H
#define GATELISTITEM_H

#include <QListWidgetItem>

class GGate;

class GateListItem : public QListWidgetItem
{
 public:
    explicit GateListItem(const QString& name,
                           QListWidget* parent = nullptr,
                           int type = 0,
                           GGate* pCut = nullptr);

    virtual ~GateListItem() {}

    GGate* getGate() { return m_pGate; }
    
private:
    GGate* m_pGate;
};

#endif // GATELISTITEM_H
