/*
** Facility:
**   Xamine  - NSCL Display program (client library).
** Abstract:
**   cliengates.cc:
**     This file implements the gate communication library routines for
**     Xamine.  We implement routines which send gates to Xamine and
**     also get gates from Xamine which have been accepted by users.
**     Note that on UNIX, multilanguage programs involving both g++
**     and f77 are unreliable.  Therefore there can unfortunately
**     be no usage of the convenient objects defined in messages.cc
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University8
***  East Lansing, MI 48824-1321
*/
static char *sccsinfo = "@(#)clientgates.c	1.1 1/28/94 ";

/*
** Include files:
*/
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif
#ifdef unix
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#endif
#ifdef VMS
#include <types.h>
#include <string.h>
#include <ssdef.h>
#include <iodef.h>
#include <descrip.h>
#include <unixlib.h>
#include <unixio.h>
#include <file.h>
#endif

#ifdef sparc
#define atexit(func)  on_exit(func, NULL);
#endif
#include "clientgates.h"

/*
** VMS External functions which represent the system services that we
** require:
*/

#ifdef VMS
#ifdef __ALPHA
#pragma member_alignment __save
#pragma nomember_alignment
#endif
typedef struct {  short status;
		  short count;
		  int   senderpid;
		} IOSB;
#ifdef __ALPHA
#pragma member_alignment __restore
#endif
typedef void (*Astrtn)(int astprm); /* AST routine. */

int sys$crembx(int    permflg, 
	       short *chan,
	       int    maxmsg,
	       int    bufquo,
	       short  promsk,
	       int    acmode,
	       struct dsc$descriptor *logname);

int sys$qio(int    efn,
	    short  chan,
	    short  func,
	    IOSB  *iosb,
	    Astrtn astadr,
	    int    astprm,
	    int    p1, int p2, int p3,
	    int    p4, int p5, int p6);

int sys$qiow(int   efn,
	     short chan,
	     int   function,
	     IOSB *iosb,
	     Astrtn astadr,
	     int   astprm,
	     int   p1, int p2, int p3,
	     int   p4, int p5, int p6);
int sys$synch(int efn,
	      IOSB *iosb);

int sys$cancel(short chan);
int sys$cantim(int reqidt, int acmode);
int sys$bintim(struct dsc$descriptor *timbuf, int *timadr);
int sys$setimr(int efn, int *daytime, Astrtn astadr, int reqidt);
int sys$setef(int efn);
#endif

/*
** The following data types are essentially stolen from messages.h
** with some modifications necessary in order to avoid c++ constructions.
** If any changes are made to these types there they must be propagated here
*/
#include "client.h"
#include "clientops.h"
/*
** The definitions below are used in composing messages:
*/

/*
** Below is static data.  In particular, we save the 
** channels/id's associated with all of the pipes:
**   Associated with each chanel is a listener:
*/
int Xamine_requests;		/* Xamine request mailbox. */
int Xamine_acks;		/* Xamine ack's come here. */
int Xamine_newgates;		/* New gates are sent here. */

/* 
** Defines which help build up the name of 
** the pipes in a system indpenent manner.
*/
#define NAMELEN  128
#ifdef unix
#define PREFIX   "/tmp"
#define REQ_NAME "/.Xamine_%x_GatesIn"      /* %x replaced by parent pid. */
#define ACK_NAME "/.Xamine_%x_GatesAck"
#define OUT_NAME "/.Xamine_%x_GatesOut"
#endif
#ifdef VMS
#define PREFIX ""
#define REQ_NAME "Xamine_%x_GatesIn"
#define ACK_NAME "Xamine_%x_GatesAck"
#define OUT_NAME "Xamine_%x_GatesOut"
#endif


/*
** The first pages of this file contain system dependent code which isolates
** The remainder from the operating system dependent ways of implementing
** named interprocess communications devices.
** The pipe implementation on unix is the fifo special file while on VMS
** we use the mailbox device and an associated logical name in the job table
** to carry the name of the pipe.
*/
/*
** Functional Description:
**   SetTimerAst:                 - System dependent -
**     This VMS specific function queues a timer AST to fire after a given
**     number of seconds of delta time.
** Formal Parameters:
**    int time:
**      # seconds until AST should fire.
**    Astrtn astadr:
**      The AST to fire.
**    int timeid:
**      The timer id/ast argument.
*/
#ifdef VMS
static void
SetTimerAst(int time, Astrtn astadr, int timeid)
{
  int day, hr, min, sec;
  char asctim[80];
  int  tquad[2];
  struct dsc$descriptor timdesc;

  sec = time % 60; time = time/60;
  min = time % 60; time = time/60;
  hr  = time % 24; day = time/24;

  sprintf(asctim, "%d %d:%d:%d.0", day, hr, min, sec);
  timdesc.dsc$a_pointer = asctim;
  timdesc.dsc$w_length  = strlen(asctim);
  timdesc.dsc$b_class   = DSC$K_CLASS_S;
  timdesc.dsc$b_dtype    = DSC$K_DTYPE_T;

  sys$bintim(&timdesc, tquad);
  sys$setimr(0, tquad, astadr, timeid);

}
#endif

/*
** Functional Description:
**   CancelAst:                 - SYSTEM DEPENDENT -
**     This VAX/VMS only function is called at AST level to cancel
**     a pending IO:
** Formal Parameters:
**   int id:
**     Channel to cancel.
*/
#ifdef VMS
static void CancelAst(int id)
{
  sys$cancel((short)id);
}
#endif
#ifdef unix
/*
** Functional Description:
**  AcceptConnection
**    Internal function to accept a connection on a pipe.
**  Unix dependent.
** Parameters:
**   char* pName - Name of the pipe.
**   int   fd    - File descriptor of the listening socket.
** Returns:
**   The result of the accept service.
*/
int AcceptConnection(char* pName, int fd)
{
  struct sockaddr_un peer;
  int                nbytes;
  int                comfid;

  nbytes = sizeof(struct sockaddr_un);
  comfid = accept(fd, (struct sockaddr*) &peer, &nbytes);
  if(comfid < 0) return -1;
  /*  It turns out that on a fast system, closing and unlinking is a bad
  **  move since the accept wont' complete always on the client side.
  */

  return comfid;
}
#endif

