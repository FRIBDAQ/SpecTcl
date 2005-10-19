
static const char* Copyright = "(C) Copyright Michigan State University 1994, All rights reserved";
/*
** Facility:
**   Xamine   - NSCL spectrum display program.
** Abstract:
**   grobjiomenu.cc  - This file contains callbacks and additional dialog 
**                   functions to handle the user interface for reading
**                   and writing Graphical object files.  
**                   Graphical object file I/O is handled
**                   via FileSelectionBox objects.  The default file filter
**                   is the Current directory with *.grobj appended.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
**/

/*
  Change log:
  $Log$
  Revision 5.1.2.2  2005/10/19 13:39:11  ron-fox
  Fix defect 188: grobj files are now written with spectrum names
  rather than Xamine spectrum ids specifying the spectrum on which they were
  created.  The software will read old files (with ids), and new files with
  names as well..however it will now only write files with names.
  This was inadvertently not fixed when fixing the same issue
  for window definitions file

  Revision 5.1.2.1  2004/12/21 17:51:14  ron-fox
  Port to gcc 3.x compilers.

  Revision 5.1  2004/11/29 16:55:36  ron-fox
  Begin port to 3.x compilers calling this 3.0

  Revision 4.10  2003/11/07 21:32:17  ron-fox
  Unconditionally include config.h

  Revision 4.9  2003/08/25 16:25:30  ron-fox
  Initial starting point for merge with filtering -- this probably does not
  generate a goo spectcl build.

  Revision 4.8  2003/04/23 13:33:58  ron-fox
  Fix double delete of filename in code path where graphical object file
  is not being ovewritten.

*/


/*
** External include files required:
**/

#include <config.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <X11/StringDefs.h>
#include "XMDialogs.h"

#include "helpmenu.h"
#include "grobjiomenu.h"
#include "panemgr.h"
#include "grobjmgr.h"
#include "refreshctl.h"
#include "errormsg.h"
#include "winiomenu.h"

/*
** #Define constants:
*/

#define GROBJ_MASK "*.grobj"	/* Names of window files end like this */
#define GROBJ_EXT  ".grobj"     /* Extension of for grobj files.       */

/*
** Local declarations:
**/

static XMFileListDialog *openbox = NULL;
static XMInformationDialog *help = NULL;

/*
** Saved callback structures:
*/
static Callback_data *ok  = NULL, 
                     *nomatch = NULL;

static char *confirm_help_text[] = {
  "  The file name that you selected for a write graphical objects operation already\n",
  "exists.  If you click on the \"Cancel\" button then the graphical object file will\n",
  "NOT be written out to that file.  If you click on the \"Confirm\" button\n",
  "then the file you chose will bie OVERWRITTEN with the current graphical object\n",
  "configuration\n",
  NULL
  };

static Xamine_help_client_data confirm_help = {
                                         "Grobj_overwrite_confirm",
					 NULL,
					 confirm_help_text
					 };

static char *open_help_text[] = {
  "  This file selection box allows you to choose a graphical object file for the\n",
  "operation you just selected.  You can use the mouse to click on a\n",
  "directory in the list of directories on the left side of the box to\n",
  "see the files available in that directory\n",
  "   You can select a file from the right hand list by double clicking it\n",
  "You can also type in the name of a file in the lower text box.  Hitting\n",
  "<RETURN> or clicking the \"OK\" button will attempt to open that file\n",
  "   The filter string at the top allows you to control which files are\n",
  "visible in the file list.  Note that by convention, Graphical object definition\n",
  "files have the extension .grobj\n\n",
  "  Summary of button usage:\n",
  "        OK     - Accept the file that is currently in the file type-in\n",
  "                 A carriage return will also do this.\n",
  "        FILTER - Update the directory and file lists according to the \n",
  "                 search specification in the Filter type-in\n",
  "        CANCEL - Cancel the open for read or write operation\n",
  "        HELP   - Display this help text\n",
  "  NOTE:\n",
  "     If this is a read operation, then the file selected must be present\n",
  "     If this is a write operation, then if the file is present you will\n",
  "     be given a chance to avoid over-writing it\n",
  NULL
  };



/*
** Functional Description:
**   kill_widget -- Local call back to kill the widget which invoked us:
** Formal Parameters:
**   XMWidget *w:
**     Calling Widget
**   XtPointer client_data:
**     If this is non-null it represents a dynamically allocated resource
**     which must be XtFree'd.
**   XtPointer callback_reason:
**     Callback data structure ignored.
*/
static 
void kill_widget(XMWidget *w, XtPointer client_data, XtPointer callback_reason)
{
  if(client_data) XtFree((char *)client_data);
  delete w;
}

