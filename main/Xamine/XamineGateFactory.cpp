#include "XamineGateFactory.h"
#include "XamineGate.h"
#include "XamineSharedMemory.h"
#include "Spectrum.h"
#include "GateContainer.h"
#include "Cut.h"
#include "PointlistGate.h"
#include "Gamma2DW.h"

#include <vector>
#include <assert.h>

using namespace std;

CXamineGateFactory::CXamineGateFactory(CXamineSharedMemory *pSharedMem)
    : m_pMemory(pSharedMem)
{
}

CXamineGate* CXamineGateFactory::fromSpecTclGate(CSpectrum& rSpectrum, CGateContainer &rGate)
{
    CXamineGate* pXGate;
    UInt_t nBindingId = m_pMemory->FindDisplayBinding(rSpectrum.getName());
    assert(nBindingId != -1); // make sure the thing was found

    CSpectrum* pSpectrum = &rSpectrum;

    // Summary spectra don't have gates displayed.

    if(pSpectrum->getSpectrumType() == keSummary) { // Summary spectrum.
        return (CXamineGate*)(kpNULL);
    }

    if((rGate->Type() == std::string("s")) ||
            (rGate->Type() == std::string("gs"))) {	// Slice gate
        CDisplayCut* pCut = new CDisplayCut(nBindingId,
                                            rGate.getNumber(),
                                            rGate.getName());
        // There are two points, , (low,0), (high,0)

        CCut& rCut = (CCut&)(*rGate);

        switch(pSpectrum->getSpectrumType()) {
        case ke1D:
        case keG1D:
        {
            // Produce the nearest channel to the gate points.
            // then add them to the display gate.
            //
            int x1 = (int)(pSpectrum->ParameterToAxis(0, rCut.getLow()));
            int x2 = (int)(pSpectrum->ParameterToAxis(0, rCut.getHigh()));
            pCut->AddPoint(x1,
                           0);
            // The weirdness below is all about dealing with a special boundary
            // case when we try to get the right side of the cut to land
            // on the right side of the channel on which it's set.
            // ..all this in the presence of gates accepted on fractional parameters.
            //(consider a fine spectrum (e.g. 400-401 with 100 bins and a coarse
            // spectrum, of the same parameter (e.g. 0-1023 1024 bins)..with
            // the gate set on 400.5, 400.51 and you'll see the thing I'm trying
            // to deal with here.
            //
            pCut->AddPoint(x1 == x2 ? x2 : x2 - 1,
                           0);
            return pCut;
            break;
        }

        }
    }
    else if ((rGate->Type() == std::string("b")) ||
             (rGate->Type() == std::string("gb"))) { // Band gate.
        pXGate = new CDisplayBand(nBindingId,
                                  rGate.getNumber(),
                                  rGate.getName());
    }
    else if ((rGate->Type() == std::string("c")) ||
             (rGate->Type() == std::string("gc"))) { // Contour gate
        pXGate = new CDisplayContour(nBindingId,
                                     rGate.getNumber(),
                                     rGate.getName());
    }
    else {			// Other.
        return (CXamineGate*)kpNULL;
    }
    // Control falls through here if 2-d and we just need
    // to insert the points.  We know this is a point list gate:

    assert((rGate->Type() == "b") || (rGate->Type() == "c") ||
           (rGate->Type() == "gb") || (rGate->Type() == "gc"));

    // If the spectrum is not 2-d the gate can't be displayed:
    //

    if((pSpectrum->getSpectrumType() == ke2D)   ||
            (pSpectrum->getSpectrumType() == keG2D)  ||
            (pSpectrum->getSpectrumType() == ke2Dm)  ||
            (pSpectrum->getSpectrumType() == keG2DD)) {


        CPointListGate& rSpecTclGate = (CPointListGate&)rGate.operator*();
        vector<FPoint> pts = rSpecTclGate.getPoints();
        //    vector<UInt_t> Params;
        //    pSpectrum->GetParameterIds(Params);

        // If necessary flip the x/y coordinates of the gate.
        // note that gamma gates never need flipping.
        //

        //    if((rSpecTclGate.getxId() != Params[0]) &&
        //   ((rSpecTclGate.Type())[0] != 'g')) {

        if ((rSpecTclGate.Type()[0] != 'g') &&
                flip2dGatePoints(pSpectrum, rSpecTclGate.getxId())) {
            for(UInt_t i = 0; i < pts.size(); i++) {	// Flip pts to match spectrum.
                Float_t x = pts[i].X();
                Float_t y = pts[i].Y();
                pts[i] = FPoint(y,x);
            }
        }
        // The index of the X axis transform is easy.. it's 0, but the
        // y axis transform index depends on spectrum type sincd gammas
        // have all x transforms first then y and so on:
        //
        int nYIndex;
        if((pSpectrum->getSpectrumType() == ke2D)   ||
                (pSpectrum->getSpectrumType() == keG2DD) ||
                (pSpectrum->getSpectrumType() == ke2Dm)) {
            nYIndex = 1;
        }
        else {
            CGamma2DW* pGSpectrum = (CGamma2DW*)pSpectrum;
            nYIndex               = pGSpectrum->getnParams();
        }

        for(UInt_t i = 0; i < pts.size(); i++) {

            CPoint pt((int)pSpectrum->ParameterToAxis(0, pts[i].X()),
                      (int)pSpectrum->ParameterToAxis(nYIndex, pts[i].Y()));
            pXGate->AddPoint(pt);

        }
    } else {
        return (CXamineGate*)kpNULL;
    }

    return pXGate;
}

/**
 * flip2dGatePoints
 *   Determine if the gate point coordinates must be flipped.  This happens
 *   for e.g. a gate on p1, p2 displayed on a spectrum with axes p2, p1
 *
 *  There's an implicit assumption that the gate is displayable on this spectrum
 *  because all we do is see if the X parameter is a match for a spectrum x parameter
 *  and, if not, flip.
 *
 * @param pSpectrum - pointer to the target spectrum.
 * @param gXparam   - Id of the x parameter of the spectrum.
 *
 * @return bool - true if it's necessary to flip axes.
 *
 */
bool
CXamineGateFactory::flip2dGatePoints(CSpectrum* pSpectrum, UInt_t gXparam)
{
  std::vector<UInt_t> params;
  pSpectrum->GetParameterIds(params);
  if (pSpectrum->getSpectrumType() == ke2Dm) {
    for (int i = 0; i < params.size(); i += 2) {
      if (gXparam == params[i]) return false;
    }
    return true;
  } else {
    return gXparam != params[0];
  }
}
