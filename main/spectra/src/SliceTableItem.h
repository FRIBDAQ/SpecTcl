//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2015.
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

} // end of namespace

Q_DECLARE_METATYPE(Viewer::SliceTableItem)


#endif // SLICETABLEITEM_H
