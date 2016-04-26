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


#ifndef TABBEDMULTISPECTRUMVIEW_H
#define TABBEDMULTISPECTRUMVIEW_H

#include "SpectrumView.h"
#include "SpecTclInterfaceObserver.h"

#include <QWidget>
#include <QStringList>

#include <memory>
#include <tuple>

class QPushButton;

namespace Ui {
class TabbedMultiSpectrumView;
}

namespace Viewer
{

class MultiSpectrumView;
class SpecTclInterface;
class TabWorkspace;

/*!
 * \brief The TabbedMultiSpectrumView class
 *
 * This is a widget that manages the toplevel tabbed viewing experience.
 * It is responsible for managing the tabs that contain MultiSpectrumViews.
 * As a derived class of the SpectrumView base class, it implements the standard
 * interface for SpectrumViews. In doing so, it interprets the requests in terms
 * of the MultiSpectrumView widget on the currently visible tab.
 *
 */
class TabbedMultiSpectrumView : public QWidget
{
    Q_OBJECT
    
public:
    /*!
     * \brief Constructor
     *
     * \param pSpecTcl  interface to use
     * \param parent    parent widget
     */
    explicit TabbedMultiSpectrumView(std::shared_ptr<SpecTclInterface> pSpecTcl,
                                     QWidget *parent = 0);

    /*!
     *  \brief Destructor - does nothing
     */
    virtual ~TabbedMultiSpectrumView();

    /*!
   * \brief addTab
   * \param title
   * \return
   */
  void addTab(const QString& title);

  void setSpecTclInterface(std::shared_ptr<SpecTclInterface> pSpecTcl);

  TabWorkspace& getCurrentWorkspace();

public slots:
  void onCurrentChanged(int index);
  void onAddTab();
  void onTabCloseRequested(int index);

  void onNewTabContentsSelected(QStringList selection);
  void onNewTabContentsFromFile(QString fileName);

private:
    Ui::TabbedMultiSpectrumView       *ui;
    MultiSpectrumView                 *m_pCurrentView;
    std::shared_ptr<SpecTclInterface>  m_pSpecTcl;
    QPushButton                       *m_pAddButton;
};


}
#endif // TABBEDMULTISPECTRUMVIEW_H
