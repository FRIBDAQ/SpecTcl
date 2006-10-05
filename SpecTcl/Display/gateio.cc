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

static const char* Copyright = "(C) Copyright Michigan State University 1994, All rights reserved";
/*
** Facility:
**   Xamine -- NSCL Display program.
** Abstract:
**   gateio.cc:
**     This file contains the functions that Xamine uses to 
**     handle message I/O involving gates
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/


/*
** Include files:
*/
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>

#include "gateio.h"
#include "XMWidget.h"
#include "messages.h"
#include "clientops.h"
#include "mapcoord.h"
#include "panemgr.h"
/*
** DEFINE's.  These DEFINE's are used to derive the names of the message
** queues used by this module:
*/
#define NAMELEN  128
#define PREFIX   "/tmp"
#define REQ_NAME "/.Xamine_%x_GatesIn"      /* %x replaced by parent pid. */
#define ACK_NAME "/.Xamine_%x_GatesAck"
#define OUT_NAME "/.Xamine_%x_GatesOut"

/*
** Below we define some externs which are declared in clientops.h, but not
** referenced anywhere but client programs.  This is needed to keep the
** VMS linker happy:
*/
int Xamine_requests, Xamine_acks, Xamine_newgates;

/*
** The object pointers below represent the objects that will be used
** for the Message queues.  First calls will cause creation.
*/
static ClientRequests *requests = NULL;
static GatesOutQueue  *inform   = NULL;

/*
** Functional Description:
**   MakeObjects:
**     This is a local function which creates all objects which don't yet 
**     exist.
*/
static void
MakeObjects()
{
  /* Make the request queue: */

  if(requests == NULL) {
    char reqname[NAMELEN];
    char ackname[NAMELEN];
    char lastpart[NAMELEN];

    /* construct the names: */

    strcpy(reqname, PREFIX);
    strcpy(ackname, PREFIX);
    sprintf(lastpart, REQ_NAME, getppid());
    strcat(reqname, lastpart);
    sprintf(lastpart, ACK_NAME, getppid());
    strcat(ackname, lastpart);

    requests = new ClientRequests(reqname, ackname,
				  O_RDONLY, O_WRONLY);
  }

  /* Make the Gates out queue: */

  if(inform == NULL) {
    char lastpart[NAMELEN];
    char name[NAMELEN];

    /* Construct the name: */
  
    strcpy(name, PREFIX);
    sprintf(lastpart, OUT_NAME, getppid());
    strcat(name, lastpart);

    inform = new GatesOutQueue(name, O_WRONLY);    
  }
}

/*
** Functional Description:
**   FormatMessageObject:
**     This local function formats a message object given a graphical object.
** Formal Parameters:
**    msg_object *o:
**      Points to a buffer for the output object.
**    grobj_generic *object:
**      The source object.
*/
static void
FormatMessageObject(msg_object &o, grobj_generic *object)
{

  /* Fill in the gates structure expected by the message queue: */

  memset(&o, 0, sizeof(msg_object));			/* Init to zero. */
  o.spectrum = object->getspectrum();
  o.type     = object->type();
  o.hasname  = object->named();
  o.id       = object->getid();
  object->getname(o.name);
  o.npts     = object->pointcount();
  for(int i = 0; i < o.npts; i++) {
    grobj_point *pt = object->getpt(i);
    o.points[i].x = pt->getx();
    o.points[i].y = pt->gety();
  }
}

/*
** Functional Description:
**   Xamine_ReportGate:
**      Reports a gate to the client process.
** Formal Parameters:
**   grobj_generic *object:
**      Points to the gate to write to inform.
** Returns:
**   number of bytes written or -1 if failure.
*/
int 
Xamine_ReportGate(grobj_generic *object)
{
  MakeObjects();		/* First make sure the objects are present. */
  msg_object o;			/* Object in form expected by message queue */

  FormatMessageObject(o, object); /* Format the object for the send. */

  /* Send the gate to the client:  */

  return inform->NotifyClientOfGate(o);
}

