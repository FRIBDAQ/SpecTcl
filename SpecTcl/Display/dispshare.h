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

/* Define a megabyte if someone else hasn't done it */

#ifndef MEG
#define MEG 1024*1024
#endif

#ifdef unix
#include <sys/types.h>
//    Maybe the two below are not really needed?
//
// #include <sys/ipc.h>
// #include <sys/shm.h>
#endif 
#ifdef ultrix
#include <machine/param.h>
#define PAGESIZE NBPG
#endif
#ifdef __ALPHA
#define PADSIZE  65536
#else
#define PADSIZE  512
#endif


#ifdef VMS
#define PAGESIZE 512
#endif
#ifndef PAGESIZE
#define PAGESIZE 512
#endif

#define DISPLAY_MAXSPEC 999	/* Maximum spectrum count. */
#define DISPLAY_SPECBYTES 8*MEG	/* Maximum number of bytes in spectra. */

#define DISPLAY_WORDS     (DISPLAY_SPECBYTES)/sizeof(short)
#define DISPLAY_LONGS     (DISPLAY_SPECBYTES)/sizeof(long)
#ifdef __ALPHA
#pragma member_alignment __save
#pragma nomember_alignment
#endif

typedef union {
                unsigned char  display_b[DISPLAY_SPECBYTES];
		unsigned short display_w[DISPLAY_WORDS];
		unsigned int  display_l[DISPLAY_LONGS];
	      } spec_spectra; /* Spectrum storage type. */

typedef struct {
                 unsigned short xchans;
		 unsigned short ychans;
	       } spec_dimension;	/* Describes the channels in a spectrum. */

typedef char spec_title[72];

typedef enum {
               undefined = 0,
               onedlong = 4,
	       onedword = 2,
	       twodword = 3,
	       twodbyte = 1
	     } spec_type;

struct spec_shared {
  spec_dimension  dsp_xy[DISPLAY_MAXSPEC];
  spec_title      dsp_titles[DISPLAY_MAXSPEC];
  unsigned int    dsp_offsets[DISPLAY_MAXSPEC];
  spec_type       dsp_types[DISPLAY_MAXSPEC];
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

#ifdef __ALPHA
#pragma member_alignment __restore
#endif
void Xamine_initspectra();
int Xamine_GetSpectrumList(char ***list);
int Xamine_GetSpectrumId(char *name);
spec_type Xamine_SpectrumType(int id);
char *Xamine_MemoryTop();
#endif

