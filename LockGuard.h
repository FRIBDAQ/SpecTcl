#ifndef LOCKGUARD_H
#define LOCKGUARD_H

template<class T> class LockGuard
{
private:
    T m_lock;
public:
    LockGuard(T obj) : m_lock(obj) {
        m_lock.lock();
    }

    ~LockGuard() {
        m_lock.unlock();
    }
};

template<class T> LockGuard<T> makeLockGuard(const T& obj) {
    return LockGuard<T>(obj);
}

#endif // LOCKGUARD_H
