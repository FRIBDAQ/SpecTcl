/*
** Facility:
**    Xamine -- NSCL display program.
**
** Abstract:
**    prccheck.c
**      This file contains code to verify the existence of processes.
**      
** Author:
**    Ron Fox
**    NSCL
**    Michigan State University
**    East Lansing, MI 48824-1321
*/
static char *sccsinfo="@(#)prccheck.c	1.1 3/4/94 ";


/*
** Include files:
*/

#ifdef unix
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#endif

#ifdef VMS
#include <descrip.h>
#include <ssdef.h>
#include <unixlib.h>
#include <jpidef.h>
#ifdef __ALPHA
#include <string.h>
#endif
#endif

#include "prccheck.h"

/*
** External references:
*/

#ifdef VMS
#ifdef __ALPHA
#pragma member_alignment __save
#pragma nomember_alignment
#endif
struct itemlist3   {
                    short buflen;
		    short code;
		    char *bufadr;
		    char *retlenadr;
		  };

#ifdef __ALPHA
#pragma member_alignment __restore
#endif
int sys$getjpiw(int efn, pid_t *pid, struct dsc$descriptor *name,
	       struct itemlist3 *items, int *iosb, int *astadr, int astprm);

#endif



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      ProcessAlive:
**         Determines if a process is present.
**
**  FORMAL PARAMETERS:
**
**      pid_t pid:
**        Process ident of the process.
**
**  RETURN VALUE:
**
**      TRUE  - PRocess exists.
**      FALSE - Process doesn't exist.
**
**
**--
*/
int ProcessAlive (pid_t pid)
#ifdef unix
{
    int status;
    int waitstat;

    /* If process is a child it could be exited or zombified... need
       to use wait to determine that:
       */

    waitstat = waitpid(pid, &status, WNOHANG);
    if(waitstat >= 0) {		/* Valid child process */
      return (waitstat == 0);	/* 0 if running pid if exited or zombie */
    }
    /*  If control passes here, then the process is not a child of the
        caller and we can only determine liveness by doing a kill 0 on it.
        kill 0 does not work with children since zombie processes can
	recieve signals in e.g. Linux.
	*/
    return (kill(pid, 0) == 0);

}
#endif
#ifdef VMS
{
    struct itemlist3 request[2];
    int Lpid;
    int stat;
    int siz;

    /* build the item list. */

    request[0].buflen = sizeof(int);
    request[0].code   = JPI$_PID;
    request[0].bufadr = (char *)&Lpid;
    request[0].retlenadr = (char *)&siz;

    memset(&request[1], 0, sizeof(struct itemlist3));

    stat = sys$getjpiw(0, &pid, 0, request, 0, 0, 0);

    return (stat == SS$_NORMAL);
    
}
#endif
