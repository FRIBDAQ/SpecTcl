/*
** Facility:
**   Xamine -- NSCL display program.
** Abstract:
**   logging.cc   - This file implements methods in LogFile class.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/
static char *sccsinfo ="@(#)logging.cc	8.1 6/23/95 \n";


/*
** Include files:
*/
#include <X11/Intrinsic.h>

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#ifdef ultrix
extern "C"
{
   char *ctime(const time_t *);
   time_t time(time_t *);
}
#endif


#define _LOGGING_CC

#include "errormsg.h"
#include "XMDialogs.h"
#include "helpmenu.h"
#include "logging.h"

/*
** Globally visible things:
*/

Boolean Xamine_logging;
char    Xamine_logfile[BUFSIZ] = "Xamine.log";
LogFile Xamine_log;

/*
** Functional Description:
**    stamp     - This function creates a time stamp string.
** Formal Parameters:
**    char *ts:
**       Points to a string buffer to hold the time stamp string
*/
static void stamp(char *ts)
{
  time_t t;
  time_t at;

  at = time(&t);			/* Get internal time. */

  strcpy(ts, ctime(&t));

}

/*
** Method Description:
**   LogFile::LogMessage -- Enters a time stamped message in a log file.
**                          We're always trying to keep files closed so
**                          this function will open for append and close to
**                          bracket the insertion.
** Formal Parameters:
**    char *text:
**      The text to log to file.
** Returns:
**    Number of characters written or -1 for failure.
*/
int LogFile::LogMessage(char *text)
{
  FILE *fp;
  int  bytes = 0;
  char timestamp[BUFSIZ];

  /* Open the log file for append */

  fp = fopen(Xamine_logfile, "a+");
  if(fp == NULL) {
    perror(Xamine_logfile);
    return -1;
  }

  /* Create and write the timestamp: */

  stamp(timestamp);		/* Fill in the timestamp. */
  if(fwrite(timestamp, strlen(timestamp)+1, 1, fp) != 1) goto error;
  bytes += strlen(timestamp)+1;

  /* Write the user's message */

  if(fwrite(text, strlen(text)+1, 1, fp) != 1) goto error;
  bytes += strlen(timestamp)+1;
  if(putc('\n', fp) == EOF) goto error;

  bytes++;

  /* Close the log file and return the number of bytes written */

  fclose(fp);
  return bytes;

 error:
  fclose(fp);
  return -1;
}

/*
** Functional Description:
**   LogFile::ContinueLogging:
**     This is called after an initial entry is made to continue a log
**     entry.  Essentially we just write the log file entry without bothering
**     to prepend a timestamp.
** Formal Parameters:
**    char *txt:
**      Log file message.
** Returns:
**     Number of bytes written or -1 if error with errno showing why.
*/
int LogFile::ContinueMessage(char *txt)
{
  FILE *fp;

  fp = fopen(Xamine_logfile, "a");
  if(fp == NULL) {
    perror(Xamine_logfile);
    return -1;
  }

  if(fwrite(txt, strlen(txt), 1, fp) != 1) {
    fclose(fp);
    return -1;
  }
  fclose(fp);
  return strlen(txt);
}


/*
** The code below is all involved with setting up the characteristics
** of the log file.  In particular the log file name.  This is prompted
** for using a Prompt dialog.
*/
static char *help_text[] = {
  "  This dialog is prompting you for the name of a new log file.  If the\n",
  "file specified already exists, it will be renamed to to it's old name \n",
  "followed by _SAVED.  A new empty log file will be created.\n",
  "   After you have specified the desired log file name, you should",
  "click on \n\n",
  "    OK      to accept the change and dismiss the dialog.\n",
  "    Cancel  to make no changes\n",
  "Clicking\n\n",
  "    Help    Displays this message\n\n",
  "NOTE:\n",
  "   If you click Ok without changing the filename, then a new version of\n",
  "the log file is opened after renaming the old version\n",
  NULL
  };

static Xamine_help_client_data help = {"Log_file_help", NULL, help_text};
static XMPromptDialog *dialog = NULL;


/*
** Functional Description:
**   ActionCallback:
**     This function is called when the Ok or Cancel buttons are pressed
**     for the prompt dialog.  This is not a custom dialog so that makes
**     some of the callback handling a bit easier.
** Formal Parameters:
**   XMWidget *w:
**     Pointer to the dialog widget object.
**   XtPointer user:
**     Unused user data.
**   XtPointer call:
**     Pointer to an XmSelectionBoxCallbackStruct which describes what has 
**     happened.
*/
static void ActionCallback(XMWidget *w, XtPointer user, XtPointer call)
{
  XMPromptDialog *dlg  = (XMPromptDialog *)w;
  XmSelectionBoxCallbackStruct *cbd   = (XmSelectionBoxCallbackStruct *)call;

  /* If the reason was OK, then save the new value and process it. */
  /* If not, then just unmanage...                                 */

  if(cbd->reason == XmCR_OK) {
    char *newstring;
    char saved_logfile[BUFSIZ];
    FILE *lf;
    char errmsg[BUFSIZ];

    if(!XmStringGetLtoR(cbd->value, XmSTRING_DEFAULT_CHARSET, &newstring)) {
	/* Failed to get new name. */
      Xamine_error_msg(w, 
 		       "logging::ActionCallback - XmStringGetLtoR failed\n");
      return;
    }
    strcpy(saved_logfile, Xamine_logfile);
    strcat(saved_logfile, "_SAVED");
    if(rename(Xamine_logfile, saved_logfile) && (errno != ENOENT)) {
      sprintf(errmsg, "Unable to rename logfile to %s\nError code says: %s\n",
	      saved_logfile, strerror(errno));
      Xamine_error_msg(w, errmsg);
      XtFree(newstring);
      return;
    }
    lf = fopen(newstring, "a+");
    if(lf == NULL) {
      rename(saved_logfile, Xamine_logfile); /* Try to restore saved log. */
      sprintf(errmsg, 
	      "Unable to open new logfile %s\n",
	      newstring);
      Xamine_error_msg(w, errmsg);
      XtFree(newstring);
      return;
    }
    fclose(lf);				/* Close logfile to prevent lock */
    strcpy(Xamine_logfile, newstring);
    XtFree(newstring);
  }

  dlg->Hide();
}

/*
** Functional Description:
**   Xamine_SetupLogFile:
**     This function sets up a Prompt dialog to prompt for a log filename.
** Formal Parameters:
**   XMWidget *w:
**     Parent of the prompt.
**  OThers are unused.
*/
void Xamine_SetupLogFile(XMWidget *w, XtPointer Userd, XtPointer clientd)
{
  if(dialog == NULL) {
    dialog = new XMPromptDialog("LogfilePrompt", *w, "Log File Name:",
				ActionCallback);
    dialog->GetApplyButton()->UnManage(); /* Don't use apply button. */
    dialog->AddCancelCallback(ActionCallback);
    dialog->GetHelpButton()->Enable();
    dialog->GetHelpButton()->AddCallback(Xamine_display_help, &help);
  }

  /* Load the text string with the value of the current filename */

  dialog->SetText(Xamine_logfile);

  /* Display the dialog: */

  dialog->Show();

}
