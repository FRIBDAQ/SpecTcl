#ifndef SLICETABLEITEM_H
#define SLICETABLEITEM_H

#include "GSlice.h"
#include <QListWidgetItem>
#include <QMetaType>

class QString;

namespace Viewer
{

class QTLine;


class SliceTableItem : public QListWidgetItem
{

public:
    SliceTableItem(QListWidget* parent = nullptr);
    explicit SliceTableItem(const QString& name,
                            QListWidget *parent = nullptr,
                            int type = 0,
                            GSlice* pSlice = nullptr);
    virtual ~SliceTableItem();

    const QTLine* getLowLine() const {
        return m_pSlice->getXLowLine();
    }

    const QTLine* getHighLine() const {

        return m_pSlice->getXHighLine();
    }

    GSlice* getSlice() { return m_pSlice; }

private:
    GSlice* m_pSlice;
};

Q_DECLARE_METATYPE(SliceTableItem)

} // end of namespace

#endif // SLICETABLEITEM_H
