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
    virtual HistogramList* getHistogramList() = 0;
   
  signals:
    void gateListChanged();
};

} // end of namespace

#endif // SPECTCLINTERFACE_H