/*
** Functional Description:
**   MakePipe:                   - SYSTEM DEPENDENT -
**    Static function to create a named pipe-like IPC primitive.
**    On UNIX this is done via a named AF_UNIX socket. Originally,
**    this was a fifo, due to issues with CYGWin's implementation (or rather
**    non-implementation of mkfifo), we have shifted to sockets instead.
**    We play the server, and each pipe made accepts a single connection.
**    that connection is returned.
**    On VMS a mailbox is used.
** Formal Parameters:
**   char *name:
**      Name of the mailbox.
**   int flags:
**      Open flags which really only have meaning in Unix.  Things like
**      O_RDONLY or O_WRONLY are expected here.
** Returns:
**   id  - id is a positive number which must be handed to I/O routines to
**         identify the pipe.  On VMS this is the mailbox channel number.
**         On unix it is the file id.
**  -1   - Failure with error syndrome in errno or in vms possibly
**         vaxc$errno.
*/
static int
MakePipe(char *name, int flags)
#ifdef unix
{
  int                lisnfid;
  struct sockaddr_un addr;

  /* Create the socket in the unix domain... */

  lisnfid = socket(AF_UNIX, SOCK_STREAM, 0);
  if(lisnfid < 0) return -1;

  /* Bind the socket:  First unlink any existing instance of the socket,
  ** then bind(2) to create a new one.
  */
  unlink(name);
  addr.sun_family = AF_UNIX;
  strcpy(addr.sun_path, name);
  if(bind(lisnfid, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) < 0) {
    return -1;
  }

  /* Listen for connections. Accept is done after Xamine is started up
   */

  if(listen(lisnfid, 1) < 0) {
    return -1;
  }

  return lisnfid;

}
#endif				/* unix implementation */
#ifdef VMS
{
  int stat;
  int chan;
  struct dsc$descriptor logname;
  short promask = 0xff00;      /* Represents system and owner full access */

  /* First we need to construc the logical name descriptor from the
  ** null terminated pipe name we've received.
  */
  logname.dsc$a_pointer = name;
  logname.dsc$w_length  = strlen(name);
  logname.dsc$b_dtype   = DSC$K_DTYPE_T;
  logname.dsc$b_class   = DSC$K_CLASS_S;
  /*
  ** Now we create the mailbox:
  */

  stat = sys$crembx(0,		/* Not permanent */
		    (short *)&chan,
		    sizeof(msg_XamineEvent)*2,
		    sizeof(msg_XamineEvent)*2,
		    promask,	/* Allow system and owner access only. */
		    0,		/* Create in user mode so it'll run down */
		    &logname);
  /* 
  ** The rest has to do with mapping VMS errors to VMS C run time errors: 
  */
  if( (stat & 1) ) {		/* Normal return */
    return chan;		/* So return the channel. */
  }
  else {			/* Abnormal return. */
    errno       = EVMSERR;
    vaxc$errno  = stat;
    return -1;
  }

}
#endif				/* VMS implementation. */

/*
** Functional Description:
**   WritePipe:             - System dependent -
**     This function writes data to a pipe.
**     In Unix this is done via the write() system call.
**     In VMS this is done using a $QIOW.
** Formal Parameters:
**     int id:
**       Identifies the pipe.   Returned from MakePipe
**     void *buffer:
**       Pointer to a data buffer to write.
**     int size:
**       Number of bytes in the data buffer.
** Returns:
**   size  - If successful.
**   -1    - If failed with error in errno etc.
*/
int 
Xamine_WritePipe_(int id, void *buffer, int size)
#ifdef unix
{
  return write(id, buffer, size);
}
#endif
#ifdef VMS
{
  IOSB iosb;
  int  status;

  status = sys$qiow(0, (short)id, IO$_WRITEVBLK | IO$M_NOW, &iosb,
		    NULL, 0, 
		    (int)buffer, size, 0,0,0,0);
  if( (status & 1) ) {		/* QIO successfully started */
    if( iosb.status & 1) {	/* QIO successfully finished */
      return iosb.count;
    }
    else {			/* QIO did not finish well... */
      errno      = EVMSERR;
      vaxc$errno = iosb.status;
      return -1;
    }
  }
  else {			/* QIO did not even get started. */
    errno      = EVMSERR;
    vaxc$errno = iosb.status;
    return -1;
  }

}
#endif

