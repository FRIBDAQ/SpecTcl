/*
** Facility:
**   Xamine - NSCL Display program.
** Abstract:
** printer.cc:
**   This file contains code to manage the printer settings for Xamine.
**   Settings are the printer type which can be ASCII, Postscript, Color
**   Postscript or Color Sixel.  We also allow the user to set up the 
**   command used to print the temporary print file. Although a default
**   suitable for NSCL is supplied.
** Ron FOx
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/

static char *sccsinfo="@(#)printer.cc	2.1 12/22/93 ";


/*
** Include files required:
*/
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <unistd.h>

#include "XMDialogs.h"
#include "XMManagers.h"
#include "XMPushbutton.h"
#include "XMText.h"
#include "XMLabel.h"
#include "panemgr.h"
#include "helpmenu.h"
#include "printer.h"

/*
** Defines.... these define the default values for print commands
**             and print temp files:
*/

#define INITIAL_PRINTER_TYPE  postscript

#ifdef unix
#define DEFAULT_TEMPFILE "./Xamine_tempprint.out"
#define DEFAULT_PRINTCMD "lpr -Pu1_color_print";
#endif

#ifdef VMS
#define DEFAULT_TEMPFILE "SYS$SCRATCH:XAMINE_TEMPPRINT.OUT"
#define DEFAULT_PRINTCMD "PRINT/DELETE/QUEUE=EXP_HP %f"
#endif


/*
** Module private classes:
*/

class SetupPrintDialog : public XMCustomDialog
{
 protected:
  XMLabel      *type_label;
  XMRowColumn  *type_manager;
  XMToggleButton *post;
  XMToggleButton *cpost;
  XMLabel        *tempfname_label;
  XMText         *tempfname;
  XMLabel        *prtcmd_label;
  XMText         *prtcmd;
 public:
  /* Constructors and destructors: */

  SetupPrintDialog(char *name, XMWidget *w, char *title);
  ~SetupPrintDialog() {
    delete type_label;
    delete prtcmd;
    delete prtcmd_label;
    delete tempfname;
    delete tempfname_label;
    delete cpost;
    delete post;
    delete type_manager;
  }
  /* Manipulators:  */


  char *printcmd();		/* Return print command template. */
  void printcmd(char *newc);	/* Set new print command template. */

  PrinterType printertype(); /* Return printer type from toggles. */
  void printertype(PrinterType ptype); /* Set new printer type */

  /* Visibility control */

  void Manage() {
    type_manager->Manage();
    XMCustomDialog::Manage();
  }

  void UnManage() {
    type_manager->UnManage();
    XMCustomDialog::UnManage();
  }

};


/*
** Module private static data:
*/
static char printcmd[100] = DEFAULT_PRINTCMD;
static PrinterType ptype   = INITIAL_PRINTER_TYPE;
static SetupPrintDialog *dialog = NULL;

static char *help_text[] = {
  "  This dialog is prompting you for the printer setup.  The top part of\n",
  "the dialog allows you to set up the printer type, the name of a temporary\n",
  "file that is used to hold the print information and an operating system\n",
  "dependent command which is used to queue the print job to the printer\n",
  "  When the print command is actually used by Xamine, any occurences of\n",
  "the text '%f' will be replaced by the temporary print filename.  If \n",
  "there are no occurences of '%f' in the print command, then Xamine will \n",
  "append the name of the temporary file to the end of the command preceded\n",
  "by a space\n\n",
  "  After you are done working in the top part of the dialog, you should\n",
  "click on the approprate button in the bottom half.  These buttons have the\n",
  "following functions:\n\n",
  "    Ok       - Accepts the settings and dismisses the dialog.\n",
  "    Cancel   - Dismisses the dialog without making any changes\n",
  "    Help     - Displays this message\n",
  NULL
  };

static Xamine_help_client_data help = {"PrinterHelp", NULL, help_text};
  

/*
** Functional Description:
**  Xamine_GetPrinterType:
**    This function returns the type of the printer.
*/
PrinterType Xamine_GetPrinterType()
{
  return ptype;
}

