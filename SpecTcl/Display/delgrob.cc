/*
** Facility:
**   Xamine -- NSCL Display program.
** Abstract:
**   delgrob.cc:
**     This file contains code which prompts for and deletes a graphical
**     object.  This is done by presenting a Selection box dialog which
**     contains all of the objects on the currently selected spectrum.
**     The user then either selects an entry from the box or types it in.
** Author:
**   ROn Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/
#ifdef unix
#pragma implementation "XMCallback.h"
#endif
static char *sccsinfo = "@(#)delgrob.cc	8.1 6/23/95 ";


/*
** Include files:
*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>


#include "XMDialogs.h"

#include "dispgrob.h"
#include "dispwind.h"
#include "helpmenu.h"
#include "delgrob.h"
#include "grobjmgr.h"
#include "panemgr.h"
#include "errormsg.h"
#include "refreshctl.h"
/*
** External references:
*/
char *upcase(char *s);

/*
** Below we define the help text for the dialog:
*/
static char *help_text[] = {
  "  This dialog deletes graphical objects.  The list at the top of the\n",
  "work area is the set of deletable graphical objects defined on the\n",
  "spectrum at the time this dialog was popped up.  To refresh the list\n",
  "request the delete function again.  You may either click on the object\n",
  "you'd like to delete or type it's name or ID number in the prompt line\n",
  "at the bottom of the work area.\n\n",
  "   The buttons at the bottom of the dialog (action area) have the\n",
  "following meanings: \n\n",
  "     Ok       - Delete the indicated graphical object and dismiss the\n",
  "                dialog.\n",
  "     Apply    - Delete the indicated graphical object and do not dismiss\n",
  "                the dialog.\n",
  "     Cancel   - Don't delete anything and dismiss the graphical object\n",
  "     Help     - Display this text\n",
  NULL
  };

static Xamine_help_client_data help = { "Delete_grobj_help",
					NULL,
					help_text
					};
/*
** Below are type flag characters used to identify individual object types
*/
static char typechars[] = " CSBCMSM";
/*
** Keep track of the current spectrum too:
*/
static int specid;
/*
** Below is the selection dialog pointer which must be instantiated the first
** call through.
*/
static XMSelectionDialog *dialog = NULL;

/*
** Functional Description:
**  SetObjectList:
**    This function builds a selection list of the objects which are
**    available to the choice widget.
** Formal Parameters:
**   XMSelectionDialog *d:
**     Dialog into which the list is built.
**   int specid:
**     Number of the spectrum for which the objects are being built.
** Returns:
**    Number of objects in list.
*/
static int SetObjectList(XMSelectionDialog *d, int specid)
{

  /* First build the list of names.  Each name is constructed from the
  ** object type, the object id and object name.
  */

  int nobject = Xamine_GetSpectrumObjectCount(specid);
  if(nobject == 0) {
    d->SetSelectionList(0, NULL);
    return 0;
  }

  grobj_name    aname;
  char          *names[GROBJ_MAXOBJECTS];
  grobj_generic *objects[GROBJ_MAXOBJECTS];
  assert(sizeof(names) == (GROBJ_MAXOBJECTS * sizeof(char *)));
  assert(sizeof(objects) == (GROBJ_MAXOBJECTS * sizeof(grobj_generic *)));
  Xamine_GetSpectrumObjects(specid,
			    objects, GROBJ_MAXOBJECTS,
			    True);

  int i;
  for(i = 0; i < nobject; i++) {
    int l = strlen(objects[i]->getname(aname)) + 10;
    names[i] = new char[l];
    sprintf(names[i], "[%c%d] %s",
	    typechars[objects[i]->type()],
	    objects[i]->getid(),
	    aname);
      
  }
  /* Hand the name list over to the selection box dialog widget for display */

  d->SetSelectionList((Cardinal)nobject, names);

  /* Delete our copy of the list. */

  for(i = 0; i < nobject; i++) {
    delete [] names[i];
  }
  return nobject;
}

