/*
** Facility:
**   Xamine   - NSCL spectrum display program.
** Abstract:
**   winiomenu.cc  - This file contains callbacks and additional dialog 
**                   functions to handle the user interface for reading
**                   and writing window files.  Window file I/O is handled
**                   via FileSelectionBox objects.  The default file filter
**                   is the Current directory with *.win appended.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
**/
#ifdef unix
#pragma implementation "XMCallback.h"
#endif
static char *sccsinfo="@(#)winiomenu.cc	8.5 6/25/97 \n";

/*
** External include files required:
**/

#include <stdio.h>
#ifdef unix
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
inline int remove(const char *path) { /* VMS compatibility.  */
  return unlink(path); 
}
#endif
#ifdef VMS
#include <types.h>
#include <stat.h>
#include <unixio.h>
#include <string.h>
#ifndef S_ISREG
#define S_ISREG(mode) ( (S_IFREG & (mode)) != 0)
#endif
#ifndef S_ISDIR
#define S_ISDIR(mode) ( (S_IFDIR & (mode)) != 0)
#endif
#endif
#include <errno.h>
#include <X11/StringDefs.h>
#include "XMDialogs.h"

#include "helpmenu.h"
#include "winiomenu.h"
#include "dispwind.h"
#include "panemgr.h"
#include "exit.h"
#include "refreshctl.h"
#include "errormsg.h"
#include "buttonsetup.h"
#include "menusetup.h"
#include <string>		// STL String so I get around length problems.
/*
** #Define constants:
*/

#define WINDOW_MASK "*.win"	/* Names of window files end like this */
#define WINDOW_EXT  ".win"	/* Default extension for window files. */


/*
** Local declarations:
**/
static Callback_data *okcb = NULL, *nomatch = NULL;
static std::string   LastFilename;

static XMFileListDialog *openbox = NULL;
static XMInformationDialog *help = NULL;

static char *confirm_help_text[] = {
  "  The file name that you selected for a write windows operation already\n",
  "exists.  If you click on the \"Cancel\" button then the windows file will\n",
  "NOT be written out to that file.  If you click on the \"Confirm\" button\n",
  "then the file you chose will bie OVERWRITTEN with the current window\n",
  "configuration\n",
  NULL
  };

static Xamine_help_client_data confirm_help = {
                                         "Window_overwrite_confirm",
					 NULL,
					 confirm_help_text
					 };

static char *open_help_text[] = {
  "  This file selection box allows you to choose a window file for the\n",
  "operation you just selected.  You can use the mouse to click on a\n",
  "directory in the list of directories on the left side of the box to\n",
  "see the files available in that directory\n",
  "   You can select a file from the right hand list by double clicking it\n",
  "You can also type in the name of a file in the lower text box.  Hitting\n",
  "<RETURN> or clicking the \"OK\" button will attempt to open that file\n",
  "   The filter string at the top allows you to control which files are\n",
  "visible in the file list.  Note that by convention, window definition\n",
  "files have the extension .win\n\n",
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
**   Xamine_GetSearchMask:
**     This function builds a search mask up out of a few elements, some of
**     which are optional:
** Formal Parameters:
**    char *envstr:
**       If not null, this is an environment variable which when translated
**       points to the directory part of the search mask (sans trailing / in
**       unix.  If NULL, then the environment variable is not used/allowed.
**    char *fallbackdir:
**       Provides the explicit directory to be used when either the environment
**       string is not provided or does not yield a valid directory.
**    char *mask:
**       Provides the filename part of the search mask (e.g. *.win).
** Returns:
**    Pointer to a search mask.
** Restrictions:
**    The directory + mask combination cannot be longer than 132 chars or
**    truncation will occur.
*/
#define MAX_SEARCHMASK 132
char *Xamine_GetSearchMask(char *envstr, char *fallbackdir, char *mask)
{
  static char searchmask[MAX_SEARCHMASK+1];
  memset(searchmask, 0, sizeof(searchmask));

  /*  Figure out the directory string from the environment string if it's
  **  Supplied and valid.
  */

  char *dstring = NULL;
  if(envstr) {
    dstring = getenv(envstr);
    if(dstring) {
      struct stat statinfo;
#ifdef unix
      if(stat(dstring, &statinfo)) {	/* STAT failed. */
	dstring = NULL;		/* Let the fallback take over. */
      }
      else {			/* Make sure the dstring specs a directory */
	if(!S_ISDIR(statinfo.st_mode)) {
	  dstring = NULL;
	} 
      }
#endif
    }
  }
  if(!dstring) dstring = fallbackdir;

  /* String together the pieces: */

  strncpy(searchmask, dstring, MAX_SEARCHMASK);
#ifdef unix
  strncat(searchmask, "/", MAX_SEARCHMASK);
#endif
  strncat(searchmask, mask, MAX_SEARCHMASK);
  return searchmask;
}


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
**    open_help -- This function is the help callback for opening window files.
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
    help = Xamine_help("Open_window_file_help",
		       help_parent,
		       open_help_text);
  }
  else {
    help->Manage();
  }
}

