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

#include "XMDialogs.h"
#include "XMManagers.h"
#include "XMPushbutton.h"
#include "XMText.h"
#include "XMLabel.h"

#include "helpmenu.h"
#include "printer.h"

/*
** Defines.... these define the default values for print commands
**             and print temp files:
*/

#define INITIAL_PRINTER_TYPE  postscript

#ifdef unix
#define DEFAULT_TEMPFILE "./Xamine_tempprint.out"
#define DEFAULT_PRINTCMD "lpr -Pexp_hp %f;rm %f"
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
  XMToggleButton *csix;
  XMToggleButton *text;
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
    delete text;
    delete csix;
    delete cpost;
    delete post;
    delete type_manager;
  }
  /* Manipulators:  */

  char *tempfile();		/* Return temp filename pointer. */
  void tempfile(char *newf);	/* Set new temp filename. */

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
static char tempfile[100] = DEFAULT_TEMPFILE;
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
**   Xamine_GetPrinterTempFilename:
**     This function returns the temp file name to the caller.
*/
char *Xamine_GetPrinterTempFilename()
{
  return tempfile;
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
** Functional Description:
**   Xamine_GetExpandedPrintCommand:
**     This function expands the filename into the command line and returns
**     the result to the caller.  The strategy is to replace each occurence
**     of the text %f with the filename.
** Formal Parameters:
**     char *buf:
**       Character buffer to hold the resulting string.  It must be large
**       enough as we do no checking for that.
** returns:
**     Pointer to *buf or NULL if it fails. (we do some mallocing here).
*/
char *Xamine_GetExpandedPrintCommand(char *buf)
{
  char *fmtstring;
  char *percent_f;
  

  fmtstring = new char[strlen(Xamine_GetPrintCommand()) + 1];
  if(fmtstring == NULL) return NULL;
  strcpy(fmtstring, Xamine_GetPrintCommand());
  if(strstr(fmtstring, "%f") == NULL) {	/* If no %f, then just concat:  */
    strcpy(buf, fmtstring);
    strcat(buf, Xamine_GetPrinterTempFilename());
  }
  else {
    while((percent_f = strstr(fmtstring, "%f")) != NULL) {
      percent_f[1] = 's';		/* Change %f to %s for sprintf... */
      sprintf(buf, fmtstring, Xamine_GetPrinterTempFilename());
      delete fmtstring;
      
      /* Set up for the next loop pass: */
      
      fmtstring = new char [strlen(buf) + 1];
      if(fmtstring == NULL) return NULL;
      strcpy(fmtstring, buf);
    }
  }
  delete fmtstring;
  return buf;
}

/*
** Method Description:
**   SetupPrintDialog::tempfile:
**     These are overloaded methods to get and set the name of the
**     temporary file.  We return a pointer to string which must eventually
**     be XtFree'd in the case of the inquiry function.
** Formal Parameters:
**   char *new:
**    The new filename string to load into the text widget.
*/
void SetupPrintDialog::tempfile(char *n)
{
  tempfname->SetText(n);
}
char *SetupPrintDialog::tempfile()
{
  return tempfname->GetText();
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
  csix->SetState(False);
  text->SetState(False);

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
  case color_sixel:
    csix->SetState(True);
    break;
  case ascii:
    text->SetState(True);
    break;
  }
}
PrinterType SetupPrintDialog::printertype()
{
  if(post->GetState()) return postscript;
  if(cpost->GetState()) return color_postscript;
  if(csix->GetState()) return color_sixel;
  if(text->GetState()) return ascii;

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
  csix = new XMToggleButton("Sixel", *type_manager);
  csix->Label("Color Sixel");
  text = new XMToggleButton("Text", *type_manager);
  text->Label("Text only Printer");

  /* Set up the labelled text widget for the temporary filename: */

  XtSetArg(chars[0], XmNleftAttachment,  XmATTACH_FORM);
  XtSetArg(chars[1], XmNrightAttachment, XmATTACH_FORM);
  XtSetArg(chars[2], XmNtopAttachment,   XmATTACH_WIDGET);
  XtSetArg(chars[3], XmNtopWidget,       type_manager->getid());
  tempfname_label = new XMLabel("FnameLabel", *work_area, "Temporary filename",
				chars, 4);
  
  XtSetArg(chars[3], XmNtopWidget, tempfname_label->getid());
  tempfname = new XMText("Filename", *work_area, 1, 32, chars, 4);

  /* Set up the labelled text widget for the print command: */

  XtSetArg(chars[3], XmNtopWidget, tempfname->getid());
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

    newfile = dlg->tempfile();
    newcmd  = dlg->printcmd();
    strcpy(tempfile, newfile);
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

  dialog->tempfile(tempfile);
  dialog->printcmd(printcmd);
  dialog->printertype(ptype);

  /* Manage the dialog to pop it up */

  dialog->Manage();
}
