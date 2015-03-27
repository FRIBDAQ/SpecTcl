//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2015.
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

static const char* Copyright = "(C) Copyright Michigan State University 2015, All rights reserved";
#include "HistogramBundle.h"
#include <QMutex>
#include <QMutexLocker>
#include <QString>
#include <TH1.h>
#include <TCutG.h>

HistogramBundle::HistogramBundle()
    : m_pMutex(nullptr),
      m_pHist(nullptr),
      m_cuts2d()
{}
HistogramBundle::HistogramBundle(QMutex& pMutex, TH1& pHist)
    : m_pMutex(&pMutex),
      m_pHist(&pHist),
      m_cuts2d()
{}

void HistogramBundle::addCut2D(TCutG* pCut) {
    QMutexLocker lock(m_pMutex);
    m_cuts2d.push_back(pCut);
}

void HistogramBundle::draw(const QString& opt) {
    const char* cOpts = opt.toAscii().constData();
    m_pMutex->lock();
    m_pHist->Draw(cOpts);
    m_pMutex->unlock();

    for (auto cut : m_cuts2d) {
        cut->Draw("same");
    }
}
