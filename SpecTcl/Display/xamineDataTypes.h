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
   This file provides a refactoring of several identical
   typedefs and constant defs that occur throughout the
   Xamine header and implementation files.  Yes, I know it
   was originally >bad bad bad< to have done this... that's
   why I'm fixing it.
*/


#ifndef __XAMINEDATATYPES_H
#define __XAMINEDATATYPES_H




/* Define a megabyte if someone else hasn't done it */

#ifndef MEG
#define MEG 1024*1024
#endif

#include <sys/types.h>
#ifndef HAVE_DECL_PADSIZE
#ifndef PADSIZE
#define PADSIZE 8192
#endif
#endif


#ifdef HAVE_MACHINE_PARAM_H
#ifndef CYGWIN
#include <machine/param.h>
#define PAGESIZE NBPG
#else
#define PAGESIZE 8192
#endif
#endif

#ifndef PAGESIZE
#ifdef HAVE_DECL_PAGESIZE
#include <limits.h>
#else
#define PAGESIZE 8192	/* Should work for systems I know about. */
#endif				
#endif

#ifndef PAGESIZE		/* Still didn't find a pagesize... */
#define PAGESIZE 8192
#endif




#define XAMINE_MAXSPEC 5000	/* Maximum spectrum count. */
#ifndef XAMINE_SPECBYTES
#define XAMINE_SPECBYTES 8*MEG	/* Default number of bytes in spectra. */
#endif

#define XAMINE_WORDS     (XAMINE_SPECBYTES)/sizeof(short)
#define XAMINE_LONGS     (XAMINE_SPECBYTES)/sizeof(long)

/* Graphical object limits etc. */

#define GROBJ_NAMELEN 80
#define GROBJ_MAXPTS   50

/* #pragma pack(1) */
typedef union {
                unsigned char  XAMINE_b[XAMINE_SPECBYTES];
		unsigned short XAMINE_w[XAMINE_WORDS];
		unsigned int  XAMINE_l[XAMINE_LONGS];
	      } spec_spectra; /* Spectrum storage type. */


typedef struct {
                 unsigned int xchans;
		 unsigned int ychans;
	       } spec_dimension;	/* Describes the channels in a spectrum. */

typedef char spec_title[128];	/* Spectrum name string */
typedef spec_title spec_label;	/* These two must be the same due to 
                                   the implementation of cvttitle in spectra.cc */

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


typedef struct _Xamine_shared {
  spec_dimension  dsp_xy[XAMINE_MAXSPEC];
  spec_title      dsp_titles[XAMINE_MAXSPEC];
  unsigned int    dsp_offsets[XAMINE_MAXSPEC];
  spec_type       dsp_types[XAMINE_MAXSPEC];
  spec_map        dsp_map[XAMINE_MAXSPEC];
  spec_spectra    dsp_spectra;
  char            page_pad[PAGESIZE];

} Xamine_shared;

/* #pragma pack(0) */

typedef struct { int x, y; } Xamine_point;


/* Graphical object stuff */

typedef char grobj_name[GROBJ_NAMELEN+1];

typedef enum {			/* Types of graphical objects. */

               generic           = -1,
               cut_1d            = 1,
	       Xamine_cut1d      = 1,
	       summing_region_1d = 2,
	       marker_1d         = 5,
	       contour_2d           = 4,
	       Xamine_contour2d  = 4,
	       band              = 3,
	       Xamine_band       = 3,
	       summing_region_2d = 6,
	       marker_2d         = 7,
	       peak1d            = 8,
	       pointlist_1d      = 100,
	       pointlist_2d      = 200
	       } grobj_type, Xamine_gatetype;

#endif
