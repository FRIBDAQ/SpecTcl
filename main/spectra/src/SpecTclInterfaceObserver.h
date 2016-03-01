#ifndef SPECTCLINTERFACEOBSERVER_H
#define SPECTCLINTERFACEOBSERVER_H

#include <memory>

namespace Viewer {

/// \class SpecTclInterfaceObserver
///
/// There should be be a single instance of the SpecTclInterface that exists at
/// any given time in the program. This instance can be swapped out though with a
/// different instance. When that happens the SpecTclInterface is replaced,
/// all of the dependent objects (HistogramView, ControlPanel, etc.) need to be passed
/// the new SpecTclInterface so that they can set up their respective signals and slots.
/// Derived classes of this interface can be registered to the MainWindow to be
/// notified when a change happens.
class SpecTclInterfaceObserver {
public:
    virtual ~SpecTclInterfaceObserver() {}
    virtual void update(std::shared_ptr<SpecTclInterface> pInterface ) = 0;
};

/// \class GenericSpecTclInterfaceObserver
///
/// A generic wrapper to implement the observer dependencies for the
/// SpecTclInterface. This is simply an adaptor class that delegates to an object
/// it maintains a reference to.
template<class T>
class GenericSpecTclInterfaceObserver : public SpecTclInterfaceObserver {
private:
    T* m_pObj; ///< the object to delegate to

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

    /*! Pass the SpecTclInterface to the object
     *
     *  \param pInterface   the new instance of SpecTclInterface
     */
    void update(std::shared_ptr<SpecTclInterface> pInterface) {
        m_pObj->setSpecTclInterface(pInterface);
    }
};

} // end of namespace

#endif // SPECTCLINTERFACEOBSERVER_H
