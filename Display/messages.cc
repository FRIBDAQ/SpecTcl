/*
** Facility:
**   Xamine -- NSCL display program.
** Abstract:
**    messages.cc:
**       This file contains implementations of the class methods defined
**       in messages.h.  These class methods allow the Xamine client
**       to interact with Xamine to modify the graphical object set that
**       Xamine has and for Xamine to notify the histogrammer of
**       gate data that has been accepted by the user.
**       The classes we define are:
**         MessageQueue   - A generic queue based interprocessor
**                          communication scheme.  The queue
**                          is capable of producing Xt callbacks.
**                          ** This class is operating system dependent **
**         GatesOutQueue  - Manages the queue of gate data from Xamine
**                          to the client.
**         RequestMsgQueue- Manages Xamine's service request queue.
**         ReplyMsgQueue  - Manages the message queue which Xamine
**                          uses to acknowledge service requests.
**         ClientRequests - Manages both a RequestMsgQueue and a ReplyMsgQueue
**                          in such a way as to present a coherent request/
**                          response behavior.
**  Author:
**    Ron Fox
**    NSCL
**    Michigan State University
**    East Lansing, MI 48824-1321
*/
static char *sccsinfo="@(#)messages.cc	1.1 1/28/94 ";


/*
** Include files:
*/
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

#ifdef unix
#include <sys/socket.h>
#ifndef CYGWIN
#include <sys/un.h>
#endif
#ifdef ultrix
#include <sys/file.h>
#endif
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#ifndef O_NONBLOCK
#define O_NONBLOCK O_NDELAY	/* This is the 'posix' draft name. */
#endif
#endif
#include <Xm/Xm.h>
#ifdef VMS
#include <unixio.h>
#include <ssdef.h>
#include <iodef.h>
#include <descrip.h>
#include "eventflags.h"
#endif

#include "messages.h"

/*
** VMS function prototypes:
*/
#ifdef VMS
extern "C" {
  int sys$crembx(int prmflg, short *chan, int maxmsg, int bufquo,
		 int promask, int acmode, dsc$descriptor *logname);
  int sys$dassgn(short channel);
  int sys$qio(int efn, short chan, short function,
	      short *iosb, void *astrtn, void *astprm, 
	      int p1, int p2, int p3, int p4, int p5, int p6);
  int sys$qiow(int efn, short chan, short function,
	       short *iosb, void *astrtn, void *astprm, 
	       int p1, int p2, int p3, int p4, int p5, int p6);
  int sys$setef(int efn);
  int sys$clref(int efn);
  int sys$waitfr(int efn);
  int sys$setimr(int efn, int *daytim, void *astadr, int *reqidt);
  int sys$bintim(struct dsc$descriptor *timbuf, int *timadr);
  int sys$cantim(int reqidt, int acmode);
  int sys$cancel(short chan);
  int sys$synch(int ef, short *iosb);
  void lib$signal(int status);
  void exit(int stat);
}

#ifndef NODEBUG
#define vmsassert(state) assertvms(state, __FILE__, __LINE__)
#else
#define vmsassert(state)
#endif
inline void assertvms(int state, char *module, int line)
{
  if(! (state & 1)) {
     fprintf(stderr, "Unsuccessful VMS status code in %s line %d\n",
	     module, line);
     lib$signal(state);
     exit(state);
  }
}
#endif
#ifdef unix
extern "C" {
  int select(int width, fd_set *readfds, 
	     fd_set *writefds, fd_set *exceptfds, 
	     timeval *timeout);
#ifdef ultrix
  int open(char *path, int flags, int mode);
  int fcntl(int fd, int request, int arg);
  void bzero(void *b1, int length);
#endif
}
#endif

