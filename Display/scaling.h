/*
** Facility:
**   Xamine - NSCL display program.
** Abstract:
**   scaling.h:
**      This file contains interface specifications for the autoscaling
**      functions available to Xamine_spectrum displayers.
** Author:
**  Ron Fox
**  NSCL
**  Michigan State University
**  East Lansing, MI 48824-1321
**    @(#)scaling.h	2.1 12/22/93 
*/
#ifndef _SCALING_H
#define _SCALING_H
#include "dispwind.h"
#include "dispshare.h"
#include "XMWidget.h"

#define SMOOTH1D_PIXELS    1
#define HISTO1D_PIXELS     1
#define LINES1D_PIXELS     1
#define POINTS1D_PIXELS    1

#define SCATT2D_PIXELS     2
#define BOXES2D_PIXELS     2
#define COLOR2D_PIXELS     1
#define CONTOUR2D_PIXELS   1
#define SURFACE2D_PIXELS   4
#define LEGO2D_PIXELS      8

unsigned int Xamine_ComputeScaling(win_attributed *att, XMWidget *pane);

/*
 ** The objects below support generic reductions of spectra:
 */
/*
 ** Sampler classes:  
 **   This class and associated virtual methods allow sampling to be done
 **   generically simply by instantiating the appropriate class up front.
 */

class Sampler {
 public:
  virtual unsigned int chan(int i) = 0;	     /* Sample a channel i. */
  virtual void          setsample(int first,  /* Set sequential sample parameters */
				  float step) = 0;
  virtual void          setsample(int first) = 0;
  virtual unsigned int sample() = 0;	     /* Get a sample and move to next one. */
};

class Samplel : public Sampler {	    /* Sample longword */
 protected:
  unsigned int *base;
  float         offset;
  float         step;
 public:
  Samplel(unsigned int *b, float s = 1.0) {
    base  = b; 
    step  = s;
    offset= 0;
  }
  virtual unsigned int chan(int i) { return base[i]; }
  virtual void         setsample(int f, float s) {
    offset = (float)f;
    step   = s;
  }
  virtual void setsample(int f) { setsample (f, step); }
  
  virtual unsigned int sample() {
    unsigned int s = 0;		    /* Will become the sample value */
    int      first = (int)(offset + 0.5);
    offset        +=  step;
    int      last  = (int)(offset + 0.5);
    for(int i = first; i < last; i++) {
      if(s < base[i]) s = base[i];
    }

    return s;
  }
};

class Samplew : public Sampler { /* Sample shortword */
 protected:
  unsigned short *base;
  float         offset;
  float         step;
 public:
  Samplew(unsigned short *b, float s = 1.0) {
    base  = b; 
    step  = s;
    offset= 0;
  }
  virtual unsigned int chan(int i) { return base[i]; }
  virtual void         setsample(int f, float s) {
    offset = (float)f;
    step   = s;
  }
  virtual void setsample(int f) { setsample(f, step); }
  virtual unsigned int sample() {
    unsigned int s = 0;		    /* Will become the sample value */
    int      first = (int)(offset + 0.5);
    offset        +=  step;
    int      last  = (int)(offset + 0.5);
    for(int i = first; i < last; i++) {
      if(s < base[i]) s = base[i];
    }
    return s;
  }
};

class Suml : public Sampler {	/* Sample via summing (long) */
 protected:
  unsigned int *base;
  float         offset;
  float         step;
 public:
  Suml(unsigned int *b, float s) {
    base  = b; 
    step  = s;
    offset= 0.0;
  }
  virtual unsigned int chan(int i) { return base[i]; }
  virtual void         setsample(int f, float s) {
    offset  = (float)f;
    step   = s;
  }
  virtual void          setsample(int f) { setsample(f, step); }
  virtual unsigned int sample() {
    unsigned int s = 0;
    int last = (int)(offset + step);
    for(int i = (int)offset; i < last; i++)
      s += base[i];
    offset += step;
    return s;
  }
};

class Sumw : public Sampler {	/* Sample via summing (word) */
 protected:
  unsigned short *base;
  float         offset;
  float         step;
 public:
  Sumw(unsigned short *b, float s) {
    base  = b; 
    step  = s;
    offset= 0.0;
  }
  virtual unsigned int chan(int i) { return base[i]; }
  virtual void         setsample(int f, float s) {
    offset  = (float)f;
    step   = s;
  }
  virtual void          setsample(int f) { setsample(f, step); }
  virtual unsigned int sample() {
    unsigned int s = 0;
    int last = (int)(offset + step);
    for(int i = (int)offset; i < last; i++)
      s += (unsigned int)base[i];
    offset += step;
    return s;
  }
};
/*  The Avg* classes are the same as the Sum* methods but just divide by
 **  bin width after summimng:
 */