/*
** Functional Description:
**    Xamine_Open_window_file -- This function sets up an open on a window
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
**      A pointer to an Xamine_winopen_client_data structure which describes
**      how to handle the File selection box dialog.
**   XtPointer callback_data:
**      Unused callback data from the read button.
*/
void Xamine_Open_window_file(XMWidget *w, XtPointer client_data, 
			     XtPointer callback_data)
{
  Xamine_winopen_client_data *open_data = 
    (Xamine_winopen_client_data *)client_data; /* Convert client data struct */

  char *full;
  int full_len;


  /* If necessary create the dialog otherwise just manage it: */

  if(!openbox) {
    openbox = new XMFileListDialog("Open_windows_file", *w);
    openbox->GetHelpButton()->AddCallback(open_help, (XtPointer)openbox);
    openbox->GetHelpButton()->Enable();
    openbox->AddCancelCallback(XMUnmanageChild);

    /* Build the directory search string */
    
    full = Xamine_GetSearchMask(XAMINE_WINDOW_ENV, XMFILE_DEFAULT_DIR, 
				WINDOW_MASK);
    openbox->DoSearch(full);
  }
  else {
    openbox->Manage();
    openbox->DoSearch();	// Update the current search stuff.
  }

  /* Set the per call characteristics of the dialog: */

  if(okcb) {
    XMRemoveCallback(okcb);
    XMRemoveCallback(nomatch);
  }
  okcb = openbox->AddOkCallback(open_data->callback, open_data->client_data);
  nomatch = openbox->AddNoMatchCallback(open_data->callback, 
					open_data->client_data);

  open_data->mustexist ? openbox->RestrictChoices() :
                         openbox->NoRestrictChoices();

}

/*
** Functional Description:
**   read_windows:
**     This function actually reads in a new windows database file and
**     sets it into the pane database.
**  Formal Parameters:
**    char *filename:
**       Name of the file to read.
**  Returns:
**    True -- Successful read.
**    False-- Failed read.
*/
static Boolean read_windows(char *filename)
{
  win_db windows;

  /* First read in the windows file: */

  if(!windows.read(filename)) return False;


  /* Set the geometry from the current windows  file: */

  
  int nx = windows.nx();
  int ny = windows.ny();
  Xamine_CancelUpdates();	// Kill off any pending updates.. since they 
                                // refer to dead drawables probably.
  Xamine_SetPaneGeometry(nx,ny);
  /* For each window define the spectrum and attributes: */
  /* We also schedule an update in order to make the spectrum visible */
    for(int col = 0; col < nx; col++) {
     for( int row = 0; row < ny; row++) {
      if(windows.defined(col, row) && (windows.getdef(col,row) != NULL)) {
	win_attributed *def = windows.getdef(col, row);
	Xamine_SetDisplay(row, col, def->spectrum()); 
	Xamine_SetDisplayAttributes(row, col, def);
	Xamine_RedrawPane(col, row);                
	
	/* If the pane is on timed update, then schedule that too. */

	if(def->update_interval() != 0) {
	  Xamine_ScheduleTimedUpdate(Xamine_GetDrawingArea(row, col),
				     col, row,
				     def->update_interval());
	}
      }
    }
  } 
  /* The zoom toggle state must now be set to match the zoom state of the
  ** window file since the window file can be zoomed in too.
  */

  Xamine_SelectPane(0,0);		// Select the upper left pane.
  Xamine_SetButtonBarZoom(windows.iszoomed());
  Xamine_SetZoomToggleButtonState(windows.iszoomed());

  return True;
}

