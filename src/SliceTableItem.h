#ifndef SLICETABLEITEM_H
#define SLICETABLEITEM_H

#include "GSlice.h"
#include <QListWidgetItem>
#include <QMetaType>

class QString;


class SliceTableItem : public QListWidgetItem
{

public:
    SliceTableItem(QListWidget* parent = nullptr);
    explicit SliceTableItem(const QString& name,
                            QListWidget *parent = nullptr,
                            int type = 0,
                            GSlice* pSlice = nullptr);
    virtual ~SliceTableItem();

    TLine* getLowLine() {
        return m_pSlice->getXLowLine();
    }

    TLine* getHighLine() {

        return m_pSlice->getXHighLine();
    }

    GSlice* getSlice() { return m_pSlice; }

private:
    GSlice* m_pSlice;
};

Q_DECLARE_METATYPE(SliceTableItem)

#endif // SLICETABLEITEM_H
