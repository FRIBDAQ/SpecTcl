#include "SpectrumView.h"
#include "QRootCanvas.h"

#include <TList.h>
#include <TCanvas.h>
#include <TH1.h>

using namespace std;

namespace Viewer
{

vector<TH1*> SpectrumView::getAllHists(QRootCanvas *pCanvas)
{
    vector<TH1*> hists;

    TIter it(pCanvas->getCanvas()->GetListOfPrimitives());
    TObject* pObject = nullptr;
    while (( pObject = it.Next() )) {
        if ( pObject->InheritsFrom(TH1::Class()) ) {
            auto pHist = dynamic_cast<TH1*>(pObject);
            if (pHist) {
                hists.push_back(pHist);
            }
        }
    }

    return hists;
}


} // end of namespace
