/*
** Facility:
**   Xamine -- NSCL Display program.
** Abstract:
**   clirequests.cc:
**     This file contains definitions needed for software which sets up the
**     gate notification.  Gate notification is done via an Xt User Input
**     event.
** Author:
**    Ron Fox
**    NSCL
**    Michigan State Univeristy
**    East Lansing, MI 48824-1321
*/
static char *sccsinfo="@(#)clirequests.cc	8.3 6/30/97 \n ";

/*
** Include files:
*/
#include <assert.h>

#ifdef VMS
#include <ssdef.h>
#endif

#include "acceptgates.h"
#include "grobjdisplay.h"
#include "convert.h"
#include "dispshare.h"
#include "XMWidget.h"
#include "errormsg.h"
#include "panemgr.h"
#include "dispgrob.h"
#include "clirequests.h"
#include "gateio.h"
#include "grobjmgr.h"
#include "refreshctl.h"
#include "buttonreq.h"
/*
** Function prototypes:
*/

#ifdef VMS
extern "C" {
   int sys$setef(int efn);
   int sys$clref(int efn);
}
#endif

/*
** External refs and defs:
*/
extern volatile  spec_shared *xamine_shared;

/*
** Functional Description:
**   DrawGate:
**     This function locates all panes that include a selected spectrum
**     and draw a gate on that spectrum.  We do this in order to reduce
**     the number of total spectrum refreshes (rather than just doing a 
**     complete redraw.
** Formal Parameters:
**   grobj_generic *object:
**    The object that we're drawing.
*/
static void 
DrawGate(grobj_generic *object)
{
  int         spec = object->getspectrum(); /* Need the spectrum. */
  grobj_type  tp   = object->type();        /* Will need object type too */
  int         nr   = Xamine_Panerows();
  int         nc   = Xamine_Panecols();
  pane_db    *db   = Xamine_GetPaneDb();

  for(int r = 0; r < nr; r++)
    for(int c = 0; c < nc; c++) {
      win_attributed *at = Xamine_GetDisplayAttributes(r,c);
      if(at != NULL) {
	if(at->spectrum() == spec) { /* Got a pane with our spectrum. */
	  if(db->refresh_state(r, c) == rfsh_idle) { /* Got one needs drawing */
	    XMWidget *w = Xamine_GetDrawingArea(r,c);
	    object->draw(w, at, True);
	  }
	}
      }
    }
}

/*
** Functional Description:
**   EnterGate:
**     This function is responsible for acknowledging and entering a 
**     gate in a spectrum.  We perform the following bits of 
**     error checking:
**       1) The spectrum exists and contains a user spectrum [NoSuchSpectrum]
**       2) The dimensionality of the spectrum is proper for the gate that's
**          being applied [InappropriateGate]
**       3) The Object is properly entered in the object table 
**          [ObjectTableFull]
** Formal Parameters:
**    grobj_generic *object:
**       Pointer to the object to enter.
*/
static void
EnterGate(grobj_generic *object)
{
  /* Make sure the spectrum is valid:  */

  int spec = object->getspectrum(); /* Fetch the spectrum number. */
  spec_type type = Xamine_SpectrumType(spec);
  if(type == undefined) {
    Xamine_EnterAck(NoSuchSpectrum);
    return;
  }
  /* Make sure the spectrum type is ok for the spectrum:  */

  grobj_type gtype = object->type();
  switch(type) {
  case onedlong:
  case onedword:
    if((gtype != cut_1d) && (gtype != peak1d) ) {
      Xamine_EnterAck(InappropriateGate);
      return;
    }
    break;
  case twodword:
  case twodbyte:
    if( (gtype != contour_2d) && (gtype != band)) {
      Xamine_EnterAck(InappropriateGate);
      return;
    }
    break;
  }
  /* Now it's all up to the entry function:  */

  grobj_generic *o;
  if(!( o = Xamine_EnterGate(object))) {
    Xamine_EnterAck(ObjectTableFull);
    return;
  }
  Xamine_EnterAck(Success);
  DrawGate(o);	/* Draw the gate on all appropriate panes. */
}

/*
** Functional Description:
**   DeleteGate:
**     This function deletes a gate from the gate data set.
**     We perform the following checks:
**     1) The spectrum exists.
**     2) A gate of the appropriate id and type exists.
**     3) The gate can be correctly deleted.
**    NOTE: If two gates share the same id the chronologically first one
**          applied will be deleted, and the other ones will remain.  Therefore
**          as with the AEDTSK, the client should avoid duplicate gate ID's.
** Formal Parameters:
**   int spec:
**     Number of spectrum to delete gate from.`
**   int id:
**     Ident of spectrum to delete gate from.
**   grobj_type type:
**      Type of gate to delete.
*/
static void
DeleteGate(int spec, int id, grobj_type type)
{

  /* Ensure the spectrum is defined: */

  spec_type stype = Xamine_SpectrumType(spec); 
  if(stype == undefined) {
    Xamine_DeleteAck(NoSuchSpectrum);
    return; 
  }
  /* Some programs like SMAUG keep separate name spaces for each type of
  ** object, therefore we require a match in type and id to delete.
  */
  int ngates = Xamine_GetSpectrumGateCount(spec);
  for(int i = 0; i < ngates; i++) {
    grobj_generic *object;
    Xamine_GetSpectrumGates(spec, &object,
			    1, 
			    ( i == 0 ) ? True : False);
    if( (type == object->type()) && (id == object->getid())) { /* Found the one */
      grobj_database *db = Xamine_GetGateDatabase();
      db->remove(object);
      Xamine_RedrawSpectrum(spec);	/* Draw spectrum if displayed. */
      Xamine_DeleteAck(Success);
      return;
    }
  }
  /* Control passes down here if there is not a gate of the appropriate id/type  
  ** set on the spectrum.
  */

  Xamine_DeleteAck(NoSuchObject);
}

