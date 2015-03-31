#ifndef SPECTCLINTERFACE_H
#define SPECTCLINTERFACE_H

class GSlice;
class TCutG;

class SpecTclInterface
{
public:
    SpecTclInterface();

    virtual void addGate(const GSlice& slice) = 0;
    virtual void editGate(const GSlice& slice) = 0;
    virtual void addGate(const TCutG& slice) = 0;
    virtual void editGate(const TCutG& slice) = 0;

};

#endif // SPECTCLINTERFACE_H
