#include <Xamine.h>
#include <histotypes.h>
#include <iostream.h>
#include <string>
#include "Xamineplus.h"
#include "XamineSpectrumPrompt.h"
#include "XaminePushButton.h"
#include "XamineNoPrompt.h"
#include "XamineEvent.h"
#include "XamineGates.h"
#include "ButtonEvent.h"
#include "DisplayGate.h"
#include "Xamine1D.h"
#include "Xamine2D.h"

#include <sys/wait.h>
void ramp(Int_t* p, UInt_t n) {
  for(UInt_t i = 0; i < n; i++) {
    *p++ = i;
  }
} 
void wedge(const CXamine2D& spec) {
  UInt_t nx = spec.getXchannels();
  UInt_t ny = spec.getYchannels();
  if(spec.getByte()) {
    UChar_t* p = (UChar_t*)spec.getStorage();
    for(int y = 0; y < ny; y++) {
      for(int x = 0; x < nx; x++) {
	p[x+y*nx] = x+y;
      }
    }
  }
  else {
    UShort_t* p = (UShort_t*)spec.getStorage();
    for(int y = 0; y < ny; y++) {
      for(int x = 0; x < nx; x++) {
	p[x+y*nx] = x+y;
      }
    }
  }
}

void CreateSpectra(CXamine& Display)
{
  // Define a 1d and 2d spectrum.

  CXamine1D spec1((Xamine_shared*)kpNULL, std::string("One dimension"),
		  512);
  Int_t* pSpec1 = (Int_t*)Display.DefineSpectrum(spec1);
  ramp(pSpec1, 512);

  CXamine2D spec2((Xamine_shared*)kpNULL, std::string("Two d spectrum"),
		  256, 256, kfFALSE);
  Display.DefineSpectrum(spec2);
  wedge(spec2);
}
void CreateButtons(CXamine& Display)
{
  CXamineNoPrompt none; 
  Display.DefineButtonBox();
  CXaminePushButton pbutton(1, std::string("ClrGates"), kfTRUE,
			    InSpectrum,
			    none);
  Display.DefineButton(0,0, pbutton);
}
void DeleteGates(CXamine& Display, UInt_t nSpec) 
{
  CXamineGates* pGates = Display.GetGates(nSpec);
  for(CDisplayGateVectorIterator p = pGates->begin(); 
      p != pGates->end(); p++) {
    Display.RemoveGate(p->getSpectrum(), p->getId(), p->getGateType());
  }
}

main()
{
  int istat;
  CXamine Display(1024*1024);

  Display.Start();
  CreateSpectra(Display);
  CreateButtons(Display);

  // Process events..

  while(Display.isAlive()) {
    CXamineEvent Event;
    if(Display.PollEvent(-1, Event)) {
      if(CDisplayGate* pGate = Event.GateCast()) {
	Display.EnterGate(*pGate);
	delete pGate;
      }
      if(CButtonEvent* pButton = Event.ButtonCast()) {
	switch(pButton->getId()) {
	case 1:			// Delete gates on selected spectrum.
	  DeleteGates(Display, pButton->getCurrentSpectrum());
	  break;
	default:
	  cerr << "Undefined button " << pButton->getId() << endl;
	  break;
	}
	delete pButton;
      }
    }
    else {
      cerr << "Poll for event returned false from Xamine\n";
    }
    
  }

  wait(&istat);
}
