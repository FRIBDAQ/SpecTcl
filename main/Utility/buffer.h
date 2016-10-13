/*

#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2013.
#
#    You may use this software under the terms of the GNU public license
#    (GPL).  The terms of this license are described at:
#
#     http://www.gnu.org/licenses/gpl.txt
#
#    Author:
#            Ron Fox
#            NSCL
#            Michigan State University
#            East Lansing, MI 48824-1321


**++
**  FACILITY:
**
**      Data acquisition system front end MASTER processor.
**
**  ABSTRACT:
**
**      This file contains type definitions used by the master processor
**  We assume the definition of the following types:
**	INT32	- 32 bit integer
**      INT16	- 16 bit integer
**	INT8	- eight bit integer.
**
**  AUTHORS:
**
**      Ron Fox
**
**
**  CREATION DATE:     7-Oct-1987
**
**  MODIFICATION HISTORY:
**     @(#)mtypes.h	1.2 3/25/94 Include
**--
**/

#ifndef __MTYPES_H	
#define __MTYPES_H
#include <daqtypes.h>
/*		Absolute time:		*/

struct bftime
    {
	INT16	month;			/* Month 1-12		*/     /* 3 */
	INT16	day;			/* Day	 1-31		*/     /* 3 */
	INT16	year;			/* e.g. 1987		*/
	INT16	hours;			/* 0-23			*/     /* 3 */
	INT16	min;			/* 0-59			*/     /* 3 */
	INT16	sec;			/* 0-59			*/     /* 3 */
	INT16	tenths;			/* 0-9.			*/     /* 3 */
    };

/*		Structures which describe the final output data buffers */

struct bheader				/* Data buffer header	*/
{
  INT16	nwds;			/* Used part of buffer	*/
  INT16	type;			/* buffer type		*/
  INT16	cks;			/* checksum over used part of buffer */
  INT16	run;			/* Run number		*/
  INT32	seq;			/* Buffer sequence number */
  INT16	nevt;			/* Event count in buffer    */
  INT16	nlam;			/* Number of lam masks	    */
  INT16	cpu;			/* Processor number	    */
  INT16	nbit;			/* Number of bit registers */
  INT16	buffmt;			/* Data format revision level */
  INT16   ssignature;		/* Short byte order signature */
  INT32   lsignature;		/* Long byte order signature  */
      INT16   nwdsHigh;
  INT16	unused;		/* Pad out to 16 words.	    */
};

struct ctlbody				/* Body of control buffer   */
    {					/* start/stop/pause/resume  */
	char    title[80];		/* Run title.		    */
	INT32	sortim;			/* Time in ticks since run start */
	struct  bftime tod;		/* Absolute time buffer was made    */
    };

struct	usrbufbody			/* Declares user buffer body. */
{
    struct bftime   usertime;		/* Time stamp for user buffer. */
    INT16  userbody[1];			/* Body of user buffer.	       */

};

struct sclbody				/* body of scaler buffers   */
    {					/* taped and snapshot	    */
	INT32	etime; /* Start time since SOR in ticks */
	INT16	unused1[3]; /* Unused words.	    */
	INT32	btime; /* End time since SOR in ticks	*/
	INT16	unused2[3]; /* Unused words.	    */
                  
	INT32	scalers[1];		/* Array with scaler data   */
    }
#ifdef __GNUC__ 
 __attribute__((packed))
#endif
;

/*	    The types below define the structure of event packets put in    */
/*	the circular buffer queue by the event acquisition processor.	    */

typedef    INT32    ctlevt;		/* Control events just have time    */

struct sclevt				/* Scaler event		    */
    {
	INT32	bticks;			/* Ticks at interval start  */
	INT32	eticks;			/* Ticks at interval end.   */
	INT16	nscl;			/* Number of scalers	    */
	INT32	scls[1];	       	/* The scaler data	    */
    };

struct phydata							       /* 4 */
    {								       /* 4 */
	INT16    cnt;			/* Size of event data in words */ /* 4 */
	INT16	 data[1];		/* actual data		    */ /* 4 */
    };								       /* 4 */

struct	usrevt				/* User generated event.	 */
{
    INT16   usrevtbody[1];		/* User event body.		  */
};



typedef    struct bheader    BHEADER;	    /* Buffer header		*/
typedef    struct event      EVENT;	    /* Event structure		*/
typedef    struct evtpool    EVTPOOL;	    /* Event pool control struct */
typedef    struct part	     PART;	    /* Partition control block	*/ /* 2 */

#endif