/*
** Functional Description:
**   Xamine_GetPrintCommand:
**     This function gets the unexpanded print command.  To get the
**     print command with the filename expanded into it, call
**     Xamine_GetExpandedPrintCommand()
*/
char *Xamine_GetPrintCommand()
{
  return printcmd;
}



/*
** Method Description:
**   SetupPrintDialog::printcmd:
**     These are overloaded methods to set and get the command to print
**     the plot file.  The metastring %f is used to substitute for the name
**     of the file.
** Formal Parameters:
**   char *new:
**     New value for the setting.
** Returns:
**   char * pointer to the command string which must be XtFree'd.
*/
void SetupPrintDialog::printcmd(char *n)
{
  prtcmd->SetText(n);
}
char *SetupPrintDialog::printcmd()
{
  return prtcmd->GetText();
}

/*
** Method Description:
**  SetupPrintDialog::printertype:
**    These are overloaded methods to get and set the state of the printer
**    device type toggles.
** Formal Parameters:
**    PrinterType newtype:
**      The new printer type for the settings method.
** Returns:
**   PrinterType:
**      The printer type represented by the toggles.
*/
void SetupPrintDialog::printertype(PrinterType newtype)
{
  /*
  ** Enforce toggle behavior by unsetting all toggles and then setting
  ** the appropriate one:
  */
  post->SetState(False);
  cpost->SetState(False);

  /* Fan out on the input argument and set the state: */

  switch(newtype) {
  default:
    fprintf(stderr, "SetupPrintDialog::printertype(!) invalid printer type\n");
    fprintf(stderr, "   Defaulting to monochrom postscript and continuing\n");
  case postscript:
    post->SetState(True);
    break;
  case color_postscript:
    cpost->SetState(True);
    break;
  }
}
PrinterType SetupPrintDialog::printertype()
{
  if(post->GetState()) return postscript;
  if(cpost->GetState()) return color_postscript;

  /* Default to postscript if nothing: */

  fprintf(stderr, 
	  "SetupPrintDialog::printertype(?) - invalid toggle state\n");
  fprintf(stderr, "   Defaulting to monochrome postscript\n");
  return postscript;
}

/*
** Method Description:
**   SetupPrintDialog::SetupPrintDialog
**    This method is a constructor for a printer setup dialog.  The dialog
**    work area is built up in a Form area created by the base class
**    XMCustomDialog.  The form will consist of a Row column widget which will
**    implement a radio box, and a pair of labelled text widgets.  The
**    Action area will display buttons for Accept, Cancel and Help.
** Formal Parameters:
**   char *name:
**     Name given to the dialog as a whole (dialog shell).
**   XMWidget *w:
**     Parent widget object.
**   char *title:
**     Title put at top of the dialog.
*/
SetupPrintDialog::SetupPrintDialog(char *name, XMWidget *w, char *title) :
       XMCustomDialog(name, *w, title)
{
  Arg chars[10];

  /*  Fill in the form:   */

  XtSetArg(chars[0], XmNleftAttachment,  XmATTACH_FORM);
  XtSetArg(chars[1], XmNtopAttachment,   XmATTACH_FORM);
  XtSetArg(chars[2], XmNrightAttachment, XmATTACH_FORM);
  type_label = new XMLabel("PtypeLabel", *work_area, "Printer Type", chars, 3);

  /* Set up the radio box: */



  XtSetArg(chars[0], XmNradioAlwaysOne, True);
  XtSetArg(chars[1], XmNradioBehavior,  True);
  XtSetArg(chars[2], XmNleftAttachment, XmATTACH_FORM);
  XtSetArg(chars[3], XmNtopAttachment,  XmATTACH_WIDGET);
  XtSetArg(chars[4], XmNtopWidget,      type_label->getid());
  XtSetArg(chars[5], XmNrightAttachment,XmATTACH_FORM);
  type_manager = new XMRowColumn("PtypeRadioBox", *work_area, chars, 6);

  post = new XMToggleButton("MonoPost", *type_manager);
  post->Label("Monochrome Postscript");
  cpost = new XMToggleButton("ColorPost", *type_manager);
  cpost->Label("Color Postscript");

  /* Set up the labelled text widget for the print command.: */

  XtSetArg(chars[0], XmNleftAttachment,  XmATTACH_FORM);
  XtSetArg(chars[1], XmNrightAttachment, XmATTACH_FORM);
  XtSetArg(chars[2], XmNtopAttachment,   XmATTACH_WIDGET);
  XtSetArg(chars[3], XmNtopWidget,       type_manager->getid());


  prtcmd_label = new XMLabel("CommandLabel", *work_area, "Print command",
			     chars, 4);

  XtSetArg(chars[3], XmNtopWidget, prtcmd_label->getid());
  XtSetArg(chars[4], XmNbottomAttachment, XmATTACH_FORM);
  prtcmd = new XMText("Command", *work_area, 1, 32, chars, 5);

  /* Last but not least, remove the Apply button: */

  Apply->UnManage();
  
}