class Avgl : public Sampler {	/* Sample via summing (long) */
 protected:
  unsigned int *base;
  float         offset;
  float         step;
 public:
  Avgl(unsigned int *b, float s) {
    base  = b; 
    step  = s;
    offset= 0.0;
  }
  virtual unsigned int chan(int i) { return base[i]; }
  virtual void         setsample(int f, float s) {
    offset  = (float)f;
    step   = s;
  }
  virtual void          setsample(int f) { setsample(f, step); }
  virtual unsigned int sample() {
    unsigned int s = 0;
    int chans = 0;
    int last = (int)(offset + step);
    for(int i = (int)offset; i < last; i++) {
      s += base[i];
      chans++;
    }
    offset += step;
    return (s/chans);
  }
};

class Avgw : public Sampler {	/* Sample via summing (word) */
 protected:
  unsigned short *base;
  float         offset;
  float         step;
 public:
  Avgw(unsigned short *b, float s) {
    base  = b; 
    step  = s;
    offset= 0.0;
  }
  virtual unsigned int chan(int i) { return base[i]; }
  virtual void         setsample(int f, float s) {
    offset  = (float)f;
    step   = s;
  }
  virtual void          setsample(int f) { setsample(f, step); }
  virtual unsigned int sample() {
    unsigned int s = 0;
    unsigned int chans = 0;
    int last = (int)(offset + step);
    for(int i = (int)offset; i < last; i++) {
      s += (unsigned int)base[i];
      chans++;
    }
    offset += step;
    return (s/chans);
  }
};

/*
 ** Generate a sampler from appropriate input:
 */

Sampler *Xamine_GenerateSampler(unsigned int *b,
				spec_type   st,
				rendition_1d rend,
				reduction_mode sr, int xl, int xh, int nx, 
				float *chanw);

/*
 ** Samplers for 2-d spectra:
 */

class Sampler2 {
 private:
  float xstep;
  int xsteps;
  void DopeVector(int xi, int xf) {
    float xoff = (float)xi;
    xbin = 0;
    xsteps = (int) ((float)(xf - xi +1)/xstep + 0.5);
    xstart = new int[xsteps];
    xend   = new int[xsteps];

    xstart[0] = xi;
    xoff     += xstep;
    xend[0]   = (int)(xoff + 0.5);
    for(int i = 1; i < xsteps; i++) {
      xstart[i] = xend[i-1];
      xoff   += xstep;
      xend[i] = (int)(xoff + 0.5);
      if(xend[i] >= xdim) {	/* Don't let ourselves go over the end. */
	xsteps = i + 1;
	xend[i] = xdim-1;
	break;
      }
    }


  }
 protected:
  float ystep;
  float yoff;
  int xdim;
  int xbin;
  int *xstart;
  int *xend;  
 public:

  /* Base class functions: */

  Sampler2(float xs, float ys, int xsiz) {
    yoff = 0.0;
    xdim = xsiz;
    ystep= ys;
    xstep= xs;
    DopeVector(0, xdim-1);
  }
  virtual int getsteps() { return xsteps; }
  virtual ~Sampler2() {
    delete []xstart;
    delete []xend;
  }
  void    setsample(int xi, int xf, int yi, float xs, float ys) {
    delete []xstart;
    delete []xend;
    xstep = xs;
    ystep = ys;
    yoff  = (float)yi;
    DopeVector(xi, xf);
  }
  void     setsample(int xi, int xf, int yi) {
    setsample(xi, xf, yi, xstep, ystep);
  }
  void next() {
    xbin++;
    if(xbin >= xsteps) {
      yoff += ystep;
      xbin  = 0;
    }
  }
  /* Pure virtual functions */

  virtual unsigned int chan(int x, int y) = 0; /* Get channel x,y */
  virtual unsigned int sample() = 0; /* Sample a channel. */
  virtual void getscanline(unsigned int *buffer) {
    int n = getsteps();
    for(int i = 0; i < n; i++) 
      *buffer++ = sample();
  }
};

class Sample2w : public Sampler2 {
 protected:
  unsigned short *base;		/* Spectrum base. */
 public:
  Sample2w(unsigned short *b, float xs, float ys, int xsiz) :
    Sampler2(xs, ys, xsiz) {
    base   = b;
  }
  virtual ~Sample2w() 
    {}				/* Force call of parent destructor.  */
  virtual unsigned int chan(int x, int y) {
    return (unsigned int)(base[x + y*xdim]);
  }