/*
** Functional Description:
**   Declares an event that the client should care about.
** Formal Parameters:
**   msg_XamineEvent *evt:
**    Pointer to the event message to deliver to the client.
** Returns:
**    number of bytes written or -1 if failed.
*/
int 
Xamine_DeclareClientEvent(msg_XamineEvent *evt)
{
  MakeObjects();
  return inform->DeclareEvent(evt);
}

/*
** Functional description:
**   Xamine_SetupRequestNotification:
**      Sets up request to create an X event which calls back the 
**      procedure passed in here whenever a request has been 
**      sent to us.
**  Formal Parameters:
**    XtAppContext ctx:
**      Application context for the add event.
**    XtInputCallbackProc proc:
**      The procedure to call in response to the Xt event.
*/
void
Xamine_SetupRequestNotification(XtAppContext ctx, XtInputCallbackProc cb)
{
  MakeObjects();
  requests->RequestMsgQueue::AddXtCallback(ctx, cb);
}

/*
** Functional Description:
**   Xamine_CheckForCommand:
**     Checks for a command message in the request message queue.
** Formal Parameters:
**    msg_command* cmd:
**       This buffer is filled in with the actual command value if there
**       is data available.
** Returns:
**    0 - No message available.
**   -1 - Some failure.
**   >0 - Number of bytes read.
*/
int 
Xamine_CheckForCommand(msg_command* cmd)
{
  MakeObjects();
  return requests->RequestMsgQueue::ReadCommand(cmd, 0);
}

/*
** Functional Description:
**   Xamine_EnterAck:
**     Acknowledge a gates enter request.
** Formal Parameters:
**     msg_status status:
**       Status to return to the caller.
** Returns:
**   Number of characters written to the message queue or -1 on failure.
*/
int
Xamine_EnterAck(msg_status status)
{
  MakeObjects();
  return requests->ReplyMsgQueue::SendEnterAck(status);
}

/*
** Functional Description:
**   Xamine_DeleteAck:
**     Acknowlege a request to delete a gate.
** Formal Parameters:
**   msg_status status:
**     The status to mail to the client.
** Returns:
**   The number of bytes sent to the client or -1 on failure.
*/
int
Xamine_DeleteAck(msg_status status)
{
  MakeObjects();
  return requests->ReplyMsgQueue::SendDeleteAck(status);
}

/*
** Functional Description:
**   Xamine_InqAck:
**     Acknowledge an inquiry request. Possibly prior to listing gates
**     to the client.
** Formal Parameters:
**   msg_status status:
**      Status to return to the client.
**   int object_count:
**      Number of objects that will be listed.
*/
int
Xamine_InqAck(msg_status status, int object_count)
{
  MakeObjects();
  return requests->ReplyMsgQueue::SendInquiryAck(status, object_count); 
}

/*
** Functional Description:
**   Xamine_ListGate:
**     Lists a gate as part of an inquiry transaction.
** Formal Parameters:
**     grobj_generic* object:
**       The object to send to the ack message queue.
** Returns:
**   Number of bytes written or -1 if fails.
*/
int
Xamine_ListGate(grobj_generic* object)
{
  MakeObjects();
  msg_object o;

  /* Format the message object as expected by the message queue: */

  FormatMessageObject(o, object);

  return requests->ReplyMsgQueue::SendAGate(o);

}
    

/*
** Functional Description:
**   Xamine_ReadGate:
**     Read a gate after receiving an Enter request.
** Formal Parameters:
**    grobj_generic** object:
**      A graphical object will be created and the pointer passed filled
**      in to point to it.  The caller must delete the object when done.
** Returns:
**   Number of bytes read from the message queue or -1
*/
int Xamine_ReadGate(grobj_generic** object)
{
  MakeObjects();
  msg_object o;
  grobj_generic *obj = NULL;
  int status = requests->RequestMsgQueue::ReadObject(&o);

  /* IF data was read, then unpack the gate: */

  if(status > 0) {
    /* First allocate the appropriate type of object: */
    switch (o.type) {
    case cut_1d:		/* Create a 1-d cut: */
      obj = (grobj_generic *)new grobj_cut1d;
      break;
    case contour_2d:
      obj = (grobj_generic *)new grobj_contour;
      break;
    case band:
      obj = (grobj_generic *)new grobj_band;
      break;
    case summing_region_1d:
      obj = (grobj_generic *)new grobj_sum1d;
      break;
    case marker_1d:
      obj = (grobj_generic *)new grobj_mark1d;
      break;
    case summing_region_2d:
      obj = (grobj_generic *)new grobj_sum2d;
      break;
    case marker_2d:
      obj = (grobj_generic *)new grobj_mark2d;
      break;
    default:
      obj = new grobj_generic;
    }
    /* Now fill in the object: */

    obj->setspectrum(o.spectrum);
    obj->setid(o.id);
    if(o.hasname) {
      obj->setname(o.name);
    }
    for(int i = 0; i < o.npts; i++) {
      obj->addpt(o.points[i].x, o.points[i].y);
    }
    *object = obj;		/* Fill in the pointer. */
  }
  else {
    *object = NULL;
  }
  return status;
}