#ifdef VMS
/*
**  The functions on this page are needed only for VMS systems:
*/
/* Functional Description:
**   timerast:
**     Set a timer AST to fire after a given number of seconds.
** Formal Parameters:
**   int seconds:
**       Number of seconds after which to notch the timeout.
**   void *rtn:
**       AST completion routine.
**   int astarg:
**       AST argument.
** Returns:
**   The ID associated with the timer.  assert fails on failure.
*/
int timerast(int seconds, void *rtn, int astarg)
{
  char time[80];		/* ASCII time */
  int  tq[2];			/* Time quadword. */
  int d,h,m,s;			/* components of time. */
  struct dsc$descriptor tdesc;

  s       = seconds % 60;
  m       = seconds / 60;
  h       = m / 60;
  d       = h / 24;

  m       = m % 60;
  h       = h % 24;

  /* Build the delta time string and its descriptor:   */

  sprintf(time, "%d %d:%02d:%02d.0", d,h,m,s);
  tdesc.dsc$a_pointer = time;
  tdesc.dsc$w_length  = strlen(time);
  tdesc.dsc$b_dtype   = DSC$K_DTYPE_T;
  tdesc.dsc$b_class   = DSC$K_CLASS_S;
  vmsassert(sys$bintim(&tdesc, tq));

  /* Set the timer AST and return the id (which is just astarg):   */

  vmsassert(sys$setimr(0, tq, rtn, (int *)astarg));
  return astarg;
  
}
/*
** Functional Description:
**   canceltimerast:
**    Cancels a timer given the id:
** Formal Parameters:
**   int id:
**     Ident of the timer.
**  assert fails if $cantim fails.
*/
void canceltimerast(int id)
{
  vmsassert(sys$cantim(id, 0));
}
#endif

/*
** Functional Description:
**    TriggerXEvent:
**      This function is an AST callback function used only by the VMS
**      implementation.  It transforms the attention AST into an event flag
**      set followed by setting another attention AST.
** Formal Parameters:
**   MessageQueue *object:
**     Points to the object for which this ast fired.
*/
#ifdef VMS
void
TriggerXEvent(MessageQueue *object)
{

  if(object->XtNotificationOn) {	/* Just ignore if he turned it off. */
    vmsassert(sys$setef(object->getef())); /* Causes the XtInputEvent to fire. */
  }
}
#endif

/*
** The pages below implement the MessageQueue.  On Unix systems this is
** implemented using named pipes (POSIX FIFO's if you will).
** On VMS the implementation is based on VMS Mailboxes.
*/

/*
** Functional Description:
**   MessageQueue::MessageQueue:
**     This function constructs a message queue.  It is entirely 
**     system dependent.
** Formal Parameters:
**    char *name:
**       Name of the message queue.
*/
MessageQueue::MessageQueue(char *name, int flags)
#ifdef unix
{
  /* A message queue is an AF_UNIX socket.  We are a client for each of the
  ** sockets
  */
  struct sockaddr_un peer;
  fid = socket(AF_UNIX, SOCK_STREAM,  0);
  assert(fid >= 0);
  peer.sun_family = AF_UNIX;
  strcpy(peer.sun_path, name);
  if(connect(fid, (struct sockaddr*)&peer, sizeof(struct sockaddr_un)) < 0) {
    char msg[100];
    sprintf(msg, "Failed to connect to %s", name);
    perror(msg);
    exit(-1);
  }


  XtNotificationOn = 0;

}
#endif
#ifdef VMS
{
  dsc$descriptor mbxname;

  /* construct the logical name descriptor: */

  mbxname.dsc$a_pointer = name;
  mbxname.dsc$w_length  = strlen(name);
  mbxname.dsc$b_dtype   = DSC$K_DTYPE_T;
  mbxname.dsc$b_class   = DSC$K_CLASS_S;

  /* Create the mailbox: */

  int status = sys$crembx(0, &chan, 
			sizeof(msg_object)*2, sizeof(msg_object)*2,
			0xff00, 0, &mbxname);
  vmsassert(status);

  /* X Notification starts off.  */

  XtNotificationOn = 0;
}
#endif

