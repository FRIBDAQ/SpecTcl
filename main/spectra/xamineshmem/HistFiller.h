#ifndef HISTFILLER_H
#define HISTFILLER_H

#include <string>

class TH1;
class TH2;

namespace x2r
{
    /*! brief Reads shared memory contents into ROOT histogram
     */
    class HistFiller {
    public:
        void fill(TH1 &rHist, std::string name);
        void fill(TH1 &rHist, int id);

    private:
        void fill1D(TH1& rHist, unsigned int* pData);
        void fill2D(TH2& rHist, unsigned int* pData);
        int spectrumDimension(int id);
    };

} // end namespace
#endif // HISTFILLER_H
