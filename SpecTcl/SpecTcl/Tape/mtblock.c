/*
**++
**  FACILITY:
**
**      Ciprico/Exabyte I/O support for NSCL front ends.
**
**  ABSTRACT:
**	mtblock.c
**	This file contains routines that implement block level I/O
**	to EXABYTE tapes.  The tape is assumed to have been allocated
**	to the calling process in all cases.
**
**
**  AUTHORS:
**      Ron Fox
**
**  CREATION DATE:      
**
**      Recreated after disk crah 12-April-1990
** SCCS info:
**	@(#)mtblock.c	2.4 8/25/92 Library_Source
*--
*/

//static char *version="mtblock.c	2.6 1/12/93 Library_Source\n";

/*
**
**  INCLUDE FILES
**
*/

#include <stdio.h>
#include <limits.h>
#include <errno.h>

#ifdef VMS
#include <ssdef.h>
#include <iodef.h>
#include "vmsstring.h"
#endif

#ifdef unix
#include <sys/types.h>
#include <sys/file.h>
#include <sys/mtio.h>
#include <sys/ioctl.h>
#endif

#include "mtinternl.h"

#ifdef Linux
static struct mtop variable = { MTSETBLK, 0 };

#endif

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      mapioctlerr - Maps return errors from ioctls to 
**		      magtape facility errors.
**
**  FORMAL PARAMETERS:
**
**      int status:
**          status of an ioctl
**
**  RETURN VALUE:
**
**      MT status
**--
*/
#ifdef __STDC__
static int mapioctlerr (int status)
#else
static int mapioctlerr(status)
int status;
#endif
{
#ifdef unix
    if(!status) return MTSUCCESS;
    switch (status)
    {
    	case EBADF:
	    return MTBADHANDLE;
	case EFAULT:
	    return errno;
	case EROFS:
	     return MTPROTECTED;
	case ETIMEDOUT:
	     return errno;
	case ENOSPC:
	     return MTEOMED;
#ifdef sparc
	  case EINVAL:
	    return MTOVERRUN;
#endif
    	default:
	    return MTINTERNAL;
    }
#endif
#ifdef VMS
    if (status == SS$_NORMAL)
    {
	return MTSUCCESS;
    }
    else
    {
	switch (status)
	{
	    case SS$_IVCHAN:
		return MTBADHANDLE;
	    case SS$_DEVALRALLOC:
		return MTALLOCATED;
	    case SS$_ENDOFTAPE:
		return MTLEOT;
	    case SS$_ENDOFFILE:
		return  MTEOF;
	    case SS$_DATAOVERUN:
		return MTOVERRUN;
	    case SS$_NOPRIV:
		return MTPROTECTED;
	    case SS$_DEVOFFLINE:
		return MTOFFLINE;
	    case SS$_ENDOFVOLUME:
		return MTLEOV;
	    case SS$_PARITY:
		return MTIO;
	    default:
		return MTINTERNAL;
	}
    }
#endif
}
    
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      mtopen	- Open a particular tape unit and prepare it for I/O
**
**  FORMAL PARAMETERS:
**
**      int unit:
**          The number of the drive to open (e.g. 0 for rmt0h).
**
**	int write:
**	    Zero to open for read, nonzero for write.
**
**  RETURN VALUE:
**
**  File id for success or -1 if not..
**
**--
*/
#ifdef __STDC__
int mtopen (int unit, int write)
#else
int mtopen(unit, write)
int unit;
int write;
#endif
{
    char devname[80]; 
    int flag;
    int status;
    int fd;
#ifdef VMS
    vmsstring dev;
    short channel;
#endif

#ifdef unix
    sprintf(devname, "/dev/tape%d", unit); /* Construct device name.*/

    if (write)

    {
    	flag = O_RDWR;
    }
    else
    {
    	flag = O_RDONLY;
    }

    fd =  open (devname, flag, 0);

#ifdef Linux			/* Linux requires additional tape setup: */
    if(fd < 0) return fd;
    
    /* Set the tape into variable block mode:  */

    status = ioctl(fd, MTIOCTOP, &variable);
    if(status < 0) {
      mtclose(fd);
      return status;
    }

#endif /* linux */

    return fd;

#endif /* unix */
#ifdef VMS
    sprintf (devname, "MAGTAPE%d:", unit);
    strtovms(&dev, devname);
    dev.dsc$b_dtype =  DSC$K_DTYPE_T;
    dev.dsc$b_class = DSC$K_CLASS_S;
    status = sys$assign (
		 &dev, 
		 &channel, 
		 0, 
		 0);
    if (status == SS$_NORMAL)
    {
	return channel;
    }
    else
    {
	errno = EVMSERR;
	vaxc$errno = status;
	return -1;
    }
#endif
}
/*
**  Functional Description:
**	mtclose	- Closes a device/channel that is open on a tape device.
**  Formal Parameters:
**	int channel	- The channel open on the device.
**
** Returns:
**	0 if successful else an error code. 
*/
#ifdef __STDC__
int mtclose(int channel)
#else
int mtclose(channel)
int channel;
#endif
{
#ifdef unix
   if(close(channel) == -1)
   {
      return mapioctlerr(errno);      
   }
   else
     return 0;
#endif
#ifdef VMS
    int status;
    status = sys$dassgn (
		 channel);
    if (status == SS$_NORMAL)
    {
	return 0;
    }
    else
    {
	errno = EVMSERR; 
	vaxc$errno = status;
	return errno;
    }
#endif
}
/*
** Panic:
**	Panic exits a program
**
**  panic(char *string)
**
*/
#ifdef __STDC__
void panic(char *string)
#else
void panic(string)
char *string;
#endif
{
   fprintf(stderr, string);
   exit(-1);
}
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      mtclearerr    - clears serious errors in tape drives
**
**  FORMAL PARAMETERS:
**
**      int fd	- File descriptor of unit to clear.
**
**--
*/
#ifdef __STDC__
void mtclearerr (int fd)
#else
void mtclearerr(fd)
int fd;
#endif
{
#ifdef ultrix
    struct mtop reset;

    reset.mt_op = MTCSE;
    reset.mt_count = 1;
    ioctl(fd, MTIOCTOP, &reset);
#endif
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      wtdrive	- Wait for a drive to become ready, tape loaded etc.
**
**  FORMAL PARAMETERS:
**
**	int	unit	- File id open on unit we're waiting for..
**
**
**--
*/
#ifdef __STDC__
void wtdrive (int unit)
#else
void wtdrive(unit)
int unit;
#endif
{
#ifdef unix
    struct mtop rewind;

    rewind.mt_op = MTREW;
    rewind.mt_count = 1;

    while (ioctl(unit, MTIOCTOP, &rewind) == -1 )
    {
    	mtclearerr(unit);
    }
#endif
#ifdef VMS
    mtrewind(unit);
#endif
   
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      mtwrite	- write a block of data to the magnetic tape.
**
**  FORMAL PARAMETERS:
**
**	int		unit	- file descriptor.
**	char		*block	- Pointer to block of data to write.
**	unsigned	count   - Number of bytes to write.
**
**	FUNCTION VALUE:
**	
**      Status of the write... which is the number of bytes written or -1
**	for error.
**
**--
*/
int 
mtwrite (unit, block, count)
int unit;
char *block;
unsigned count;
{
    int status;
#ifdef unix
    while(((status = write (unit, block, count)) == -1) && (errno == EINTR))
      ;
    if(status > 0)
      return MTSUCCESS;
    else
      return mapioctlerr(0);
#endif
#ifdef VMS

    status = mwrite(&unit, block, &count);
    if (status != SS$_NORMAL)
    {
	return mapioctlerr(status);
    }
    return MTSUCCESS;
#endif
}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      mtrewind    - This function initiates a rewind operation on an EXABYTE
**
**  FORMAL PARAMETERS:
**
**	int		unit	- Number of the unit to rewind.
**
**  FUNCTION VALUE:
**
**      status of the rewind attempt.
**
**--
*/
int 
mtrewind (unit)
int unit;
{
#ifdef unix
    struct mtop rewind;

    rewind.mt_op = MTREW;
    rewind.mt_count = 1;
    return mapioctlerr(ioctl(unit, MTIOCTOP, &rewind));
#endif
#ifdef VMS
    short iosb[4];
    int status;

    status = SYS$QIOW (
		 0, 
		 unit, 
		 IO$_REWIND, 
		 iosb, 
		 0, 
		 0, 
		 0, 
		 0, 
		 0, 
		 0, 
		 0, 
		 0);
    if (status != SS$_NORMAL)
    {
	return mapioctlerr(status);
    }
    if (iosb[0] != SS$_NORMAL)
    {
	return mapioctlerr(iosb[0]);
    }
    return MTSUCCESS;
#endif
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      mtload	- Load a tape cartridge.
**
**  FORMAL PARAMETERS:
**
**	int		unit	- Number of the drive to load.
**  
**
**  FUNCTION VALUE:
**
**	0 if it works, -1 if it fails.
**--
*/
void
mtload (unit)
int unit;
{
    wtdrive(unit);
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      mtunload    - Unload a tape cartridge from the drive.
**
**  FORMAL PARAMETERS:
**
**	int		unit	- Unit number of drive to unload.
**
**
**--
*/
void 
mtunload (unit)
int unit;
{
#ifdef unix
    struct mtop unload;

    unload.mt_op = MTOFFL;
    unload.mt_count = 1;

    ioctl(unit, MTIOCTOP, &unload);
#endif
#ifdef VMS
    short iosb[4];
    int   status;

    status = SYS$QIOW (
		 0, 
		 unit, 
		 IO$_UNLOAD, 
		 iosb, 
		 0, 
		 0, 
		 0, 
		 0, 
		 0, 
		 0, 
		 0, 
		 0);

#endif
}
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      mtread	- This function reads a block of data from the drive.
**
**  FORMAL PARAMETERS:
**
**	int		unit	- Number of unit to read from.
**	char		*block	- Pointer to the block to read.
**	unsigned	count	- Number of bytes to read.
**
** RETURNS:
**	Number of bytes read.
**	0 for EOF mark.
**     -1 for error.
**
**--
*/
int mtread (unit, block, count, actual)
int unit;
char *block;
unsigned count;
unsigned *actual;
{
    int status;

#ifdef VMS
    short iosb[4];
#endif

    *actual = 0;

#ifdef unix
    while (((status =  read (unit, block, count)) == -1) && (errno ==EINTR))
      ;
#endif

#ifdef VMS
    status = mread(&unit, actual, iosb, block, &count);
    if (status != SS$_NORMAL)
    {
	return mapioctlerr(status);
    }
    sys$synch (
	0, 
	iosb);
#endif

#ifdef unix
    if (status == 0)
    {
    	mtclearerr(unit);
	return MTEOF;
    }
    if (status < 0)
    {
    	switch (errno)
	{
	    case EINVAL:
	         return MTOVERRUN;
	    case EBADF:
	    	 return MTBADHANDLE;
	    case EINTR:
	    	 return mtread(unit, block, count, actual);
	    case EIO:
	    case ETIMEDOUT:
	    	 return MTIO;
	    default:
	    	 return MTINTERNAL;
	}
    }
    else
    {
	*actual = status;
    	return MTSUCCESS;
    }
#endif

#ifdef VMS
    *actual = iosb[1];
    return mapioctlerr(iosb[0]);
#endif

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      mtweof	-- Writes a long end file mark on the tape.  A long end file
**		mark is preceded by an erase gap long enough to allow the tape
**		to be positioned prior to the eof mark so that the eof mark 
**		can be overwritten.  Long filemarks require 2.21 Mbytes of
**		tape length.
**
**  FORMAL PARAMETERS:
**
**	int		unit	- Unit number to weof on.
**	unsigned	count	- Number of filemarks to write.
**
**--
*/
int  mtweof (unit, count)
int unit;
unsigned count;
{
#ifdef unix
    struct mtop weof;

    weof.mt_op = MTWEOF;
    weof.mt_count = count;

    return mapioctlerr(ioctl(unit, MTIOCTOP, &weof));
#endif
#ifdef VMS
    int i;
    int stat;
    for (i = 0;  i < count;  i++)
    {
	stat = mweof(&unit);
	if (stat != SS$_NORMAL)
	{
	    return mapioctlerr(stat);;
	}
    }
    return MTSUCCESS;
#endif
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      mtspacef    - Space files forward or backwards.
**
**  FORMAL PARAMETERS:
**
**	int		unit	- Number of tape unit to space on.
**	int		count   - Number of filemarks to skip, + is forward,
**				  negative is backwards.
**
**
**--
*/
int 
mtspacef (unit, count)
int unit, 
    count;
{
#ifdef unix
    struct mtop spacef;

    if (count > 0)
    {
    	spacef.mt_op = MTFSF;
	spacef.mt_count = count;
    }
    else
    {
    	spacef.mt_op = MTBSF;
	spacef.mt_count = -count;
    }
    return mapioctlerr(ioctl(unit, MTIOCTOP, &spacef));
#endif
#ifdef VMS
    return mapioctlerr(mspacef(&unit, &count));
#endif
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      mtspacer    - Space records forwards or backwards.
**
**  FORMAL PARAMETERS:
**
**	int		unit	- Unit number.
**	int		count	- Count of records to skip + is forwards,
**				    negative is backwards.
**
**
**--
*/
int 
mtspacer (unit, count)
int unit, 
    count;
{
#ifdef unix
    struct mtop spacer;

    if (count > 0)
    {
    	spacer.mt_op = MTFSR;
	spacer.mt_count = count;
    }
    else
    {
    	spacer.mt_op = MTBSR;
	spacer.mt_op = -count;
    }
    return mapioctlerr(ioctl(unit, MTIOCTOP, &spacer));
#endif
#ifdef VMS
    int status;
    short iosb[4];

    status = SYS$QIOW (
		 0, 
		 unit, 
		 IO$_SKIPRECORD, 
		 iosb, 
		 0, 
		 0, 
		 count, 
		 0, 
		 0, 
		 0, 
		 0, 
		 0);
     if (status != SS$_NORMAL)
     {
	 return mapioctlerr(status);
     }
     return mapioctlerr(iosb[0]);
#endif
}









