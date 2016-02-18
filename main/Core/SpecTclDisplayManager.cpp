#include "SpecTclDisplayManager.h"

#include "NullDisplay.h"
#include "Xamineplus.h"
#include "SpectraLocalDisplay.h"

CSpecTclDisplayInterface::CSpecTclDisplayInterface()
    : CDisplayInterface()
{
    getFactory().addCreator("null",   *(new CNullDisplayCreator));
    getFactory().addCreator("xamine", *(new CXamineCreator()));
    getFactory().addCreator("spectra",
                            *(new Spectra::CSpectraLocalDisplayCreator()));

    createDisplay("none", "null");
    setCurrentDisplay("none");

}
