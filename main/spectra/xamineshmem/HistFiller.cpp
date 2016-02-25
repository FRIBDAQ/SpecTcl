#include "HistFiller.h"

#include <TH1.h>
#include <TH2.h>
#include <dispshare.h>

#include <stdexcept>

#include <iostream>

extern spec_shared* xamine_shared;

namespace x2r
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
        int nx = xamine_shared->getxdim(id);

        if (nx != rHist.GetNbinsX()) {
            throw std::runtime_error("HistFiller::fill() Axis dimensions do not match.");
        }

        TAxis* pXaxis = rHist.GetXaxis();

        for (int xbin=pXaxis->GetFirst(); xbin<=pXaxis->GetLast(); xbin++) {
                unsigned int content = xamine_shared->getchannel(id, xbin-1);
                rHist.SetBinContent(xbin, content);
        }
    }

    void HistFiller::fill2D(TH2& rHist, int id) {
        int nx = xamine_shared->getxdim(id);
        int ny = xamine_shared->getydim(id);

        if (nx != rHist.GetNbinsX() || ny != rHist.GetNbinsY()) {
            throw std::runtime_error("HistFiller::fill() Axis dimensions do not match.");
        }

        TAxis* pXaxis = rHist.GetXaxis();
        TAxis* pYaxis = rHist.GetYaxis();

        for (int ybin=pYaxis->GetFirst(); ybin<=pYaxis->GetLast(); ybin++) {
            for (int xbin=pXaxis->GetFirst(); xbin<=pXaxis->GetLast(); xbin++) {
                unsigned int content = xamine_shared->getchannel(id, xbin-1, ybin-1);
                std::cout << content << std::endl;
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
        default:
            dim = 0;
            break;
        }
        return dim;
    }

}