/*
** Functional Description:
**   ReadPipe:                     - System Dependent -
**     This function allows the caller to read from a pipe.  The read is done
**     with an optional timeout which if present will cause the read to 
**     finish after the specified number of seconds if no data is
**     available.
**     On Unix, this timeout is implemented using the select() system
**     service while on VMS it's necessary to use AST timers and I/O 
**     cancellation to get the same effect.
** Formal Parameters:
**   int id:
**     The pipe id that was returned from MakePipe.
**   int timeout:
**     The timeout period which can be:
**      > 0    - Number of seconds to wait.
**      = 0    - Indicates an immediate poll for data.
**      < 0    - Indicates no timeout is desired.
**    void *buffer:
**      Points to the buffer into which data will be placed.
**    int size:
**      Size of the read buffer.
** Returns:
**    > 0      - Number of bytes read.
**    = 0      - Read timed out.
**    < 0      - Read failed with the reason for failure stored in errno etc.
*/
int Xamine_ReadPipe_(int id, int timeout, void *buffer, int size)
#ifdef unix
{
  struct timeval to;
  char            *pBuffer = (char*)buffer;
  int remainder = size;
  fd_set         readfds, writefds, exceptfds;

  if(timeout >= 0) {
    int selstat;
    /* Set up the timeout:    */

    to.tv_sec = timeout;
    to.tv_usec= 0;

    /* Set up the file descriptor sets. */

    FD_ZERO(&readfds);
    FD_ZERO(&writefds);		/* Initialize all fd sets to empty. */
    FD_ZERO(&exceptfds);

    FD_SET(id, &readfds);	/* The user passed the read fd.  */

    selstat = select(FD_SETSIZE, &readfds, &writefds, &exceptfds,
		     &to);
    if(selstat != 1) 
      return selstat;		/* Return on failure or timeout. */
  }
  /*
  ** If control fell here, then either the select indicated that data
  ** was available or no timeout was specified and we can retrieve the data
  ** via a blocking read.
  */

  while(remainder) {
    int status;
    status = read(id, pBuffer,remainder);
    if(status < 0) return status;
    if(status == 0) {		/* Broken pipe. */
      errno = EPIPE;
      return -1;
    }
    remainder -= status;
    pBuffer    += status;
  }
  return size;
}
#endif
#ifdef VMS
{
  IOSB iosb;
  int  stat;
  short func = IO$_READVBLK;	/* Initial guess at I/O function. */

  /* If the timeout is greater than zero, then we need to set up the
  ** cancel AST.  If the timeout is zero, then we need to 
  ** or in IO$M_NOW to the function code.
  */
  if(timeout > 0) {
    SetTimerAst(timeout,  CancelAst, id);
  }
  if(timeout == 0) {
    func = func | IO$M_NOW;
  }

  /* Now we start the QIOW off  in status handling, we treat 
  ** final statuses of SS$_ABORT or SS$_CANCEL as timeout AST's firing
  ** and hence normal completion with timeout.
  */

  stat = sys$qiow(0,(short)id, func, &iosb,
		  NULL, 0,
		  (int)buffer, size, 0,0,0,0);
  sys$cantim(id, 0);		/* Make sure AST's cancelled. */

  /* Now evaluate the status: */

  if( (stat & 1) ) {		/* QIO got started. */
    if( (iosb.status & 1) ) {	/* NOrmal completion. */
      return iosb.count;	/* Return transfer count. */
    }
    else if ( (iosb.status == SS$_CANCEL) ||
	      (iosb.status == SS$_ABORT))  { /* Timeout */
      return 0;
    }
    else {			/* Error in final I/O status */
      errno = EVMSERR;
      vaxc$errno = iosb.status;
      return -1;
    }
  }
  else {			/* Error in QIO initiation */
    errno = EVMSERR;
    vaxc$errno = stat;
    return -1;
  }


}
#endif

/*
** The functions on this page are VMS only functions which are needed
** to support the AEDTSK compatibility mode interface to Xamine.
** In particular we need to do event flag terminated reads
** of gates with an AST routine to reformat the data for 
** AEDTSK clients.
*/
#ifdef VMS
static struct vms_astdata{ struct dsc$descriptor  userbuf;
			   IOSB                   iostat;
			   msg_XamineEvent        event;
			   int                    eventflag;
			   IOSB                  *useriosb;
			   Astrtn		  ast;
			   void                  *astprm;
			 } GateReadData;
/*
** Functional Description:
**   ReformatAst:               - System dependent - 
**      This function is called at AST level on QIO completion
**      to reformat an object received from the newgates
**      mailbox to AEDTSK compatibility format.
** Formal Parameters:
**   struct vms_astdata *d:
**      Pointer to the ast data block shown above.
*/
static void
ReformatAst(struct vms_astdata *d)
{
  /* If the I/O status block is bad, then don't bother reformatting
  */
  if( d->iostat.status == SS$_NORMAL ) {
    msg_object *o;		/* Pointer to the msg object. */
    char       chunk[48];	/* Buffer for chunk of fmttedgate */
    int        size   = d->userbuf.dsc$w_length;  /* Remaining sz */
    char      *udata  = d->userbuf.dsc$a_pointer; /* User buffer. */
    char      *p;
    o                 = &(d->event.data.gate);

    /*
    ** If the event is a button press, that's not supported at present 
    */

    if(d->event.event != Gate) {
      d->iostat.status = SS$_BADPARAM;
    }
    else {
      memset(udata, 0, size);
      size--;			/* Ensure null termination  */
      o->name[20] = 0;		/* AEDTSK's limit. */
      p = &o->name[19];	        /* Blank fill name for AEDTSK clients. */
      while(p > o->name) {
	if(isgraph(*p) || isspace(*p)) {
	  break;
	}
	*p = ' '; p--;
      }
      
      /* first put in header data */
      
      strcpy(udata,o->name);
      size -= strlen(udata);
      sprintf(chunk, "%4d%4d%4d",
	      o->type, o->spectrum, o->npts);
      if(strlen(chunk) < size) {
	int ip;
	strcat(udata, chunk);
	size -= strlen(chunk);
	for(ip = 0; ip < o->npts; ip++) {	/* Fit all pts or what fits */
	  sprintf(chunk, "%5d%5d", 
		  o->points[ip].x, o->points[ip].y);
	  size -= strlen(chunk);
	  if(size > 0) {
	    strcat(udata, chunk);
	  }
	  else {
	    break;		/* Exit loop when no room. */
	  }
	}
	/* Modify all nulls to blanks to make f77 happy: */
	
	size = d->userbuf.dsc$w_length;
	udata= d->userbuf.dsc$a_pointer + (size-1);
	while( (size >= 0) && (*udata == 0)) {
	  *udata = ' ';
	  udata--;
	  size--;
	}
	d->iostat.count = size+1;	/* Set length to match fmtted len. */
      }
    }
  }
  /* Set the user's event flag to indicate completion:
   */
  sys$setef(d->eventflag);
}

/*
** Functional Description:
**   SMGQioAst:
**     This function is called at AST completion time for the QIO that
**     was initiated by f77xamine_queuegateio().  This is intended to
**     support SMAUG's direct $QIO interface to the AEDTSK mailbox.
** Formal Parameters:
**    struct vms_astdata *d:
**       Pointer to the AST data block.
*/
static void SMGQioAst(struct vms_astdata *d)
{
   /* The first bit of business is to reformat the data for the user buffer */

    ReformatAst(d);

  /* Now fill in the user's I/O status block if they supplied one: */

   if(d->useriosb) {
     memcpy(d->useriosb, &d->iostat, sizeof(IOSB));
   }

  /* Call the user's AST routine: */
 
   d->ast((int)d->astprm);
}

