/*
** Facility:
**   Xamine - NSCL Display program.
** Abstract:
**   convert.h:
**     This include file contains the definitions needed for clients of
**     the converters.  Converters are classes which convert coordinates
**     between screen and world coordinates.  A base converter class
**     is used to support polymorphism between 1-d and 2-d objects.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** History:
**   @(#)convert.h	8.1 6/23/95 
*/

#ifndef _CONVERT_H_INSTALLED
#define _CONVERT_H_INSTALLED

#include "XMWidget.h"
#include "dispwind.h"
#include "dispshare.h"
/*
** The structure below describes fully a spectrum location
** that you can get when translating a screen position.
*/
struct spec_location {
                        /* Meaning of the cursor position... note if 2-d then
                        ** xpos and ypos are channel numbers
                        */
                       int xpos; /* X position channels or count [flipped] */
		       int ypos;  /* Y position channels [flipped] or counts */
		       unsigned int counts; /* # counts in the spectrum here */
		     };

/*
** The class below is a base class for all of the converters.
*/

class Xamine_Converter {
 protected:
  XMWidget       *pane;
  win_attributed *attributes;
  volatile spec_shared    *spectra;
  Boolean         clipping;
 public:
  Xamine_Converter(XMWidget *p, win_attributed *a,volatile spec_shared *s) {
    pane       = p;
    attributes = a;
    spectra    = s;
    clipping   = True;		/* Clip illegal coords to -1.  */
  }
  virtual void ScreenToSpec(spec_location *loc, int xpix, int ypix = 0) = 0;
  virtual void SpecToScreen(int *xpix, int *ypix, int xval, int yval = 0) = 0;
  void    Clip()   { clipping = True; }
  void    NoClip() { clipping = False; }
};

/*
** The following classes derive behavior from the base converter class in 
** a polymorphic way
*/
class Xamine_Convert1d  : public Xamine_Converter {
 public:
  Xamine_Convert1d(XMWidget *p, win_attributed *a,volatile spec_shared *s) :
     Xamine_Converter(p, a, s) {}
  virtual void ScreenToSpec(spec_location *loc, int xpix, int ypix = 0);
  virtual void SpecToScreen(int *xpix, int *ypix, int chan, int counts = 0);
};
class Xamine_Convert2d  : public Xamine_Converter {
 public:
  Xamine_Convert2d(XMWidget *p, win_attributed *a,volatile spec_shared *s) :
     Xamine_Converter(p, a, s) {}
  virtual void ScreenToSpec(spec_location *loc, int xpix, int ypix = 0);
  virtual void SpecToScreen(int *xpix, int *ypix, int xval, int yval = 0);
};
#endif
