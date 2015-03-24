#include "ParameterList.h"
#include "ParameterInfo.h"

#include <algorithm>
#include <functional>
#include <stdexcept>


ParameterList::~ParameterList()
{}

void ParameterList::addParameter(const SpJs::ParameterInfo& info)
{
  QMutexLocker lock(&m_mutex);

  auto iter = std::find(m_list.begin(), m_list.end(), info);
  if (iter==m_list.end()) {
    throw std::runtime_error("Parameter already exists");
  } else {
    m_list.push_back(info);
  }
}

ParameterList::iterator ParameterList::getParameter(const std::string &name)
{
  QMutexLocker lock(&m_mutex);

  // define a simple predicate
  auto predicate = [&name](const SpJs::ParameterInfo& rhs) {
    return (name == rhs.s_name);
  };

  return std::find_if(m_list.begin(), m_list.end(), predicate);
}

ParameterList::iterator ParameterList::getParameter(int id)
{
  QMutexLocker lock(&m_mutex);
  // define a simple predicate
  auto predicate = [id](const SpJs::ParameterInfo& rhs) {
    return (id == rhs.s_id);
  };

  return std::find_if(m_list.begin(), m_list.end(), predicate);
}