/*
** Functional Description:
**    ActionCallback:
**      This function is called when either the Ok or the Cancel button is
**      selected from the printer setup dialog.  If the OK button was 
**      clicked, then we save the state of the dialog as the defaults.
**      For Either, the dialog is unmanaged, making it invisible.
** Formal Parameters:
**   XMWidget *w:
**     The widget which is resonsible for our being called.  In this case,
**     since we are a custom dialog we'll either have the Ok or Cancel
**     button widget object.
**  XtPointer user:
**     User data, in this case the pointer to the dialog object itself.
**  XtPointer call:
**     Motif Call back data, in this case an ignored XmPushButtonCallbackStruct
*/
void ActionCallback(XMWidget *w, XtPointer user, XtPointer call)
{
  XMPushButton *button    = (XMPushButton *)w;
  SetupPrintDialog *dlg   = (SetupPrintDialog *)user;

  char *newfile, *newcmd;

  /*  If the Ok button was hit then process it: */

  if(button == dlg->ok()) {

    /* Deal with the textuals: */

    newcmd  = dlg->printcmd();
    strcpy(printcmd, newcmd);
    XtFree(newfile); XtFree(newcmd);
   
    ptype = dlg->printertype();
  }

  /* Pop down the dialog */

  dlg->UnManage();
}

/*
** Functional Description:
**   Xamine_SetupPrinter:
**     Prompts for the printer setup using a SetupPrintDialog.
**     The dialog is cached for multiple re-use.
** Formal Parameters:
**    XMWidget *w:
**      The widget to be used as the parent for the dialog widget subtree.
**   All other parameters are the same as for a callback but are ignored.
*/
void Xamine_SetupPrinter(XMWidget *w, XtPointer user, XtPointer call)
{
  /* If the widget does not exist, it must be instantiated and callbacks */
  /* must be specified.                                                  */

  if(!dialog) {
    dialog = new SetupPrintDialog("PrinterSetup", w, "Setup Printer Options");

    dialog->AddOkCallback(ActionCallback, dialog);
    dialog->AddCancelCallback(ActionCallback, dialog);
    dialog->AddHelpCallback(Xamine_display_help, &help);
  }
  
  /* Make the dialog contents match the current defaults. */

  dialog->printcmd(printcmd);
  dialog->printertype(ptype);

  /* Manage the dialog to pop it up */

  dialog->Manage();

}

/*
  FindImport
    Locates the import command in the file system.
    Search in :
      /usr/bin/X11
      /usr/local/bin
      /optd/bin
      /usr/bin
      /opt/bin
*/
const char* FindImport()
{
  if(!access("/usr/bin/X11/import", X_OK))
    return "/usr/bin/X11/import";

  if(!(access("/usr/local/bin/import", X_OK)))
    return "/usr/local/bin/import";
  
  if(!(access("/optd/bin/import", X_OK)))
    return "/optd/bin/import";

  if(!(access("/usr/bin/import", X_OK))) 
    return "/usr/bin/import";

  if(!(access("/opt/bin/import", X_OK)))
    return "/opt/bin/import";
      
  return NULL;
}
/*
   Print
     Print with a specified import command
 */
