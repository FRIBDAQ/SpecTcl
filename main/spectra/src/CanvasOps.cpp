#include "CanvasOps.h"
#include "QRootCanvas.h"

#include "TList.h"
#include "TPad.h"
#include "TCanvas.h"
#include "TH1.h"

namespace Viewer {
namespace CanvasOps {


std::vector<QString> extractAllHistNames(QRootCanvas& rCanvas) {

    return extractAllHistNames(*(rCanvas.getCanvas()));
}

std::vector<QString> extractAllHistNames(TPad &rPad) {

    std::vector<QString> histNames;

    auto pList = rPad.GetListOfPrimitives();

    TObject *pObject = nullptr;
    TIter it(pList);
    while (( pObject = it.Next() )) {

        // recurse into subpads
        if (pObject->InheritsFrom(TPad::Class())) {

            auto results = extractAllHistNames(dynamic_cast<TPad&>(*pObject));

            histNames.insert(histNames.end(), results.begin(), results.end());

        } else if (pObject->InheritsFrom(TH1::Class())) {
            auto pHist = dynamic_cast<TH1*>(pObject);

            histNames.push_back(QString(pHist->GetName()));
        }
    }


    return histNames;
}

}
}
