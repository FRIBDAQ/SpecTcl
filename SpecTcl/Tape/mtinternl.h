/*
**++
**  FACILITY:
**
**      EXABYTE tape support
**
**  ABSTRACT:
**
**      mtinternl.h - Internal definition file.  This file contains
**		      the actual definitions of all the opaque types and 
**		      constants.
**
**  AUTHORS:
**
**      Ron Fox
**
**
**  CREATION DATE:      28-Mar-1990
**	@(#)mtinternl.h	2.1 1/27/92 Include_File	
**--
*/


/*
**
**  INCLUDE FILES
**
*/

#include <time.h>
#include "mtshare.h"

/*
**
**	Macro definitions
**
*/

	/* We use magic numbers to catch some data structure errors. */

#define UCB_MAGIC	0x12345600	/* Unlikely pattern identifying UCB. */
#define VOLUME_MAGIC	0x12345602	/* Unlikely patter identifying	    */
					/*   volume data block.


	    /* Sizes of some label fields in ANSI tapes. */

#define IDSIZE		4		/* Size of label ID field. */
#define FILEIDENT	17		/* Filename */
#define VOLLABEL	6		/* Volume label */
#define IMPSIZE		13		/* Chars in implementation name. */
#define OWNERSIZE	14		/* Characters in volume owner field. */
#define FSIDSIZE	6		/* Size of file section Id size. */
#define NUMSIZE		4		/* Size of typical numeric fields. */
#define VERSIONSIZE	2		/* Size of version fields. */
#define DATESIZE	6		/* Characters in datestamps. */
#define BLKSIZE		5		/* Characters in a block/record len */
#define OFFSIZE		2		/* Chars in an offset size. */
#define BLKCNTSIZ	6		/* chars in block count. */

	    /* ANSI fields are all blank filled: */

#define FILLCHAR	((char)(0x20))
#define STDOWNER	"NSCL"		/* Standard owner of volumes. */

#ifdef unix
#define STDIMP		"UNXDAQ-1.0"
#endif

#ifdef vms
#define STDIMP		"VMSDAQ-1.0"
#endif

#define DEFAULTRECSIZE	16384		/* Default record size. */

	/*  Ansi record formats. */

#define FIXED		'F'
#define VARIABLE	'V'
#define SEGMENTED	'S'

/*
**
**  Type definitions
**
*/

	/* This data structure contains the unit specific information for */
	/* an exabyte.							  */

typedef struct {
	          int magic;		/* Corruption indicator (magic #) */
		  char *device;		/* Device ucb is attached to.	*/
#ifdef unix
		  int fd;		/* UNIX file descriptor */
#endif
#ifdef vms
		  int chan;		/* VMS channel		*/
#endif
	       } tape_ucb;



	    /* Type to keep track of special case tape locations. */

typedef enum {
	    BOT, LEOT, PEOT,  MTMIDDLE, LEOV
	} TapeLocation;

#ifdef EOF
#undef EOF
#endif
typedef enum {
		BOF, EOF, FILEMIDDLE, HEADERS, TRAILERS
	      }   FileLocation;
#ifdef unix
typedef struct tm tm_t;
#endif
typedef struct {
		 unsigned readable : 1, 
			  writeable: 1, 
			  mounted  : 1, 
			  fileopen : 1;
	       } TapeProtection;
typedef struct {
		int magic;		/* Corruption indicator */
		int fd;	/* Pointer to ciprico handle*/
		int unit;		/* Unit volume is controlling. */
		TapeProtection access;	/* Bitmask indicating access. */
		TapeLocation volloc;	/* Where tape is. */
		char volname[VOLLABEL+1];	/* Volume name (file set) */
		int filenumber;		/* Which file we're in. */
		char filename[FILEIDENT+1]; /* Currently created file */
		int blockswritten;	/* # blocks written */
		FileLocation fileloc;	/* Where in file we are. */
		tm_t	creation;	/* File creation date. */
	        int reclen;		/* File record length */
	        int offlen;		/* File offset length */
	       } volume;

	    /* ANSI VOL1 Header format. */

typedef struct {
		    char volid[IDSIZE], 
			 volname[VOLLABEL], 
			 volaccess, 
			 reserved1[13], 
			 implementor[IMPSIZE], 
			 volowner[OWNERSIZE], 
			 reserved2[28], 
			 version;
		} VOL1;

	    /* The FLBL1 data type describes the structure of HDR1, EOF1,   */
	    /* and EOV1 file lables. */

typedef struct {
		    char hdrid[IDSIZE],		/* Label type id. */
			 filename[FILEIDENT],	/* Name of file. */
			 fsetid[FSIDSIZE],	/* Name of fileset id. */
			 secnum[NUMSIZE],	/* file section number. */
			 seqnum[NUMSIZE],	/* File sequence number. */
			 generation[NUMSIZE],	/* Generation and. */
			 version[VERSIONSIZE],	/* version. */
			 credate[DATESIZE],	/* Creation date. */
			 expdate[DATESIZE],	/* Expiration date. */
			 access,		/* Protection field. */
			 blkcnt[BLKCNTSIZ],	/* Blocks in file. */
			 implementor[IMPSIZE],	/* ANSI implementor which   */
						/* wrote the file. */
			 reserved[7];
		} FLBL1;


	    /* The FLBL2 data type describes the structure of the   */
	    /* HDR2, EOF2 and EOV2 file labels. */

typedef struct {
		    char hdrid[IDSIZE],		    /* Label type. */
			 recordfmt,		    /* Record format. */
			 blocklen[BLKSIZE],	    /* size of blocks. */
			 recordlen[BLKSIZE],	    /* Longest record. */
			 reserved1[35], 
			 offlen[OFFSIZE],	    /* Offset to data. */
			 reserved2[28];
	       } FLBL2;

