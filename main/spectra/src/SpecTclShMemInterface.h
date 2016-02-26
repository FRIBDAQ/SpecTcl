#ifndef VIEWER_SPECTCLSHMEMINTERFACE_H
#define VIEWER_SPECTCLSHMEMINTERFACE_H

#include "SpecTclRESTInterface.h"

#include <memory>

namespace Viewer {

class SpecTclShMemInterface : public Viewer::SpecTclInterface
{
    Q_OBJECT
public:
    SpecTclShMemInterface();
    virtual ~SpecTclShMemInterface();

public:
    virtual void addGate(const GSlice& slice);
    virtual void editGate(const GSlice &slice);
    virtual void deleteGate(const GSlice &slice);

    virtual void addGate(const GGate& gate);
    virtual void editGate(const GGate& gate);
    virtual void deleteGate(const GGate& gate);

    virtual void deleteGate(const QString& name);

    virtual void enableGatePolling(bool enable);

    virtual GateList* getGateList();

    virtual void enableHistogramInfoPolling(bool enable);

    virtual HistogramList* getHistogramList();

    virtual void requestHistContentUpdate(QRootCanvas *pCanvas);
    virtual void requestHistContentUpdate(TPad* pPad);
    virtual void requestHistContentUpdate(const QString& hName);

signals:
    void gateListChanged();
    void histogramListChanged();
    void histogramContentUpdated(HistogramBundle* pBundle);

private:
    std::unique_ptr<SpecTclRESTInterface> m_pRESTInterface;
};

} // namespace VIewer

#endif // VIEWER_SPECTCLSHMEMINTERFACE_H