/*
** Functional Description:
**   f77xamine_queuegateio:
**      This function is supplied for SMAUG and SMAUG alone.  It's because
**      the idiot that wrote that program did not use the AED published 
**      interfaces like he should have and instead used the direct SYS$QIO/AST
**      method to get gates from the AEDTSK.  What I should do is just let 
**      him roast, but then people would scream without understanding what
**      their screaming about.  What I'll do instead is be nice and provide
**      a VMS only routine that does substantially what he was using the
**      $QIO for, and modify SMAUG to use this routine instead so it
**      goes 'back' to using only published interfaces.
**      What we do is fill in the vms_astdata structure with all sorts
**      of goodies and set up a $QIO of our own that will eventually fire our
**      own ast routine.  Our ast routine will reformat the data
**      and then fire off his AST routine.
** Formal Parameters:
**   short short *iosb:
**      Pointer to the user's I/O status block which will get copied at AST
**      time from the gate data's.
**   void *astrtn:
**      Pointer to the entry mask of the function the caller would like to
**      see invoked on I/O completion.
**   void *astarg:
**      Parameter that will be passed without interpretation to the user's
**      ast function.
**   struct dsc$descriptor *buffer:
**      Pointer to a character string descriptor which describes the buffer
**      that will be filled in with the AEDTSK formatted data.
** Returns:
**   $QIO status.
*/
int f77xamine_queuegateio(short *iosb, void *astrtn, void *astarg,
			  struct dsc$descriptor *buffer)
{
    /* First fill in the GateReadData AST block:  */

    memset(&GateReadData , 0, sizeof(struct vms_astdata));
    memcpy(&GateReadData.userbuf, buffer, sizeof(struct dsc$descriptor));
    GateReadData.eventflag = 0;	    /* Use the 'default event flag' */
    GateReadData.useriosb  = (IOSB *)iosb;
    GateReadData.ast       = (Astrtn)astrtn;
    GateReadData.astprm    = astarg;

    /* Now issue the QIO on the mailbox   */

    return sys$qio(0, newgates, IO$_READVBLK, &GateReadData.iostat,
                   (Astrtn)SMGQioAst, (int)&GateReadData,
		   (int)&GateReadData.event, sizeof(msg_XamineEvent),
		   0,0,0,0);
}
/*
** Functional Description:
**   f77xamine_cancel:
**      Cancel pending input.
** Formal Parameters:
**   int *can:
**      Status of the cancel request.
*/
void f77xamine_cancel(int *can)
{
  *can = sys$cancel(newgates);
}
/*
** Functional Description:
**   Xamine_Wait:                   - System dependent - 
**    Waits for I/O completion of Xamine_Read:
** Formal Parameters:
**    int *isize:
**      Returns number of characters finally formatted.
** Returns:
**    status from final I/O status block.
*/
int 
Xamine_Wait(int *isize)
{
  sys$synch(GateReadData.eventflag, &GateReadData.iostat);
  *isize = GateReadData.iostat.count;
  return GateReadData.iostat.status;
}
/*
** Functional Description:
**  Xamine_Read                        - System dependent -
**    This function queues a possibly non blocking read on the 
**    gates mailbox.  We set up a buffer reformatting AST to fire
**    when the read completes.
** Formal Parameters:
**    struct dsc$descriptor *userbuf:
**      The user's buffer.
**    int eventflag:
**      An event flag that will be set when the data is 
**      formatted properly.  Note the AST routine sets this
**      rather than the QIO and all QIO's use eventflag 0 to finish
** Returns:
**    VMS status code of operation.
*/
int 
Xamine_Read(struct dsc$descriptor *userbuf, int eventflag)
{
  int stat;
  /*
  ** First fill in the GateReadData block:
  */
  memset(&GateReadData, 0, sizeof(struct vms_astdata));
  memcpy(&GateReadData.userbuf, userbuf, sizeof(struct dsc$descriptor));
  GateReadData.eventflag = eventflag;

  /*
  ** Now start the read off.  If the event flag was zero, then
  ** we'll also synch on it afterwards:
  */

  stat = sys$qio(0, newgates, IO$_READVBLK, &GateReadData.iostat,
		 (Astrtn)ReformatAst,  (int)&GateReadData, 
		 (int)&GateReadData.event, sizeof(msg_XamineEvent),
		 0,0,0,0);
  if( (stat & 1) && (eventflag == 0) ) {
    sys$synch(eventflag, &GateReadData.iostat);
    return GateReadData.iostat.status;
  }
  return stat;
}
#endif

static char reqname[NAMELEN];
static char ackname[NAMELEN];
static char outname[NAMELEN];

