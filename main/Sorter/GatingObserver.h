#ifndef GATINGOBSERVER_H
#define GATINGOBSERVER_H

class CGateContainer;
class CSpectrum;
class CHistogrammer;

class CGatingObserver
{
public:
    virtual ~CGatingObserver() {};

    virtual void onApply(const CGateContainer& rGate, CSpectrum& rSpectrum,
                         CHistogrammer& rSorter) = 0;
    virtual void onRemove(const CGateContainer& rGate, CSpectrum& rSpectrum,
                          CHistogrammer& rSorter) = 0;

};

#endif // GATINGOBSERVER_H