/*
** Functional Description:
**   MessageQueue::~MessageQueue:
**     This method is the destructor for a message queue.
**     our action is to remove any XtInput callback that was set and
**     close the device.
**      SYSTEM SPECIFIC CODE
*/
MessageQueue::~MessageQueue()
#ifdef unix
{
  if(XtNotificationOn) XtRemoveInput(XtInputHandlerId);
  
  close(fid);
}
#endif
#ifdef VMS
{
  if(XtNotificationOn) {
    XtRemoveInput(XtInputHandlerId);
    freeef(efn);
  }
  
  vmsassert(sys$dassgn(chan));
}
#endif

/*
** Functional Description:
**   MessageQueue::write:
**     Blocking write of a message to a message queue.  Blocking means that
**     if the reader does not have a read active you may block until the
**     reader issues it's read.  You also may not... it's operating system
**     dependent.
** Formal Parameters:
**     void *buffer:
**        Data to write.
**     int bytes:
**        Number of databytes to write.
** Returns:
**    # bytes written or -1 if failed.
**      Failure with the error info in errno.
*/
int
 MessageQueue::write(void *buffer, unsigned int bytes)
#ifdef unix
{
  return ::write(fid, buffer, bytes);
}
#endif
#ifdef VMS
{
  int istat = sys$qiow(0, chan, IO$_WRITEVBLK, iosb, 
		       0, 0, (int)buffer, bytes,0,0,0,0);
  assert(istat);
  assert(iosb[0]);
  if(istat != SS$_NORMAL) {
    errno = EVMSERR;
    vaxc$errno = istat;
    return -1;
  }
  if(iosb[0] != SS$_NORMAL) {
    errno = EVMSERR;
    vaxc$errno = istat;
    return -1;
  }
  return iosb[1];
}
#endif

/*
** Functional Description:
**    MessageQueue::read:
**      This method does a blocking read on the message queue.
**      Blocking read means that we don't return until the data
**      has been made available to us.
** Formal Parameters:
**    void *buf:
**      Points to the data to read.
**    int bytes:
**      Number of bytes to read.
** Returns:
**    Number of bytes read or -1 if failed.
*/
int 
 MessageQueue::read(void *buf, unsigned int bytes)
#ifdef unix
{
  return ::read(fid, buf, bytes);
}
#endif
#ifdef VMS
{
  int istat = sys$qiow(0, chan, IO$_READVBLK,iosb,
		       0,0,(int)buf,bytes,0,0,0,0);
  if(istat != SS$_NORMAL) {
    errno =EVMSERR;
    vaxc$errno = istat;
    return -1;
  }
  if(iosb[0] != SS$_NORMAL) {
    errno = EVMSERR;
    vaxc$errno = iosb[0];
    return -1;
  }
  if(XtNotificationOn) {
    vmsassert(sys$clref(efn) ); /* Clear the event flag.  */
  }
  return iosb[1];
}
#endif

