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
**   Display spectrum access manager
** Abstract:     dispshare.h
**   This include file describes the shared memory regions which contain
**   spectrum and control information for the spectrum display program.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** Version:
**     @(#)dispshare.h	2.3 5/26/94 
*/

#ifndef _DISPSHARE_H_INSTALLED_
#define _DISPSHARE_H_INSTALLED_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
/* Define a megabyte if someone else hasn't done it */

#ifndef MEG
#define MEG 1024*1024
#endif



#ifdef HAVE_MACHINE_PARAM_H
#include <machine/param.h>
#ifndef CYGWIN
#define PAGESIZE NBPG
#else
#define PAGESIZE 8192
#endif
#endif


#ifndef HAVE_DECL_PADSIZE
#define PADSIZE 8192
#endif


#ifndef HAVE_DECL_PAGESIZE
#ifndef PAGESIZE
#define PAGESIZE 8192
#endif
#endif

#ifndef PAGESIZE
#define PAGESIZE 8192
#endif


#define DISPLAY_MAXSPEC 5000	/* Maximum spectrum count. */
#define DISPLAY_SPECBYTES 8*MEG	/* Maximum number of bytes in spectra. */

#define DISPLAY_WORDS     (DISPLAY_SPECBYTES)/sizeof(short)
#define DISPLAY_LONGS     (DISPLAY_SPECBYTES)/sizeof(long)

typedef union {
                unsigned char  display_b[DISPLAY_SPECBYTES];
		unsigned short display_w[DISPLAY_WORDS];
		unsigned int  display_l[DISPLAY_LONGS];
	      } spec_spectra; /* Spectrum storage type. */

typedef struct {
                 unsigned int xchans;
		 unsigned int ychans;
	       } spec_dimension;   /* Describes the channels in a spectrum. */

typedef char spec_title[72];
typedef char spec_label[72];

typedef enum {
               undefined = 0,
	       twodlong = 5,
               onedlong = 4,
	       onedword = 2,
	       twodword = 3,
	       twodbyte = 1
	     } spec_type;

typedef struct {
  float xmin;
  float xmax;
  float ymin;
  float ymax;
  spec_label xlabel;
  spec_label ylabel;
} spec_map;

struct spec_shared {
  spec_dimension  dsp_xy[DISPLAY_MAXSPEC];
  spec_title      dsp_titles[DISPLAY_MAXSPEC];
  unsigned int    dsp_offsets[DISPLAY_MAXSPEC];
  spec_type       dsp_types[DISPLAY_MAXSPEC];
  spec_map        dsp_map[DISPLAY_MAXSPEC];
  spec_spectra    dsp_spectra;
  char            page_pad[PAGESIZE];
  void setdim(int id, int xd) volatile {
    dsp_xy[id-1].xchans = xd;
    dsp_xy[id-1].ychans = 0;
  }
  void setdim(int id, int xd, int yd) volatile {
    dsp_xy[id-1].xchans = xd;
    dsp_xy[id-1].ychans = yd;
  }
  
  float getxmax_map(int id) volatile { return dsp_map[id-1].xmax; }
  float getxmin_map(int id) volatile { return dsp_map[id-1].xmin; }
  float getymax_map(int id) volatile { return dsp_map[id-1].ymax; }
  float getymin_map(int id) volatile { return dsp_map[id-1].ymin; }

  volatile char *getxlabel_map(spec_label label, int id) volatile;
  volatile char *getylabel_map(spec_label label, int id) volatile;

  // Gets and sets for the other non-mapping stuff...
  int getspecid(char *name) volatile;
  int getspectrumcount() volatile  {return DISPLAY_MAXSPEC; }
  int getmaxchannels() volatile    {return DISPLAY_SPECBYTES; }
  spec_type gettype(int id) volatile;
  void settype(int id, spec_type tp) volatile {
    dsp_types[id-1] = tp;
  }
  spec_type gettype(char *name) volatile
  { 
    int id;
    id = getspecid(name);
    return ((id < 0)? undefined :
	    gettype(id)); 
  }
  int getchannels(int id)  volatile
  { 
    return (((dsp_types[id-1] == onedlong) ||
	     (dsp_types[id-1] == onedword))? 
	    dsp_xy[id-1].xchans :
	    dsp_xy[id-1].xchans * dsp_xy[id-1].ychans); 
  }
  int getchannels(char *name) volatile 
  {
    int id;
    id = getspecid(name);
    return ((id < 0)? -1:
	    getchannels(id)); 
  }
  int getxdim(int id) volatile  { return dsp_xy[id-1].xchans; }
  int getxdim(char *name) volatile 
  { 
    int id;
    id = getspecid(name);
    return ((id < 0) ? -1:
	    getxdim(id)); 
  }
  int getydim(int id) volatile  { return dsp_xy[id-1].ychans; }
  int getydim(char *name) volatile 
  { 
    int id;
    id = getspecid(name);
    return ((id < 0) ? -1:
	    getydim(id)); 
  }
  volatile char *getname(spec_title name, int id) volatile;
  unsigned int getchannel(int id, int ix) volatile;
  unsigned int getchannel(int id, int ix, int iy) volatile;
  volatile unsigned int *getbase(int id) volatile;
  char *getversion() volatile;
};

void Xamine_initspectra();
int Xamine_GetSpectrumList(char ***list);
int Xamine_GetSpectrumId(char *name);
spec_type Xamine_SpectrumType(int id);
char *Xamine_MemoryTop();
#endif