/*
** Functional Description:
**  DeletePipes:
**   Exit handler called in the Unix environment to clean up the pipes:
*/
#ifdef unix
static void DeletePipes()
{
  unlink(reqname);
  unlink(ackname);
  unlink(outname);
}
#endif
/*
** Functional Description:
**   Xamine_OpenPipes:
**      This function is called by the Xamine client initialization
**      software.  It opens/creates pipes for the gates 
**      notification pipes, and the request/acknowledget pipes.
**      Unix requires the pipes be open, and listening, 
**      when Xamine starts up in order to prevent race conditions.
**      once Xamine is started, its connections can be accepted
**     with Xamine_AcceptPipeConnections.
**
** Returns:
**    1 - Successful operation.
**    0 - Failed with error in errno etc.
*/
int
Xamine_OpenPipes()
{
  /* First build up the complete names of the gates.
  ** We encode our process id into them for uniqueness.
  */
  char suffix[NAMELEN];
  pid_t pid;

  pid  = getpid();

  strcpy(reqname, PREFIX);
  sprintf(suffix, REQ_NAME, pid);
  strcat(reqname, suffix);

  strcpy(ackname, PREFIX);
  sprintf(suffix, ACK_NAME, pid);
  strcat(ackname, suffix);

  strcpy(outname, PREFIX);
  sprintf(suffix, OUT_NAME, pid);
  strcat(outname, suffix);

  /* Now create the pipes with the appropriate accesses: */

  requests = MakePipe(reqname, O_WRONLY);
  if(requests < 0) return 0;

  acks     = MakePipe(ackname, O_RDONLY);
  if(acks < 0) return 0;

  newgates = MakePipe(outname, O_RDONLY);
  if(newgates < 0) return 0;

#ifdef unix
  atexit(DeletePipes);
#endif
  return 1;
  
}
#ifdef unix
/*
**  Accept Xamine's connections on all the pipes:
**  The pipe fds' are now the listening fds..  they'll be replaced by
**  the accepted connection fds.
*/
int Xamine_AcceptPipeConnections()
{
  requests = AcceptConnection(reqname, requests);
  if(requests < 0) {
    perror("Accept failed");
    return 0;
  }

  acks    = AcceptConnection(ackname, acks);
  if(acks < 0) {
    perror("Accept failed");
    return 0;
  }

  newgates = AcceptConnection(outname, newgates);
  if(newgates < 0) {
    perror("Accept failed");
    return 0;
  }
  return 1;
 
}

#endif
void Xamine_Closepipes()
{
  close(requests);
  close(acks);
  close(newgates);

  DeletePipes();
}
/*
** All the code in the following pages is completely system
** independent.  It is built on top of the system dependent code
** above and can therefore be safely called in any operating system
** environment.
*/

/*
** Functional Description:
**   Xamine_PollForGate:
**     This function does a partially blocking check for newgates.
**     A timeout parameter controls the extent of the block.
** Formal Parameters:
**   int timeout:
**     >= 0   - Number of seconds to block where 0 measn immediate
**              return/poll.
**      < 0   - Block as long as necessary.
**   int *spectrum:
**      Returns the spectrum number on which the gate was accepted.
**   Xamine_gatetype *typ:
**      Returns the type of the gate accepted.
**   char *name:
**      Returns the name of the gate.
**   int *npts:
**      Returns the number of points.
**   Xamine_Point *pts:
**      Returns the points of the gate.
** Returns:
**   >0  - Success, gate read in (number of bytes read).
**   0  - Timed out.
**  -1  - Failed with error in errno etc.
*/
int
Xamine_PollForGate(int timeout, int *spectrum, Xamine_gatetype *type,
		   char *name, int *npts, Xamine_point *pts)
{
  int p;
  int status;
  msg_XamineEvent event;
  msg_object object;
  /*
  ** First read the data:
  */
  status = ReadPipe(newgates, timeout, 
		    &event, sizeof(msg_XamineEvent));
  if(status <= 0) return status;

  if(event.event != Gate) {
    errno = EINVAL;
    return -1;
  }
  memcpy(&object, &(event.data.gate), sizeof(msg_object));

  /* If data was read, then unpack it into the user's parameter list
  */

  *spectrum = object.spectrum;
  *type     = object.type;
  *name     = 0;
  if( object.hasname ) strcpy(name, object.name);
  *npts     = object.npts;

  for(p = 0; p < object.npts; p++) {
    pts[p].x   = object.points[p].x;
    pts[p].y   = object.points[p].y;
  }

  return status;
}

/*
** Functional Description:
**   Xamine_ReadGate:
**     Performs a blocking read of a gate.
**     Essentially we call Xamine_PollForGate but with a timeout
**     that blocks forever.
** Formal Parameters:
**    As for Xamine_PollForGate above, but no timeout param.
*/
int
Xamine_ReadGate(int *spectrum, Xamine_gatetype *type, char *name,
		int *npts, Xamine_point *pts)
{
  return Xamine_PollForGate(-1, spectrum, type, name, npts, pts);
}

/*
** Functional Description:
**   Xamine_PollForEvent:
**     This function polls for an Xamine event. An Xamine event is an
**     abstract concept of something happening in Xamine that a client
**     program might be interested in hearing about.  Examples of Xamine
**     events are user's accepting gates or thwacking client buttons.
** Formal Parameters:
**   int timeout:
**     A timeout value for the poll where:
**     >= 0 means the number of seconds to block waiting for input and
**     <  0 means block until an event comes in.
**   msg_XamineEvent *event:
**     A buffer provided by the user to hold the event data.
** Returns:
**    > 0   Success, the gate was read (number of bytes read).
**      0   Poll timed out with no events.
**    -1    Poll failed in some serious way with error in errno
*/
int Xamine_PollForEvent(int timeout, void *event)
{

  return ReadPipe(newgates, timeout, event, 
		  sizeof(msg_XamineEvent));

}
/* Blocking version... waits forever for the event... */

int Xamine_ReadEvent(void *event)
{
  return ReadPipe(newgates, -1, event, 
		  sizeof(msg_XamineEvent));
}


/* Fortran bindings for above: */

int
#ifdef VMS
f77xamine_pollforevent
#else
f77xamine_pollforevent_
#endif
                     (int *timeout, void *event)
{
  return Xamine_PollForEvent(*timeout, event);
}

int
#ifdef VMS
f77xamine_readevent
#else
f77xamine_readevent_
#endif
                    (void *event)
{
  return Xamine_ReadEvent(event);
}

/*
** The functions on this page allow the user to request a list of
** gates applied to a spectrum and to iterate through them.
** Iteration is managed by a context block which includes the set
** of gates a count of gates and the current position.  
** The form of this context block is show below:
*/
typedef struct {  int numgates;
		  int current;
		  msg_object *objects;
		} GateContext, *GateContextPtr;

