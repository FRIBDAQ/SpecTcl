#ifndef PARAMETERLIST_H
#define PARAMETERLIST_H

#include <ParameterInfo.h>
#include <vector>
#include <string>

class ParameterList
{

private:
    static ParameterList* m_instance;

    static std::vector<SpJs::ParameterInfo> m_list;

protected:
    ParameterList() = default;
    ParameterList(const ParameterList&) = delete;
    ~ParameterList();

public:
    static ParameterList* getInstance() {
        if (m_instance==nullptr) {
            m_instance = new ParameterList();
        }
        return m_instance;
    }

    static void addParameter(const SpJs::ParameterInfo& info);
    static SpJs::ParameterInfo getParameter(int id);
    static SpJs::ParameterInfo getParameter(const std::string& name);

};

#endif // PARAMETERLIST_H