/* Functional Description:
**   FindObject:
**     This function locates an object in the graphical object database by
**     name and spectrum id and returns the object id to the caller.
**     Note that name comparisons are done case blind but multiple
**     blanks are significant.  
** Formal Parameters:
**     int specid:
**       Number of the spectrum to check.
**     char *name:
**       The object name... note, this is turned into upper case.
**     int *id:
**       integer to hold the object ident.
** Returns:
**    True if a match was found, False otherwise.
*/
Boolean FindObject(int specid, char *name, int *id)
{
  grobj_name aname;
  grobj_database *db = Xamine_GetObjectDatabase();
  grobj_generic *o;
  upcase(name);

  o =  db->find_first(specid);	/* Start the search. */
  while(o != NULL) {
    upcase(o->getname(aname));	/* Get the object name. */
    if(strcmp(aname, name) == 0) {
      *id = o->getid();
      return True;
    }
    o = db->find_next();
  }
  return False;
}

/* 
** Functional Description:
**   GetGrobjId:
**     Get the graphical object identifier associated with an id string
**     of the form [tnum]  name
**     where t = object type.
**          num= Ojbect id number.
** Formal Parameters;
**   char *s:
**     Name string.
**   int *id:
**     POinter to place to stuff id.
** Returns:
**   True on success... that is if the object fits that format.
**   False if not.
*/
Boolean GetGrobjId(char *n, int *id)
{
  /* Skip leading whitespace.  */

  while(isspace(*n)) n++;
  if(strlen(n) == 0) return False; /* Failed if no more string. */

  /* Fail if the first character is not a [: */
  /* Skip 2 chars if so.  (the [ and object type code.            */

  if(*n != '[') return False;
  n++;if(*n == '\000') return False; /* Skip char fail if no more. */
  n++;if(*n == '\000') return False; /* Skip char fail if no more. */

  /* Try to get the id: */

  if(sscanf(n, "%d", id) != 1) return False;
  return True;
}

/* Functional Description:
**    DeleteObject:
**     This function is called when it's actually time to delete an object
**     from the list of graphical objects associated with a spectrum.
** Formal Parameters:
**    XmString name:
**       This is the motif compound string which is represents what the user
**       selected or typed in.
**    int specid:
**       The id of the spectrum that we're deleting objects from.
** Returns:
**    True on success, False on failure.
*/
static Boolean DeleteObject(XmString name, int specid)
{
  /* First convert the string to an ASCIZ string. */

  char *cname;
  if(!XmStringGetLtoR(name, XmSTRING_DEFAULT_CHARSET, &cname)) {
    Xamine_error_msg(Xamine_Getpanemgr(), 
		     "DeleteObject -- Unable to get selection from Motif");
    return False;
  }

  /* Next we try to decode the name. First we try to decode the form
  ** Expected from a double click on a name:
  */
  int id;
  grobj_name n;
  char       junk[80];

  if(!GetGrobjId(cname, &id)) {	/* Try to get id from string itself. */

    /* Try to pick off the ID by just sscanf ing it */

    if(sscanf(cname, "%d", &id) != 1) {
      
      /* treat cname as a name string and try to get a matching object id */

      if(!FindObject(specid, cname, &id)) {
	XtFree(cname);		/* Free the string. */
	Xamine_error_msg(Xamine_Getpanemgr(),
		 "No such graphical object. Please choose one from the list");
	return False;
      }
    }
  }
  XtFree(cname);		/* Free the string storage. */

  /* When control passes here, id contains the id of the object to delete */
  /* We locate and delete the object                                      */

  if(!Xamine_DeleteGrobj(specid, id)) {
    Xamine_error_msg(Xamine_Getpanemgr(),
	     "Graphical object no longer exists in the database");
    return False;
  }
  return True;
}