/*
** Functional Description:
**    open_help -- This function is the help callback for opening graphical
**                 object files.
**                 A help dialog is displayed.
** Formal Parameters:
**   XMWidget *w:
**    The help button gadget.
**   XtPointer client_data:
**     Client data which is actually a legitimate parent for the help dialog
**     we must pop up.
**   XtPointer callback_data:
**     Unreferenced callback reason.
*/
static 
  void open_help(XMWidget *w, XtPointer client_data, XtPointer callback_data)
{
  XMWidget *help_parent = (XMWidget *)client_data;

  if(!help) {
    help = Xamine_help("Open_Grobj_file_help",
		       help_parent,
		       open_help_text);
    help->AddCallback(XtNdestroyCallback, NullPointer, (XtPointer)&help);
  }
  else {
    help->Manage();
  }
}

/*
** Functional Description:
**    Xamine_Open_grobj_file -- This function sets up an open on a graphical object
**                               file via a file selection box dialog.
**                               It is intended to be supplied as a callback
**                               function for some other widget.
**                               The client data is a structure which 
**                               describes what to do on the Ok Callback.
**                               (The 'apply' button is a 'filter' button for
**                               file selection boxes).  The value of
**                               the XmNmustMatch resource is also supplied
**                               in this function as that allows the same
**                               code to open for read or write.
** Formal Parameters:
**   XMWidget *w:
**      Widget of the button which invoked us.  This is the Appropriate
**      open button.  We can't use it to anchor children since there may
**      be more than one button involved.  Therefore we anchor children
**      on the file box dialog itself.
**   XtPointer client_data:
**      A pointer to an Xamine_grobjopen_client_data structure which describes
**      how to handle the File selection box dialog.
**   XtPointer callback_data:
**      Unused callback data from the read button.
*/
void Xamine_Open_grobj_file(XMWidget *w, XtPointer client_data, 
			     XtPointer callback_data)
{
  Xamine_grobjopen_client_data *open_data = 
    (Xamine_grobjopen_client_data *)client_data; /* Convert client data struct */

  char *full;



  /* If necessary create the dialog otherwise just manage it: */

  if(!openbox) {
    openbox = new XMFileListDialog("Open_grobj_file", *w);
    openbox->GetHelpButton()->AddCallback(open_help, (XtPointer)openbox);
    openbox->GetHelpButton()->Enable();
    openbox->AddCancelCallback(XMUnmanageChild);
    /* Build the directory search string */

    full = Xamine_GetSearchMask(XAMINE_GROBJ_ENV, XMFILE_DEFAULT_DIR,
			      GROBJ_MASK);
    openbox->DoSearch(full);
    openbox->AddCallback(XtNdestroyCallback, NullPointer, (XtPointer)&openbox);
  }
  else {
    openbox->Manage();
    openbox->DoSearch();
  }

  /* Set the per call characteristics of the dialog: */

  if(ok != NULL) {
    XMRemoveCallback(ok);
    XMRemoveCallback(nomatch);
  }
  ok = openbox->AddOkCallback(open_data->callback, open_data->client_data);
  nomatch = openbox->AddNoMatchCallback(open_data->callback, 
					open_data->client_data);

  open_data->mustexist ? openbox->RestrictChoices() :
                         openbox->NoRestrictChoices();

  /* Free dynamic string resources used: */

}