/*
** Functional Description:
**    MessageQueue::AddXtCallback:
**      This function tells the X toolkit to fire a callback procedure
**      whenever input is ready on the message queue.  For VMS there's a bit
**      of skulduggery that we must do to make this work.  In particular,
**      since variable length data is travelling from here to there, we don't
**      want to just queue a read and let the event flag finish.  Instead we
**      will set up a write attention AST and have it set the event flag.
** Formal Parameters:
**   XtAppContext ctx:
**     Application context.
**   XtInputCallbackProc proc:
**     The procedure for Xt to call.
**     This procedure is called with the following arguments (in order):
**      XtPointer user_data:
**         We pass our 'this' pointer.
**      int *source:
**         Points to the source descriptor... system dependent contents.
**      XtInputId *id:
**         Points to the id of the input request.
*/
void 
MessageQueue::AddXtCallback(XtAppContext ctx, XtInputCallbackProc proc)
#ifdef unix
{

  if(XtNotificationOn) {
    CancelXtCallback();
  }
  XtNotificationOn = True;
  XtInputHandlerId = XtAppAddInput(ctx, fid, 
				   (XtPointer)XtInputReadMask, proc, this);
}
#endif
#ifdef VMS
{
  if(XtNotificationOn) {
    CancelXtCallback();
  }
  XtNotificationOn = True;
  efn = ::getef();			/* Get an unused event flag */

  XtInputHandlerId = XtAppAddInput(ctx, efn,
				(void *)NULL, proc, this);

  /* Now setup the input attention AST which will actually set the event flg */

  int istat = sys$qiow(0, chan, IO$_SETMODE | IO$M_WRTATTN, iosb,
		       0,0,(int)TriggerXEvent, (int)this, 0,0,0,0);
  vmsassert(istat);
  vmsassert(iosb[0]);

}
/*
** Functional Description:
**   MessageQueue::ResetNotificationAst:
**     Needed because of the way that VMS mailbox Attention Ast's work.
**     The function re-sets the AST for write attentions. It should be called
**     when the user leaves the processing function.
*/
void MessageQueue::ResetNotificationAst()
{
  assert(XtNotificationOn);

  int istat = sys$qiow(0, chan, IO$_SETMODE | IO$M_WRTATTN, iosb,
		       0,0,(int)TriggerXEvent, (int)this, 0,0,0,0);
  vmsassert(istat);
  vmsassert(iosb[0]);
}
#endif


/*
** Functional Description:
**   MessageQueue::CancelXtCallback:
**     This function cancels the XtInputcallback that might be present
**     on the message queue.  This is just a matter of calling XtRemoveInput
**     and then setting the XtNotificationOn flag False so that the rest
**     of us know about this.  This is system independent.
*/
void 
MessageQueue::CancelXtCallback()
{
  if(XtNotificationOn) {
    XtRemoveInput(XtInputHandlerId);
#ifdef VMS
    freeef(efn);		/* VMS needs to free the event flag. */
#endif
    XtNotificationOn = False;
  }
}

/*
** Functional Description:
**   MessageQueue::WaitForInputData:
**      This function waits for input data on a message queue.
**      This function is not compatible with XtNotification so we error
**      out if that's true with EBUSY.
**      Unix implementation uses select(), while the 
**      VMS implementation uses event flag terminated reads with a timer
**      AST used to cancel the read and fire the event flag. 
** Formal Parameters:
**   int timeout:
**     The number of seconds to wait with the following special cases:
**       0  - Wait forever.
**      -1  - Poll for data now... return immediately if no data is
**            available.
**   void *buf:
**      Pointer to the buffer.
**   unsigned int bytes:
**      size of buffer.
**   Returns:
**      -1   - error in the read.
**       0   - read timed out.
**       >0  - Number of bytes read.
*/
int 
MessageQueue::WaitForInputData(int timeout, void *buf, int bytes)
#ifdef unix
{
  fd_set readfds, writefds, exceptfds;
  struct timeval to;
  int status;
  to.tv_sec = timeout;
  to.tv_usec= 0;
  if(timeout < 0) to.tv_sec = 0; /* -1 is a poll. */

  /* Set up the fd_set's for the selects: */

  FD_ZERO(&readfds);
  FD_SET(fid, &readfds);
  FD_ZERO(&writefds);
  FD_ZERO(&exceptfds);

  /* This is the polling case and the timed wait case:  */

  if( timeout != 0) {
    status = select(FD_SETSIZE, &readfds, &writefds, &exceptfds,
		    &to);
    if(status != 1) {
      return 0;
    }
  }
  /* control falls here if either the timeout reduces to a blocking read
  ** or the select indicated that data was available. 
  */
  return read(buf, bytes);
}
#endif
#ifdef VMS
{
  int iofn = IO$_READVBLK;	/* Base I/O function code. */
  int ef   = getef();
  int id = 0;
  int istat;
  
  if(timeout == -1)
    iofn |= IO$M_NOW;		/* Poll so don't wait. */
  if(timeout > 0)		/* Set the timeout ast. */
    id = timerast(timeout, (void *)sys$setef, ef);
  istat = sys$qio(ef, chan, iofn, iosb,
		  0,0,
		  (int)buf, bytes, 0,0,0,0);
  if(istat != SS$_NORMAL) {
    errno = EVMSERR;
    vaxc$errno = istat;
    return -1;
  }
  sys$waitfr(ef);		/* Wait for the Event flag to fire. */
  if(iosb[0] == 0) {		/* timed out... */
    if(id) {
      canceltimerast(id);
    }
    sys$cancel(chan);		/* Cancel the read. */
    sys$synch(ef,iosb);	/* Wait for the cancel to signal done. */
  }
  freeef(ef);

  /* If the read was cancelled or the IO$M_NOW produced an end file indication
  ** then we had a timeout:
  */
  if( (iosb[0] == SS$_ABORT)  ||
      (iosb[0] == SS$_CANCEL) ||
      (iosb[0] == SS$_ENDOFFILE)) {
    return 0;			/* No bytes transferred. */
  }
  /* If there was an error in I/O completion, return that too: */

  if(iosb[0] != SS$_NORMAL) {
    errno = EVMSERR;
    vaxc$errno = iosb[0];
    return -1;
  }
  /* If we got here, then the read completed and iosb[1] has the transfer 
  ** count
  */
  return iosb[1];

}
#endif

