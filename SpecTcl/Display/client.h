/*
** Facility:
**    Xamine - NSCL display program.
** Abstract:
**    client.h:
**      This file should be included by clients of Xamine.  It includes
**      definitions required to call entries in the Xamine library for
**      C language clients.
** Author:
**   Ron FOx
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
**     @(#)client.h	2.2 1/28/94 
*/
#ifndef XAMINE_CLIENT_H
#define XAMINE_CLIENT_H



/* Define a megabyte if someone else hasn't done it */

#ifndef MEG
#define MEG 1024*1024
#endif

#ifdef VMS
#ifndef __SOCKET_TYPEDEFS
#if !defined(CADDR_T_DEFINED) && !defined(__CADDR_T)
#define CADDR_T_DEFINED
#define __CADDR_T
typedef char *caddr_t;
#endif
#endif
#include <descrip.h>
#endif
#ifdef unix
#include <sys/types.h>
#endif 
#ifdef __ALPHA
#define PADSIZE  65536
#else
#define PADSIZE 512
#endif

#ifndef PAGESIZE
#ifdef HPUX
#include <limits.h>
#else
#define PAGESIZE 512		/* Should work for systems I know about. */
#endif
#endif				/* Too big is better than too small.     */

#define XAMINE_MAXSPEC 999	/* Maximum spectrum count. */
#ifndef XAMINE_SPECBYTES
#define XAMINE_SPECBYTES 8*MEG	/* Maximum number of bytes in spectra. */
#endif

#define XAMINE_WORDS     (XAMINE_SPECBYTES)/sizeof(short)
#define XAMINE_LONGS     (XAMINE_SPECBYTES)/sizeof(long)

