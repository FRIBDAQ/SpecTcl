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
#include "GateList.h"
#include <algorithm>
#include <TCutG.h>

using namespace std;

GateList::GateList()
    : m_gates()
{
}


void GateList::addGate(std::unique_ptr<TCutG> cut)
{
    // the unique ptr give pretty strong assurance that there
    // is no copy in the list already (it is still possible but if
    // we managed to do that, we have major issues)
    m_gates.push_back(std::move(cut));
}

GateList::iterator GateList::getGate(const QString &name)
{
    return find_if(m_gates.begin(),
                   m_gates.end(),
                   [&](const unique_ptr<TCutG>& cut) -> bool {
                      QString cname = cut->GetName();
                      return (cname==name);
                   });
}