/*
** Functional Description:
**   MessageQueue::unblockedread:
**     This function performs an unblocked read.
**     In UNIX, we flip the file descriptor into nonblocking I/O
**     do the read and then flip it back.
**     In VMS, we use IO$M_NOW on the read.
**   Formal Parameters:
**     void *buf:
**       Points to where to put the data.
**     unsigned int bytes:
**       number of bytes to transfer.
** Returns:
**    < 0   - I/O failure of some sort.
**      0   - No data to transfer.
**    > 0   - Number of bytes to transfer.
*/
int 
MessageQueue::unblockedread(void *buf,unsigned int bytes)
#ifdef unix
{
  int fdflags;
  int noblockflags;


  /* Get the file into the nonblocking mode:   */


  fdflags = fcntl(fid, F_GETFL, 0);
  if(fdflags < 0 ) return -1;

  noblockflags = fdflags | O_NONBLOCK;

  if(fcntl(fid, F_SETFL, noblockflags) < 0)
    return -1;

  /* Try the read.  If we fail and errno == EWOULDBLOCK or EAGAIN, then
  ** there was no data to read and we return 0
  */
  int nread = ::read(fid, buf, bytes);
  fcntl(fid, F_SETFL, fdflags);	/* Restore the control flag settings. */
  if(nread >= 0) return nread;	/* Successful read. */

  if( (errno == EWOULDBLOCK) || (errno == EAGAIN)) return 0;
  return -1;
  
}
#endif

#ifdef VMS
{
  short iosb[4];
  int istat = sys$qiow(0, chan, IO$_READVBLK | IO$M_NOW, iosb,
		       0,0,
		       (int)buf, bytes, 0,0,0,0);
  if(istat != SS$_NORMAL) {
    errno = EVMSERR;
    vaxc$errno = istat;
    return -1;
  }
  if(iosb[0] == SS$_ENDOFFILE) 
    return 0;
  if(iosb[0] != SS$_NORMAL) {
    errno = EVMSERR;
    vaxc$errno = iosb[0];
    return -1;
  }
  return iosb[1];
}
#endif

/*
** The code on the pages that follow is all system indpendent in that
** we rely on the system dependent code defined in the pages above.
** We are implementing the classes that are derived from the MessageQueue
** class.
*/

/*
** Functional Description:
**   GatesOutQueue::NotifyClientOfGate:
**     This function is used by Xamine to send a new gate to a client
**     process. Essentially we just do a write request on the MessageQueue
**     for the object we have been passed:
** Formal Parameters:
**   msg_object &object:
**     The object to notify the client of.
** Returns:
**   # bytes written or -1 if the write failed.
*/
int
GatesOutQueue::NotifyClientOfGate(msg_object &object)
{
  msg_XamineEvent event;

  /* first format the event: */

  event.event = Gate;		                   // We're sending a gate.
  memcpy(&event.data.gate, 
	 &object, sizeof(msg_object)); // Copy the gate data..

  return write(&event, sizeof(msg_XamineEvent));
}