//
// Functional Description:
//   Xamine_ReadPeak:
//       Reads a peak position marker from the user mailbox, after receiving
//       an EnterPeakPosition command.
// Formal Parameters:
//   grobj_generic** object:
//       If the function succeeds, this is filled in with a pointer to the
//       graphical object which was generated from the client data. If not,
//       it will be null.
// Returns:
//    n > 0  - For success, the number of bytes read from the mailbox.
//    -1     - In case of failure.
//
int Xamine_ReadPeak(grobj_generic** object)
{
  MakeObjects();		// Ensure the pipes are all set up.
  msg_peakentry peak;		// This will hold the user's data.
  grobj_generic *obj = NULL;	// This will hold the generated object.

  int status = requests->RequestMsgQueue::ReadPeak(&peak);

  if(status > 0) {		// There was data to read and it worked...
    obj = new grobj_Peak1d(peak.nSpectrum, 
			   peak.nId,
			   (char *)(peak.nHasName ?
			          peak.szName : NULL),
			   peak.fCentroid,
			   peak.fWidth);
  }
  *object = obj;		// If new or read failed, obj is still NULL.
  return (obj ? status : -1);   // Return status if everything worked.
			   
}
/*!
   Read a fitline specification from the user mailbox.
   after receiving an EnterFitline operation code.
  \param grobj_generic** object [out]
     The object will be read into a dynamically allocated grobj_Fitline. A pointer
     to that object will be placed in this buffer.
     On failure, this will be null.
  \return int
  \retval -1 An error of some sort occured.
  \retval >0 Number of bytes that was actually read from thje mailbox.
*/
int
Xamine_ReadFitline(grobj_generic** object)
{
  MakeObjects();		// Ensure the pipes are all set up.
  msg_fitline   fitline;        // The fitline that will be read in.
  grobj_generic* obj = NULL;

  // read the message...and on success generate the new fitline object

  int status = requests->RequestMsgQueue::ReadFitline(&fitline);
  if (status > 0) {
    obj = new grobj_Fitline(fitline.nSpectrum,
			    fitline.nId,
			    (char*)(fitline.nHasName ?
				    fitline.szName : NULL),
			    fitline.low, fitline.high,
			    fitline.tclProc);
  }
  *object = obj;
  return (obj ? status : -1);
}

/*
** Functional Description:
**  Xamine_ReadDeleteData:
**    Reads delete data after receiving a remove request.
** Formal Parameters:
**   int *spec:
**     Receives the spectrum number of the delete request.
**   int *id:
**     Receives the graphical object id of the delete request.
**   grobj_type *type:
**     Receives the type of graphical object to be deleted.
** Returns:
**   Number of bytes read or -1 for failures.
*/
int
Xamine_ReadDeleteData(int *spec, int *id, grobj_type *type)
{
  MakeObjects();
  msg_remove r;

  int status = requests->RequestMsgQueue::ReadDeleteData(&r);
  if(status > 0) {
    *spec = r.spectrum;
    *id   = r.object;
    *type = r.type;
  }
  return status;
}

/*
** Functional Description:
**   Xamine_ReadInqData:
**      This function reads the data associated with an inquiry.
** Formal Parameters:
**    int *spec:
**      returns the spectrum that we're inquiring about.
** Returns:
**   Number of bytes read or -1 if failed.
*/
int Xamine_ReadInqData(int *spec)
{
  MakeObjects();
  return requests->ReadObjectListData((unsigned int *)spec);
}