/*
** Functional Description:
**   Xamine_StartSearch:
**     This function begins searching the set of gates applied to 
**     a spectrum.  Xamine is sent a request message for the list
**     of gates.  The gates are read into a context block and
**     the pointer to this context block is returned to the caller.
**     The context block is dynamically allocated and 
**     therefore the user must call Xamine_EndSearch to
**     release this storage to prevent systemic memory leaks.
** Formal Parameters:
**    int spectrum:
**      The sepctrum about which information is requested.
**    int *status:
**      Will contain the status from Xamine of the request.
**      Note that unless the value of this is Success (0), the
**      return context value is useless.
** Returns:
**    integerized (cast) pointer to the a GateContext or NULL
**    if there's a problem.
*/
long
Xamine_StartSearch(int spectrum, int *status)
{
  struct msg_command    cmd = { Inquire }; /* Command msg packet. */
  struct msg_inquiremsg data;	/* Inquire data. */
  struct msg_inquireack iack;	/* Ack to expect from inquire. */
  GateContextPtr        ctx;	/* Context pointer            */
  int    i;

  /* First we transact with Xamine to attempt to get it to ship
  ** us a gate list for the requested spectrum.
  */
  data.spectrum = spectrum;
  if(!Xamine_Alive()) {		/* If Xamine is dead, no point. */
    *status = (int)NoProcess;
    return 0;
  }
  if(WritePipe(requests, &cmd, sizeof(struct msg_command)) < 0) {
    *status = (int)CheckErrno;
    return 0;
  }
  if(WritePipe(requests, &data, sizeof(struct msg_inquiremsg)) < 0) {
    *status = (int)CheckErrno;
    return 0;
  }
  if(ReadPipe(acks, -1, &iack, sizeof(struct msg_inquireack)) <= 0) {
    *status = (int)CheckErrno;
    return 0;
  }
  /* Now we have the acknowlege message.  If it's status field does 
  ** not indicate success, then we must return a failure without
  ** attempting to read gate records which won't be comming.
  */
  if(iack.status != Success) {
    *status = iack.status;
    return 0;
  }
  /* Allocate storage for the context and gates array.  If we fail,
  ** then we'll use the CheckErrno status to flag that.
  */
  ctx = (GateContextPtr)malloc(sizeof(GateContext));
  if(ctx == (GateContextPtr)NULL) {
    *status = CheckErrno;
    return 0;
  }
  /* If there are no objects, then return NoSuchObject status: */
  if(iack.nobjects == 0) {
    *status = NoSuchObject;
    return 0;
  }
  ctx->objects = (msg_object *)
                     malloc(sizeof(msg_object)*iack.nobjects);
  if(ctx->objects == (msg_object *)NULL) {
    *status = CheckErrno;
    free(ctx);
    return 0;
  }
  /* Initialize the remainder of the context block.    */

  ctx->numgates = iack.nobjects;
  ctx->current  = 0;

  /* Now read the gates from Xamine throught the ack pipe into
  ** the buffers provided by ctx->objects.
  */
  for(i = 0; i < ctx->numgates; i++) {
    if(ReadPipe(acks, -1, &ctx->objects[i], 
		sizeof(msg_object)) < 0) {
      *status = CheckErrno;
      free(ctx->objects);
      free(ctx);
      return 0;
    }
  }
  *status = Success;
  return (long)ctx;
}

/*
** Functional Description:
**   Xamine_NextGate:
**     Returns iformation about the next gate in the context list.
** Formal Parameters:
**   int context:
**     Really a GateContextPtr in disguise.
**   int *id:
**     Will hold the gate id if the next gate if there is one.
**   Xamine_gatetype *type:
**     Will hold the type of the gate if there is a next one.
**   int *npts:
**     Will hold the number of points in the next gate.
**   Xamine_point *pts:
**     Will hold the set of points that make up the gate if there is
**     a next one.
**  Returns:
**    1  - If there is a next gate or
**    0  - If there is no next gate.
*/
int Xamine_NextGate(long context, int *id, Xamine_gatetype *type,
		    int *npts, Xamine_point *pts)
{
  GateContextPtr ctx = (GateContextPtr)context;
  int i;

  if(context == 0)		/* If Xamine_StartSearch yielded a failure... */
    return 0;			/* simulate end of gate list, avoid crash   */

  if(ctx->current >= ctx->numgates) return 0; /* No free gates. */

  /* Copy the fixed size part of the gate over: */

  *id    = ctx->objects[ctx->current].id;
  *type  = ctx->objects[ctx->current].type;
  *npts  = ctx->objects[ctx->current].npts;
  /*
  ** Copy the individual points to the user.
  */
  for(i = 0; i < *npts; i++) {
    pts[i].x = ctx->objects[ctx->current].points[i].x;
    pts[i].y = ctx->objects[ctx->current].points[i].y;
  }

  ctx->current++;		/* Make next gate current.  */
  return 1;

}

/*
** Functional Description:
**   Xamine_EndSearch:
**     Terminates a search of a list of gate set on a spectrum.
** Formal Parameters:
**   int context:
**      Really a GatesContextPtr in disguise.
*/
void
Xamine_EndSearch(long context)
{
  GateContextPtr ctx = (GateContextPtr)context;
  if(context == 0) {
    return;			/* Avoid crash of client on stupidity. */
  }

  free(ctx->objects);
  free(ctx);
}

/*
** Funtional Description:
**   Xamine_RemoveGate:
**     This function removes a gate from the Xamine gate store.
** Formal Parameters:
**   int spec:
**     The spectrum from which the gate is to be removed.
**   int id:
**     The identifier of the gate.
**   Xamine_gatetype ntype:
**     The type of the gate to remove.
** Returns:
**   Status from the Acknowledge message.  This is 0 for Success
**   and negative for failures.
*/
int
Xamine_RemoveGate(int spec, int id, Xamine_gatetype ntype)
{
  struct msg_command cmd = { Remove };
  struct msg_remove  rmv;
  struct msg_rmvack  rmvack;

  /* Format the removal data packet.  */

  rmv.spectrum = spec;
  rmv.object   = id;
  rmv.type     = ntype;

  /* Make sure that Xamine is still alive and if it is then conspire
  ** with it to remove the gate.
  */
  if(!Xamine_Alive()) {
    return NoProcess;
  }
  if(WritePipe(requests, &cmd, sizeof(struct msg_command)) < 0) {
    return CheckErrno;
  }
  if(WritePipe(requests, &rmv, sizeof(struct msg_remove)) < 0) {
    return CheckErrno;
  }
  if(ReadPipe(acks, -1, &rmvack, sizeof(struct msg_rmvack)) < 0) {
    return CheckErrno;
  }
  return rmvack.status;
}