/* 
** Functional Description:
**   GatesOutQueue::ReadGate:
**     This method is used by clients to read gates sent to them by
**     Xamine.
**  Formal Parameters:
**     msg_object &object:
**       Reference to the graphical object to read.
**  Returns:
**    nbytes read or -1 if there was an error.
*/
int
GatesOutQueue::ReadGate(msg_object &object)
{
  msg_XamineEvent event;
  int is =  read(&event, sizeof(msg_XamineEvent));
  if(is >= 0) {
    if(event.event != Gate){
      errno = EINVAL;
      return -1;
    }
    memcpy(&object, &event.data.gate, sizeof(msg_object));
  }
  return is;
}

/*
** Functional Description::
**   GatesOutQueue::DeclareEvent:
**     This function writes an arbitrary event structure to the event mailbox
** Formal Parameters:
**    msg_XamineEvent *event:
**       Pointer to the event to write.
** Returns:
**    number of bytes written or negative if error.
*/
int GatesOutQueue::DeclareEvent(msg_XamineEvent *event)
{
  return write(event, sizeof(msg_XamineEvent));
}

/*
** Functional Description:
**   ReplyMsgQueue::SendEnterAck:
**     This function is used by Xamine to acknowledge receipt of a
**      message which enters a gate from a client.
** Formal Parameters:
**    msg_status status:
**      Status of the acknowledgement.
** Returns:
**    Number of bytes sent or -1 if there was a failure.
*/
int
ReplyMsgQueue::SendEnterAck(msg_status status)
{
  msg_enterack ack;

  ack.status = status;

  return write(&ack, sizeof(msg_enterack));

}

/* 
** Functional Description:
**    ReplyMsgQueue::SendDeleteAck:
**      Send an acknowledgement to a delete request.
** Formal Parameters:
**    msg_status status:
**      Status of the send request.
** Returns:
**    # bytes sent or < 0 if failed.
*/
int
ReplyMsgQueue::SendDeleteAck(msg_status status)
{
  msg_rmvack ack;

  ack.status = status;
  return write(&ack, sizeof(msg_rmvack));
}

/*
** Functional Description:
**   ReplyMsgQueue::SendInquiryAck:
**    Acknowledges a request to inquire the set of gates set on a spectrum.
** Formal Parameters:
**    msg_status status:
**       The status of the inquire to be returned.
**    int object_count:
**       Note that if the status is not Success, then we assert that this
**       argument must be zero.
** Returns:
**    Number of bytes written or < 0 if failure.
*/
int
ReplyMsgQueue::SendInquiryAck(msg_status status, int object_count)
{
  msg_inquireack ack;

  ack.status = status;
  if(status != Success) 
    assert(object_count == 0);

  ack.nobjects = object_count;
  return write(&ack, sizeof(msg_inquireack));

}

/*
** Functional Description:
**   ReplyMsgQueue::SendAGate:
**     This function sends a gate to a reply queue. The reply message queue
**     has presumably first sent an inquiry acknowledge to the client which
**     indicates how many gates will be sent.
** Formal Parameters:
**    msg_object &object:
**      The gate to send.
** Returns:
**   Number of bytes written or -1 if failure.
*/
int
ReplyMsgQueue::SendAGate(msg_object &object)
{
  return write(&object, sizeof(msg_object) );
}


/*
** Functional Description:
**   ReplyMsgQueue::ReadEnterAck:
**      Read an acknowledgement message corresponding to an enter operation.
** Formal Parameters:
**    msg_status &status:
**     Reference to the buffer that will be filled in with the
**     status of the acknowledge.
** Returns:
**    status of read...
*/
int 
ReplyMsgQueue::ReadEnterAck(msg_status &status)
{
  msg_enterack ack;
  int stat;
  stat = read(&ack, sizeof(msg_enterack));
  status = ack.status;
  return stat;
}

