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

#include <stdexcept>
#include <iostream>

namespace Viewer {
namespace CanvasOps {


std::vector<QString> extractAllHistNames(QRootCanvas& rCanvas) {

    return extractAllHistNames(*(rCanvas.getCanvas()));
}

std::vector<QString> extractAllHistNames(TVirtualPad &rPad) {

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


QString getDrawOption(TVirtualPad *pPad, TObject *pObj)
{
    if (pPad == nullptr) {
        throw std::runtime_error("CanvasOps::getDrawOption() passed nullptr to pad");
    }

    if (pObj == nullptr) {
        throw std::runtime_error("CanvasOps::getDrawOption() passed nullptr to TObject");
    }

    QString option;

    TObjLink* pLink = nullptr;
    if (( pLink = findOptionLink(*pPad, *pObj) )) {
        option = pLink->GetOption();
    }else {
        throw std::runtime_error("CanvasOps::getDrawOption() TObject not found in pad");
    }

    return option;
}


void setDrawOption(TVirtualPad *pPad, TObject *pObj, const QString &opt)
{
    if (pPad == nullptr) {
        throw std::runtime_error("CanvasOps::setDrawOption() passed nullptr to pad");
    }

    if (pObj == nullptr) {
        throw std::runtime_error("CanvasOps::setDrawOption() passed nullptr to TObject");
    }

    TObjLink* pLink = nullptr;
    if (( pLink = findOptionLink(*pPad, *pObj) )) {
        std::cout << "Setting option to : '" << opt.toUtf8().constData() << "'" << std::endl;
        pLink->SetOption(opt.toUtf8().constData());
    } else {
        throw std::runtime_error("CanvasOps::setDrawOption() TObject not found in pad");
    }

}

TObjLink* findOptionLink(TVirtualPad &pad, TObject &obj)
{
    TObjLink* pFoundLink = nullptr;

    TObjLink* pLink = pad.GetListOfPrimitives()->FirstLink();
    while (pLink) {
        if (pLink->GetObject() == &obj) {
            pFoundLink = pLink;
            break;
        }
        pLink = pLink->Next();
    }

    return pFoundLink;
}

}
}
