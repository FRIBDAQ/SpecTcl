#ifndef SINGLETON_HPP
#define SINGLETON_HPP

// This is a flexible implementation of a singleton pattern
// taht can be used to turn any generic type into a singleton.
// It can be set up to create any type of object using a
// specific creator function.
//
// Usage:
//  struct A {};
//  A* createA() { return new A; }
//  auto& rInstance = Singleton<A>::getInstance();
//
template<class T, class Creator=T()>
class Singleton {

private:
    static bool    m_initialized;
    static T       m_instance;
    static Creator m_creator;

private:
    Singleton() {}
    Singleton(const Singleton<T>& rhs) = 0;
    Singleton<T>& operator=() = 0;

    ~Singleton() {
        delete m_pInstance;
    }

public:

    static T& getInstance() {
        if (! m_initialized) {
            m_instance = m_creator();
        }

        return m_instance;
    }

    /*! The user can change the creator up until the
     *  time the instance is created. Once it is created,
     *  there is no changing it.
     */
    static bool setCreator(Creator creator) {
        bool success = false;

        if (!m_initialized) {
            m_creator = creator;
            success = true;
        }

        return success;
    }

};


#endif // SINGLETON_HPP
