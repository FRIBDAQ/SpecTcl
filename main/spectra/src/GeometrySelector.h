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

#ifndef GEOMETRYSELECTOR_H
#define GEOMETRYSELECTOR_H

#include "ui_GeometrySelector.h"

#include <QWidget>
#include <memory>

namespace Viewer
{

/*!
 * \brief The GeometrySelector class
 *
 * A simple widget that combines two spinboxes together. It is
 * intended to be used for selecting the grid dimensions of the
 * MultiSpectrumView.
 */
class GeometrySelector : public QWidget
{
    Q_OBJECT
    
public:
    /*!
     * \brief GeometrySelector
     * \param parent
     */
    explicit GeometrySelector(QWidget *parent = 0);

    /*!
     * Destructor
     */
    ~GeometrySelector();
    
    int getRowCount() const;
    int getColumnCount() const;

    //////////////////////////////////////////////////////////////////////////
public slots:
    void onRowCountChanged(int nRows);
    void onColumnCountChanged(int nColumns);

    //////////////////////////////////////////////////////////////////////////
signals:
    void rowCountChanged(int nRows);
    void columnCountChanged(int nColumns);

    //////////////////////////////////////////////////////////////////////////
private:
    std::unique_ptr<Ui::GeometrySelector> ui;
};


} // end of namespace

#endif // GEOMETRYSELECTOR_H
