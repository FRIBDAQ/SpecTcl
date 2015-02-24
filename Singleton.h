#ifndef SINGLETON_H
#define SINGLETON_H

#include <memory>

template<class T> class Singleton
{

    // in case anyone wants to derive a test version
protected:
    static std::unique_ptr<T> m_singleton;
    Singleton() : m_singleton(new T) {}
    Singleton(const Singleton&) = delete;
    ~Singleton() = default;

public:
    T* getInstance() {
        if (m_singleton==nullptr) {
            m_singleton.reset(new T);
        }
        return m_singleton.get();
    }
};

template<class T> std::unique_ptr<T> Singleton<T>::m_singleton(nullptr);

#endif // SINGLETON_H
