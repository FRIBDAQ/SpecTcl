//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2016.
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
