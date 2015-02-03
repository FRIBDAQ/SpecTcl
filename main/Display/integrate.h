/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

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
#include "mapcoord.h"

class Integrate_1d {
 protected:
  grobj_sum1d *sumregion;
  float          centroid;
  float          deviance;
  float          volume;
  bool           m_fMapped;	/* True if mapped spectrum. */
 public:
  Integrate_1d(grobj_sum1d *sr, bool mapped = false) :
    sumregion(sr),
    centroid(0.0),
    deviance(0.0),
    volume(0.0),
    m_fMapped(mapped)
  {

  }
  virtual void Perform() = 0;
  float GetCentroid() { return centroid; }
  float GetStdDev()   { return deviance; }
  float GetVolume()   { return volume;   }
  double Channel(int n) {
    if(m_fMapped) {
      // Return the middle of the channel.

      return (Xamine_XChanToMapped(sumregion->getspectrum(),(float)n) +
	      Xamine_XChanToMapped(sumregion->getspectrum(),(float)(n+1)))/2.0;
    }
    else {
      return (double)n;
    }
  }
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
  bool           m_fMapped;	// True if mapped spectrum.
public:
  Integrate_2d(int xd, grobj_sum2d *sr, bool mapped=false) :
    sumregion(sr),
    xchans(xd),
    xcentroid(0.0),
    ycentroid(0.0),
    xdeviance(0.0),
    ydeviance(0.0),
    volume(0.0),
    m_fMapped(mapped)
    
  {
  }
  virtual void Perform() = 0;
  float   GetVolume()    { return volume;   }
  float   GetXCentroid() { return xcentroid; }
  float   GetYCentroid() { return ycentroid; }
  float   GetXStdDev()   { return xdeviance; }
  float   GetYStdDev()   { return ydeviance; }
  double   XChannel(int n) {
    if(m_fMapped) {
      // Return the middle of the channel just like the ticker.

      return (Xamine_XChanToMapped(sumregion->getspectrum(), (float)n) +
	      Xamine_XChanToMapped(sumregion->getspectrum(), (float)(n+1)))/2.0;
    }
    else {
      return (double)n;
    }
  }
  double   YChannel(int n) {
    if(m_fMapped) {
      // Return the middle of the channel just like the ticker.

      return (Xamine_YChanToMapped(sumregion->getspectrum(), (float)n) +
	      Xamine_YChanToMapped(sumregion->getspectrum(), (float)(n+1)))/2.0;
    }
    else {
      return (double)n;
    }
  }
  
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
  Integrate_1dw(unsigned short  *spc, grobj_sum1d *sr, bool mapped=false) :
    Integrate_1d(sr, mapped) {
      spectrum = spc;
    }
  virtual void Perform();
};

class Integrate_1dl : public Integrate_1d {
 protected:
  unsigned int *spectrum;
 public:
  Integrate_1dl(unsigned int *spc, grobj_sum1d *sr, bool mapped=false) : 
    Integrate_1d(sr, mapped) {
      spectrum = spc;
    }
  virtual void Perform();
};


class Integrate_2db : public Integrate_2d {
 protected:
  unsigned char *spectrum;
 public:
  Integrate_2db(unsigned char *spc, int xd, grobj_sum2d *sr, bool mapped=false) :
    Integrate_2d(xd, sr, mapped) {
      spectrum = spc;
    }
  virtual void Perform();
};

class Integrate_2dw : public Integrate_2d {
 protected:
  unsigned short *spectrum;
 public:
  Integrate_2dw(unsigned short *spc, int xd, grobj_sum2d *sr, bool mapped=false) :
    Integrate_2d(xd, sr, mapped) {
      spectrum = spc;
    }
  virtual void Perform();
};
class Integrate_2dl : public Integrate_2d {
protected:
  unsigned int* spectrum;
public:
  Integrate_2dl(unsigned int *spc, int xd, grobj_sum2d *sr, bool mapped=false) :
    Integrate_2d(xd, sr, mapped) 
  {
    spectrum = spc;
  }
  virtual void Perform();
};

#endif
