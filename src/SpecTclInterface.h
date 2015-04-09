#ifndef SPECTCLINTERFACE_H
#define SPECTCLINTERFACE_H

class GSlice;
class GGate;

class SpecTclInterface
{
public:
    SpecTclInterface();

    virtual void addGate(const GSlice& slice) = 0;
    virtual void editGate(const GSlice& slice) = 0;
    virtual void addGate(const GGate& slice) = 0;
    virtual void editGate(const GGate& slice) = 0;

};

#endif // SPECTCLINTERFACE_H
