/*
** Facility:
**   Xamine - NSCL display program.
** Abstract:
**    clientbuttons.c:
**      This file contains client side routines to manage the client definable
**      buttons.  Really about all we do is take requests and transform them
**      into messages sent to Xamine.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/

/*
** Include files
*/
#include <stdlib.h>
#include <string.h>

#include "client.h"

#include "clientops.h"


/*
** Functional Description:
**   Xamine_DefineButtonBox:
**     This function defines a box which will be filled with a grid of
**     client buttons.  The client specifies the size of the grid.
**     Our job is to format and send a request to Xamine to do this and
**     to look at the status return.
** Formal Parameters:
**    int ncol, nrow:
**       Number of rows and columns to put in the button box.
** Returns:
**    0   - If successful.
**   neg. - Failure code describing the problem.
*/
int Xamine_DefineButtonBox(int ncol, int nrow)
{
  struct msg_command         cmd = { Button_CreateBox };
  struct msg_CreateButtons   data;
  struct msg_enterack        ack;

  /*
  **  Format the data area:
  */
  data.size.column  = ncol;
  data.size.row     = nrow;

  /* Ensure that Xamine is still alive */

  if(!Xamine_Alive()) {
    return NoProcess;		/* It's dead so return an error. */
  }
				/* Write the request... */

  if(WritePipe(requests, &cmd, sizeof(struct msg_command)) < 0) {
    return CheckErrno;
  }
				/* Send the command parameters. */

  if(WritePipe(requests, &data, sizeof(struct msg_CreateButtons)) < 0) {
    return CheckErrno;
  }
				/* Get the ack msg. */
  if(ReadPipe(acks, -1, &ack, sizeof(struct msg_enterack)) < 0) {
    return CheckErrno;
  }
  return ack.status;
 
}

int f77xamine_definebuttonbox_(int *ncol, int *nrow)
{
  return Xamine_DefineButtonBox(*ncol, *nrow);
}

/*
** Functional Description:
**   Xamine_DefineButton:
**      This function is called by a client to create a button and
**      attach it to the button box.   The button box must have first been
**      created.
** Formal Parameters:
**    int nrow, int ncol:
**       The coordinates of the button in the box.
**     ButtonDescription *button:
**       Describes the type of button to create.
** Returns:
**   Success for success or some negative number for an error.
**   The negative number describes the error, but note that the
**   value CheckErrno means that the actual error reason is in the
**   C errno variable.
*/
int Xamine_DefineButton(int nrow, int ncol, void *b)
{
  ButtonDescription *button    = (ButtonDescription *)b;
  struct msg_command       cmd = { Button_DefineButton };
  struct msg_DefineButton  def;
  struct msg_enterack      ack;

  /* Set up the button description: */

  def.which.row    = nrow;
  def.which.column = ncol;
  memcpy(&def.button, button, sizeof(ButtonDescription));

  /* Ensure that Xamine is running still: */

  if(!Xamine_Alive()) 
    return NoProcess;		/* If not then tell the caller forget it. */

  /* Send the command and parameters to Xamine... for each problem
  ** we report the CheckErrno status since that's where the WritePipe
  ** operation will leave this sort of error.
  */

  if(WritePipe(requests, &cmd, sizeof(struct msg_command)) < 0) {
    return CheckErrno;
  }
  if(WritePipe(requests, &def, sizeof(struct msg_DefineButton)) < 0) {
    return CheckErrno;
  }
  /*   So far so good, We've got the request sent to Xamine.  Now let's see */
  /*     how Xamine will respond to the request. */

  if(ReadPipe(acks, -1, &ack, sizeof(struct msg_enterack)) < 0) {
    return CheckErrno;
  }
  return ack.status;
}

int f77xamine_definebutton_(int *nrow, int *ncol, void *button)
{
  return Xamine_DefineButton(*nrow, *ncol, button);
}


/*
** Functional Description:
**   Xamine_EnableButton:
**      This function enables a client button.   Enabled buttons react to
**      clicks and also are not displayed in a ghosted manner.
** Formal Parameters:
**   int r, c:
**     Row and column coordinates of the button.
** Returns:
**   Status of the attempt.
*/

