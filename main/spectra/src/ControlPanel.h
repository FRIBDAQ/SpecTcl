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

#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QWidget>

#include <memory>

namespace Ui {
class ControlPanel;
}

namespace Viewer
{
  class GeometrySelector;
  class SpecTclInterface;
  class SpectrumView;

class ControlPanel : public QWidget
{
    Q_OBJECT
    
public:
    explicit ControlPanel(std::shared_ptr<SpecTclInterface> pSpecTcl,
                          SpectrumView* pView, QWidget *parent = 0);
    ~ControlPanel();

    void setSpecTclInterface(std::shared_ptr<SpecTclInterface> pSpecTcl);

public slots:
    void onUpdateSelected();
    void onUpdateAll();
    void onColumnCountChanged(int);
    void onRowCountChanged(int);
    void onRefresh();

signals:
    void updateSelected();
    void updateAll();
    void geometryChanged(int nRows, int nCols);

private:
    Ui::ControlPanel *ui;
    GeometrySelector* m_pGeoSelector;
    std::shared_ptr<SpecTclInterface> m_pSpecTcl;
    SpectrumView* m_pView;
};

} // end of namespace

#endif // CONTROLPANEL_H
