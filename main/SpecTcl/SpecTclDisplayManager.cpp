#include "SpecTclDisplayManager.h"

#include "NullDisplay.h"
#include "Xamineplus.h"

CSpecTclDisplayInterface::CSpecTclDisplayInterface()
    : CDisplayInterface()
{
    getFactory().addCreator("null",   *(new CNullDisplayCreator));
    getFactory().addCreator("xamine", *(new CXamineCreator(20*1024*1024)));


    createDisplay("none", "null");
    setCurrentDisplay("none");

}