/*
** Functional Description:
**   ListGates:
**     This function sends the client the set of gates that are defined on
**     a particular spectrum.  The first thing we do is check the legality
**     of the spectrum.  If the spectrum is legal then we send the ack back
**     with the count of the set of gates. This is followed by the gates
**     themselves.
** Formal Parameters:
**   int spec:
**     The spectrum to list.
*/
static void
ListGates(int spec)
{
  grobj_generic *object;
  int           i;

  /* Make certaint the spectrum is defined and ack a failure if not. */

  spec_type stype = Xamine_SpectrumType(spec);
  if(stype == undefined) {
    Xamine_InqAck(NoSuchSpectrum, 0);
    return;
  }
  /* Get the number of gates and send the ack:   */

  int ngates = Xamine_GetSpectrumGateCount(spec);
  int nonPeakGates = 0;

  //
  // Count the number of non peak marker gates:
  //
  for(i = 0; i < ngates; i++)
  {
    Xamine_GetSpectrumGates(spec, &object, 1, 
			    (i == 0) ? True : False);
    if(object->type() != peak1d) nonPeakGates++;
  }
  //
  // Tell the client how many to expect..
  //
  Xamine_InqAck(Success, nonPeakGates);
  //
  //   Then deliver them one at a time.
  //
  for(i = 0; i < ngates; i++) {
    Xamine_GetSpectrumGates(spec, &object, 1, 
			    (i == 0) ? True : False);
    if(object->type() != peak1d)
      Xamine_ListGate(object);
  }
}

/*
** Functional Description:
**   Xamine_ProcessClientRequests:
**      This function is called whenever there is a client request to 
**      process.  Client requests include requests to add gates, delete gates
**      and list the set of existing gates.
**        The loop below is coded as it is mostly in deference to the VMS
**      implementation of Xamine.  In that implementation, Xt input events
**      happen when an event flag is set.  Since we don't want to just hang
**      an event flag terminated read (don't know sizes etc). on the mailbox,
**      the VMS implementation uses write attention AST's.  Following the
**      Write attention AST, the mailbox must be drained or there could be
**      a deadlock condition resulting from a full mailbox with
**      the event flag clear from last time around.
**      In addition because of the drain, it's possible to get called with
**      an empty mailbox and hence the while loop.
** Formal Parameters:
**   XtPointer client_data:
**      In this case a pointer to message queue object that actually triggered
**      the read.  For the VMS implementation we need to clear the event flag
**      here.
**   int source_fd:
**      The source id.  This is either a channel or an event flag depending
**      on the implementation.
**   XtInputId input_id:
**      The input id assigned by Xt to this event.
*/
void
Xamine_ProcessClientRequests(XtPointer client_data,
			     int       *source_fd,
			     XtInputId *input_id)
{
  /* If the implementation is for VMS clear the trigger event flag */

#ifdef VMS
  assert((sys$clref(*source_fd) & 1));
#endif

  /* Process the events:  */

  msg_command cmd;
  grobj_generic *object;
  int spec, id;
  grobj_type type;

  while(Xamine_CheckForCommand(&cmd)) {
    switch(cmd.command) {
    case Enter:
      Xamine_ReadGate(&object);
      if(object) {
	EnterGate(object);
	delete object;
      }
      break;
    case Remove:
      Xamine_ReadDeleteData(&spec, &id, &type);
      DeleteGate(spec, id, type);
      break;
    case Inquire:
      Xamine_ReadInqData(&spec);
      ListGates(spec);
      break;
    case Button_CreateBox:
    case Button_DefineButton:
    case Button_EnableButton:
    case Button_DisableButton:
    case Button_ModifyButton:
    case Button_InquireButton:
    case Button_DeleteButton:
    case Button_DeleteBox:
      Xamine_ProcessButtonRequest(cmd.command);
      break;
    case EnterPeakPosition:	// Accept peak position from client...
      Xamine_ReadPeak(&object); // Read peak from user,  make grobj_Peak1d
      if(object) {
	EnterGate(object);
	delete object;
      }
      break;
    default:
      printf("Invalid client request..\n");
      exit(0);
      break;
    }
  }
#ifdef VMS
  Xamine_ResetGatesAttnAst();
#endif
}
