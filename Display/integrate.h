/*
** Facility:
**   Xamine -- NSCL Display program.
* Abstract:
**   integrate.h:
**     This file contains definitions of functions which 
**     perform integrations over sections of spectra.  These are modelled
**     as a class which have the methods (in addition to constructors):
**       peform   - perform calculations.
**       centroid - Get centroid positions.
**       stddev   - Get Standard Deviation.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** SCCS information:
**   @(#)integrate.h	8.1 6/23/95 
*/
#ifndef _INTEGRATE_H_INSTALLED
#define _INTEGRATE_H_INSTALLED

#include "dispgrob.h"
#include "dispshare.h"

class Integrate_1d {
 protected:
  grobj_sum1d *sumregion;
  float          centroid;
  float          deviance;
  float          volume;
 public:
  Integrate_1d(grobj_sum1d *sr) {
    sumregion  = sr;
    centroid   = 0;
    deviance   = 0;
    volume     = 0;
  }
  virtual void Perform() = 0;
  float GetCentroid() { return centroid; }
  float GetStdDev()   { return deviance; }
  float GetVolume()   { return volume;   }
};

class Integrate_2d {
 protected:
  grobj_sum2d    *sumregion;
  unsigned short xchans;	/* For indexing. */
  float          xcentroid;
  float          ycentroid;
  float          xdeviance;
  float          ydeviance;
  float          volume;
 public:
  Integrate_2d(int xd, grobj_sum2d *sr) {
    xchans     = xd;
    sumregion  = sr;
    xcentroid  = 0;
    ycentroid  = 0;
    xdeviance  = 0;
    ydeviance  = 0;
    volume     = 0;
  }
  virtual void Perform() = 0;
  float   GetVolume()    { return volume;   }
  float   GetXCentroid() { return xcentroid; }
  float   GetYCentroid() { return ycentroid; }
  float   GetXStdDev()   { return xdeviance; }
  float   GetYStdDev()   { return ydeviance; }
};

/*
** For each of these base classes, there will be a 
** version for each type of 1-d or 2-d spectrum which is slighty
** specific to the channel's data type.
*/

class Integrate_1dw : public Integrate_1d {
 protected:
  unsigned short  *spectrum;		/* Points to spectrum base. */
 public:
  Integrate_1dw(unsigned short  *spc, grobj_sum1d *sr) :
    Integrate_1d(sr) {
      spectrum = spc;
    }
  virtual void Perform();
};

class Integrate_1dl : public Integrate_1d {
 protected:
  unsigned int *spectrum;
 public:
  Integrate_1dl(unsigned int *spc, grobj_sum1d *sr) : 
    Integrate_1d(sr) {
      spectrum = spc;
    }
  virtual void Perform();
};


class Integrate_2db : public Integrate_2d {
 protected:
  unsigned char *spectrum;
 public:
  Integrate_2db(unsigned char *spc, int xd, grobj_sum2d *sr) :
    Integrate_2d(xd, sr) {
      spectrum = spc;
    }
  virtual void Perform();
};

class Integrate_2dw : public Integrate_2d {
 protected:
  unsigned short *spectrum;
 public:
  Integrate_2dw(unsigned short *spc, int xd, grobj_sum2d *sr) :
    Integrate_2d(xd, sr) {
      spectrum = spc;
    }
  virtual void Perform();
};
#endif
