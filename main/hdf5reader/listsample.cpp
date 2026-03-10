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
    }


    return 0;
}