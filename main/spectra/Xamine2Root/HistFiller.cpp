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

#include "HistFiller.h"

#include <TH1.h>
#include <TH2.h>
#include <dispshare.h>

#include <stdexcept>

#include <iostream>

extern spec_shared* xamine_shared;

namespace Xamine2Root
{
    void HistFiller::fill(TH1 &rHist, std::string name) {
        char* pName = const_cast<char*>(name.c_str());
        int id = xamine_shared->getspecid(pName);

        fill(rHist, id);
    }

    void HistFiller::fill(TH1 &rHist, int id) {
        if (rHist.GetDimension() != spectrumDimension(id)) {
            std::cout << rHist.GetDimension() << std::endl;
            std::cout << spectrumDimension(id) << std::endl;
            throw std::runtime_error("HistFiller::fill() Spectrum dimension does not match ROOT hist dimension.");
        }

        if (spectrumDimension(id) == 1) {
            fill1D(rHist, id);
        } else {
            // this will enforce the validity of the TH2 claim
            fill2D(dynamic_cast<TH2&>(rHist), id);
        }

    }

    void HistFiller::fill1D(TH1& rHist, int id) {
        using namespace std;

        int nx = xamine_shared->getxdim(id);

        if (nx != rHist.GetNbinsX()) {
            throw std::runtime_error("HistFiller::fill() Axis dimensions do not match.");
        }

        TAxis* pXaxis = rHist.GetXaxis();

        for (int xbin=1; xbin<=pXaxis->GetNbins(); xbin++) {
                unsigned int content = xamine_shared->getchannel(id, xbin-1);
                rHist.SetBinContent(xbin, content);
        }
    }

    void HistFiller::fill2D(TH2& rHist, int id) {
        using namespace std;

        int nx = xamine_shared->getxdim(id);
        int ny = xamine_shared->getydim(id);

        if (nx != rHist.GetNbinsX() || ny != rHist.GetNbinsY()) {
            throw std::runtime_error("HistFiller::fill() Axis dimensions do not match.");
        }

        TAxis* pXaxis = rHist.GetXaxis();
        TAxis* pYaxis = rHist.GetYaxis();

        for (int ybin=1; ybin<=pYaxis->GetNbins(); ybin++) {
            for (int xbin=1; xbin<=pXaxis->GetNbins(); xbin++) {
                unsigned int content = xamine_shared->getchannel(id, xbin-1, ybin-1);
                rHist.SetBinContent(xbin, ybin, content);
            }
        }
    }

    int HistFiller::spectrumDimension(int id) {
        spec_type type = xamine_shared->gettype(id);

        int dim = 0;
        switch (type) {
        case twodlong:
        case twodword:
        case twodbyte:
            dim = 2;
            break;
        case onedlong:
        case onedword:
            dim = 1;
            break;
        case undefined:
            dim = 0;
            break;
        }
        return dim;
    }

}