/**
** Functional Description:
**   Xamine_Read_grobj_file  - This callback is intended to be invoked
**                              when a file has been selected on the
**                              graphical object file open dialog.  The file
**                              must exist or else an error dialog is
**                              popped up indicating that problem.
**                              If the file does exist, then it is opened,
**                              and control is transferred to application
**                              specific software to read in the graphical 
**                              object file.
** Formal Parameters:
**   XMWidget *w:
**     Widget of the calling dialog.
**   XtPointer client_data:
**     The client data for the call. At present unused.
**   XtPointer callback_data:
**     The pointer to a XmFileSeletionBoxCallbackStruct which describes
**     how and why the callback was invoked.
**/
void Xamine_Read_grobj_file(XMWidget *w, XtPointer client_data, 
			     XtPointer callback_data)
{
  XmFileSelectionBoxCallbackStruct *reason = 
    (XmFileSelectionBoxCallbackStruct *)callback_data;

  char *filename;
  struct stat statbuf;
  char error_msg[1024];
  FILE *config;

  /* Convert the name of the file to a C ASCIZ string: */

  if(!XmStringGetLtoR(reason->value, XmSTRING_DEFAULT_CHARSET, &filename)) {
    fprintf(stderr, 
	    "Xamine_Read_grobj_file -- Could not get filename string\n");
    exit(-1);
  }


  /* If there is no "." in the filename, then we add the default
  ** extension:
  */
  if(strrchr(filename, '.') == NULL) {
    char *newfilename = XtMalloc(strlen(filename) + strlen(GROBJ_EXT) + 1);
    if(newfilename == NULL) {
      Xamine_error_msg(Xamine_Getpanemgr(), 
		       "XtMallocFailed in default extenstion processing");
      return;
    }
    strcat(strcpy(newfilename, filename), GROBJ_EXT);
    XtFree(filename);
    filename = newfilename;
  }

  /* If the file was found, then we test for accessibility for read 
  ** If unable to read, then we must pop up a dialog indicating the file
  ** was not readable or funny in some way.
  */
  /* Check for non regular file: */

  if(stat(filename, &statbuf)) {
    sprintf(error_msg,
	    "Error attempting to open file %s:\n%s%s%s",
	    filename,
	    "Xamine was not able to determine the characteristics of the ",
	    "file or file does not exist.\n",
	    strerror(errno));
    new XMErrorDialog("Stat_Failed", *Xamine_Getpanemgr(), error_msg, 
		      kill_widget);
    XtFree(filename);
    return;
  }
  if(!S_ISREG(statbuf.st_mode)) {
    sprintf(error_msg,
	    "Error attempting to open file %s:\n%s%s",
	    filename,
	    "This file is not suitable for use as a graphical object input file\n",
	    "Most likely it is a directory or a pipeline/mailbox file\n");
    new XMErrorDialog("Bad_file",  *Xamine_Getpanemgr(), 
		      error_msg, kill_widget);
    XtFree(filename);
    return;
  }
  /* Check for read accessibility: */

  if((config = fopen(filename, "r")) == NULL) {
    sprintf(error_msg,
	    "Error attempting to open file %s:\n%s%s%s%s%s%s",
	    filename,
	    "You do not have read access to this file.  If you own the\n",
	    "account which owns this file, then change it's protections\n",
	    "so that the account you are running on can read it.  If you do\n",
	    "not own this file, then ask whoever does to change the\n",
	    "protections so that you can read it.\n",
	    strerror(errno));
    new XMErrorDialog("Read_Protected", *Xamine_Getpanemgr(), 
		      error_msg, kill_widget);
    XtFree(filename);
    return;
  }

  /* Read in the graphical objects: */

  if(Xamine_ReadGraphicalObjects(config) == EOF) {
    sprintf(error_msg,
	    "Failed to read graphical objects file %s\n%s",
	    filename,
	    strerror(errno));
    fclose(config);
  }
  else {
    Xamine_UpdateAll(Xamine_Getpanemgr(), NULL, NULL);
    fclose(config);
  }
  XtFree(filename);
  w->UnManage();

  return;
}
Xamine_grobjopen_client_data Xamine_Open_grobj_read =
    { 
      Xamine_Read_grobj_file,
      NULL,
      True
    };

/*
** Functional Description:
**   write_grobjs  -     This internal function is called to 
**                             write a graphical object file out to disk.
**                             It is configured as a callback function
** Formal Parameters:
**    XMWidget *w:
**      Pointer to a widget object which was pushed to activate us.
**    XtPointer client_data:
**      Pointer to ASCIZ string of filename
**    XtPointer callback_data:
**      unused callback information from Motif.
*/
static
void write_grobjs(XMWidget *w, XtPointer client_data, 
		       XtPointer callback_data)
{
  char *filename = (char *)client_data;

  char msg[512];
  FILE *config;

  /*  Attempt to open the file for write...if fails then that's an error
  **  dialog and everything goes down
  */

  if((config = fopen(filename, "w+")) == NULL) {
    sprintf(msg, "Open failed for file: %s\n%s",
	    filename,
	    strerror(errno));
    new XMErrorDialog("Open_Failed",*Xamine_Getpanemgr(),  msg, kill_widget);
    XtFree(filename);
    return;
  }

  if(Xamine_WriteGraphicalObjects(config) == EOF) {
    sprintf(msg, "Write failed for file %s\n%s",
	    filename, strerror(errno));
    new XMErrorDialog("Write_failed", *Xamine_Getpanemgr(), msg, kill_widget);
  }

  fclose(config);
  XtFree(filename);
}

