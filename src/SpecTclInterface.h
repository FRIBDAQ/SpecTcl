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
#ifndef SPECTCLINTERFACE_H
#define SPECTCLINTERFACE_H

#include <QObject>

#include <vector> 

class QString;

namespace SpJs {
  class GateInfo;
}

namespace Viewer
{

class GSlice;
class GGate;
class GateList;
class HistogramList;

/*! \brief A facade for the actual interaction with SpecTcl
 *
 * This will form a nice API that can be called to accomplish
 * certain things. The caller needs to know nothing about the implementation.
 *
 * This way, we provide the ability to potentially run on a REST interface
 * or something else.
 *
 */
class SpecTclInterface : public QObject
{
  Q_OBJECT

public:
    SpecTclInterface();

    virtual void addGate(const GSlice& slice) = 0;
    virtual void editGate(const GSlice& slice) = 0;
    virtual void deleteGate(const GSlice& slice) = 0;

    virtual void addGate(const GGate& slice) = 0;
    virtual void editGate(const GGate& slice) = 0;
    virtual void deleteGate(const GGate& slice) = 0;

    virtual void deleteGate(const QString& name) = 0;

    virtual void enableGatePolling(bool enable) = 0;

    virtual GateList* getGateList() = 0;

    virtual void enableHistogramInfoPolling(bool enable) = 0;

    virtual HistogramList* getHistogramList() = 0;
   
  signals:
    void gateListChanged();
    void histogramListChanged();
};

} // end of namespace

#endif // SPECTCLINTERFACE_H
