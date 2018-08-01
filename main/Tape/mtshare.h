/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


/* DEC/CMS REPLACEMENT HISTORY, Element MTSHARE.H*/
/* *2    12-JUL-1990 11:17:59 FOX "Remove double definition for prot fields"*/
/* *1     4-JUN-1990 16:25:16 FOX "Common definitions between client and mtaccess"*/
/* DEC/CMS REPLACEMENT HISTORY, Element MTSHARE.H*/
/*
**++
**  FACILITY:
**
**      EXABYTE access
**
**  ABSTRACT:
**
**      mtshare.h   - This include file contains defintions which are identical
**		      in the public and private include files for this
**		      system.
**
**  AUTHORS:
**
**      Ron Fox
**
**
**  CREATION DATE:      28-Mar-1990
**	Include_file 1/27/92 @(#)mtshare.h	2.1
**
**--
*/

#ifndef MTSHARE
#define MTSHARE

/*
**
**  MACRO DEFINITIONS
**
*/

	/* Volume list item codes: */

#define VOL_REQLBL	1	/* Requested label. */
#define VOL_PROTECT	2	/* Protection information */
#define VOL_LABEL	3	/* Label read from volume. */
#define VOL_ACCESS	4	/* Access field from volume */
#define VOL_OWNER	5	/* Owner field from volume */
#define VOL_ENDLIST	0	/* No more items on list */
 
	/* File list item codes: */
 
#define FILE_NAMEREQ	1	/* Requested name. */
#define FILE_REQRECLEN	2	/* Requested record length. */
#define FILE_REQPREFIX	3	/* Requested prefix length */
#define FILE_NAME	4	/* Actual filename. */
#define FILE_CREDATE	5	/* File creation date. */
#define FILE_EXPDATE	6	/* File expiration date. */
#define FILE_ACCESS	7	/* File access field. */
#define FILE_RECLEN	8	/* File actual record length. */
#define FILE_BLOCK	9	/* File actual block length. */
#define FILE_PREFIX	10	/* File actual prefix length. */
#define FILE_ENDLIST	0	/* No more items on list. */

	/* Volume access restrictions.		    */


#define READABLE	0
#define WRITEABLE	1
#define VOLREADONLY     READABLE
#define VOLWRITEONLY    WRITEABLE
/*
**
**  Type Definitions
**
*/


	/* Volume item list data structure. */

typedef struct {
		int code;
		char *data;
	       } volume_item;

	/* File item list data structure. */

typedef struct {
		int code;
		char *data;
	       } file_item;

	/*  Error codes: */

#define MTSUCCESS	0		/* Successful completion. */
#define MTINSFMEM	101		/* Insufficient memory. */
#define MTBADHANDLE	102		/* Bad data structure. */
#define MTBADRAM	103		/* RAM failed self test. */
#define MTBADPROM	104		/* PROM failed selftest. */
#define MTINTERNAL	105		/* Internally detected error. */
#define MTALLOCATED	106		/* Device already allocated. */
#define MTBADUNIT	107		/* Bad unit number. */
#define MTREQSENSE	108		/* Request sense data. */
#define MTNOTALLOC	109		/* Device is not allocated. */
#define MTLEOT		110		/* Logical end of tape reached. */
#define MTEOMED		111		/* Physical end of tape reached. */
#define MTOVERRUN	112		/* Block bigger than buffer. */
#define MTEOF		113		/* file mark encountered on read. */
#define MTINVLBL	114		/* Bad volume label. */
#define MTNOTANSI	115		/* Volume not ansi. */
#define MTWRONGVOL	116		/* volume name not that requested. */
#define MTBADPROT	117		/* Invalid protection field. */
#define MTNOTMOUNTED	118		/* Device not mounted. */
#define MTFILEOPEN	119		/* Device has open files. */
#define MTPROTECTED	120		/* Protection violation. */
#define MTFNAMEREQ	121		/* File name is required. */
#define MTFILECLOSE	122		/* File not open. */
#define MTBADLENGTH	123		/* Buffer length invalid. */
#define MTOFFLINE	124		/* Tape is off line. */
#define MTAVAILABLE	125		/* Tape is available. */
#define MTLEOV		126		/* End of logical volume. */
#define MTNOTFOUND	127		/* File not found. */
#define MTIO		128		/* Transfer error */
#define MAXMTERRORCODE	128		/* Must be value of last code. */

#endif