/**
** Functional Description:
**   Xamine_Write_grobj_file  - This callback is intended to be invoked
**                              when a file has been selected on the
**                              graphical object file open dialog.  
**                              If the file exists and is writable,
**                              the user must confirm the write operation.
** Formal Parameters:
**   XMWidget *w:
**     Widget of the calling dialog.
**   XtPointer client_data:
**     The client data for the call. At present unused.
**   XtPointer callback_data:
**     The pointer to a XmFileSeletionBoxCallbackStruct which describes
**     how and why the callback was invoked.
**/
void Xamine_Write_grobj_file(XMWidget *w, XtPointer client_data, 
			     XtPointer callback_data)
{
  XmFileSelectionBoxCallbackStruct *reason = 
    (XmFileSelectionBoxCallbackStruct *)callback_data;
  XMWarningDialog *overwrite;

  char *filename;
  struct stat statbuf;
  char error_msg[1024];

  /* Convert the name of the file to a C ASCIZ string: */

  if(!XmStringGetLtoR(reason->value, XmSTRING_DEFAULT_CHARSET, &filename)) {
    fprintf(stderr, 
	    "Xamine_Write_grobj_file -- Could not get filename string\n");
    exit(-1);
  }


  /* If there is no "." in the filename, then we add the default
  ** extension:
  */
  if(strrchr(filename, '.') == NULL) {
    char *newfilename = XtMalloc(strlen(filename) + strlen(GROBJ_EXT) + 1);
    if(newfilename == NULL) {
      Xamine_error_msg(Xamine_Getpanemgr(), 
		       "XtMallocFailed in default extenstion processing");
      return;
    }
    strcat(strcpy(newfilename, filename), GROBJ_EXT);
    XtFree(filename);
    filename = newfilename;
  }
 

  /* If the file was found, then we test for accessibility for write 
  ** If unable to write, then we must pop up a dialog indicating the file
  ** was not writeable or funny in some way.
  */
  /* Check for non regular file: */

  if(!stat(filename, &statbuf)) {
    if(!S_ISREG(statbuf.st_mode)) {
      sprintf(error_msg,
	      "Error attempting to open file %s:\n%s%s",
	      filename,
	      "This file is not suitable for use as a graphical object definition file\n",
	      "Most likely it is a directory or a pipeline/mailbox file\n");
      new XMErrorDialog("Bad_file",  *Xamine_Getpanemgr(), error_msg, kill_widget);
      XtFree(filename);
      return;
    }
  /* Check for write accessibility: */

  if(access(filename, W_OK)) {
    sprintf(error_msg,
	    "Error attempting to open file %s:\n%s%s%s%s%s%s",
	    filename,
	    "You do not have write access to this file.  If you own the\n",
	    "account which owns this file, then change it's protections\n",
	    "so that the account you are running on can write it.  If you do\n",
	    "not own this file, then ask whoever does to change the\n",
	    "protections so that you can write it.\n",
	    strerror(errno));
    new XMErrorDialog("Write_Protected", *Xamine_Getpanemgr(), 
		      error_msg, kill_widget);
    XtFree(filename);
    return;
  }
    
    
    sprintf(error_msg,
	  "WARNING: %s exists:\n%s",
	  filename,
	  "Click \"Overwrite\" to overwrite it or click \"Cancel\" to abort\n"
	  );
    overwrite = new  XMWarningDialog("Good_file", *Xamine_Getpanemgr(), 
				   error_msg, write_grobjs, 
				   (XtPointer)filename);
    overwrite->AddOkCallback(kill_widget); /* After write... kill off dialog */
    overwrite->LabelOkButton("Overwrite");
    overwrite->LabelCancelButton("Cancel");
    overwrite->AddCancelCallback(kill_widget, filename);
    overwrite->GetCancelButton()->Enable();
    overwrite->DefaultToCancel();
    overwrite->GetHelpButton()->AddCallback(Xamine_display_help,
					  (XtPointer)&confirm_help);
    overwrite->GetHelpButton()->Enable();

  }
  else if(errno != ENOENT) {
    sprintf(error_msg,
	    "Error attempting to open file %s:\n%s%s%s",
	    filename,
	    "Xamine was not able to determine the characteristics of the ",
	    "file\n",
	    strerror(errno));
    new XMErrorDialog("Stat_Failed", *Xamine_Getpanemgr(), error_msg, 
		      kill_widget);
    XtFree(filename);
    return;
  }
  else { 
    write_grobjs(w, filename, NULL);
    w->UnManage();
    return;
  }

  w->UnManage();

  return;
}
Xamine_grobjopen_client_data Xamine_Open_grobj_write =
    { 
      Xamine_Write_grobj_file,
      NULL,
      True
    };