/*
** Functional Description:
**   ReplyMsgQueue::ReadDeleteAck:
**     This function reads the acknowledge message corresponding to a
**     delete ack.
** Formal Parameters:
**     msg_status &status:
**       reference to the buffer to hold the status from the ack buffer.
** Returns:
**    result of read()
*/
int
ReplyMsgQueue::ReadDeleteAck(msg_status &status)
{
  int s;
  msg_rmvack ack;

  s      = read(&ack, sizeof(msg_rmvack));
  status = ack.status;
  return s;
}

/*
** Functional Description:
**   ReplyMsgQueue::ReadInquiryAck:
**     Reads the acknowledgement associated with an inquire operation.
**     Note that the ack is parcelled up into it's constituent pieces for
**     the caller.
** Formal Parameters:
**    msg_status &status:
**      Status of the inquire.
**    int &object_count:
**      Number of objects that will be returned.
** Returns:
**    Value of read:
*/
int
ReplyMsgQueue::ReadInquiryAck(msg_status &status, int &object_count)
{
  int s;
  msg_inquireack ack;

  s = read(&ack, sizeof(msg_inquireack));
  status       = ack.status;
  object_count = ack.nobjects;
  return s;
}

/*
** Functional Description:
**   ReplyMsgQueue::ReadAGate:
**     Reads a gate from the acknowledge message queue.
**     These gates come in response to Inquiry requests.
** Formal Parameters:
**    msg_object &object:
**      Reference to the object to hold the result.
** Returns:
**    read status.
*/
int
ReplyMsgQueue::ReadAGate(msg_object &object)
{
  return read(&object, sizeof(msg_object));
}


/*
** Functional Description:
**   RequestMsgQueue::EnterGate:
**     This function is called by a client to request that a gate be sent
**     to Xamine. We format an enter object request and follow that up
**     by sending the object passed in as an argument.  Note that the
**     enter request and the enter data are not sent atomically.
** Formal Parameters:
**   msg_object &object:
**     The object to enter.
** Returns:
**   The number of bytes sent or -1 if failed.
*/
int
RequestMsgQueue::EnterGate(msg_object &object)
{
  msg_command cmd;
  int nbytes;
  int totbytes;

  cmd.command = Enter;		/* We're entering a gate. */

  nbytes      = write(&cmd, sizeof(msg_command));

  if(nbytes < 0) return -1;
  totbytes    = nbytes;

  totbytes += nbytes = write(&object, sizeof(msg_object));
  if(nbytes < 0) return -1;
  return totbytes;
}


/*
** Functional Description:
**   RequestMsgQueue::DeleteGate:
**     This function sends a request to Xamine to delete an existing gate.
**     The delete request and the delete data are sent by this function.
**     These sends are not atomic since the VMS mailboxes are record oriented.
** Formal Parameters:
**    msg_remove &object:
**      Describes the object to remove.
** Returns:
**     total number of bytes written or -1 if failed.
*/
int
RequestMsgQueue::DeleteGate(msg_remove &object)
{
  msg_command cmd;
  int nbytes, totbytes;

  cmd.command = Remove;
  totbytes = nbytes  = write(&cmd, sizeof(msg_command));
  if(nbytes < 0) return -1;
  
  totbytes += nbytes = write(&object, sizeof(msg_remove));
  if(nbytes < 0) return -1;
  return totbytes;

}

