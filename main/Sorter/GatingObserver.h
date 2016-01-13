#ifndef GATINGOBSERVER_H
#define GATINGOBSERVER_H

class CGateContainer;
class CSpectrum;

class CGatingObserver
{
public:
    virtual ~CGatingObserver() {};

    virtual void onApply(const CGateContainer& rGate, CSpectrum& rSpectrum ) = 0;

};

#endif // GATINGOBSERVER_H
