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


  /*!
 * \brief The ControlPanel class
 *
 * The megawidget that contains the update selected, update all, refresh,
 * and geometry selector controls.
 *
 * This also implements the requisite methods to be used in the
 * GenericSpecTclInterfaceObserver class.
 *
 */
class ControlPanel : public QWidget
{
    Q_OBJECT
    
public:
    /*!
     * \brief Constructor
     *
     * \param pSpecTcl  reference to the spectcl interface
     * \param pView     the viewer widget
     * \param parent    the owning widget
     *
     * The megawidget is constructed.
     */
    explicit ControlPanel(std::shared_ptr<SpecTclInterface> pSpecTcl,
                          SpectrumView* pView, QWidget *parent = 0);

    /*!
     * Destructor
     */
    ~ControlPanel();

    /*!
     * \brief Pass in a new SpecTclInterface to use
     *
     * \param pSpecTcl  the new interface
     */
    void setSpecTclInterface(std::shared_ptr<SpecTclInterface> pSpecTcl);


    //////////////////////////////////////////////////////////////////////////
public slots:
    /*!
     * \brief Logic for "update selected" button
     *
     * Makes a direct call to SpecTclInterface
     */
    void onUpdateSelected();

    /*!
     * \brief Logic for "update all" button
     *
     * Makes a direct call to SpecTclInterface
     */
    void onUpdateAll();

    /*!
     * \brief Logic for "refresh" button
     */
    void onRefresh();

    /*!
     * \brief Logic for when column count changes
     */
    void onColumnCountChanged(int);

    /*!
     * \brief Logic for when row count changes
     */
    void onRowCountChanged(int);

    void onDrawButtonClicked();

    ///////////////////////////////////////////////////////////////////////////
signals:

    /*!
     * \brief geometryChanged
     *
     * \param nRows     the new number of rows
     * \param nCols     the new number of columns
     *
     * The MultiSpectrumView will receive these signals.
     */
    void geometryChanged(int nRows, int nCols);

    //////////////////////////////////////////////////////////////////////////
    // Helper methods
private:
    void connectSignalsAndSlots();
    void assembleWidgets();

private:
    Ui::ControlPanel                    *ui;
    GeometrySelector                    *m_pGeoSelector;
    std::shared_ptr<SpecTclInterface>   m_pSpecTcl;
    SpectrumView*                       m_pView;
};

} // end of namespace

#endif // CONTROLPANEL_H