/*
** Functional Description:
**   RequestMessage::RequestObjectList:
**     This function is used by a client to ask for the list of objects
**     that are currently in place on a given spectrum.
** Formal Parameters:
**   unsigned int spectrum:
**      The spectrum we're asking about.
** Returns:
**   Number of bytes written or negative if this fails.
*/
int
RequestMsgQueue::RequestObjectList(unsigned int spectrum)
{
  msg_command cmd;
  msg_inquiremsg inq;
  int nbytes,totbytes;

  cmd.command  = Inquire;
  inq.spectrum = spectrum;

  totbytes = nbytes = write(&cmd, sizeof(msg_command));
  if(nbytes < 0) return -1;

  totbytes += nbytes = write(&inq, sizeof(msg_inquiremsg));
  if(nbytes < 0) return -1;
  return totbytes;
}

/*
** Functional Description:
**   RequestMsgQueue::ReadCommand:
**     This function reads a command from a request message queue.  It's
**     intended to be used by Xamine to process requests from the client.
** Formal Parameters:
**   msg_command *command:
**     points to the buffer to receive the command.
**   int wait:
**     if zero, then the read is done without waiting.  If nonzero then
**     waits are performed.
** Returns:
**   Number of bytes read, 0 if nowait read had no data and negative on
**   general failures.
*/
int
RequestMsgQueue::ReadCommand(msg_command *command, int wait)
{
  if(wait) {
    return read(command, sizeof(msg_command));
  }
  else {
    return unblockedread(command, sizeof(msg_command));
  }
}

/*
** Functional Description:
**   RequestMsgQueue::ReadObject:
**     This function reads an object from a request message queue.
**     presumably Xamine has first gotten an Enter command request message.
** Formal Parameters:
**    msg_object *object:
**      Points to the buffer into which the object is read.
** Returns:
**    -1 on failure or number of bytes read.
*/
int
RequestMsgQueue::ReadObject(msg_object *object)
{
  return read(object, sizeof(msg_object));
}
//
//  RequestMsgQueue::ReadPeak:
//      Read a peak object definition from a request message queue.
// Formal Parameters:
//     msg_peakentry *peak:
//       Points to the buffer to contain the peak.
// Returns:
//   -1 on failure else # bytes read.
//
int RequestMsgQueue::ReadPeak(msg_peakentry* peak)
{
  return read(peak, sizeof(msg_peakentry));
}

/*
** Functional Description:
**   RequestMsgQueue::ReadDeleteData:
**     This function reads the data part of a Remove request from the
**     request queue.  Xamine invokes this method after it has received
**     a request to remove a gate in order to determine which gate to remove.
** Formal Parameters:
**   msg_remove *object:
**     points to a buffer which will contain the removal request data.
** Returns:
**   Number of bytes read or -1 if failed.
*/
int
RequestMsgQueue::ReadDeleteData(msg_remove *object)
{
  return read(object, sizeof(msg_remove));
}

/*
** Functional Description:
**    RequestMsgQueue::ReadObjectListData:
**      This function reads the spectrum for which an object list is 
**      requested:
** Formal Parameters;
**   unsigned int *spectrum:
**     Reference to the spectum to be  listed.
** Returns:
**   Result of read.
*/
int 
RequestMsgQueue::ReadObjectListData(unsigned int *spectrum)
{
  return read(spectrum, sizeof(unsigned int));
}


/*
** Functional Description:
**   RequestMsgQueue::ReadButtonBoxGeometry:
** Formal Parameters:
**    Cardinal *columns:
**       Number of columns to make in the button box (returned).
**    Cardinal *rows:
**       Number of rows to make in the button box (returned);
** Returns:
**       Value of the read.
*/
int
RequestMsgQueue::ReadButtonBoxGeometry(Cardinal *columns, Cardinal *rows)
{
  int status;
  ButtonCoordinate size;

  status = read(&size, sizeof(ButtonCoordinate));
  *columns  = size.column;
  *rows     = size.row;

  return status;

}

/*
** Functional Description:
**   ReplyMessageQueue::SendAck:
**     Sends a bare ACK message consisting of only a status value
**     to the client.
** Formal Parameters:
**    msg_status &status:
**      status to send.
** Returns:
**     Value of the write.
*/
int ReplyMsgQueue::SendAck(msg_status status)
{
  return write(&status, sizeof(msg_status) );
}
