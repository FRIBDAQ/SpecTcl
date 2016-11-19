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

#include "DisplayFactory.h"
#include "daqdatatypes.h"

using namespace std;


CDisplayFactory::CDisplayFactory() : m_creators() {}


CDisplayFactory::~CDisplayFactory()
{
    typedef typename map<string, CDisplayCreator*>::iterator iterator;

    iterator it  = m_creators.begin();
    iterator end = m_creators.end();

    while ( it != end ) {
        delete it->second;
        ++it;
    }

}

CDisplay* CDisplayFactory::create(const string &type)
{
    CDisplayCreator* pCreator = getCreator(type);

    if (pCreator != kpNULL) {
        return pCreator->create();
    } else {
        return static_cast<CDisplay*>(kpNULL);
    }
}

CDisplayCreator* CDisplayFactory::getCreator(const std::string& type)
{
    map<string, CDisplayCreator*>::iterator found;

    found = m_creators.find(type);
    if (found != m_creators.end()) {
        return found->second;
    } else {
      return static_cast<CDisplayCreator*>(kpNULL);
    }
}

bool CDisplayFactory::addCreator(const string &type, CDisplayCreator &rCreator)
{
    pair<map<string, CDisplayCreator*>::iterator, bool> result;

    result = m_creators.insert(make_pair(type, &rCreator));

    return result.second;
}

CDisplayCreator* CDisplayFactory::removeCreator(const std::string& type)
{
    CDisplayCreator* pCreator = static_cast<CDisplayCreator*>(kpNULL);
    map<string, CDisplayCreator*>::iterator found;

    found = m_creators.find(type);
    if (found != m_creators.end()) {
        pCreator = found->second;
        m_creators.erase(found);
    }

    return pCreator;
}
