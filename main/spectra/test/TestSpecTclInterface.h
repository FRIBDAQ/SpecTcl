#ifndef TESTSPECTCLINTERFACE_H
#define TESTSPECTCLINTERFACE_H

#include "SpecTclInterface.h"

class QString;
class TVirtualPad;

namespace Viewer
{

class HistogramList;
class MasterGateList;
class GGate;
class GSlice;
class QRootCanvas;
class HistogramBundle;

class TestSpecTclInterface : public SpecTclInterface
{
private:
    HistogramList* m_pHistList;
    MasterGateList* m_pGateList;

public:
    TestSpecTclInterface();

    virtual ~TestSpecTclInterface();

    virtual void addGate(const GSlice& slice);
    virtual void editGate(const GSlice& slice);
    virtual void deleteGate(const GSlice& slice);

    virtual void addGate(const GGate& slice);
    virtual void editGate(const GGate& slice);
    virtual void deleteGate(const GGate& slice);

    virtual void deleteGate(const QString& name);

    virtual void enableGatePolling(bool enable);
    virtual bool gatePollingEnabled() const;

    virtual MasterGateList* getGateList();

    virtual void enableHistogramInfoPolling(bool enable);
    virtual bool histogramInfoPollingEnabled() const;

    virtual HistogramList* getHistogramList();

    virtual void requestHistContentUpdate(QRootCanvas* pCanvas);
    virtual void requestHistContentUpdate(TVirtualPad *pPad);
    virtual void requestHistContentUpdate(const QString& hName);

// signals:
    void gateListChanged() {}
    void histogramListChanged() {}
    void histogramContentUpdated(HistogramBundle* pBundle) {}
};

} // end Viewer namespace

#endif // TESTSPECTCLINTERFACE_H
