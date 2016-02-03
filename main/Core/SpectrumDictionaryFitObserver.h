#ifndef SPECTRUMDICTIONARYFITOBSERVER_H
#define SPECTRUMDICTIONARYFITOBSERVER_H

#include "Dictionary.h"

#include <string>

class CSpectrum;

class SpectrumDictionaryFitObserver : public DictionaryObserver<CSpectrum*>
{
public:
    SpectrumDictionaryFitObserver();

    void onAdd(std::string name, CSpectrum *&item);
    void onRemove(std::string name, CSpectrum *&item);
};

#endif // SPECTRUMDICTIONARYFITOBSERVER_H