  virtual unsigned int sample() {
    unsigned int val = 0;
    int      xl      = xstart[xbin];
    int      xh      = xend[xbin];
    int      yl      = (int)(yoff + 0.5);
    int      yh      = (int)(yoff + ystep + 0.5);

    for(int j = yl; j < yh; j++) {
      for(int i = xl; i < xh; i++) {
        unsigned int c = (unsigned int)base[i+j*xdim];
	if(val < c) val = c;
      }
    }
    next();

    return val;
  }

};


class Sample2b : public Sampler2 {

 protected:
  unsigned char *base;		/* Spectrum base. */
 public:
  Sample2b(unsigned char *b, float xs, float ys, int xsiz) :
    Sampler2(xs, ys, xsiz) {
    base   = b;
  }
  virtual ~Sample2b() 
    {}				/* Force call of parent destructor.  */
  virtual unsigned int chan(int x, int y) {
    return (unsigned int)(base[x + y*xdim]);
  }

  virtual unsigned int sample() {
    unsigned int val = 0;
    int      xl      = xstart[xbin];
    int      xh      = xend[xbin];
    int      yl      = (int)(yoff + 0.5);
    int      yh      = (int)(yoff + ystep + 0.5);

    for(int j = yl; j < yh; j++) {
      for(int i = xl; i < xh; i++) {
        unsigned int c = (unsigned int)base[i+j*xdim];
	if(val < c) val = c;
      }
    }

    next();

    return val;
  }

};

/*
** Summing samplers:
*/
class Sum2w : public Sampler2 {
 protected:
  unsigned short *base;		/* Spectrum base. */
 public:
  Sum2w(unsigned short *b, float xs, float ys, int xsiz) :
    Sampler2(xs,ys, xsiz) {

      base = b;
  }
  virtual ~Sum2w() {}
  virtual unsigned int chan(int x, int y) {
    return (unsigned int)(base[x + y*xdim]);
  }
  virtual unsigned int sample() {
    int x0  = xstart[xbin];
    int y0  = (int)(yoff); /* These are box lower left coords... */

    int x1  = xend[xbin];
    int y1  = (int)(yoff + ystep); /* These are the box upper right coords */

    unsigned int sum = 0;	/* Accumulate the sum into here. */

    int ychan = y0*xdim;
    for(int y = y0; y <= y1; y++) {
      for(int x = x0; x <= x1; x++) {
	sum += (unsigned int)(base[x + ychan]);
      }
      ychan += xdim;
    }

    /* Adjust offsets to the next summing box. */

    next();

    return sum;
  }

  /* Restriction -- only gets a full scanline */

  virtual void getscanline(unsigned int *buffer) {
    int nx = getsteps();
    for(int i = 0; i < nx; i++) {
      int x0 = xstart[i];
      int x1 = xend[i];
      int y0 = (int)yoff;
      int y1 = (int)(yoff+ystep);
      unsigned int sum = 0;
      int ychan = y0*xdim;
      for(int y = y0; y <= y1; y++) {
	for(int x = x0; x <= x1; x++) {
	  sum += (unsigned int)(base[x+ychan]);
	}
	ychan += xdim;
      }
      *buffer++ = sum;
    }
    xbin = 0;
    yoff += ystep;
  }
};
class Sum2b : public Sampler2 {
 protected:
  unsigned char *base;		/* Spectrum base. */
 public:
  Sum2b(unsigned char *b, float xs, float ys, int xsiz) :
    Sampler2(xs,ys, xsiz) {

      base = b;
  }
  virtual unsigned int chan(int x, int y) {
    return (unsigned int)(base[x + y*xdim]);
  }
  virtual ~Sum2b() {}
  virtual unsigned int sample() {
    int x0  = xstart[xbin];
    int y0  = (int)(yoff); /* These are box lower left coords... */

    int x1  = xend[xbin];
    int y1  = (int)(yoff + ystep); /* These are the box upper right coords */

    unsigned int sum = 0;	/* Accumulate the sum into here. */

    int ychan = y0*xdim;
    for(int y = y0; y <= y1; y++) {
      for(int x = x0; x <= x1; x++) {
	sum += (unsigned int)(base[x + ychan]);
      }
      ychan += xdim;
    }

    /* Adjust offsets to the next summing box. */

    next();

    return sum;
  }

  /* Restriction -- only gets a full scanline */

