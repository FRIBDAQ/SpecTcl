
/*
**++
**  FACILITY:  MultiRoute
**
**  MODULE DESCRIPTION:
**
**      getbufvms.c
**	   This module provides a VMS like interface to the multi source
**	   router.  Clients of this module should be cautioned that use
**	   of function(s) provided by this module will result in non
**	   portable code
**
**  AUTHORS:
**
**      Ron Fox
**      NSCL
**	Michigan State University
**	East Lansing, MI 48824-1321
**
**  CREATION DATE:  March 25, 1994
**--
*/

#ifdef unix
THIS MODULE IS FOR VMS ONLY.
#endif
/*
**
**  INCLUDE FILES
**
*/
#include <ssdef.h>
#include <iodef.h>
#include <stdlib.h>

#include "router.h"

/* 
** Data type definitions:
*/
#ifdef __ALPHA
#pragma member_alignment __save
#pragma nomember_alignment
#endif
typedef struct {		    /* VMS final I/O status block */
		 short status;
		 short xfercount;
		 unsigned int senderpid;
	       } IOSB;


#ifdef __ALPHA
#pragma member_alignment __restore
#endif
typedef struct {		     /* AST data block used in $QIO */
		 routershared *base; /* Pointer to router base memory area */
		 NOTIFYMSG msg;	     /* Mailbox message */
		 BCB      **bcb;     /* Pointer to BCB pointer buffer */
		 IOSB      finalsts; /* Final I/O status of the read */
		 int       efn;	    /* Event flag number.	    */
		 void      (*astrtn)(); /* AST block.                */
		 void      *astprm;  /* AST parameter		    */
               } ASTBLOCK;


/*
** External functions required:
*/
int sys$qio(int efn, int chan, int function, IOSB *iosb, void (*astrtn)(), 
	    void *astprm,
	    int p1, int p2, int p3, int p4, int p5, int p6);

int sys$setef(int efn);

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      rtr_DataAst:
**	    This function is called at AST level when the raw read from the
**	    stream mail box is completed.  We need to get the BCB pointer,
**          fill in the user's idea of it and call the AST routine prior to
**	    setting the event flag for the user.  The AST block is released too.
**
**  FORMAL PARAMETERS:
**
**      ASTBLOCK *ablk:
**          Pointer to the AST definition block.
**
**--
*/
void rtr_DataAst(ASTBLOCK *ablk)
{
    BCB *bcb;
    SCB *scb;
    routershared *base = ablk->base;



    sys$setef(ablk->efn);

    if (ablk->finalsts.status != SS$_NORMAL)
    {
    	if (ablk->astrtn != NULL)
	{
	    ablk->astrtn(NULL, &ablk->finalsts, NULL, ablk->astprm);
	}
    }
    else {
	bcb = &base->buffer_control[ablk->msg.bcbptr];
	scb = &base->streams[ablk->msg.scbptr];
	if (ablk->bcb != NULL)
	{
	    *(ablk->bcb) = bcb;
	}
	 
	if (ablk->astrtn != NULL)
	{
	    ablk->astrtn(base, scb, bcb, ablk->astprm);
	}
    }
    free(ablk);
    return;
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      rtrgetbufvms:
**        Get multirouter buffer with VMS completion model.
**
**  FORMAL PARAMETERS:
**
**      int efn:
**          Event flag which is signalled on completion of the I/O
**
**      void *base:
**	    Pointer to the shared memory base.
**	 
**      SCB *stream:
**          Pointer to the stream control block from which the read will
**	    occur.
**	 
**      BCB **bcb:
**          If not null, pointer to a buffer to hold a pointer to the BCB
**	    of the buffer gotten.
**	 
**      void (*astrtn)():
**          If not null, then this points to a routine that will be called
**	    at AST level when the stream read is completed.  The routine
**	    is called as follows: astrtn(void *base, SCB *stream BCB *bcb,
**				         void *astprm);
**	    NOTE: If the I/O is cancelled or otherwise fails, the call
**		  sequence of the AST routine will be:
**			astrtn(NULL, IOSB *final_status, NULL, void *astprm);
**		   since base can never be NULL Normally, this is
**		   distinguishable.  Failure of the AST routine to check for
**		   base == NULL can give unpredictable results.
**      void *astprm:
**          A parameter passed to the user AST routine without interpretation
**	    (can be null).
**
**  RETURN VALUE:
**
**      status from the SYS$QIO of the stream mailbox... This will be a pure
**      VMS status code.
**
**--
*/
extern int rtrgetbufvms (int efn, void *base, SCB *stream, BCB **bcb,
			 void (*astrtn)(), void *astprm)
{
    ASTBLOCK *ablk;

    /*
    ** First allocate the storage for the AST data block.
    */
    ablk = (ASTBLOCK *)malloc(sizeof (ASTBLOCK));
    if (ablk == NULL)
    {
    	return SS$_INSFMEM;
    }
    /*
    ** Fill in the AST data block:
    */
    ablk->base =base;
    ablk->efn  = efn;
    ablk->astrtn = astrtn;
    ablk->astprm = astprm;
    ablk->bcb    = bcb;

    /* Start the QIO off... return the status to the user  */

    return sys$qio(0, stream->consumer_chan, IO$_READVBLK,
		   &ablk->finalsts, rtr_DataAst, ablk,
		   (int)(&ablk->msg), sizeof(NOTIFYMSG), 0,0,0,0);
}
/*
** Fortran version of the routine:
*/
extern int f77rtrgetbufvms(int *efn, void **base, SCB **stream,
			   BCB **bcb, void( *astrtn)(), void *astprm)
{
   return rtrgetbufvms(*efn, *base, *stream, bcb, astrtn, astprm);
}
