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


#ifndef BULKDRAWCHOOSER_H
#define BULKDRAWCHOOSER_H

#include <QWidget>
#include <QString>
#include <QList>

#include <vector>


// Forward declarations

class QShowEvent;
class QHideEvent;

namespace Ui {
class BulkDrawChooser;
}

namespace Viewer
{

class HistogramList;


/*!
 * \brief The BulkDrawChooser class
 *
 *  This is part of the tab display setup sequence that handles when a user
 *  chooses the bulk draw option. It is just a list of all the spectra
 *  and a filter for choosing them. The filter line edit grabs the keyboard
 *  with the intent that this should be able to set up a tab quickly for
 *  names that are similar to each other.
 */
class BulkDrawChooser : public QWidget
{
    Q_OBJECT
    
public:
    explicit BulkDrawChooser(HistogramList& rHistList,
                              QWidget *parent = 0);
    ~BulkDrawChooser();

    void setHistogramList(const QList<QString>& rHistNames);
    void grabKeyboardForFilter();
    void releaseKeyboardFromFilter();

    void showEvent(QShowEvent* pEvent);
    void hideEvent(QHideEvent* pEvent);

signals:
    void backClicked();
    void draw(QStringList selection);

public slots:
    void onBackClicked();
    void onDrawClicked();

    void reapplyFilter(const QString& filter);

private:
    void clearList();

private:
    Ui::BulkDrawChooser    *ui;
    HistogramList&          m_rHistList;
    bool                    m_selectAll;
};


} // end Viewer namespace

#endif // BULKDRAWCHOOSER_H
