#ifndef GATEBINDEROBSERVER_H
#define GATEBINDEROBSERVER_H

#include "Histogrammer.h"

#include <string>

class CDisplayInterface;
class CHistogrammer;


class CGateBinderObserver : public CGateObserver
{
private:
    CDisplayInterface* m_pInterface;
    CHistogrammer*     m_pSorter;

public:
    CGateBinderObserver(CDisplayInterface& rInterface, CHistogrammer& rSorter);

    void onAdd(std::string name, CGateContainer &rGate);
    void onRemove(std::string name, CGateContainer &rGate);
    void onChange(std::string name, CGateContainer& rGate);
};

#endif // GATEBINDEROBSERVER_H
