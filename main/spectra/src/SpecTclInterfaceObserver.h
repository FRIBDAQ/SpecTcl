#ifndef SPECTCLINTERFACEOBSERVER_H
#define SPECTCLINTERFACEOBSERVER_H

#include <memory>

namespace Viewer {

class SpecTclInterfaceObserver {
public:
    virtual ~SpecTclInterfaceObserver() {}
    virtual void update(std::shared_ptr<SpecTclInterface> pInterface ) = 0;
};



///  A generic wrapper to implement the observer dependencies
template<class T>
class GenericSpecTclInterfaceObserver : public SpecTclInterfaceObserver {
private:
    T* m_pObj;

public:
    GenericSpecTclInterfaceObserver(T& obj) : m_pObj(&obj) {}
    GenericSpecTclInterfaceObserver(const GenericSpecTclInterfaceObserver& rhs)
        : m_pObj(rhs.m_pObj) {}
    ~GenericSpecTclInterfaceObserver() {}

    GenericSpecTclInterfaceObserver& operator=(const GenericSpecTclInterfaceObserver& rhs) {
        if (this != &rhs) {
            m_pObj = rhs.m_pObj;
        }
        return *this;
    }

    void update(std::shared_ptr<SpecTclInterface> pInterface) {
        m_pObj->setSpecTclInterface(pInterface);
    }
};

} // end of namespace

#endif // SPECTCLINTERFACEOBSERVER_H
