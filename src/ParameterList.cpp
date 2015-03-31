//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2015.
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    Authors:
//    Jeromy Tompkins 
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321

static const char* Copyright = "(C) Copyright Michigan State University 2015, All rights reserved";
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