/**
** Functional Description:
**   Xamine_Read_window_file  - This callback is intended to be invoked
**                              when a file has been selected on the
**                              window file open dialog.  The file
**                              must exist or else an error dialog is
**                              popped up indicating that problem.
**                              If the file does exist, then it is opened,
**                              and control is transferred to application
**                              specific software to read in the windows file.
** Formal Parameters:
**   XMWidget *w:
**     Widget of the calling dialog.
**   XtPointer client_data:
**     The client data for the call. At present unused.
**   XtPointer callback_data:
**     The pointer to a XmFileSeletionBoxCallbackStruct which describes
**     how and why the callback was invoked.
**/
void Xamine_Read_window_file(XMWidget *w, XtPointer client_data, 
			     XtPointer callback_data)
{
  XmFileSelectionBoxCallbackStruct *reason = 
    (XmFileSelectionBoxCallbackStruct *)callback_data;

  char *filename;
  struct stat statbuf;
  char error_msg[1024];
  FILE *fp;

  /* Convert the name of the file to a C ASCIZ string: */
  if(!XmStringGetLtoR(reason->value, XmSTRING_DEFAULT_CHARSET, &filename)) {
    sprintf(error_msg, 
	    "Xamine_Read_window_file -- Could not get filename from prompter");
    new XMErrorDialog("Failure", *Xamine_Getpanemgr(), error_msg, kill_widget);
    return;
  }

  /* If there is no "." in the filename, then we add the default
  ** extension:
  */
  if(strrchr(filename, '.') == NULL) {
    char *newfilename = XtMalloc(strlen(filename) + strlen(WINDOW_EXT) + 1);
    if(newfilename == NULL) {
      Xamine_error_msg(Xamine_Getpanemgr(), 
		       "XtMallocFailed in default extenstion processing");
      return;
    }
    strcat(strcpy(newfilename, filename), WINDOW_EXT);
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
	    "Error attempting to stat file %s:\n%s%s%s",
	    filename,
	    "Xamine was not able to determine the characteristics of the ",
	    "file\n",
	    strerror(errno));
    new XMErrorDialog("Stat_Failed", *Xamine_Getpanemgr(), error_msg, 
		      kill_widget);
    XtFree(filename);
    return;
  }
  if(!S_ISREG(statbuf.st_mode)) {
    sprintf(error_msg,
	    "Error attempting to open file %s:\n%s%s%s",
	    filename,
	    " File does not exist or is not suitable for use as a\n",
	    "windows input file.\n",
	    "Possibly it is a directory or a pipeline/mailbox file\n");
    new XMErrorDialog("Bad_file",  *Xamine_Getpanemgr(), error_msg, kill_widget);
    XtFree(filename);
    return;
  }
  /* Check for read accessibility: */

  if((fp = fopen(filename, "r")) == NULL) {
    sprintf(error_msg,
	    "Error attempting to open file %s:\n%s%s%s%s%s%s",
	    filename,
	    "You do not have read access to this file or it does not exist.\n",
	    "If you own the account which owns this file, then change it's\n",
	    "protections so that the account you are running on can read it.\n",
	    "If you do not own this file, then ask whoever does to change the\n",
	    "protections so that you can read it.\n",
	    strerror(errno));
    new XMErrorDialog("Read_Protected", *Xamine_Getpanemgr(), 
		      error_msg, kill_widget);
    XtFree(filename);
    return;
  }
  fclose(fp);

  /* Read in the window file.  */

  if(read_windows(filename)) {
    std::string title("Xamine -- ");
    title += filename;

    LastFilename = filename;
    SetWindowLabel(w->getid(), (char*)title.c_str());
    Xamine_SavedWindows();	/* Reading is like saving */
  }
  XtFree(filename);
  w->UnManage();

  return;

}
Xamine_winopen_client_data Xamine_Open_win_read =
    { 
      Xamine_Read_window_file,
      NULL,
      True
    };

