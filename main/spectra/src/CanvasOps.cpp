//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2016.
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    Authors:
//    Jeromy Tompkins
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321

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