/*
** Functional Description:
**  Xamine_EnterGate:
**    This function asks Xamine to enter a gate in it's gate 
**    database.  Gates in Xamine are accepted, handed to the client
**    and then forgotten.  The client if it desires can hand the
**    gate back to Xamine for display.
** Formal Parameters:
**   int spec:
**     Spectrum on which the gate should be entered.
**   int id:
**     A unique identifier for the gate object.
**   Xamine_gatetype type:
**     The type of the gate to delete.
**   char *name:
**     The optional name of the gate.  If this pointer is NULL,
**     gates will be entered without a name.
**   int npts:
**     Number of points in the gate.
**   Xamine_point *pts:
**     The array of npts points in the gate.
** Returns:
**   Status from the transaction which is 0 for success and negative
**   for failures.
*/
int
Xamine_EnterGate(int spec, int id, Xamine_gatetype type, char *name,
		 int npts, Xamine_point *pts)
{
  struct msg_command  cmd = { Enter };
  msg_object   obj;
  struct msg_enterack eack;
  int    i;

  /* First format the object request message: */

  obj.spectrum = spec;
  obj.id       = id;
  obj.type     = type;
  if(name != (char *)NULL) {
    obj.hasname = TRUE;
    memset(obj.name, 0, sizeof(grobj_name));
    strncpy(obj.name, name, sizeof(grobj_name) - 1);
  }
  else {
    obj.hasname = FALSE;
  }
  obj.npts = npts;
  for(i = 0; i < npts; i++) {
    obj.points[i].x = pts[i].x;
    obj.points[i].y = pts[i].y;
  }
  /* Next if Xamine is still alive interact with it to put a gate
  ** across:
  */
  if(!Xamine_Alive()) {
    return NoProcess;
  }
  if(WritePipe(requests, &cmd, sizeof(struct msg_command)) < 0) {
    return CheckErrno;
  }
  if(WritePipe(requests, &obj, sizeof(msg_object)) < 0) {
    return CheckErrno;
  }
  if(ReadPipe(acks, -1, &eack, sizeof(struct msg_enterack)) < 0) {
    return CheckErrno;
  }
  return eack.status;
}

/*
** The functions that follow comprise the Fortran native bindings to
** the exported entry points in this file.  There are system dependencies
** for two reasons:
**   1) argument passing conventions differ as do naming conventions
**   2) There are some functions which only exist in VMS.
*/
/*
** The first set of fortran jackets is the VMS only functions. 
*/
#ifdef VMS

int f77xamine_wait(int *isize)
{
  return Xamine_Wait(isize);
}
int f77xamine_read(struct dsc$descriptor *userbuf, int *eventflag)
{
  return Xamine_Read(userbuf, *eventflag);
}
#endif
/*
** The following bindings are available for both VMS and Unix caller.
*/
#ifdef unix
int f77xamine_pollforgate_(int *timeout, int *spectrum, Xamine_gatetype *type,
			   char *name, int *npts, Xamine_point *pts)
{
  return Xamine_PollForGate(*timeout, spectrum, type, name, npts, pts);
}
#endif
#ifdef vms
int f77xamine_pollforgate(int *timeout, int *spectrum, Xamine_gatetype *type,
			  struct dsc$descriptor *name, int *npts,
			  Xamine_point *pts)
{
  int status;
  status = Xamine_PollForGate(*timeout, spectrum, type,
			      name->dsc$a_pointer, npts, pts);
  if(status > 0) {
    int l = strlen(name->dsc$a_pointer);
    int i;
    for(i = l; i < name->dsc$w_length; i++) 
      name->dsc$a_pointer[i] = ' ';        /* Blank fill name. */
  }
  return status;
}

#endif
#ifdef unix
int f77xamine_readgate_(int *spectrum, Xamine_gatetype *type, char *name,
			int *npts, Xamine_point *pts)
{
  return Xamine_ReadGate(spectrum, type, name, npts, pts);
}
#endif
#ifdef VMS
int f77xamine_readgate(int *spectrum,  Xamine_gatetype *type, 
		       struct dsc$descriptor *name,
		       int *npts, Xamine_point *pts)
{
  int to = -1;
  return f77xamine_pollforgate(&to, spectrum, type, name, npts, pts);
}
#endif

void 
#ifdef unix
  f77xamine_endsearch_
#endif
#ifdef VMS
  f77xamine_endsearch
#endif
  (long *context)
{
  Xamine_EndSearch(*context);
}

#ifdef unix
int f77xamine_nextgate_
#endif
#ifdef VMS
int f77xamine_nextgate
#endif
  (long *context, int *id, Xamine_gatetype *type, 
   int *npts, Xamine_point *pts)
{
  return Xamine_NextGate(*context, id, type, npts, pts);
}

long
#ifdef unix
f77xamine_startsearch_
#endif
#ifdef VMS
f77xamine_startsearch
#endif
  (int *spec, int *status)
{
  return Xamine_StartSearch(*spec, status);
}

