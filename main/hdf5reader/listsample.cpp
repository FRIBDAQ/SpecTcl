/*
Example program, listing spectra in the sample data file.
*/

#include "hdf5spectrumReader.h"
#include <iostream>
int main(void) {
    hdfSpectrumReader spectra("sample.hdf");
    auto names = spectra.listSpectra();
    std::cout << "Spectra in file: \n";
    for (auto n : names) {  
        std::cout << n 
            << " type " << spectra.spectrumType(n.c_str())
            << " data type " << spectra.dataType(n.c_str())
            << std::endl;
        auto xaxis = spectra.getXaxis(n.c_str());
        std::cout << "X axis: low: " << xaxis.s_low
            << " high: " << xaxis.s_high
            << " bins: "  << xaxis.s_bins 
            << std::endl;
        if (spectra.hasYAxis(n.c_str())) {
            auto yaxis = spectra.getYaxis(n.c_str());
            std::cout << "X axis: low: " << yaxis.s_low
            << " high: " << yaxis.s_high
            << " bins: "  << yaxis.s_bins 
            << std::endl;
        }
        std::vector<std::string> params = spectra.getParameters(n.c_str());
        std::cout << "(X) Parameters:\n";
        for (auto s: params) {
            std::cout << "   " << s << std::endl;
        }
        if (spectra.hasYparameters(n.c_str())) {
            params = spectra.getYParameters(n.c_str());
            std::cout << "Y Parameters:\n";
            for (auto s: params) {
                std::cout << "   " << s << std::endl;
            }
        }
        std::cout << " Has " << (spectra.hasYparameters(n.c_str()) ? " some " : " no ") << " yparameters\n";
        std::cout << "Gated on " << spectra.getGate(n.c_str()) << std::endl;


        std::cout << "----------------------------------\n";
    }


    return 0;
}