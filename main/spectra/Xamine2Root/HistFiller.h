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

#ifndef HISTFILLER_H
#define HISTFILLER_H

#include <string>

class TH1;
class TH2;

namespace Xamine2Root
{
    /*! brief Reads shared memory contents into ROOT histogram
     */
    class HistFiller {
    public:
        void fill(TH1 &rHist, std::string name);
        void fill(TH1 &rHist, int id);

    private:
        void fill1D(TH1& rHist, int id);
        void fill2D(TH2& rHist, int id);
        int spectrumDimension(int id);
    };

} // end namespace
#endif // HISTFILLER_H