static void
Print(const char* importcmd, const char* tofile)
{
  char printcommand[512];
  char backgroundcmd[1024];

  sprintf(backgroundcmd, 
	  "(sleep 2;%s;%s %s;rm %s)&",
	  importcmd, printcmd, tofile, tofile);
  system(backgroundcmd);
  

}
/*
  PrintWindow:
    Given a window Id produces uses import to print the window to 
    the 'current printer'.  This is done by doing an import to a tempfile,
    printing the tempfile, and then deleting it. All of the shell commands
    are done via system(3) to ensure synchronization with the command.

 Formal Parameters:
    Window w - the window to dump.  We assume that Xlib defines
               'Window' as an unsigned long.  This is true in X11R6.

  Note the window is printed recursively, that is all subwindows are printed.
	     
*/
static void 
PrintWindow(Window w)
{
  const char *importpath;
  char importcmd[100+PATH_MAX];
  char filename[PATH_MAX];
  sprintf(filename, "%s.ps", tmpnam(NULL));

  importpath=FindImport();
  if(!importpath) {
    fprintf(stderr, "Unable to locate imagemagick import command\n");
    return;
  }
  
  sprintf(importcmd, "%s -window %d -transparency black %s",
	  importpath, w, filename);
  
  Print(importcmd, filename);
  

}
/*
  Print an area of the root window given by x1,y1 - x2,y2
  */
static void
PrintArea(int x1, int y1, int x2, int y2)
{
  int wid = (x2-x1);
  int ht  = (y2-y1);
  char importcmd[100+PATH_MAX];
  char filename[PATH_MAX];
  const char*importpath;

  sprintf(filename, "%s.ps", tmpnam(NULL));

  importpath = FindImport();
  if(!importpath) {
    fprintf(stderr, "Unable to locate imagemagick import command\n");
    return;
  }

  
  sprintf(importcmd, 
	  "%s -window root -transparency black -crop %dx%d+%d+%d %s",
	  importpath, wid,ht,x1,y1, filename);
  
  Print(importcmd, filename);

  
}


/* 
  Xamine_printSelected
   Prints the currently selected pane.
 */
void
Xamine_printSelected(XMWidget* w, XtPointer user, XtPointer call)
{
  // Ask the panemanager for the selected drawing area, figure out the
  // corresonding window window and send that on to PrintWindow.

  XMWidget* Pane = Xamine_GetSelectedDrawingArea();
  Widget    Wid  = Pane->getid();
  Window    Win  = XtWindow(Wid);

  PrintWindow(Win);
}

/*
   Xamine_printAll:
    Prints all of the panes in the display.  This is done painfully 
    by translating the window shape into root coordinates, and asking
    for a dump of that part of the root window.

 */
void
Xamine_printAll(XMWidget *w, XtPointer user, XtPointer call)
{

  // Get the window id of the panemanager:

  XMWidget* pMgr = Xamine_Getpanemgr();
  Widget    Wid  = pMgr->getid();
  Window    Win  = XtWindow(Wid);

  // Get the root window id:

  Window root;			// Which will be stored here.
  Window parent;
  Window *pChildren;
  unsigned int    nChildren;
  Display* pDisplay = XtDisplay(Wid);

  XQueryTree(pDisplay, Win, &root, &parent, &pChildren, &nChildren);
  XFree(pChildren);

  // Get the size of the Pane manager window:
  //
  int nx, ny;
  pMgr->GetAttribute(XmNwidth, (void*)&nx);
  pMgr->GetAttribute(XmNheight, (void*)&ny);

  // and translate the 0,0 and nx,ny points to root coordinates:

  int rootx1, rooty1;
  int rootx2, rooty2;

  Window  child;
  XTranslateCoordinates(pDisplay,
			Win,
			root,
			0,0, &rootx1, &rooty1, &child);

  XTranslateCoordinates(pDisplay,
			Win,
			root,
			nx,ny, &rootx2, &rooty2, &child);

  PrintArea(rootx1, rooty1, rootx2, rooty2);

}
