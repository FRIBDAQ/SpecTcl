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

#ifndef __CONFIG_H
#include <config.h>
#define __CONFIG_H
#endif

#ifndef __XAMINEDATATYPES_H
#include "xamineDataTypes.h"
#endif


#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/* Create Shared Memory -- Creates the shared memory region for Xamine */

int Xamine_CreateSharedMemory(int specbytes, volatile Xamine_shared **ptr);
int f77xamine_createsharedmemory_(int *specbytes,volatile Xamine_shared **ptr);
int Xamine_DetachSharedMemory();

/* Start -- Starts up the Xamine process  */

int Xamine_Start();
int f77xamine_start_();

/* Stop -- Stop the Xamine Process      */

int Xamine_Stop();
int f77xamine_stop_();

int Xamine_Alive();
int f77xamine_alive_();

void Xamine_GetMemoryName(char *namebuffer);
void f77xamine_getmemoryname_(char *namebuffer, int maxlen);

int Xamine_MapMemory(char *name, int specbytes,volatile Xamine_shared **ptr);
int f77xamine_mapmemory_(char *name, int *specbytes,
			 volatile Xamine_shared **ptr, int namesize);

/*
** The functions below interface to Xamine's gate handling.
*/


int Xamine_EnterGate(int spec, int id, Xamine_gatetype type,
		     char *name,
		     int npts, Xamine_point *pts);
int f77xamine_entergate_(int *spec, int *id, Xamine_gatetype *type, 
			 char *name, int *npts, Xamine_point *pts);

int Xamine_RemoveGate(int spec, int id, Xamine_gatetype ntype);
int f77xamine_removegate_(int *spec, int *id, Xamine_gatetype *ntype);

long Xamine_StartSearch(int spec, int *status);
long f77xamine_startsearch_(int *spec, int *status);

int Xamine_NextGate(long context, int *id, Xamine_gatetype *type, 
		    int *npts, Xamine_point *pts);
int f77xamine_nextgate_(long *context, int *id, Xamine_gatetype *type, 
			int *npts, Xamine_point *pts);

void  Xamine_EndSearch(long context);
void f77xamine_endsearch_(long *context);

int Xamine_ReadGate(int *spectrum, Xamine_gatetype *type , char *name,
		    int *npts, Xamine_point *pts);
int f77xamine_readgate_(int *spectrum, Xamine_gatetype *type, char *name,
			int *npts, Xamine_point *pts);

int Xamine_PollForGate(int timeout, int *spectrum, 
		       Xamine_gatetype *type, 
		       char *name,
		       int *npts, Xamine_point *pts);
int f77xamine_pollforgate_(int *timeout, int *spectrum, Xamine_gatetype *type, 
			   char *name, int *npts, 
			   Xamine_point *pts);


/*
** Routines below allow spectrum numbers to be allocated, deallocated
** and reserved.
*/

int Xamine_AllocateSpectrum();	    /* Get lowest numbered free spectrum. */
int f77Xamine_AllocateSpectrum_();

int Xamine_ReserveSpectrum(int spno); /* Reserve a particular spectrum. */
int f77Xamine_ReserveSpectrum_(int *spno); 

void Xamine_FreeSpectrum(int id);    /* Free a spectrum.   */
void f77Xamine_FreeSpectrum_(int *id); 

/*
**   Low level spectrum memory routines:
*/

void Xamine_ManageMemory();	/* Must be called first to init. */
void f77Xamine_ManageMemory_();

caddr_t Xamine_AllocMemory(int size); /* Allocate Spectrum memory */
long     f77Xamine_AllocMemory_(int *size);

void    Xamine_FreeMemory(caddr_t loc);	/* Free memory chunk */
void    f77Xamine_FreeMemory_(int  *offset);

/*
** Low level routine to fill in a spectrum definition
*/

void Xamine_DescribeSpectrum(int spno, int xdim, int ydim,
			     const char *title, caddr_t loc,
			     spec_type type);
void f77Xamine_DescribeSpectrum_(int *spno, int *xdim, int *ydim,
				 char *title, int *loc,
				 spec_type *type, int tsize);

/*
** High level routines to allocated, and describe spectra to the
** the memory management routines.
*/

caddr_t Xamine_Allocate1d(int *spno, int xdim, const char *title, int word);
caddr_t Xamine_Allocate2d(int *spno, int xdim, int ydim, const char *title, 
			  int type);
long f77Xamine_Allocate1d_(int *spno, int *xdim, char *title, 
			  int *word, int tlen);
long f77Xamine_Allocate2d_(int *spno, int *xdim, int *ydim, char *title, 
			   int *byte, int tlen);

void Xamine_SetMap1d(int spno, float xmin, float xmax, const spec_label xlabel);
void Xamine_SetMap2d(int spno, float xmin, float xmax, const spec_label xlabel,
		     float ymin, float ymax, const spec_label ylabel);


/*
** The following functions control the user defined button boxes.
*/

				/* Create a button box ncol x nrow  */

int Xamine_DefineButtonBox(int ncol,int nrow);
int f77xamine_definebuttonbox_(int *ncol, int *nrow);
int Xamine_DefineButton(int nrow, int ncol, void *button);
int f77xamine_definebutton_(int *nrow, int *ncol, void *button);
int Xamine_ModifyButton(int r, int c, void *button);
int f77xamine_modifybutton_(int *r, int *c, void *button);

int Xamine_PollForEvent(int timeout, void *event);
int Xamine_ReadEvent(void *event);

int f77xamine_pollforevent_(int *timeout, void *event);
int f77xamine_readevent_(void *event);

int Xamine_EnableButton(int r, int c);
int Xamine_DisableButton(int r, int c);

int f77xamine_enablebutton_(int *r, int *c);
int f77xamine_disablebutton_(int *r, int *c);
int Xamine_DeleteButton(int r, int c);

int f77xamine_deletebutton_(int *r, int *c);
int Xamine_DeleteButtonBox();
int f77xamine_deletebuttonbox_();
int Xamine_InquireButton(int r, int c,
			 void  *ack);

int f77xamine_inquirebutton_(int *r, int *c, void *ack);

int Xamine_EnterPeakMarker(int nSpectrum, int nId,
			   char*              szName,
			   float              fCentroid,
			   float              fWidth);
int f77xamine_enterpeakmarker_(int*  nSpectrum,
			       int*  nId,
			       char* szName,
			       float* fCentroid,
			       float* fWidth,
			       int  nNameLength);

int Xamine_EnterFitline(int nSpectrum,  int nId, char* szName, 
                        int low, int high, char* szEvalScript);

void Xamine_setOverflow(unsigned nSpectrum, int axis, unsigned value);
void Xamine_setUnderflow(unsigned nSpectrum, int axis, unsigned value);
void Xamine_clearStatistics(unsigned nSpetrum);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#include "clientops.h"
#endif
