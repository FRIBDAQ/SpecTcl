/*
** Facility:
**   Xamine  - Help menu.
** Abstract:
**   help.cc  - This file contains the callback code which is attached to
**              the HELP menu buttons.  Two public routines are provided:
**               Xamine_display_help - Displays help data.
**               Xamine_help      - Tool function to generate help dialogs
**                                  from ASCII string arrays.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** Versioning:
**   @(#)helpmenu.cc	8.9 10/19/98 
*/

/*
** Include files:
*/
#include <stdio.h>
#ifdef unix
#include <strings.h>
#endif
#ifdef VMS
#include <string.h>
#endif
#include <Xm/Xm.h>
#include "XMDialogs.h"
#include "helpmenu.h"



/*
** Static local declarations of the help strings and associated dialogs. 
*/

static char *About[]
  = { "Xamine -- The NSCL X/Motif histogram display program\n",
      "               Author: Ron Fox\n",
      "                       NSCL\n",
      "                       Michigan State University\n",
      "                       East Lansing, MI 48824-1321\n",
      "                       fox@foxsun.nscl.msu.edu\n",
      "               Copyright (c) NSCL, 1995,1996,1997,1998 All rights reserved\n",
      "\n",
      "               Version: Xamine - X2.0-003\n",
      "               O/S support for:   VAX/VMS\n",
      "                                  AXP/VMS\n",
      "                                  Ultrix 4.x\n",
      "                                  SunOS 4.1.x\n",
      "                                  OSF/1 AXP\n",
      "                                  Linux with Lesstif\n",
      "               Recent enhancements:\n",
      "                    June 24, 1997:\n"
      "                      Added support for client supplied peak markers\n",
      "                    July 13, 1998:\n",
      "                      Fixed problems doing graphics to deleted pixmaps\n",
      "                      Fixed problems producing corrupt default files\n",
      "                    July 23, 1998:\n",
      "                      Completed port to Linux with Lesstif\n",
      "                    October 19, 1998\n",
      "                      Fixed error in points prompter on client buttons\n",
      "                    Looking at crashes on window reconfigs [2]\n",
	NULL
	};
static char *aboutname = "About";   /* For VMS C++ compiler bug */

Xamine_help_client_data Xamine_About = { aboutname, NULL, About };
                       
static char *Help[] 
  = { "Xamine is the NSCL X/Motif histogram display program\n\n",
      "   The highlighted pane is the \"selected\" pane. All operations\n",
      " that require a pane are performed on that pane. To get a feel\n",
      " for the functionality of the program, explore the menu bar at the \n",
      " top of the set of panes. Where appropriate, dialogs that result\n",
      " from selecting menu buttons, or buttons on the quick select button\n",
      " bar below the pane region include context sensitive documentation.\n",
      " By exploring these options and reading the associated help where\n",
      " the consequences of your actions are not clear, you can get a hands\n",
      " on introduction to Xamine.\n",
    "\n   For more information about the program, refer to the Xamine paper\n",
      " documentation\n",
      NULL
      };

static char *overname = "Help_Overview";
Xamine_help_client_data Xamine_Help_Overview = {overname,  NULL, Help };



/**
** Functional Description:
*    Xamine_help  - This function builds a help dialog object and returns it
**                   to the caller.  The help dialog is built up from a list
**                   of string pointers.  The Help dialog will be managed
**                   at the time we exit.  The caller can do whatever it
**                   wants with the dialog including recycling it for later
**                   use or allowing it to be destroyed.  
**                     A Help dialog will be an information dialog with
**                   a single button labelled "Dismiss".  The "Dismiss" button,
**                   when pressed, by default will just pop down the dialog.
**                   To alter this behavior, the caller must invoke the
**                   AddOkCallback method on the returned dialog to string
**                   a callback onto the Dismiss button.
** Formal Parameters:
**    char     *name:
**       Name of the dialog.
**    XMWidget *parent:
**       Parent widget for the help dialog.
**    char **help_text:
**       A NULL terminated list of character string pointers which will be
**       used to construct the help message string.
** Returns:
**    XMInformationDialog *  - Pointer to the created information dialog.
**    NULL                   - If failed.
*/
XMInformationDialog *Xamine_help(char *name, XMWidget *parent, 
				 char **help_text)
{
  char **ht;			/* Used to traverse the help text. */
  int    ht_size;		/* Number of characters of help text. */
  char *help;			/* Points to the concatenated strings. */
  XMInformationDialog *dialog;	/* Will point to the dialog.          */
  Widget help_parent = parent->getid();

  while(!XtIsTopLevelShell(help_parent)) help_parent = XtParent(help_parent);

  /* First size the help text: */

  ht = help_text;
  ht_size = 1;			/* Count the null terminator. */
  while(*ht) 
    ht_size += strlen(*ht++);	/* Add in the sizes of each string. */

  /* Next build a concatenated string into dynamic storage allocated 
  ** and pointed to by help.
  */
  help = XtMalloc(ht_size);	/* Allocate the string */
  if(!help)			/* Failure is fatal at this stage */
    return (XMInformationDialog *)NULL;
  ht = help_text;
  *help = '\0';			/* Init *help to empty */
  while(*ht)
    strcat(help, *ht++);	/* String together the strings. */

  /* Create the dialog and set up it's characteristics: */

  dialog = new XMInformationDialog(name, help_parent, help);
  XtFree(help);			/* Don't forget to delete the text storage */
  
  dialog->GetCancelButton()->UnManage(); /* Make cancel button invisible */
  dialog->GetHelpButton()->UnManage();   /* Make Help button vanish too */

  /* Return the dialog to the caller: */

  return dialog;
}

/*
** Functional Description:
**    Xamine_display_help   - This function displays some help text in
**                            a help dialog.  A structure called the
**                            Xamine_help_client_data is passed as client
**                            data.  It includes a pointer to an
**                            XMInformationDialog widget object and
**                            pointer to the help text strings.
**                            This function only creates the information
**                            dialog if the widget object field of that
**                            structure is NULL.
** Formal Parameters:
**   XMWidget *caller:
**       Widget which triggered the callback. This widget will also be
**       the parent of the information dialog.
**   XtPointer client_data:
**       Actually a pointer to an Xamine_help_client_data structure.
**       This structure contains a pointer to an XMInformationDialog widget
**       which will hold the resulting information widget or already does,
**       and a pointer to an array of strings which contain the help text.
**       The string array is open ended NULL terminated as expected by
**       Xamine_help
**   XtPointer reason:
**        Reason for the call back (ignored).
**/
void Xamine_display_help(XMWidget *caller, 
			 XtPointer client_data, XtPointer reason)
{
  Xamine_help_client_data *help = (Xamine_help_client_data *)client_data;

  if(!help->dialog) 
    help->dialog = Xamine_help(help->name, caller,  help->text);
  else
    help->dialog->Manage();
}
