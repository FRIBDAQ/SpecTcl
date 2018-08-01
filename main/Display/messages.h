/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/*
** Facility:
**   Xamine -- NSCL display program.
** Abstract:
**   messages.h:
**     This file is intended for both clients (C++) and Xamine.
**     it contains definitions of classes and messages used to pass
**     gate information between Xamine and the rest of the world.
**     NOTE NOTE NOTE:
**       Owing to mixed language problems on Unix systems, some of the
**       type defnitions here are duplicated in clientgates.c
**       Any changes here must be propagated as needed there.
**
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/

#ifndef MESSAGES_H_INSTALLED
#define MESSAGES_H_INSTALLED
#include <Xm/Xm.h>
#include "dispgrob.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "clientops.h"

#ifdef OLD_VERSION
/*
** The definitions below are used in composing messages:
*/

    /* Operation request codes recoginized by Xamine */

enum operation {
                 Enter,		/* Enter a gate. */
		 Remove,	/* Remove a gate. */
		 Inquire	/* Inquire a gate list. */
	       };

    /* Status return codes in the ack mailbox: */


typedef int msg_status;
static const int  Successful = 0;
static const int  NoSuchSpectrum        = -1;
static const int  InappropriateGate     = -2;
static const int  ObjectTableFull       = -3;
static const int  DuplicateId           = -4;
static const int  NoSuchObject          = -5;
static const int  NoProcess             = -6;
static const int  CheckErrno            = -7;
		  

    /* A point, as represented by messages. */

struct msg_point { 
               int x;
	       int y;
	     };

    /* A message command structure sent to the request exchange */

struct msg_command {
                          operation command;
		   };

    /* Below is the representation of an object in messages sent around: */

struct msg_object  {
                      unsigned int spectrum; /* Number of spectrum of object */
		      unsigned int id;      /* Object identification number. */
		      grobj_type   type;     /* Object type code. */
		      int          hasname;  /* Non zero if there's a name. */
		      grobj_name   name;     /* Object name.      */
		      int          npts;     /* Number of points in object */
		      msg_point    points[GROBJ_MAXPTS]; /* the points     */
		    };

    /* The acknowledgment message for enter messages: */

struct msg_enterack {
                      msg_status status;
		    };
    /* The data message for remove gate requests. */

struct msg_remove {
                     unsigned int spectrum; /*  Spectrum gate is set on. */
		     unsigned int object;   /*  Identifier of object to rmv */
		     grobj_type   type;     /*  Type of object to rmv.   */
		  };

     /* Acknowledge for remove requests.  */

struct msg_rmvack {
                     msg_status status; /* Status of the attempt. */
		  };

     /* Below asks for the list of objects on a spectrum: */

struct msg_inquiremsg {
                        int spectrum; /* Spectrum id. */
		      };

     /* Below is the response prior to the list of msg_object structures */
     /* that come back from an inquire:                                  */

struct msg_inquireack {
                        msg_status status;    /* Status of the attempt. */
			int        nobjects;  /* number of objects returned */
		      };

#endif
/*
** MessageQueue is a generic system independent named pipe between
** two processes.  On Unix it is implemented with mkfifo and on 
** vms with SYS$CREMBX
*/
class MessageQueue {
  static bool pipeSigBlocked;
 public:
  int   XtNotificationOn;  /* True if Xt input source   */
  MessageQueue(char *name, int flags);	             /* Construct msg q. */
  virtual ~MessageQueue();                           /* Delete  msg q.   */
  int write(void *buf, unsigned int bytes);          /* Write data block */
  int  read(void *buf, unsigned int bytes);          /* Read data block. */
  void AddXtCallback(XtAppContext ctx,
		     XtInputCallbackProc proc);      /* Add input avail cb. */
  void CancelXtCallback();
  int  getid() { return fid; }
  int  WaitForInputData(int timeout, 
			void *buf, int bytes);   /* Read with timeout */
  int unblockedread(void *buf, unsigned int bytes);

 private:
  int   XtInputHandlerId;  /* Input id from XtAppAddInput */
  int fid;	               /* File id open on the FIFO. */
  void blockPipesig();
};


/*
** Xamine clients get gates from GatesOutQueue.
*/
class GatesOutQueue : public MessageQueue
{
 public:
  GatesOutQueue(char *name, int flags) : MessageQueue(name, flags) {  }
  int NotifyClientOfGate(msg_object &object);
  int ReadGate(msg_object &object);
  int DeclareEvent(msg_XamineEvent *event); /* Declare arbitrary event. */

};

/*
** Xamine clients get responses to requests back from a ReplyMsgQueue:
*/
class ReplyMsgQueue : public MessageQueue
{
 public:
  ReplyMsgQueue(char *name, int flags) : MessageQueue(name, flags) {}
  int ReadEnterAck(msg_status &status);
  int ReadDeleteAck(msg_status &status);
  int ReadInquiryAck(msg_status &status, int &object_count);
  int ReadAGate(msg_object &object);

  int SendEnterAck(msg_status status);
  int SendDeleteAck(msg_status status);
  int SendInquiryAck(msg_status status, int object_count);
  int SendAGate(msg_object &object);
  int SendAck(msg_status status);
};

/*
** Xamine clients send requests via a RequestMsgQueue:
*/
class RequestMsgQueue : public MessageQueue
{
 public:
  RequestMsgQueue(char *name, int flags) : MessageQueue(name, flags) {}
  int EnterGate(msg_object &object);
  int DeleteGate(msg_remove &object);
  int RequestObjectList(unsigned int spectrum);
  int ReadCommand(msg_command *command, int wait);
  int ReadObject(msg_object *object);
  int ReadPeak(msg_peakentry* object);
  int ReadFitline(msg_fitline* object);
  int ReadDeleteData(msg_remove *object);
  int ReadObjectListData(unsigned int *spectrum);
  int ReadButtonBoxGeometry(Cardinal *columns, Cardinal *rows);
};
/*
** The combination of a request message queue and a reply message queue
** represents the two way communication between client and Xamine
** for actions initiated by the client:
*/
class ClientRequests : public RequestMsgQueue, public ReplyMsgQueue
{
 public:
  ClientRequests(char *requestname, char *ackname,
		 int reqflags, int ackflags) :
    RequestMsgQueue(requestname, reqflags),
    ReplyMsgQueue(ackname, ackflags)
      {}

};

#endif





