#ifndef SPECTCLINTERFACE_H
#define SPECTCLINTERFACE_H

class GSlice;
class GGate;


/*! \brief A facade for the actual interaction with SpecTcl
 *
 * This will form a nice API that can be called to accomplish
 * certain things. The caller needs to know nothing about the implementation.
 *
 * This way, we provide the ability to potentially run on a REST interface
 * or something else.
 *
 */
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
