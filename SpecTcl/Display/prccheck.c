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


/*
** Include files:
*/

#include <config.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>


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
#ifdef HAVE_WINDOWS_H    /* Cygwin */
{
  return -1;			/* Let shutdown of pipe kill me off. */
}
#else 
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
        caller and we can only deteyrmine liveness by doing a kill 0 on it.
        kill 0 does not work with children since zombie processes can
	recieve signals in e.g. Linux.
    */

    return (kill(pid, 0) == 0);

}
#endif