#ifdef unix
int f77xamine_entergate_(int *spec, int *id, Xamine_gatetype *type,
			 char *name, int *npts, Xamine_point *pts)
{
  return Xamine_EnterGate(*spec, *id, *type,
			  name, *npts, pts);
}
#endif
#ifdef VMS
int f77xamine_entergate(int *spec, int *id, Xamine_gatetype *type,
			struct dsc$descriptor *name, int *npts, 
			Xamine_point *pts)
{
  char *n;
  int   i;

  if(name == NULL)		/* No name present... */
    return Xamine_EnterGate(*spec, *id, *type, NULL, *npts, pts);

  n = malloc(name->dsc$w_length+1);
  if(n == NULL)
    return CheckErrno;

  memset(n, 0, name->dsc$w_length + 1);
  memcpy(n, name->dsc$a_pointer, name->dsc$w_length);

  for(i = name->dsc$w_length - 1; i >= 0; i--) {
    if(n[i] == ' ') {
      n[i] = 0;
    }
    else {
      break;
    }
  }

  i = Xamine_EnterGate(*spec, *id, *type, n, *npts, pts);
  free(n);
  return i;
}
#endif
int
#ifdef unix
f77xamine_removegate_
#endif
#ifdef VMS
f77xamine_removegate
#endif
  (int *spec, int *id, Xamine_gatetype *ntype)
{
  return Xamine_RemoveGate(*spec, *id, *ntype);
}


#ifdef VMS
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      Xamine_QueueEventRead:
**        This is a *VMS* only function which allows an event read to be 
**        queued with event flags and optional AST routines fired on the
**        read completion.  
**
**  FORMAL PARAMETERS:
**
**      void *usrbuf:
**            Event buffer which will hold the Xamine event that was read.
**      int eventflag:
**            Number of the event flag to set on read completion.
**      void *astrtn:
**           Pointer to the optional AST routine (NULL if no AST).
**      void *astdata:
**           Pointer to argument sent in to AST routine (NULL If none).
**      short *iosb:
**           Pointer to a user's I/O status block or null if none desired.
**
**
**  RETURN VALUE:
**
**      Return value from the VMS $qio.
**
**
**--
*/

int Xamine_QueueEventRead (void *usrbuf, int eventflag, void *astrtn, 
			   void *astdata, short *iosb)
{

    return sys$qio(0, newgates, IO$_READVBLK,
		   iosb, (Astrtn)astrtn, (int)astdata,
		   (int *)usrbuf, sizeof(msg_XamineEvent),
		   0,0,0,0);
}

int f77xamine_queueeventread(void *usrbuf, int *eventflag, void *astrtn,
			    void *astdata, short *iosb)
{
    return Xamine_QueueEventRead(usrbuf, *eventflag, astrtn, astdata, iosb);
}

#endif

/*
** Functional Description:
**    Xamine_EnterPeakMarker:
**      This function enters a peak marker graphical object into the gate 
**      database.  A peak marker illustrates the position and width of a peak.
**  Formal Parameters:
**     int nSpectrum:
**           The number of the spectrum on which this marker will be entered.
**     int nId:
**           A unique integer which will be used to identify this marker.
**           The ID is used to identify the object in later calls to the API.
**     grobj_name szName:
**           The name/comment attached to the object in the INFO field.
**     float fCentroid:
**           Floating point channel number at which the peak marker centroid
**           will be located.
**     float fWidth:
**           Floating point peak width (in channels).
** Returns:
**    0   - Success,
**   <0   - Failure:
**                NoProcess  - Xamine isn't running
**                CheckErrno - Some C runtime error occured, look at errno
**                             for the true failure reason.
*/

int Xamine_EnterPeakMarker(int nSpectrum, int nId,
			  grobj_name szName,
			  float      fCentroid,
			  float      fWidth)
{
  struct msg_command cmd = { EnterPeakPosition };
  msg_peakentry      pk;
  struct             msg_enterack eack;

  /* First format the requst:  */

  pk.nSpectrum = nSpectrum;
  pk.nId       = nId;
  if(szName) {
    pk.nHasName = TRUE;
    strcpy(pk.szName, szName);
  }
  else
    pk.nHasName = FALSE;
  pk.fCentroid = fCentroid;
  pk.fWidth = fWidth;

  /* If Xamine is still alive, then interact with it to try to set the peak
  ** position:
  */
  if(!Xamine_Alive()) 
    return NoProcess;

  if(WritePipe(requests, &cmd, sizeof(struct msg_command)) < 0) {
    return CheckErrno;
  }
  if(WritePipe(requests, &pk, sizeof(pk)) < 0) {
    return CheckErrno;
  }
  /* Read the status value: */

  if(ReadPipe(acks, -1, &eack, sizeof(struct msg_enterack)) < 0)
    return CheckErrno;

  return eack.status;

}
/*
** Fortran bindings to this function just marshall the parameters and call
** Xamine_EnterPeakMarker; note that there are both f77 and c version.
*/

int
#ifdef VMS
f77xamine_enterpeakmarker(int* nSpectrum,
			  int* nId,
			  struct dsc$descriptor* dscName,
#else
f77xamine_enterpeakmarker_(int* nSpectrum,
			  int* nId,
			  char* szName,
		       
#endif
			  float* fCentroid,
			  float* fWidth
#ifndef VMS
			  , int nNameLength
#endif
                          )
{
  char* szNameString;		/* This will be what actually is passed to */
  int   i;
  /*
  ** Setup szNameString so that it has a null terminated string which is
  ** the name of the peak position indicator.
  */

#ifdef VMS			/* the C bindings... */
  szNameString = malloc(dscName->dsc$w_length + 1);
  memset(szNameString, 0, dscName->dsc$w_length+1);
  strncpy(szNameString, dscName->dsc$a_pointer, dscName->dsc$w_length);
#else
  szNameString = malloc(nNameLength+1);
  memset(szNameString, 0, nNameLength+1);
  strncpy(szNameString, szName, nNameLength);
#endif

  /* Trim off trailing blanks to make printing nicer.. since fortran blank pads
  ** strings:
  */
  for(i = strlen(szNameString); i >= 0; i--) {
    if(szNameString[i] == ' ') szNameString[i] = '\0';
    else break;
  }
  /*
  ** Now we can call the C binding:
  */
  i = Xamine_EnterPeakMarker(*nSpectrum, *nId,
			     szNameString, *fCentroid, *fWidth);

  /* Deallocate the title string memory and return the status from the
  ** C bindings call:
  */
  free(szNameString);
  return i;
}