/*
** Functional Description:
**   Xamine_ReadButtonGeometry:
**     This function reads the button box geometry for the button box
**      from the client.
** Formal Parameters:
**   Cardinal *cols, *rows:
**     Point to buffers for the number of rows and columns.
** Returns:
**     Number of bytes read or negative on error.
*/
int Xamine_ReadButtonGeometry(Cardinal *cols, Cardinal *rows)
{
  MakeObjects();		// Make sure we can communicate
  
  int stat = requests->ReadButtonBoxGeometry(cols, rows);
  return stat;
}

/**
** Functional Description:
**   Xamine_SetClientStatus:
**      This function returns a status message to the client.  The
**      status message says something about the success or failure of an
**      operation.
** Formal Parameters:
**    msg_status stat:
**      The status to send.
** Returns:
**    Size of write or negative on failure.
*/
int Xamine_SetClientStatus(msg_status stat)
{
  return requests->SendEnterAck(stat);
}


/*
** Functional Description:
**   Xamine_SupplyButtonInfo:
**     Returns a buton information structure to the client.
* Formal Parameters:
**   msg_InquireButtonAck *ack:
**     The button information data structure.
** Returns:
**   status of the write attempt.
*/

int Xamine_SupplyButtonInfo(msg_InquireButtonAck *ack)
{
  MakeObjects();

  return requests->ReplyMsgQueue::write(ack, sizeof(msg_InquireButtonAck));
}

/*
** Functional Description:
**    Xamine_ReadButtonDefinition:
**      Accepts a button description from the requests mailbox.
**  Formal Parameters:
**      msg_DefineButton *bdef:
**          Points to the buffer to receive the button definition.
** Returns:
**    number of bytes read or negative on error.
*/
int Xamine_ReadButtonDefinition(msg_DefineButton *bdef)
{
  int status;
  MakeObjects();

  status = requests->RequestMsgQueue::read(bdef, sizeof(msg_DefineButton));
  return status;

}


/*
** Functional Description:
**   Xamine_ReadEnableLocation:
**   Xamine_ReadDisableLocation:
**     Reads the argument data block associated with a button enable/disable
** Formal Parameters:
**   msg_(Enable/Disble)Button *which:
**     Points to a buffer to contain the data block.
** Returns:
**   Status of the read from RequestMsgQueue::read()
*/
int Xamine_ReadEnableLocation(msg_EnableButton *which)
{

  MakeObjects();

  return requests->RequestMsgQueue::read(which, sizeof(msg_EnableButton));
}
int Xamine_ReadDisableLocation(msg_DisableButton *which)
{

  MakeObjects();

  return requests->RequestMsgQueue::read(which, sizeof(msg_DisableButton));
}

/*
** Functional Description:
**   Xamine_ReadDeleteLocation:
**     Reads the argument data block associated with a button deletion.
** Formal Parameters:
**   msg_DeleteButton *which:
**     Points to a buffer to contain the data block.
** Returns:
**   Status of the read from RequestMsgQueue::read()
*/
int Xamine_ReadDeleteLocation(msg_DeleteButton *which)
{

  MakeObjects();

  return requests->RequestMsgQueue::read(which, sizeof(msg_DeleteButton));
}




/*
** Functional Description:
**   Xamine_ReadInquireLocation:
**     Reads the location of the button about which an inquiry is being
**     performed.
** Formal Parameters:
**   msg_InquireButton *which:
**     Points to buffer to return the button coordinates.
** Returns:
**   Value of RequestMsgQueue::read()
*/
int
Xamine_ReadInquireLocation(msg_InquireButton *which)
{
  MakeObjects();
  return requests->RequestMsgQueue::read(which, sizeof(msg_InquireButton));
}   




/*
** Functional Description:
**   Xamine_ReadBModifyBlock:
**     Read the data associated with a modify button request.
** Formal Parameters:
**   msg_ModifyButton *mblock:
**     points to the data buffer.
** Returns:
**    Status of the attemtp.
*/

int Xamine_ReadBModifyBlock(msg_ModifyButton *mblock)
{
  MakeObjects();

  return requests->RequestMsgQueue::read(mblock, sizeof(msg_ModifyButton));
}      
