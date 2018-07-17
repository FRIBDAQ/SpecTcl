#include "TestSpecTclInterface.h"
#include "HistogramList.h"
#include "MasterGateList.h"

namespace Viewer
{

TestSpecTclInterface::TestSpecTclInterface()
    : m_pHistList(new HistogramList),
      m_pGateList(new MasterGateList)
{
}

TestSpecTclInterface::~TestSpecTclInterface() {
    delete m_pHistList;
    delete m_pGateList;
}

void TestSpecTclInterface::addGate(const GSlice& slice){}
void TestSpecTclInterface::editGate(const GSlice& slice){}
void TestSpecTclInterface::deleteGate(const GSlice& slice){}

void TestSpecTclInterface::addGate(const GGate& slice){}
void TestSpecTclInterface::editGate(const GGate& slice){}
void TestSpecTclInterface::deleteGate(const GGate& slice){}

void TestSpecTclInterface::deleteGate(const QString& name){}

void TestSpecTclInterface::enableGatePolling(bool enable){}
  bool TestSpecTclInterface::gatePollingEnabled() const{ return true;}

MasterGateList* TestSpecTclInterface::getGateList(){ return m_pGateList;}

void TestSpecTclInterface::enableHistogramInfoPolling(bool enable){}
  bool TestSpecTclInterface::histogramInfoPollingEnabled() const{ return true; }

HistogramList* TestSpecTclInterface::getHistogramList(){ return m_pHistList; }

void TestSpecTclInterface::requestHistContentUpdate(QRootCanvas* pCanvas){}
void TestSpecTclInterface::requestHistContentUpdate(TVirtualPad* pPad){}
void TestSpecTclInterface::requestHistContentUpdate(const QString& hName){}


} // end Viewer namespace
