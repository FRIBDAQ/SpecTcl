

#include <json/json.h>
#include <TH1.h>
#include <TH2.h>
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <stdexcept>
#include "HistFactory.h"
#include "HistInfo.h"
#include "JsonParser.h"

using namespace std;

namespace SpJs
{

  std::unique_ptr<TH1> HistFactory::create(const char* jsonByteRep) 
  {
    return create(Json::Value(jsonByteRep));
  }

  std::unique_ptr<TH1> HistFactory::create(const Json::Value& value) 
  {
    // throws if bad
    std::vector<HistInfo> infos;
    try {
      infos = JsonParser().parseListCmd(value);
    } catch (std::exception& exc) {
      return std::unique_ptr<TH1>();
    }

    // only deals with the first one
    return create(infos.at(0));
  }

  std::unique_ptr<TH1> HistFactory::create(const HistInfo& info) {
    std::unique_ptr<TH1> pHist;

    if (info.s_axes.size() == 1) {
        if (info.s_type == "s" || info.s_type == "gs") {
            // summary spectra look only specify a single axis
            // but they are really 2d spectra
            pHist.reset(new TH2D(info.s_name.c_str(),
                                 info.s_name.c_str(),
                                 info.s_params.size(),
                                 0,
                                 info.s_params.size(),
                                 info.s_axes.at(0).s_nbins,
                                 info.s_axes.at(0).s_low,
                                 info.s_axes.at(0).s_high));

            TAxis* pAxis = pHist->GetXaxis();
            pAxis->SetTitle("");
            for (int bin=pAxis->GetFirst(); bin<=pAxis->GetLast(); ++bin) {
                pAxis->SetBinLabel(bin,
                                   info.s_params.at(bin-pAxis->GetFirst()).c_str());
            }

            pHist->GetYaxis()->SetTitle("");

        } else {
          pHist.reset(new TH1D(info.s_name.c_str(),
                               info.s_name.c_str(),
                               info.s_axes.at(0).s_nbins,
                               info.s_axes.at(0).s_low,
                               info.s_axes.at(0).s_high));

          if (info.s_type == "g1") {
              // there is no clear axis label for a gamma 1d spectrum
              pHist->GetXaxis()->SetTitle("");
              pHist->GetYaxis()->SetTitle("Counts");
          } else {
              pHist->GetXaxis()->SetTitle(info.s_params.at(0).c_str());
              pHist->GetYaxis()->SetTitle("Counts");
          }
        }
    } else if (info.s_axes.size() == 2) {
        pHist.reset(new TH2D(info.s_name.c_str(),
                             info.s_name.c_str(),
                             info.s_axes.at(0).s_nbins,
                             info.s_axes.at(0).s_low,
                             info.s_axes.at(0).s_high,
                             info.s_axes.at(1).s_nbins,
                             info.s_axes.at(1).s_low,
                             info.s_axes.at(1).s_high));

        if (info.s_type == "g2") {
            // gamma2d axes do not have well defined labels on the axes bc
            // they involve many parameters
            pHist->GetXaxis()->SetTitle("");
            pHist->GetYaxis()->SetTitle("");
        } else {
            pHist->GetXaxis()->SetTitle(info.s_params.at(0).c_str());
            pHist->GetYaxis()->SetTitle(info.s_params.at(1).c_str());
        }
    }
    pHist->SetDirectory(0);
    return std::move(pHist);
  }

}



