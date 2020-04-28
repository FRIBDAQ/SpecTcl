#include <SpecTclDatabase.h>
#include <SaveSet.h>
#include <DBParameter.h>
#include <DBSpectrum.h>

#include <stdlib.h>
#include <iostream>
#include <string>
#include <stdexcept>

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "Usage:  specdef database-name\n";
        exit(EXIT_FAILURE);
    }
    
    SpecTclDB::CDatabase db(argv[1]);
    SpecTclDB::SaveSet* pSvSet(nullptr);
    try {
        pSvSet = db.getSaveSet("a saveset");
        
        std::vector<const char*> s1Params = {"p1"};
        std::vector<const char*> s2Params = {"p1", "p2"};
        SpecTclDB::SaveSet::SpectrumAxis x = {0, 1023, 1024};
        SpecTclDB::SaveSet::SpectrumAxis y = {-10.0, 1.0, 100};
        
        std::vector<SpecTclDB::SaveSet::SpectrumAxis>
            s1axes = {x};
        std::vector<SpecTclDB::SaveSet::SpectrumAxis>
            s2axes = {x, y};
            
        delete pSvSet->createSpectrum("s1", "1", s1Params, s1axes);
        delete pSvSet->createSpectrum("s2", "2", s2Params, s2axes);
        
        auto spectra = pSvSet->listSpectra();
        std::cerr << "Names of spectra in saveset:\n";
        for (int i =0; i < spectra.size(); i++) {
            auto& info = spectra[i]->getInfo();
            std::cerr << info.s_base.s_name << std::endl;
            delete spectra[i];
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        delete pSvSet;
        exit(EXIT_FAILURE);
    }
    
    delete pSvSet;
    exit(EXIT_SUCCESS);
}
