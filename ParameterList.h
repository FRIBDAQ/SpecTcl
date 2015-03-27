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