/*
** Functional Description:
**   TakeAction:
**      This function is called under the following conditions:
**        Cancel Button Clicked:
**           We unmanage the dialog and return doing nothing else.
**        Apply Button Clicked:
**           We decode the selection and if it exists, delete the corresponding
**           graphical object.
**        Ok Button Clicked:
**           Same as apply, but after words we unmanage the dialog.
**        Double clicked name or CR in text field:
**           indistinguishable from default button which is Ok.
**      These cases are distinguishable by looking at the call data which
**      contains a reason field defined as follows:
**        XmCR_APPLY  - apply button clicked.
**        XmCR_CANCEL - Cancel button clicked.
**        XmCR_OK     - OK button clicked.
**     The XmCR_NOMATCH option is not enabled at this time... and triggers
**     a bug message for now.
** Formal Parameters:
**   XMWidget *w:
**     Actually points to an XMSelectionDialog widget which is invoking the
**     callback.
**   XtPointer ud:
**      Unused user data.
**   XtPointer cd:
**     Call data which in this case is a pointer to an 
**     XmSelectionBoxCallbackStruct.
*/
static void TakeAction(XMWidget *w, XtPointer ud, XtPointer cd) 
{
  /* First cast the argument list entries to approprate types: */

  XMSelectionDialog              *d = (XMSelectionDialog *)w;
  XmSelectionBoxCallbackStruct *cbd = (XmSelectionBoxCallbackStruct *)cd;

  /* Get the spectrum id by looking at the current spectrum. */

  

  /* Next we branch out depending on the type of the callback. If we have
  ** a callback that we don't handle then that's considered a non fatal error
  */

  switch(cbd->reason) {
  case XmCR_CANCEL:		/* Cancel button clicked. */
    d->UnManage();
    break;;
  case XmCR_OK:
    if(DeleteObject(cbd->value, specid)) {
      d->SetText("");
      d->UnManage();
      Xamine_RedrawSelectedPane();
    }
    break;
  case XmCR_APPLY:
    if(DeleteObject(cbd->value, specid)) {
      if(SetObjectList(d, specid) <= 0) d->UnManage();
      d->SetText("");
      Xamine_RedrawSelectedPane();
    }
    break;
  case XmCR_NO_MATCH:
    Xamine_error_msg(Xamine_Getpanemgr(),
	     "Non-fatal-bug: NOMATCH callback reason present [delobj].");
    break;
  default:
    Xamine_error_msg(Xamine_Getpanemgr(),
	     "Non-fatal-bug: Invalid callback reason present [delobj]. ");
    break;
  }
  return;
}


/*
** Functional Description:
**   Xamine_DeleteObject:
**      This function prompts for a graphical object to delete from the
**      local graphical object database.  Only summing regions and markers
**      are elligible for deletion.  Histogrammer contributed graphical
**      objects may not be deleted since that would require coordination
**      with the histogrammer which is not supported by the AEDTSK.
** Formal Parameters:
**   XMWidget *w:
**     Widget which invoked us.. most likely some button somewhere.
**     Used as the parent widget for the dialog.
**   XtPointer ud, cd:
**     User and call data which are ignored.
*/
void Xamine_DeleteObject(XMWidget *w, XtPointer ud, XtPointer cd)
{
  /* Get the list of graphical objects.  Note that if there are none, then */
  /* we pop up an error message and return.                                */

  XMWidget       *m  = Xamine_Getpanemgr();
  win_attributed *a  = Xamine_GetSelectedDisplayAttributes();
  if(a == NULL) {
    Xamine_error_msg(m,
		     "There is no spectrum in this pane");
    return;
  }
   specid = a->spectrum();
  int nobject= Xamine_GetSpectrumObjectCount(specid);
  if(nobject == 0) {
    Xamine_error_msg(m,
		     "There are no grahpical objects on this spectrum");
    return;
  }

  

  /* If the dialog is NULL, then it must be created.  */

  if(dialog == NULL) {
    dialog = new XMSelectionDialog("Delete_Grobj",
				   *w,
				   "Object name or ID",
				   TakeAction);
    /* Add callbacsk for cancel and help: */

    dialog->AddCancelCallback(TakeAction);
    dialog->GetHelpButton()->Enable();
    dialog->GetHelpButton()->AddCallback(Xamine_display_help, (XtPointer)&help);

    
  }
  /* Now set the list of objects and manage the selection box. */

  SetObjectList(dialog, specid);

  dialog->Manage();

}



