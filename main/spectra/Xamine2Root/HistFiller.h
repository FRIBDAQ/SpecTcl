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