int Xamine_EnableButton(int r, int c)
{
  struct msg_command      cmd;
  struct msg_EnableButton b;
  struct msg_enterack     ack;

  /* First we set up the operation description:  */

  cmd.command = Button_EnableButton;
  b.which.row    = r;
  b.which.column = c;

  /* Now send the command to Xamine if Xamine is still alive: */

  if(!Xamine_Alive())
    return NoProcess;
  if(WritePipe(requests, &cmd, sizeof(struct msg_command)) < 0) {
    return CheckErrno;
  }

  /* Now send the coordinates of the buton to Xamine too: */

  if(WritePipe(requests, &b, sizeof(struct msg_EnableButton)) < 0) {
    return CheckErrno;
  }

  /* Finally read Xamine's reply from the pipe and return the status to
  ** the caller/client
  */

  if(ReadPipe(acks, -1, &ack, sizeof(struct msg_enterack)) < 0) {
    return CheckErrno;
  }
  return ack.status;
}   

/* Fortran binding is trivial:  */
int f77xamine_enablebutton_(int *r, int *c)

{
  return Xamine_EnableButton(*r, *c);
}



/*
** Functional Description:
**   Xamine_DisableButton:
**      This function Disables a client button.   Disabled buttons react to
**      clicks and also are not displayed in a ghosted manner.
** Formal Parameters:
**   int r, c:
**     Row and column coordinates of the button.
** Returns:
**   Status of the attempt.
*/

int Xamine_DisableButton(int r, int c)
{
  struct msg_command      cmd;
  struct msg_DisableButton b;
  struct msg_enterack     ack;

  /* First we set up the operation description:  */

  cmd.command = Button_DisableButton;
  b.which.row    = r;
  b.which.column = c;

  /* Now send the command to Xamine if Xamine is still alive: */

  if(!Xamine_Alive())
    return NoProcess;
  if(WritePipe(requests, &cmd, sizeof(struct msg_command)) < 0) {
    return CheckErrno;
  }

  /* Now send the coordinates of the buton to Xamine too: */

  if(WritePipe(requests, &b, sizeof(struct msg_DisableButton)) < 0) {
    return CheckErrno;
  }

  /* Finally read Xamine's reply from the pipe and return the status to
  ** the caller/client
  */

  if(ReadPipe(acks, -1, &ack, sizeof(struct msg_enterack)) < 0) {
    return CheckErrno;
  }
  return ack.status;
}   

/* Fortran binding is trivial:  */
int f77xamine_disablebutton_(int *r, int *c)
{
  return Xamine_DisableButton(*r, *c);
}


/*
** Functional Description:
**   Xamine_DeleteButton:
**      This function Deletes a client button.   Deleted buttons react to
**      clicks and also are not displayed in a ghosted manner.
** Formal Parameters:
**   int r, c:
**     Row and column coordinates of the button.
** Returns:
**   Status of the attempt.
*/

int Xamine_DeleteButton(int r, int c)
{
  struct msg_command      cmd;
  struct msg_DeleteButton b;
  struct msg_enterack     ack;

  /* First we set up the operation description:  */

  cmd.command = Button_DeleteButton;
  b.which.row    = r;
  b.which.column = c;

  /* Now send the command to Xamine if Xamine is still alive: */

  if(!Xamine_Alive())
    return NoProcess;
  if(WritePipe(requests, &cmd, sizeof(struct msg_command)) < 0) {
    return CheckErrno;
  }

  /* Now send the coordinates of the buton to Xamine too: */

  if(WritePipe(requests, &b, sizeof(struct msg_DeleteButton)) < 0) {
    return CheckErrno;
  }

  /* Finally read Xamine's reply from the pipe and return the status to
  ** the caller/client
  */

  if(ReadPipe(acks, -1, &ack, sizeof(struct msg_enterack)) < 0) {
    return CheckErrno;
  }
  return ack.status;
}   

/* Fortran binding is trivial:  */
int f77xamine_deletebutton_(int *r, int *c)

{
  return Xamine_DeleteButton(*r, *c);
}


