/* DEC/CMS REPLACEMENT HISTORY, Element MTYPES.H*/
/* *8    18-APR-1992 18:08:42 FOX "Add signature bytes to buffer header structure"*/
/* *7    20-MAR-1990 17:08:52 FOX "Remove special fields from user events."*/
/* *6     6-FEB-1990 16:57:19 FOX "Add srcpid field to bufamsg structure"*/
/* *5     7-DEC-1989 16:44:54 FOX "Add feature variable to support multiple inclusion"*/
/* *4    17-NOV-1989 13:55:14 FOX "Add user event/buffer definitions"*/
/* *3    31-AUG-1989 17:44:22 FOX "Use CCONFIG logical rather than config"*/
/* *2    19-SEP-1988 08:50:00 FOX "Add 3 unused words to buffer"*/
/* *1     3-FEB-1988 11:35:46 FOX "Include file of master data types"*/
/* DEC/CMS REPLACEMENT HISTORY, Element MTYPES.H*/
/* DEC/CMS REPLACEMENT HISTORY, Element MTYPES.H*/
/* *4     9-OCT-1987 10:33:48 FOX "Make phydata a structured type"*/
/* *3     8-OCT-1987 18:10:39 FOX "Make bftime fields all INT16"*/
/* *2     7-OCT-1987 16:05:08 FOX "Add partition control blocks"*/
/* *1     7-OCT-1987 11:28:18 FOX "Master processor data types"*/
/* DEC/CMS REPLACEMENT HISTORY, Element MTYPES.H*/
/*
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
	INT16	unused[2];		/* Pad out to 16 words.	    */
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
	INT32	etime;			/* Start time since SOR in ticks */
	INT16	unused1[3];		/* Unused words.	    */
	INT32	btime;			/* End time since SOR in ticks	*/
	INT16	unused2[3];		/* Unused words.	    */
	INT32	scalers[1];		/* Array with scaler data   */
    };

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