/*
** Functional Description:
**   write_windows  -     This internal function is called to 
**                             write a windows file out to disk.
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
void write_windows(XMWidget *w, XtPointer client_data, 
		       XtPointer callback_data)
{
  char *filename = (char *)client_data;

  char msg[512];
  FILE *config;


  /*  Attempt to open the file for write...if fails then that's an error
  **  dialog and everything goes down
  */
  if( (config = fopen(filename, "w")) == NULL) {
    if(errno != EEXIST) {
      sprintf(msg, "Open failed for file: %s\n%s",
	      filename,
	      strerror(errno));
      new XMErrorDialog("Open_Failed", 
			*Xamine_Getpanemgr(),  msg, kill_widget);
      return;
    }
  }
  if(config != NULL) fclose(config); /* Allow for VMS open failure here. */
  if(remove(filename)) {	/* Remove the file prior to writing for VMS */
    sprintf(msg, "Failed to remove temp file %s\n%s", filename,
    strerror(errno));
    new XMErrorDialog("Remove_Failed", *Xamine_Getpanemgr(), msg, kill_widget);
  }

  /* Now write the pane database out to file: */

  pane_db *db = Xamine_GetPaneDb();

  if(!(db->win_db::write(filename))) {
    sprintf(msg, "Failed to write window file %s\n%s",
	    filename,
	    strerror(errno));
    new XMErrorDialog("Write_failed", *Xamine_Getpanemgr(), msg, kill_widget);
  }
  else {
    std::string title("Xamine -- ");
    LastFilename = filename;
    title += filename;

    SetWindowLabel(openbox->getid(), (char*)title.c_str());  /* Set the shell title banner. */
    Xamine_SavedWindows();
  }


}

/**
** Functional Description:
**   Xamine_Write_window_file  - This callback is intended to be invoked
**                              when a file has been selected on the
**                              window file open dialog.  
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
void Xamine_Write_window_file(XMWidget *w, XtPointer client_data, 
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
    sprintf(error_msg, 
	    "Xamine_Write_window_file -- Could not get filename string\n");
    new XMErrorDialog("String_GetFailed", *Xamine_Getpanemgr(), error_msg,
		      kill_widget);
    return;
  }

  /* If there is no "." in the filename, then we add the default
  ** extension:
  */
  if(strrchr(filename, '.') == NULL) {
    char *newfilename = XtMalloc(strlen(filename) + strlen(WINDOW_EXT) + 1);
    if(newfilename == NULL) {
      Xamine_error_msg(Xamine_Getpanemgr(), 
		       "XtMallocFailed in default extenstion processing");
      return;
    }
    strcat(strcpy(newfilename, filename), WINDOW_EXT);
    XtFree(filename);
    filename = newfilename;
  }

  /*  Test for accessibility for write.  If the file does not exist, then
  ** we write unconditionally.
  ** If unable to write, then we must pop up a dialog indicating the file
  ** was not writeable or funny in some way.
  */
  /* Check for non regular file: */
  
  if(!stat(filename, &statbuf)) { /* Stat worked... */
    if(!S_ISREG(statbuf.st_mode)) {
      sprintf(error_msg,
	      "Error attempting to open file %s:\n%s%s",
	      filename,
	      "This file is not suitable for use as a window definition file\n",
	      "Most likely it is a directory or a pipeline/mailbox file\n");
      new XMErrorDialog("Bad_file",  
			*Xamine_Getpanemgr(), error_msg, kill_widget);
      XtFree(filename);
      return;
    }				/* Control lands here if file ok to write */
    sprintf(error_msg,		/* but exists.                            */
	    "WARNING: %s exists:\n%s",
	    filename,
	    "Click \"Overwrite\" to overwrite it or click \"Cancel\" to abort\n"
	    );
    overwrite = new  XMWarningDialog("Good_file", *Xamine_Getpanemgr(), 
				     error_msg, write_windows, 
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
  else if(errno != ENOENT) {	/* Stat failed on existing file. */
    sprintf(error_msg,
	    "Error attempting to stat file %s:\n%s%s%s",
	    filename,
	    "Xamine was not able to determine the characteristics of the ",
	    "file\n",
	    strerror(errno));
    new XMErrorDialog("Stat_Failed", *Xamine_Getpanemgr(), error_msg, 
		      kill_widget);
    XtFree(filename);
    return;
  }
  else {			/* File does not exist if here: */
    write_windows(w, filename, NULL);	/* So it's always safe to write. */
    XtFree(filename);
  }
  w->UnManage();
  
  return;
}
Xamine_winopen_client_data Xamine_Open_win_write =
{ 
  Xamine_Write_window_file,
  NULL,
  True
  };

/*
 ** Functional Description:
 **   Xamine_GetLastWIndowFile:
 **    Returns a character pointer to the last window file opened.
 */
char *Xamine_GetLastWindowFile()
{
  return (char*)LastFilename.c_str();
}