/*
** Functional Description:
**   Xamine_DeleteButtonBox:
**     Deletes the button box.
** Returns:
**   Status of the attempt.
*/
int
Xamine_DeleteButtonBox()
{
  struct msg_command      cmd;
  struct msg_enterack     ack;

  /* First we set up the operation description:  */

  cmd.command = Button_DeleteBox;

  /* Now send the command to Xamine if Xamine is still alive: */

  if(!Xamine_Alive())
    return NoProcess;
  if(WritePipe(requests, &cmd, sizeof(struct msg_command)) < 0) {
    return CheckErrno;
  }

  /* Finally read Xamine's reply from the pipe and return the status to
  ** the caller/client
  */

  if(ReadPipe(acks, -1, &ack, sizeof(struct msg_enterack)) < 0) {
    return CheckErrno;
  }
  return ack.status;

}   

int f77xamine_deletebuttonbox_()
{
  return Xamine_DeleteButtonBox();
}


/*
** Functional Description:
**   Xamine_InquireButton:
**     Asks for a description of a client button.
** Formal Parameters:
**   int r,c:
**     Coordinates of the button.
**   struct msg_InquireButtonAck *ack:
**     points to a button description block.
** Returns:
**   Status of the inquiry. attempt.
*/

int Xamine_InquireButton(int r, int c, void *info)
{
  struct msg_command  cmd;
  struct msg_enterack ack;
  struct msg_InquireButton b;

  /* Describe the operation:  */

  cmd.command = Button_InquireButton;
  b.which.row    = r;
  b.which.column = c;

  /* Now send the command to Xamine if Xamine is still alive: */

  if(!Xamine_Alive())
    return NoProcess;
  if(WritePipe(requests, &cmd, sizeof(struct msg_command)) < 0) {
    return CheckErrno;
  }

  /* Send the button coordinates: */

  if(WritePipe(requests, &b, sizeof(struct msg_InquireButton)) < 0) {
    return CheckErrno;
  }

  /* Now get Xamine's reply.. this will be two messages, an ack and
  ** the button info.  The button info is even sent on failure acks.
  */

  if(ReadPipe(acks, -1, &ack, sizeof(struct msg_enterack)) < 0) {
    return CheckErrno;
  }
  if(ReadPipe(acks, -1, info, sizeof(struct msg_InquireButtonAck)) < 0) {
    return CheckErrno;
  }

  return ack.status;

}   
/*
** The following is the trivial Fortran binding:
*/

int f77xamine_inquirebutton_(int *r, int *c, void *info)
{
  return Xamine_InquireButton(*r, *c, info);
}


/*
** Functional Description:
**   Xamine_ModifyButton:
**     This function modifies a button in the button box.  It can also be
**     used to create a button if the target coordinates are not known
**     to contain a button already.
** Formal Parameters:
**   int r,c:
**    Coordinates of the button position to modify.
**   void *b:
**     Pointer to the button description block.  This is actually of type
**     ButtonDescription.
** Returns:
**   Status of the attempt.  Note that ButtonCreated is a success code which
**   means there was no prior button in the target coordinates, but that one
**   was created.
*/

int Xamine_ModifyButton(int r, int c, void *bd)
{
  struct msg_command      cmd;
  struct msg_ModifyButton descr;
  struct msg_enterack     ack;


  /* Build the descriptions:
  */

  cmd.command   = Button_ModifyButton;

  descr.which.row    = r;
  descr.which.column = c;
  memcpy(&descr.button, bd, sizeof(ButtonDescription));

  /*
  * Don't bother messaging Xamine if it already died:
  */

  if(!Xamine_Alive())
    return NoProcess;

  /* Now send the command and description: */

  if(WritePipe(requests, &cmd, sizeof(struct msg_command)) < 0) {
    return CheckErrno;
  }

  if(WritePipe(requests, &descr, sizeof(struct msg_ModifyButton)) < 0) {
    return CheckErrno;
  }

  /*  Now get Xamine's reply.  This should just be a status message */

  if(ReadPipe(acks, -1, &ack, sizeof(struct msg_enterack)) < 0) {
    return CheckErrno;
  }

  return ack.status;
}   