#ifdef __ALPHA
#pragma member_alignment __save
#pragma nomember_alignment
#endif
#ifndef _DISPSHARE_H_INSTALLED_
typedef union {
                unsigned char  XAMINE_b[XAMINE_SPECBYTES];
		unsigned short XAMINE_w[XAMINE_WORDS];
		unsigned int  XAMINE_l[XAMINE_LONGS];
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

#endif
typedef struct _Xamine_shared {
                 spec_dimension  dsp_xy[XAMINE_MAXSPEC];
		 spec_title      dsp_titles[XAMINE_MAXSPEC];
		 unsigned int    dsp_offsets[XAMINE_MAXSPEC];
                 spec_type       dsp_types[XAMINE_MAXSPEC];
		 spec_spectra    dsp_spectra;
		 char            page_pad[PADSIZE];
	       } Xamine_shared;

#ifdef __ALPHA
#pragma member_alignment __restore
#endif

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/* Create Shared Memory -- Creates the shared memory region for Xamine */

int Xamine_CreateSharedMemory(int specbytes, volatile Xamine_shared **ptr);
int
#ifdef VMS
    f77xamine_createsharedmemory
#endif
#ifdef unix
    f77xamine_createsharedmemory_
#endif
                                 (int *specbytes,volatile Xamine_shared **ptr);
int Xamine_DetachSharedMemory();

/* Start -- Starts up the Xamine process  */

int Xamine_Start();
int
#ifdef VMS
    f77xamine_start();
#endif
#ifdef unix
    f77xamine_start_();
#endif

/* Stop -- Stop the Xamine Process      */

int Xamine_Stop();
int
#ifdef VMS
    f77xamine_stop();
#endif
#ifdef unix
    f77xamine_stop_();
#endif

int Xamine_Alive();
#ifdef VMS
int f77xamine_alive();
#endif
#ifdef unix
int f77xamine_alive_();
#endif

void Xamine_GetMemoryName(char *namebuffer);
#ifdef VMS
void f77xamine_getmemoryname(struct dsc$descriptor  *namedesc);
#endif
#ifdef unix
void f77xamine_getmemoryname_(char *namebuffer, int maxlen);
#endif

int Xamine_MapMemory(char *name, int specbytes,volatile Xamine_shared **ptr);
#ifdef VMS
int f77xamine_mapmemory(struct dsc$descriptor *namedesc,
			 int    *specbytes,
			 volatile Xamine_shared **ptr);
#endif
#ifdef unix
int f77xamine_mapmemory_(char *name, int *specbytes,volatile Xamine_shared **ptr,
			 int namesize);
#endif

/*
** The functions below interface to Xamine's gate handling.
*/
typedef struct { int x, y; } Xamine_point;
typedef enum {
               Xamine_cut1d      = 1,
	       Xamine_contour2d  = 4,
	       Xamine_band        = 3
             } Xamine_gatetype;

int Xamine_EnterGate(int spec, int id, Xamine_gatetype type,
		     char *name,
		     int npts, Xamine_point *pts);
int
#ifdef unix
f77xamine_entergate_
  (int *spec, int *id, Xamine_gatetype *type, 
   char *name, int *npts, Xamine_point *pts);
#endif
#ifdef VMS
f77xamine_entergate
  (int *spec, int *id,Xamine_gatetype *type, 
   struct dsc$descriptor *name, int *npts, Xamine_point *pts);
#endif


int Xamine_RemoveGate(int spec, int id, Xamine_gatetype ntype);
int
#ifdef unix
f77xamine_removegate_
#endif
#ifdef VMS
f77xamine_removegate
#endif
  (int *spec, int *id, Xamine_gatetype *ntype);

long Xamine_StartSearch(int spec, int *status);
long
#ifdef unix
f77xamine_startsearch_
#endif
#ifdef VMS
f77xamine_startsearch
#endif
  (int *spec, int *status);

int Xamine_NextGate(long context, int *id, Xamine_gatetype *type, 
		    int *npts, Xamine_point *pts);
int
#ifdef unix
f77xamine_nextgate_
#endif
#ifdef VMS
f77xamine_nextgate
#endif
  (long *context, int *id, Xamine_gatetype *type, int *npts, 
   Xamine_point *pts);

void  Xamine_EndSearch(long context);
void
#ifdef unix
f77xamine_endsearch_
#endif
#ifdef VMS
f77xamine_endsearch
#endif
  (long *context);

int Xamine_ReadGate(int *spectrum, Xamine_gatetype *type , char *name,
		    int *npts, Xamine_point *pts);
int
#ifdef unix
f77xamine_readgate_
  (int *spectrum, Xamine_gatetype *type, char *name,
   int *npts, Xamine_point *pts);
#endif
#ifdef VMS
f77xamine_readgate
  (int *spectrum, Xamine_gatetype *type, struct dsc$descriptor *name,
   int *npts, Xamine_point *pts);
#endif


int Xamine_PollForGate(int timeout, int *spectrum, 
		       Xamine_gatetype *type, 
		       char *name,
		       int *npts, Xamine_point *pts);
int
#ifdef unix
f77xamine_pollforgate_
  (int *timeout, int *spectrum, Xamine_gatetype *type, 
   char *name, int *npts, 
   Xamine_point *pts);
#endif
#ifdef VMS
f77xamine_pollforgate
  (int *timeout, int *spectrum, Xamine_gatetype *type, 
   struct dsc$descriptor *name, int *npts, 
   Xamine_point *pts);
#endif

/* The following routines below are used for Xamine gating in AED emulation
** mode and are only supported on the VMS systems.
*/

#ifdef VMS

int Xamine_Read(struct dsc$descriptor *userbuf, int eventflag);
int f77xamine_read(struct dsc$descriptor *userbuf, int *eventflag);
int  Xamine_Wait(int *isize);
int f77xamine_wait(int *isize);
void f77xamine_cancel(int *can);
int f77xamine_queuegateio(short *iosb, void *astrtn, void *astarg,
			  struct dsc$descriptor *buffer);

#endif
/*
** Routines below allow spectrum numbers to be allocated, deallocated
** and reserved.
*/

int Xamine_AllocateSpectrum();	    /* Get lowest numbered free spectrum. */
#ifdef unix
int f77Xamine_AllocateSpectrum_();
#endif
#ifdef VMS
int f77Xamine_AllocateSpectrum();
#endif

int Xamine_ReserveSpectrum(int spno); /* Reserve a particular spectrum. */
#ifdef unix
int f77Xamine_ReserveSpectrum_(int *spno); 
#endif
#ifdef VMS
int f77Xamine_ReserveSpectrum(int *spno);
#endif

void Xamine_FreeSpectrum(int id);    /* Free a spectrum.   */
#ifdef unix
void f77Xamine_FreeSpectrum_(int *id); 
#endif
#ifdef VMS
void f77Xamine_FreeSpectrum(int *id);
#endif

/*
**   Low level spectrum memory routines:
*/

void Xamine_ManageMemory();	/* Must be called first to init. */
#ifdef unix
void f77Xamine_ManageMemory_();
#endif
#ifdef VMS
void f77Xamine_ManageMemory();
#endif

caddr_t Xamine_AllocMemory(int size); /* Allocate Spectrum memory */
#ifdef  unix
long     f77Xamine_AllocMemory_(int *size);
#endif
#ifdef VMS
long     f77Xamine_AllocMemory(int *size);
#endif

void    Xamine_FreeMemory(caddr_t loc);	/* Free memory chunk */
#ifdef VMS
void    f77Xamine_FreeMemory(int  *offset);
#endif
#ifdef unix
void    f77Xamine_FreeMemory_(int  *offset);
#endif

/*
** Low level routine to fill in a spectrum definition
*/

void Xamine_DescribeSpectrum(int spno, int xdim, int ydim,
			     char *title, caddr_t loc,
			     spec_type type);
#ifdef unix
void f77Xamine_DescribeSpectrum_(int *spno, int *xdim, int *ydim,
			     char *title, int *loc,
			     spec_type *type, int tsize);
#endif
#ifdef VMS
void f77Xamine_DescribeSpectrum(int *spno, int *xdim, int *ydim,
			     struct dsc$descriptor  *title, int *loc,
			     spec_type *type);

#endif


/*
** High level routines to allocated, and describe spectra to the
** the memory management routines.
*/

caddr_t Xamine_Allocate1d(int *spno, int xdim, char *title, int word);
caddr_t Xamine_Allocate2d(int *spno, int xdim, int ydim, char *title, 
			  int byte);
#ifdef unix
long f77Xamine_Allocate1d_(int *spno, int *xdim, char *title, 
			  int *word, int tlen);
long f77Xamine_Allocate2d_(int *spno, int *xdim, int *ydim, char *title, 
			  int *byte, int tlen);
#endif

#ifdef VMS
long f77Xamine_Allocate1d(int *spno, int *xdim, struct dsc$descriptor *title, 
			 int *word);
long f77Xamine_Allocate2d(int *spno, int *xdim, int *ydim, 
			 struct dsc$descriptor *title, 
			 int *byte);

#endif


/*
** The following functions control the user defined button boxes.
*/

				/* Create a button box ncol x nrow  */

int Xamine_DefineButtonBox(int ncol,int nrow);
#ifdef VMS
int f77xamine_definebuttonbox
#else
int f77xamine_definebuttonbox_
#endif
                             (int *ncol, int *nrow);

int Xamine_DefineButton(int nrow, int ncol, void *button);

#ifdef VMS
int f77xamine_definebutton
#else
int f77xamine_definebutton_
#endif 
                            (int *nrow, int *ncol, void *button);



int Xamine_ModifyButton(int r, int c, void *button);
#ifdef VMS
int f77xamine_modifybutton
#else
int f77xamine_modifybutton_
#endif
                           (int *r, int *c, void *button);


/*
** The following is a VMS only routine for doing event flag/ast synchronized
** event handling... note that if the astrtn is NULL, then no
** AST will be declared.
*/

#ifdef VMS
int Xamine_QueueEventRead(void *userbuf, int eventflag, void *astrtn, 
			  void *astdata, short *iosb);
int f77xamine_queueeventread(void *userbuf, int *eventflag, void *astrtn,
			  void *astdata, short *iosb);
#endif
			  

int Xamine_PollForEvent(int timeout, void *event);
int Xamine_ReadEvent(void *event);

int
#ifdef VMS
f77xamine_pollforevent
#else
f77xamine_pollforevent_
#endif
                         (int *timeout, void *event);
int
#ifdef VMS
f77xamine_readevent
#else
f77xamine_readevent_
#endif
                (void *event);

int Xamine_EnableButton(int r, int c);
int Xamine_DisableButton(int r, int c);

int
#ifdef VMS
f77xamine_enablebutton
#else
f77xamine_enablebutton_
#endif
                        (int *r, int *c);

int
#ifdef VMS
f77xamine_disablebutton
#else
f77xamine_disablebutton_
#endif
                         (int *r, int *c);
 
int Xamine_DeleteButton(int r, int c);

int
#ifdef VMS
f77xamine_deletebutton
#else
f77xamine_deletebutton_
#endif
                      (int *r, int *c);

int Xamine_DeleteButtonBox();
int 
#ifdef VMS
f77xamine_deletebuttonbox();
#else
f77xamine_deletebuttonbox_();
#endif

int Xamine_InquireButton(int r, int c,
			 void  *ack);

int
#ifdef VMS
f77xamine_inquirebutton
#else
f77xamine_inquirebutton_
#endif
                        (int *r, int *c,
			 void *ack);

int Xamine_EnterPeakMarker(int nSpectrum, int nId,
			   char*              szName,
			   float              fCentroid,
			   float              fWidth);
int
#ifdef VMS
f77xamine_enterpeakmarker(int*   nSpectrum,
			  int*   nId,
			  struct dsc$descriptor* dscName,
			  
#else
f77xamine_enterpeakmarker_(int*  nSpectrum,
			   int*  nId,
			   char* szName,
#endif
			   float* fCentroid,
			   float* fWidth
#ifdef VMS
			   );
#else
			  , int  nNameLength
			  );
#endif

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#include "clientops.h"
#endif




