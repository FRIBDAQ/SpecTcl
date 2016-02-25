#include "HistFiller.h"

#include <TH1.h>
#include <dispshare.h>

#include <stdexcept>

namespace XToR
{
    void HistFiller::fill(TH1 &rHist, std::string name) {
        fill(rHist, ::getspecid(name.data()));
    }

    void HistFiller::fill(TH1 &rHist, int id) {
        if (rHist.GetDimension() != spectrumDimension(id)) {
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
        int nx = ::getxdim(id);

        if (nx != rHist.GetNbinsX()) {
            throw std::runtime_error("HistFiller::fill() Axis dimensions do not match.");
        }

        TAxis* pXaxis = rHist.GetXaxis();

        for (int xbin=pXaxis->GetFirst(); xbin<=pXaxis->GetLast(); xbin++) {
                unsigned int content = ::getchannel(id, xbin);
                rHist.SetBinContent(xbin, content);
        }
    }

    void HistFiller::fill2D(TH1& rHist, int id) {
        int nx = ::getxdim(id);
        int ny = ::getydim(id);

        if (nx != rHist.GetNbinsX() || ny != rHist.GetNbinsY()) {
            throw std::runtime_error("HistFiller::fill() Axis dimensions do not match.");
        }

        TAxis* pXaxis = rHist.GetXaxis();
        TAxis* pYaxis = rHist.GetYaxis();

        for (int xbin=pXaxis->GetFirst(); xbin<=pXaxis->GetLast(); xbin++) {
            for (int ybin=pYaxis->GetFirst(); ybin<=pYaxis->GetLast(); ybin++) {
                unsigned int content = ::getchannel(id, xbin, ybin);
                rHist.SetBinContent(xbin, ybin, content);
            }
        }
    }

    int HistFiller::spectrumDimension(int id) {
        spec_type type = ::gettype(id);

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
