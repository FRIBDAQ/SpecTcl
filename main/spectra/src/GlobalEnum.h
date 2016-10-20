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


#ifndef GLOBALENUM_H
#define GLOBALENUM_H

namespace Viewer
{
  // Defines some enumerations that are used throughout the rest of the
  // application.

  namespace Vwr {
    // these are used in the TGo4CreateNewHistogram class
    enum Axis {
      XAxis,
      YAxis,
      ZAxis
    };
    
  } // end of Vwr namespace

} // end of Viewer namespace

#endif