  virtual void getscanline(unsigned int *buffer) {
    int nx = getsteps();
    for(int i = 0; i < nx; i++) {
      int x0 = xstart[i];
      int x1 = xend[i];
      int y0 = (int)yoff;
      int y1 = (int)(yoff+ystep);
      unsigned int sum = 0;
      int ychan = y0*xdim;
      for(int y = y0; y <= y1; y++) {
	for(int x = x0; x <= x1; x++) {
	  sum += (unsigned int)(base[x+ychan]);
	}
	ychan += xdim;
      }
      *buffer++ = sum;
    }
    xbin = 0;
    yoff += ystep;
  }
};


/*
** Averaging samplers.
*/
class Average2w : public Sampler2 {
 protected:
  unsigned short *base;		/* Spectrum base. */
 public:
  Average2w(unsigned short *b, float xs, float ys, int xsiz) :
    Sampler2(xs, ys, xsiz)
    {
    base   = b;
  }
  virtual ~Average2w() {}
  virtual unsigned int chan(int x, int y) {
    return (unsigned int)(base[x + y*xdim]);
  }

  virtual unsigned int sample() {
    int x0  = xstart[xbin];
    int y0  = (int)(yoff); /* These are box lower left coords... */

    int x1  = xend[xbin];

    int y1  = (int)(yoff + ystep); /* These are the box upper right coords */


    unsigned int sum = 0;	/* Accumulate the sum into here. */

    int ychan = y0*xdim;
    for(int y = y0; y <= y1; y++) {
      for(int x = x0; x <= x1; x++) {
	sum += (unsigned int)(base[x + ychan]);
      }
      ychan += xdim;
    }

    /* Adjust offsets to the next summing box. */

    next();

    return (sum/((x1-x0+1)*(y1-y0+1)));
  }

  /* Restriction -- only gets a full scanline */

  virtual void getscanline(unsigned int *buffer) {
    int nx = getsteps();
    for(int i = 0; i < nx; i++) {
      int x0 = xstart[i];
      int x1 = xend[i];
      int y0 = (int)yoff;
      int y1 = (int)(yoff+ystep);
      unsigned int sum = 0;
      int ychan = y0*xdim;
      for(int y = y0; y <= y1; y++) {
	for(int x = x0; x <= x1; x++) {
	  sum += (unsigned int)(base[x+ychan]);
	}
	ychan += xdim;
      }
      *buffer++ = sum / ((x1-x0+1)*(y1-y0+1));
    }
    xbin = 0;
    yoff += ystep;
  }
};


class Average2b : public Sampler2 {
 protected:
  unsigned char *base;		/* Spectrum base. */
 public:
  Average2b(unsigned char *b, float xs, float ys, int xsiz) :
    Sampler2(xs, ys, xsiz)
    {
    base   = b;
  }
  virtual ~Average2b() {}
  virtual unsigned int chan(int x, int y) {
    return (unsigned int)(base[x + y*xdim]);
  }

  virtual unsigned int sample() {
    int x0  = xstart[xbin];
    int y0  = (int)(yoff); /* These are box lower left coords... */

    int x1  = xend[xbin];

    int y1  = (int)(yoff + ystep); /* These are the box upper right coords */

    unsigned int sum = 0;	/* Accumulate the sum into here. */

    int ychan = y0*xdim;
    for(int y = y0; y <= y1; y++) {
      for(int x = x0; x <= x1; x++) {
	sum += (unsigned int)(base[x + ychan]);
      }
      ychan += xdim;
    }

    /* Adjust offsets to the next summing box. */

    next();

    return (sum/((x1-x0+1)*(y1-y0+1)));
  }

  /* Restriction -- only gets a full scanline */

  virtual void getscanline(unsigned int *buffer) {
    int nx = getsteps();
    for(int i = 0; i < nx; i++) {
      int x0 = xstart[i];
      int x1 = xend[i];
      int y0 = (int)yoff;
      int y1 = (int)(yoff+ystep);
      unsigned int sum = 0;
      int ychan = y0*xdim;
      for(int y = y0; y <= y1; y++) {
	for(int x = x0; x <= x1; x++) {
	  sum += (unsigned int)(base[x+ychan]);
	}
	ychan += xdim;
      }
      *buffer++ = sum / ((x1-x0+1)*(y1-y0+1));
    }
    xbin = 0;
    yoff += ystep;
  }
};



/*
** Xamine_GenerateSampler2  choses and generates the correct sampler
**                          given enough information
*/

Sampler2 *Xamine_GenerateSampler2(int b,
				  spec_type st,
				  rendition_2d rend,
				  reduction_mode sr,
				  int xl, int xh, int yl, int yh,
				  int nx, int ny, 
				  float *chanwx, float *chanwy);


#endif
