#ifndef PARAMETERLIST_H
#define PARAMETERLIST_H

#include <ParameterInfo.h>
#include <QMutex>
#include <vector>
#include <string>

class ParameterList
{
  public:
    using iterator = std::vector<SpJs::ParameterInfo>::iterator;

private:
    QMutex m_mutex;
    std::vector<SpJs::ParameterInfo> m_list;

public:
    ParameterList() = default;
    ParameterList(const ParameterList&) = delete;
    ~ParameterList();

    void addParameter(const SpJs::ParameterInfo& info);

    iterator getParameter(int id);
    iterator getParameter(const std::string& name);

    // Iterator capabilities
    //
    // The caller should lock the ParameterList before using
    // the begin() and end() methods. After done inspecting the data
    // the caller is then responsible for unlocking it.
    void lock() {
      m_mutex.lock();
    }
    void unlock() {
      m_mutex.unlock();
    }

    iterator begin() { return m_list.begin(); }
    iterator end() { return m_list.end(); }
};

#endif // PARAMETERLIST_H
