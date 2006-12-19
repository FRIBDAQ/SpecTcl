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
**   Xamine - NSCL Display program.
** Abstract:
** printer.cc:
**   This file contains code to manage the printer settings for Xamine.
**   Settings are the printer type which can be ASCII, Postscript, Color
**   Postscript or Color Sixel.  We also allow the user to set up the 
**   command used to print the temporary print file. Although a default
**   suitable for NSCL is supplied.
** Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/
/*
 $Log$
 Revision 4.14.4.3  2006/11/17 19:43:26  ron-fox
  Complete the 228 defect fix.

 Revision 4.14.4.2  2006/11/17 18:42:21  ron-fox
 Backport defect 228 - Xamine's printer defaults could be incompletely
 initialized causing bad Xamine.Defaults files and
 prints to fail or cause Xamine to crash.

 Revision 4.14.4.1  2006/05/23 13:28:52  ron-fox
 - Fix defect 195: gri was outputting an EPSF header which squashed multipage
   output to the printer onto a single page (gv worked just fine).  Added
   -p (-postsript) switch to make it emit a plain old %%!Ps-Adobe header
   modified griprint.cc to invoke gri with that switch.
 - Fix defect 196: printer.cc - the print setup dialog print command
   XMText widget evidently had duplicated the name of another widget leading
   to strange results.  Uniquified the name of that widget.

 Revision 4.14  2003/08/25 16:25:30  ron-fox
 Initial starting point for merge with filtering -- this probably does not
 generate a goo spectcl build.

 Revision 4.13  2003/04/04 17:44:21  ron-fox
 Catch dialog destruction for cached widgets so that the destroyed dialog is re-created when it's needed.  Prior behavior would usually crash Xamine because deleted widgets would be referenced.

 Revision 4.12  2003/04/02 18:36:12  ron-fox
 Connect the options->Setup Printer... dialog appropriately with the defaults file and the default information used by Jason's printer dialogs.

 Revision 4.11.2.1  2003/03/21 17:16:56  venema
 Added support for GNU autotools, and fixed a problem with mapped spectrum which caused Xamine to crash on Tru64 on 1d spectra.

 Revision 4.11  2003/01/02 16:13:00  venema
 Major version upgrade to SpecTcl 2.0. This version adds arbitrary user-coordinate mapping to Xamine using, and a special mapping button on the Xamine GUI. Also, print options are now sticky and are maintained in the Xamine.Defaults file.

 Revision 4.10  2002/11/05 18:40:27  ron-fox
 Fix issues with library updates and prototypes, so that this compiles
 on g++ 2.96.

 Revision 4.9  2002/08/15 18:23:17  ron-fox
 griprint.cc - Remove extra temp.ps on print command since it's
               put in to replace %s. Also simplify logic to put filename in
               print command.
 print.cc    - Put %s in unix print command, change Cygwin print command to use
               /'s not \'s.

 Revision 4.8  2002/08/14 20:22:25  venema
 Added support for Cygwin to print using its own print syntax.

 Revision 4.7  2002/07/29 18:59:53  venema
 Removed printing functions and placed them in griprint.cc. Implemented new tabs widget used for selecting print options for Xamine using the Gri plotting package.

 Revision 4.6  2002/07/11 13:50:03  ron-fox
 - Ignore prior log entry.
 - Change -transparency switch on import invocation to -transparent to bring it
   in line with newer versions of image magick.

 */

/*
** Include files required:
*/
#include <string>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <fstream.h>
#include <math.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include "XMDialogs.h"
#include "XMManagers.h"
#include "XMPushbutton.h"
#include "XMText.h"
#include "XMLabel.h"
#include "XMTab.h"
#include "XMList.h"
#include "panemgr.h"
#include "helpmenu.h"
#include "printer.h"
#include "dispshare.h"
#include "axes.h"
#include "dispgrob.h"
#include "grobjmgr.h"
#include "griprint.h"
#include "dfltmgr.h"

extern "C" {
#ifndef HAVE_SYS_TIME_H
  time_t time(time_t *tloc);
#endif
}

/*
** Externally referenced global:
*/
#ifdef __NEED_OWN_ERRNO
extern int errno;
#endif

extern volatile spec_shared *xamine_shared;
extern win_db *database;

extern grobj_database Xamine_DefaultObjectDatabase;
extern grobj_database Xamine_DefaultGateDatabase;

/*
** Defines.... these define the default values for print commands
**             and print temp files:
*/

#define INITIAL_PRINTER_TYPE  postscript

#define DEFAULT_TEMPFILE "./Xamine_tempprint.out"

#ifdef HAVE_WINDOWS_H      /* Cygwin */
#define DEFAULT_PRINTCMD "cp %s //win-cluster/west_print1"
#else
#define DEFAULT_PRINTCMD "lpr -Pu1_color %s";
#endif


struct PrintAllData Print_Options;
struct ContoursData Contours_Data;
struct PrinterOptionsData Printer_Options_Data;
struct DefaultPrintOptions* dflt_print_opts = NULL;

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
static char printcmd[1000] = DEFAULT_PRINTCMD;
static PrinterType ptype   = INITIAL_PRINTER_TYPE;
static SetupPrintDialog *dialog = NULL;

static PrintSpectrumDialog* ps_dialog = NULL;

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

static char *help_setup_text[] = {
  "                 *****This dialog is prompting you for the page setup and spectrum options.*****\n\n",
  "The dialog is broken up into three tabs, each of which controls a\n",
  "separate aspect of the printing.\n\n",
  "   Page Setup -- This tab allows you to control the setup of the printed output on the page.\n",
  "     Portrait  -- This is the default page layout.\n",
  "     Landscape -- This layout rotates the image 90 degrees.\n",
  "     Print Selected Spectrum -- Prints only the selected spectrum\n",
  "     Print All Spectra -- Prints all spectra displayed\n",
  "     Rows & Columns -- Allows you to set the geometry of the spectra when multiple spectra are being printed\n",
  "     Size Selection -- The lengths of the axes can be specified in the 'Spectrum Options' tab, or one of the preset values\n",
  "                       can be selected.\n\n",
  "   Spectrum Options -- This tab allows you to specify items to include on the printed copy of the spectrum.\n",
  "     Spectrum Title -- The title is positioned at the top of the page. The default title is that shown on Xamine.\n",
  "     Axis Name -- The x- and y-axis labels can be specified if desired\n",
  "     Axis Length (in) -- The axis lengths can be specified in inches (provided the 'Specify size in Spectrum Options'\n",
  "                         radio button is selected in the 'Page Setup' tab). NOTE: Xamine will not stop you from\n",
  "                         printing outside of the page borders, so choose your axis lengths with discretion.\n",
  "     Contour Increment -- The contour increment will apply only if the 'Draw contours' button is selected. This options\n",
  "                          will draw contour lines at an increment that is specified by the user. The default value is\n",
  "                          reasonable, but can be altered. Please also note, drawing contour lines can be a very TIME\n",
  "                          CONSUMING process and can cause the image to take up to 10 times LONGER to render, especially on\n",
  "                          large spectra!\n",
  "     Draw color palette -- The color palette is a rectangular box that graphically illustrates the colors of the full\n",
  "                           range of spectrum values for 2d spectra.\n",
  "     Draw contour lines -- When selected, the contour increment can be set.\n\n",
  "     Draw time stamp -- Draws the date and time that the printout was created above the spectrum\n",
  "     Superpositions use symbols -- Draws superposition spectra (if any exist) using symbols, instead of lines. This can be\n",
  "                                   useful when printing to a monochrome printer, to distinguish between the spectra.\n",
  "   Tic Marks -- This tab allows you to specify the major and minor tic intervals for a spectrum\n",
  "     Use Xamine tic intervals -- Use the tic intervals that Xamine uses\n",
  "     Specify tic intervals -- Specify your own major and minor tic intervals. Note that major, or minor can be specified\n",
  "                              by themselves. If major is unspecified, the Xamine intervals will be used.\n\n",
  "   Output Options -- This tab allows you to specify the type of output that Xamine should produce.\n",
  "     To Printer -- With this button selected, the output will go directly to the printer specified in the 'Print Command'\n",
  "                   text entry box.\n",
  "     To File -- With this button selected, the output will be stored in a file. The file will be given the name supplied\n",
  "                by the user in the 'Output File Path' text entry box or, if no name is supplied, the file will be given a\n",
  "                name of the form 'spectcl-xx.ps' where 'xx' is a 2-digit number that ensures the file produced is unique.\n",
  "     Print Command -- The command used to print.\n",
  "     Output File Path -- The path and filename of the output file. The full path can be specified, but if it is not the\n",
  "                         file will be created in the directory from which SpecTcl was invoked.\n",
  "     Selection Box -- Allows the user to select the output file format.\n",
  "     Selected Output Resolution -- The output resolution controls the granularity of the output image, and it only applies\n",
  "                                   to 2d spectra. A resolution of 1:1 will reproduce the image such that it is completely\n",
  "                                   faithful to the original image as displayed by Xamine. The granularity gradually gets\n",
  "                                   worse with less resolution, but the speed at which the image can be rendered (and\n",
  "                                   printed) increases dramatically, especially with large spectra.\n",
  NULL
};

static Xamine_help_client_data help = {"PrinterHelp", NULL, help_text};
static Xamine_help_client_data help_setup = {"SetupHelp", NULL, help_setup_text};

DefaultPrintOptions::DefaultPrintOptions()
{
    layout = portrait;
    num    = printsel;
    dest   = toprinter;
    res    = one;
    file_type  = -1;
    xlen       = 4;
    ylen       = 4;
    time_stamp = 0;
    color_pal  = 0;
    contours   = 0;
    symbols    = 0;
    
    strcpy(print_cmd, printcmd);
    sprintf(rows, "%d", Xamine_Panerows());
    sprintf(cols, "%d", Xamine_Panecols());
 }
  

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
  if(!dflt_print_opts) {
    dflt_print_opts = new struct DefaultPrintOptions;
  }
  return dflt_print_opts->print_cmd;
}


string Xamine_GetOutputFilename()
{
  string sFilename;
  char filename[20];
  char ps_filename[20];
  for(int i = 0; i < 99; i++) {
    FILE* trial;
    sprintf(filename, "spectcl-%02d", i);
    sprintf(ps_filename, "spectcl-%02d.ps", i);
    trial = fopen(ps_filename, "r");
    if(trial == NULL) {  // if no such file exists, then use this filename
      sprintf(ps_filename, "spectcl-%02d.jpg", i);
      trial = fopen(ps_filename, "r");
      if(trial == NULL) {
	sprintf(ps_filename, "spectcl-%02d.png", i);
	trial = fopen(ps_filename, "r");
	if(trial == NULL) {
	  sprintf(ps_filename, "spectcl-%02d.jpg.0", i);
	  trial = fopen(ps_filename, "r");
	  if(trial == NULL) {
	    sprintf(ps_filename, "spectcl-%02d.png.0", i);
	    trial = fopen(ps_filename, "r");
	    if(trial == NULL)
	      break;
	  }
	}
      }
    }
    fclose(trial);
  }
  sFilename = string(filename);
  return sFilename;
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
  post->SetState(True);
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

char* PrintSpectrumDialog::getrows()
{
  return t_rows->GetText();
}

void PrintSpectrumDialog::setrows(char* r)
{
  t_rows->SetText(r);
}

char* PrintSpectrumDialog::getcols()
{
  return t_cols->GetText();
}

void PrintSpectrumDialog::setcols(char* c)
{
  t_cols->SetText(c);
}

char* PrintSpectrumDialog::gettitle()
{
  return t_title->GetText();
}

void PrintSpectrumDialog::settitle(char* n)
{
  t_title->SetText(n);
}

char* PrintSpectrumDialog::getxname()
{
  return t_name_x->GetText();
}

void PrintSpectrumDialog::setxname(char* n)
{
  t_name_x->SetText(n);
}

char* PrintSpectrumDialog::getyname()
{
  return t_name_y->GetText();
}

void PrintSpectrumDialog::setyname(char* n)
{
  t_name_y->SetText(n);
}

float PrintSpectrumDialog::getxlen()
{
  if(ten_by_ten->GetState())
    return 10;
  if(ten_by_fifteen->GetState())
    return 15;

  // Convert the inches measurement into centimeters
  double dLen;
  if(strlen(t_length_x->GetText()) == 0) 
    dLen = 10.0;
  else
    dLen = atof(t_length_x->GetText());
  double dLenInCm = (double)(dLen * 2.54);
  return dLenInCm;
}

void PrintSpectrumDialog::setxlen(char* n)
{
  t_length_x->SetText(n);
}

float PrintSpectrumDialog::getylen()
{
  if(ten_by_ten->GetState()) {
    return 10;
  }
  if(ten_by_fifteen->GetState())
    return 10;

  // Convert inches into centimeters for Gri
  double dLen;
  if(strlen(t_length_x->GetText()) == 0)
    dLen = 10.0;
  else
    dLen = atof(t_length_y->GetText());
  double dLenInCm = (double)(dLen * 2.54);
  return dLenInCm;
}

void PrintSpectrumDialog::setylen(char* n)
{
  t_length_y->SetText(n);
}

char* PrintSpectrumDialog::getcontour_inc()
{
  return t_contour_inc->GetText();
}

void PrintSpectrumDialog::setcontour_inc(char* inc)
{
  t_contour_inc->SetText(inc);
}

double PrintSpectrumDialog::getxmajor()
{
  if(strlen(t_xmajor->GetText()) == 0)
    return 0;
  else
    return atof(t_xmajor->GetText());
}

void PrintSpectrumDialog::setxmajor(char* maj)
{
  t_xmajor->SetText(maj);
}

double PrintSpectrumDialog::getxminor()
{
  if(strlen(t_xminor->GetText()) == 0)
    return 0;
  else
    return atof(t_xminor->GetText());
}

void PrintSpectrumDialog::setxminor(char* min)
{
  t_xminor->SetText(min);
}

double PrintSpectrumDialog::getymajor()
{
  if(strlen(t_ymajor->GetText()) == 0)
    return 0;
  else
    return atof(t_ymajor->GetText());
}

void PrintSpectrumDialog::setymajor(char* maj)
{
  t_ymajor->SetText(maj);
}

double PrintSpectrumDialog::getyminor()
{
  if(strlen(t_yminor->GetText()) == 0)
    return 0;
  else
    return atof(t_yminor->GetText());
}

void PrintSpectrumDialog::setyminor(char* min)
{
  t_yminor->SetText(min);
}

void PrintSpectrumDialog::setcmd(char* prtcmd)
{
  t_cmd->SetText(prtcmd);
}

void PrintSpectrumDialog::setfile(char* f)
{
  t_file->SetText(f);
}

char* PrintSpectrumDialog::getfile()
{
  return t_file->GetText();
}

char* PrintSpectrumDialog::gettype()
{
  char* string;
  XmStringTable st = list_type->GetSelectedItems();
  XmStringGetLtoR(st[0], XmSTRING_DEFAULT_CHARSET, &string);
  return string;
}

char* PrintSpectrumDialog::getcmd()
{
  return t_cmd->GetText();
}

void PrintSpectrumDialog::setspecified()
{
  user_specified->SetState(True);
}

void PrintSpectrumDialog::setlayout(PrintLayout layout)
{
  switch(layout) {
  case portrait:
    port->SetState(True);
    land->SetState(False);
    break;
  case landscape:
    port->SetState(False);
    land->SetState(True);
    break;
  default:
    fprintf(stderr, "SetupPrintDialog::printertype(!) invalid printer type\n");
    fprintf(stderr, "   Defaulting to monochrom postscript and continuing\n");
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

PrintLayout PrintSpectrumDialog::getlayout()
{
  if(port->GetState()) return portrait;
  if(land->GetState()) return landscape;

  /* Default to portrait if nothing: */

  fprintf(stderr, 
	  "PrintSpectrumDialog(?) - invalid toggle state\n");
  fprintf(stderr, "   Defaulting to portrait\n");
  return portrait;
}

void PrintSpectrumDialog::setnum(PrintNum num)
{
  switch(num) {
  case printall:
    print_all->SetState(True);
    print_sel->SetState(False);
    allow_setsize(False);
    break;
  case printsel:
    print_all->SetState(False);
    print_sel->SetState(True);
    allow_setsize(True);
    break;
  default:
    fprintf(stderr, "SetupPrintDialog::printertype(!) invalid printer type\n");
    fprintf(stderr, "   Defaulting to monochrom postscript and continuing\n");
  }
}

PrintNum PrintSpectrumDialog::getnum()
{
  if(print_all->GetState()) return printall;
  if(print_sel->GetState()) return printsel;

  /* Default to print selected if nothing: */

  fprintf(stderr, 
	  "PrintSpectrumDialog(?) - invalid toggle state\n");
  fprintf(stderr, "   Defaulting to print selected\n");
  return printsel;
}

void PrintSpectrumDialog::setdest(PrintDest pd)
{
  switch(pd) {
  case tofile:
    to_file->SetState(True);
    to_printer->SetState(False);
    list_type->SetAttribute(XmNsensitive, (long int)True);
    t_cmd->SetAttribute(XmNsensitive, (long int)False);
    break;
  case toprinter:
    to_file->SetState(False);
    to_printer->SetState(True);
    list_type->SetAttribute(XmNsensitive, (long int)False);
    t_cmd->SetAttribute(XmNsensitive, (long int)True);
    break;
  default:
    fprintf(stderr, "PrintSpectrumDialog::setdest(!) invalid destination\n");
    fprintf(stderr, "   Defaulting to printer and continuing\n");
  }
}

PrintDest PrintSpectrumDialog::getdest()
{
  if(to_file->GetState()) return tofile;
  if(to_printer->GetState()) return toprinter;

  /* Default to printer if nothing: */

  fprintf(stderr, 
	  "PrintSpectrumDialog(?) - invalid toggle state\n");
  fprintf(stderr, "   Defaulting to printer selected\n");
  return toprinter;
}

void PrintSpectrumDialog::setres(Resolution res)
{
  switch(res) {
  case one:
    one_to_one->SetState(True);
    break;
 case two:
   one_to_two->SetState(True);
   break;
  case four:
    one_to_four->SetState(True);
    break;
  case eight:
    one_to_eight->SetState(True);
    break;
  default:
    fprintf(stderr, "PrintSpectrumDialog::setres(!) invalid resolution\n");
    fprintf(stderr, "   Defaulting to 1:1\n");
  }
}

Resolution PrintSpectrumDialog::getres()
{
  if(one_to_one->GetState()) return one;
  if(one_to_two->GetState()) return two;
  if(one_to_four->GetState()) return four;
  if(one_to_eight->GetState()) return eight;

  /* Default to full resolution if nothing: */

  fprintf(stderr, 
	  "PrintSpectrumDialog(?) - invalid toggle state\n");
  fprintf(stderr, "   Defaulting to full resolution\n");
  return one;
}

void PrintSpectrumDialog::settics(Tics t)
{
  switch(t) {
  case deflt:
    default_tics->SetState(True);
    break;
  case user:
    user_tics->SetState(True);
    break;
  default:
    fprintf(stderr, "PrintSpectrumDialog::settics(!) invalid tics\n");
    fprintf(stderr, "   Defaulting to default\n");    
  }
}

Tics PrintSpectrumDialog::gettics()
{
  if(default_tics->GetState()) return deflt;
  if(user_tics->GetState()) return user;

  /* Default to default tics if nothing: */

  fprintf(stderr, 
	  "PrintSpectrumDialog(?) - invalid toggle state\n");
  fprintf(stderr, "   Defaulting to default tics\n");
  return deflt;
}

void PrintSpectrumDialog::setlist_type(int t)
{
  list_type->SelectItem(t);
}

void PrintSpectrumDialog::setdraw_palette(bool b)
{
  XtArgVal val = (XtArgVal)b;
  draw_palette->SetAttribute(XmNsensitive, (long int)val);
}

void PrintSpectrumDialog::setdraw_contours(bool b)
{
  XtArgVal val = (XtArgVal)b;
  draw_contours->SetAttribute(XmNsensitive, (long int)val);
}

void PrintSpectrumDialog::setdraw_time(bool b)
{
  XtArgVal val = (XtArgVal)b;
  draw_time->SetAttribute(XmNsensitive, (long int)val);
}

void PrintSpectrumDialog::setuse_symbols(bool b)
{
  XtArgVal val = (XtArgVal)b;
  use_symbols->SetAttribute(XmNsensitive, (long int)val);
}

void PrintSpectrumDialog::setcontour_val(bool b)
{
  XtArgVal val = (XtArgVal)b;
  t_contour_inc->SetAttribute(XmNsensitive, (long int)val);
}

void PrintSpectrumDialog::set_select_res(bool b)
{
  XtArgVal val = (XtArgVal)b;
  one_to_one->SetAttribute(XmNsensitive, (long int)val);
  one_to_two->SetAttribute(XmNsensitive, (long int)val);
  one_to_four->SetAttribute(XmNsensitive, (long int)val);
  one_to_eight->SetAttribute(XmNsensitive, (long int)val);
}

void PrintSpectrumDialog::set_palette(bool b)
{
  draw_palette->SetState(b);
}

void PrintSpectrumDialog::set_contours(bool b)
{
  draw_contours->SetState(b);
  setdraw_contours(b);
  setcontour_val(b);
}

void PrintSpectrumDialog::set_time(bool b)
{
  draw_time->SetState(b);
}

void PrintSpectrumDialog::set_symbols(bool b)
{
  use_symbols->SetState(b);
  setuse_symbols(b);
}

void PrintSpectrumDialog::allow_geom(bool b)
{
  XtArgVal val = (XtArgVal)b;
  t_rows->SetAttribute(XmNsensitive, (long int)val);
  t_cols->SetAttribute(XmNsensitive, (long int)val);
}

void PrintSpectrumDialog::allow_tics(bool b)
{
  XtArgVal val = (XtArgVal)b;
  t_xmajor->SetAttribute(XmNsensitive, (long int)val);
  t_xminor->SetAttribute(XmNsensitive, (long int)val);
  t_ymajor->SetAttribute(XmNsensitive, (long int)val);
  t_yminor->SetAttribute(XmNsensitive, (long int)val);
}

void PrintSpectrumDialog::allow_setsize(bool b)
{
  XtArgVal val = (XtArgVal)b;
  ten_by_ten->SetAttribute(XmNsensitive, (long int)val);
  ten_by_fifteen->SetAttribute(XmNsensitive, (long int)val);
  user_specified->SetAttribute(XmNsensitive, (long int)val);
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
  XMCustomDialog(name, *w, title),
    type_label(0),
    type_manager(0),
    post(0),
    cpost(0),
    tempfname_label(0),
    tempfname(0),
    prtcmd_label(0),
    prtcmd(0)
{

  Arg chars[100];

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
  prtcmd = new XMText("commandprintsetup", *work_area, 1, 32, chars, 5);

  /* Last but not least, remove the Apply button: */

  Apply->UnManage();
  
}

/*
** Method Description:
**   PrintSpectrumDialog::PrintSpectrumDialog
**    This method is a constructor for a print setup dialog.  The dialog
**    work area is built up in a Tab area created by the base class
**    XMTab.  There will be 3 tabs consisting of column widgets which will
**    implement radio boxes, labelled text widgets, and frames.  The
**    Action area will display buttons for Accept, Cancel and Help.
** Formal Parameters:
**   char *name:
**     Name given to the dialog as a whole (dialog shell).
**   XMWidget *w:
**     Parent widget object.
**   char *title:
**     Title put at top of the dialog.
 */
PrintSpectrumDialog::PrintSpectrumDialog(char *name, 
					 XMWidget* w, 
					 char *title) :
  XMCustomDialog(name, *w, title)
{
  Arg chars[100];

  /* Create the main tab widget to hold everything else */
  tab = new XMTab("Tab", *work_area);

  /* Here, we create three tabs in the widget */
  XtSetArg(chars[0], XmNwidth, 320);
  XtSetArg(chars[1], XmNheight, 305);
  first_tab = new XMForm("Page Setup", *tab, chars, 2);
  second_tab = new XMForm("Spectrum Options", *tab, chars, 2);
  third_tab  = new XMForm("Tic Marks", *tab, chars, 2);
  fourth_tab = new XMForm("Output Options", *tab, chars, 2);

  /* Next, we create the widgets needed for the page setup tab */
  XtSetArg(chars[0], XmNshadowThickness, 2);
  XtSetArg(chars[1], XmNwidth, 320);
  XtSetArg(chars[2], XmNheight, 75);
  XtSetArg(chars[3], XmNtopAttachment, XmATTACH_FORM);
  XtSetArg(chars[4], XmNrightAttachment, XmATTACH_FORM);
  XtSetArg(chars[5], XmNleftAttachment, XmATTACH_FORM);
  layout_frame = new XMFrame("PSFrameOne", *first_tab, chars, 6);

  XtSetArg(chars[0], XmNradioAlwaysOne, True);
  XtSetArg(chars[1], XmNradioBehavior,  True);
  XtSetArg(chars[2], XmNleftAttachment, XmATTACH_FORM);
  XtSetArg(chars[3], XmNtopAttachment,  XmATTACH_FORM);
  XtSetArg(chars[4], XmNrightAttachment,XmATTACH_FORM);
  type_manager = new XMRowColumn("PtypeRadioBox", *layout_frame, chars, 5);

  XtSetArg(chars[0], XmNradioAlwaysOne, True);
  XtSetArg(chars[1], XmNradioBehavior,  True);
  XtSetArg(chars[2], XmNleftAttachment, XmATTACH_FORM);
  XtSetArg(chars[3], XmNtopAttachment, XmATTACH_WIDGET);
  XtSetArg(chars[5], XmNtopWidget, type_manager->getid());
  num_manager = new XMRowColumn("PnumRadioBox", *first_tab, chars, 6);

  XtSetArg(chars[0], XmNshadowThickness, 2);
  XtSetArg(chars[1], XmNwidth, 320);
  XtSetArg(chars[2], XmNheight, 40);
  XtSetArg(chars[3], XmNtopAttachment, XmATTACH_WIDGET);
  XtSetArg(chars[5], XmNleftAttachment, XmATTACH_FORM);
  XtSetArg(chars[6], XmNtopWidget, num_manager->getid());
  spp_frame = new XMFrame("PsppFrame", *first_tab, chars, 7);

  XtSetArg(chars[0], XmNtopAttachment, XmATTACH_FORM);
  XtSetArg(chars[1], XmNleftAttachment, XmATTACH_FORM);
  XtSetArg(chars[2], XmNrightAttachment, XmATTACH_FORM);
  spp_mgr = new XMRowColumn("PtextRowCol", *spp_frame, chars, 3);
  spp_mgr->SetOrientation(XmHORIZONTAL);
  spp_mgr->SetPacking(XmPACK_TIGHT);

  port = new XMToggleButton("Portrait", *type_manager);
  port->Label("Portrait");
  land = new XMToggleButton("Landscape", *type_manager);
  land->Label("Landscape");
  print_sel = new XMToggleButton("PrintSel", *num_manager);
  print_sel->Label("Print Selected Spectrum");
  print_sel->AddCallback(PrintSelCallback, &Print_Options);
  print_all = new XMToggleButton("PrintAll", *num_manager);
  print_all->Label("Print All Spectra");
  print_all->AddCallback(PrintAllCallback, &Print_Options);

  XtSetArg(chars[0], XmNleftAttachment, XmATTACH_FORM);
  rows = new XMLabel("Prows", *spp_mgr, "Rows", chars, 1);
  XtSetArg(chars[0], XmNleftAttachment, XmATTACH_WIDGET);
  XtSetArg(chars[1], XmNleftWidget, rows->getid());
  t_rows = new XMText("rows", *spp_mgr, 1, 2, chars, 2);

  XtSetArg(chars[1], XmNleftWidget, t_rows->getid());
  cols = new XMLabel("Pcols", *spp_mgr, "Columns", chars, 2);
  XtSetArg(chars[1], XmNleftWidget, cols->getid());
  t_cols = new XMText("cols", *spp_mgr, 1, 2, chars, 2);

  XtSetArg(chars[0], XmNrightAttachment, XmATTACH_FORM);
  XtSetArg(chars[1], XmNleftAttachment, XmATTACH_FORM);
  XtSetArg(chars[2], XmNtopAttachment, XmATTACH_WIDGET);
  XtSetArg(chars[3], XmNtopWidget, spp_frame->getid());
  XtSetArg(chars[4], XmNshadowThickness, 2);
  XtSetArg(chars[5], XmNwidth, 320);
  XtSetArg(chars[6], XmNheight, 110);
  size_frame = new XMFrame("PsizeFrame", *first_tab, chars, 7);

  XtSetArg(chars[0], XmNradioAlwaysOne, True);
  XtSetArg(chars[1], XmNradioBehavior,  True);
  XtSetArg(chars[2], XmNleftAttachment, XmATTACH_FORM);
  XtSetArg(chars[3], XmNtopAttachment,  XmATTACH_FORM);
  XtSetArg(chars[4], XmNrightAttachment,XmATTACH_FORM);
  size_mgr = new XMRowColumn("PtypeRadioBox", *size_frame, chars, 5);

  user_specified = new XMToggleButton("User", *size_mgr);
  user_specified->Label("Specify size in Spectrum Options");
  user_specified->AddCallback(PresetSizeCallback, &Print_Options);
  ten_by_ten = new XMToggleButton("Square", *size_mgr);
  ten_by_ten->Label("4\" x 4\"");
  ten_by_ten->AddCallback(PresetSizeCallback, &Print_Options);
  ten_by_fifteen = new XMToggleButton("Rectangle", *size_mgr);
  ten_by_fifteen->Label("6\" x 4\"");
  ten_by_fifteen->AddCallback(PresetSizeCallback, &Print_Options);

  /* Here, we create the widgets needed for the Spectrum Options tab */

  XtSetArg(chars[0], XmNleftAttachment, XmATTACH_FORM);
  XtSetArg(chars[1], XmNrightAttachment, XmATTACH_FORM);
  XtSetArg(chars[2], XmNtopAttachment, XmATTACH_FORM);
  row_one = new XMRowColumn("ProwOne", *second_tab, chars, 3);
  row_one->SetOrientation(XmHORIZONTAL);
  row_one->SetPacking(XmPACK_TIGHT);

  XtSetArg(chars[2], XmNtopAttachment, XmATTACH_WIDGET);
  XtSetArg(chars[3], XmNtopWidget, row_one->getid());
  row_two = new XMRowColumn("ProwTwo", *second_tab, chars, 4);
  row_two->SetOrientation(XmHORIZONTAL);
  row_two->SetPacking(XmPACK_TIGHT);

  XtSetArg(chars[3], XmNtopWidget, row_two->getid());
  row_three = new XMRowColumn("ProwThree", *second_tab, chars, 4);
  row_three->SetOrientation(XmHORIZONTAL);
  row_three->SetPacking(XmPACK_TIGHT);
  
  XtSetArg(chars[3], XmNtopWidget, row_three->getid());
  row_four = new XMRowColumn("ProwFour", *second_tab, chars, 4);
  row_four->SetOrientation(XmHORIZONTAL);
  row_four->SetPacking(XmPACK_TIGHT);
  
  XtSetArg(chars[3], XmNtopWidget, row_four->getid());
  row_five = new XMRowColumn("ProwFive", *second_tab, chars, 4);
  row_five->SetOrientation(XmHORIZONTAL);
  row_five->SetPacking(XmPACK_TIGHT);

  XtSetArg(chars[3], XmNtopWidget, row_five->getid());
  row_six = new XMRowColumn("ProwSix", *second_tab, chars, 4);
  row_six->SetOrientation(XmHORIZONTAL);
  row_six->SetPacking(XmPACK_TIGHT);

  XtSetArg(chars[3], XmNtopWidget, row_six->getid());
  XtSetArg(chars[4], XmNbottomAttachment, XmATTACH_FORM);
  drawing_mgr = new XMRowColumn("PdrawMgr", *second_tab, chars, 5);
  drawing_mgr->SetOrientation(XmHORIZONTAL);
  drawing_mgr->SetPacking(XmPACK_TIGHT);

  XtSetArg(chars[0], XmNleftAttachment, XmATTACH_FORM);
  XtSetArg(chars[1], XmNtopAttachment, XmATTACH_FORM);
  XtSetArg(chars[2], XmNbottomAttachment, XmATTACH_FORM);
  XtSetArg(chars[3], XmNalignment, XmALIGNMENT_END);
  spec_title = new XMLabel("Ptitle", *row_one, "Spectrum Title", chars, 4);
  XtSetArg(chars[0], XmNrightAttachment, XmATTACH_FORM);
  t_title = new XMText("title", *row_one, 1, 25, chars, 3);

  XtSetArg(chars[0], XmNleftAttachment, XmATTACH_FORM);
  name_x = new XMLabel("Pxname", *row_two, "X-Axis Name", chars, 4);
  XtSetArg(chars[0], XmNrightAttachment, XmATTACH_FORM);
  t_name_x = new XMText("xname", *row_two, 1, 28, chars, 3);

  XtSetArg(chars[0], XmNleftAttachment, XmATTACH_FORM);
  name_y = new XMLabel("Pyname", *row_three, "Y-Axis Name", chars, 4);
  XtSetArg(chars[0], XmNrightAttachment, XmATTACH_FORM);
  t_name_y = new XMText("yname", *row_three, 1, 28, chars, 3);

  XtSetArg(chars[0], XmNleftAttachment, XmATTACH_FORM);
  length_x = new XMLabel("Pxlen", *row_four, "X-Axis Length (in)", chars, 4);
  XtSetArg(chars[0], XmNrightAttachment, XmATTACH_FORM);
  t_length_x = new XMText("xlen", *row_four, 1, 21, chars, 3);

  XtSetArg(chars[0], XmNleftAttachment, XmATTACH_FORM);
  length_y = new XMLabel("Pylen", *row_five, "Y-Axis Length (in)", chars, 4);
  XtSetArg(chars[0], XmNrightAttachment, XmATTACH_FORM);
  t_length_y = new XMText("ylen", *row_five, 1, 21, chars, 3);

  XtSetArg(chars[0], XmNleftAttachment, XmATTACH_FORM);
  contour_inc = new XMLabel("Pcont", *row_six, "Contour Increment", chars, 4);
  XtSetArg(chars[0], XmNrightAttachment, XmATTACH_FORM);
  t_contour_inc = new XMText("cont", *row_six, 1, 22, chars, 3);

  draw_palette = new XMToggleButton("palette", *drawing_mgr);
  draw_palette->Label("Draw color palette");
  draw_contours = new XMToggleButton("contours", *drawing_mgr);
  draw_contours->Label("Draw contour lines"); 
  draw_contours->AddCallback(DrawContoursCallback, &Contours_Data);
  draw_time = new XMToggleButton("time", *drawing_mgr);
  draw_time->Label("Draw time stamp   ");
  use_symbols = new XMToggleButton("symbols", *drawing_mgr);
  use_symbols->Label("Superpositions use symbols");

  Contours_Data.t_contour_inc = t_contour_inc;
  Contours_Data.tb = draw_contours;

  /* Here are the widgets for the Tic Marks tab */

  XtSetArg(chars[0], XmNrightAttachment, XmATTACH_FORM);
  XtSetArg(chars[1], XmNleftAttachment, XmATTACH_FORM);
  XtSetArg(chars[2], XmNtopAttachment, XmATTACH_FORM);
  XtSetArg(chars[3], XmNshadowThickness, 2);
  XtSetArg(chars[4], XmNwidth, 320);
  XtSetArg(chars[5], XmNheight, 75);
  tics_frame = new XMFrame("Ptics", *third_tab, chars, 6);

  XtSetArg(chars[2], XmNtopAttachment, XmATTACH_WIDGET);
  XtSetArg(chars[5], XmNheight, 75);
  XtSetArg(chars[6], XmNtopWidget, tics_frame->getid());
  tics_entry_frame = new XMFrame("Pticsentry", *third_tab, chars, 7);

  XtSetArg(chars[2], XmNtopAttachment, XmATTACH_FORM);
  XtSetArg(chars[3], XmNradioAlwaysOne, True);
  XtSetArg(chars[4], XmNradioBehavior,  True);
  tics_mgr = new XMRowColumn("Pticsmgr", *tics_frame, chars, 5);

  tics_entry_mgr = 
    new XMRowColumn("Pticsentrymgr", *tics_entry_frame, chars, 3);
  tics_entry_mgr->SetOrientation(XmHORIZONTAL);
  tics_entry_mgr->SetPacking(XmPACK_TIGHT);
  
  default_tics = new XMToggleButton("default", *tics_mgr);
  default_tics->Label("Use Xamine tic intervals");
  default_tics->AddCallback(UseDefaultTicsCallback, &Print_Options);
  user_tics    = new XMToggleButton("user", *tics_mgr);
  user_tics->Label("Specify tic intervals");
  user_tics->AddCallback(SpecifyTicsCallback, &Contours_Data);

  XtSetArg(chars[0], XmNleftAttachment, XmATTACH_FORM);
  XtSetArg(chars[1], XmNtopAttachment, XmATTACH_FORM);
  xmajor = new XMLabel("Pxmaj", *tics_entry_mgr, "X Major", chars, 2);  
  XtSetArg(chars[0], XmNrightAttachment, XmATTACH_FORM);
  t_xmajor = new XMText("xmaj", *tics_entry_mgr, 1, 15, chars, 2);

  XtSetArg(chars[0], XmNleftAttachment, XmATTACH_FORM);
  XtSetArg(chars[1], XmNtopAttachment, XmATTACH_WIDGET);
  XtSetArg(chars[2], XmNtopWidget, xmajor->getid());
  xminor = new XMLabel("Pxmin", *tics_entry_mgr, "X Minor", chars, 3);
  XtSetArg(chars[0], XmNrightAttachment, XmATTACH_FORM);
  XtSetArg(chars[2], XmNtopWidget, t_xmajor->getid());
  t_xminor = new XMText("xmin", *tics_entry_mgr, 1, 15, chars, 3);

  XtSetArg(chars[0], XmNleftAttachment, XmATTACH_FORM);
  XtSetArg(chars[2], XmNtopWidget, xminor->getid());
  ymajor = new XMLabel("Pymaj", *tics_entry_mgr, "Y Major", chars, 3);
  XtSetArg(chars[0], XmNrightAttachment, XmATTACH_FORM);
  XtSetArg(chars[2], XmNtopWidget, t_xminor->getid());
  t_ymajor = new XMText("ymaj", *tics_entry_mgr, 1, 15, chars, 3);

  XtSetArg(chars[0], XmNleftAttachment, XmATTACH_FORM);
  XtSetArg(chars[2], XmNtopWidget, ymajor->getid());
  yminor = new XMLabel("Pymin", *tics_entry_mgr, "Y Minor", chars, 3);
  XtSetArg(chars[0], XmNrightAttachment, XmATTACH_FORM);
  XtSetArg(chars[2], XmNtopWidget, t_ymajor->getid());
  t_yminor = new XMText("ymin", *tics_entry_mgr, 1, 15, chars, 3);

  Print_Options.t_rows = t_rows;
  Print_Options.t_cols = t_cols;
  Print_Options.t_title = t_title;
  Print_Options.t_name_x = t_name_x;
  Print_Options.t_name_y = t_name_y;
  Print_Options.t_length_x = t_length_x;
  Print_Options.t_length_y = t_length_y;
  Print_Options.ten_by_ten = ten_by_ten;
  Print_Options.ten_by_fifteen = ten_by_fifteen;
  Print_Options.user_specified = user_specified;
  Print_Options.draw_palette = draw_palette;
  Print_Options.draw_contours = draw_contours;
  Print_Options.t_xmajor = t_xmajor;
  Print_Options.t_ymajor = t_ymajor;
  Print_Options.t_xminor = t_xminor;
  Print_Options.t_yminor = t_yminor;

  /* These are the widgets for the Output options tab */
  XtSetArg(chars[0], XmNshadowThickness, 2);
  XtSetArg(chars[1], XmNwidth, 320);
  XtSetArg(chars[2], XmNheight, 75);
  XtSetArg(chars[3], XmNtopAttachment, XmATTACH_FORM);
  XtSetArg(chars[4], XmNrightAttachment, XmATTACH_FORM);
  XtSetArg(chars[5], XmNleftAttachment, XmATTACH_FORM);
  dest_frame = new XMFrame("PprintOpt", *fourth_tab, chars, 6);

  XtSetArg(chars[0], XmNradioAlwaysOne, True);
  XtSetArg(chars[1], XmNradioBehavior,  True);
  XtSetArg(chars[2], XmNleftAttachment, XmATTACH_FORM);
  XtSetArg(chars[3], XmNtopAttachment,  XmATTACH_FORM);
  XtSetArg(chars[4], XmNrightAttachment,XmATTACH_FORM);
  dest_mgr = new XMRowColumn("PdestMgr", *dest_frame, chars, 4);

  XtSetArg(chars[3], XmNtopAttachment, XmATTACH_WIDGET);
  XtSetArg(chars[4], XmNtopWidget, dest_frame->getid());
  cmd_mgr = new XMRowColumn("PcmdMgr", *fourth_tab, chars, 5);
  cmd_mgr->SetOrientation(XmHORIZONTAL);
  cmd_mgr->SetPacking(XmPACK_TIGHT);

  XtSetArg(chars[4], XmNtopWidget, cmd_mgr->getid());
  file_mgr = new XMRowColumn("PfileMgr", *fourth_tab, chars, 5);
  file_mgr->SetOrientation(XmHORIZONTAL);
  file_mgr->SetPacking(XmPACK_TIGHT);

  XtSetArg(chars[4], XmNtopWidget, file_mgr->getid());
  XtSetArg(chars[5], XmNrightAttachment, XmATTACH_FORM);
  XtSetArg(chars[6], XmNselectionPolicy, XmSINGLE_SELECT);
  list_type = new XMScrolledList("PlistType", *fourth_tab, 5, chars, 7);
  list_type->AddItem("Postscript (*.ps)");
  list_type->AddItem("JPEG, 72 dpi (*.jpg)");
  list_type->AddItem("JPEG, 300 dpi (*.jpg)");
  list_type->AddItem("Portable Network Graphics, 72 dpi (*.png)");
  list_type->AddItem("Portable Network Graphics, 300 dpi (*.png)");
  list_type->SelectItem(1);

  to_printer = new XMToggleButton("toPrinter", *dest_mgr);
  to_printer->Label("To Printer");
  to_printer->AddCallback(ToPrinterCallback, &Printer_Options_Data);

  to_file    = new XMToggleButton("toFile", *dest_mgr);
  to_file->Label("To File");
  to_file->AddCallback(ToFileCallback, &Printer_Options_Data);

  XtSetArg(chars[0], XmNleftAttachment, XmATTACH_FORM);
  XtSetArg(chars[1], XmNtopAttachment, XmATTACH_FORM);
  XtSetArg(chars[2], XmNbottomAttachment, XmATTACH_FORM);
  cmd = new XMLabel("cmdName", *cmd_mgr, "Print Command", chars, 3);
  XtSetArg(chars[0], XmNrightAttachment, XmATTACH_FORM);
  t_cmd  = new XMText("cmd", *cmd_mgr, 1, 28, chars, 3);

  XtSetArg(chars[0], XmNleftAttachment, XmATTACH_FORM);
  file = new XMLabel("fileName", *file_mgr, "Output File Path", chars, 3);
  XtSetArg(chars[0], XmNrightAttachment, XmATTACH_FORM);
  t_file = new XMText("filen", *file_mgr, 1, 25, chars, 3);

  XtSetArg(chars[0], XmNleftAttachment, XmATTACH_FORM);
  XtSetArg(chars[1], XmNtopAttachment, XmATTACH_WIDGET);
  XtSetArg(chars[2], XmNrightAttachment, XmATTACH_FORM);
  XtSetArg(chars[3], XmNtopWidget, list_type->getid());
  res = new XMLabel("resolution", *fourth_tab, "Select 2d Output Resolution", 
		    chars, 4);

  XtSetArg(chars[0], XmNleftAttachment, XmATTACH_FORM);
  XtSetArg(chars[1], XmNtopAttachment, XmATTACH_WIDGET);
  XtSetArg(chars[2], XmNtopWidget, res->getid());
  XtSetArg(chars[3], XmNrightAttachment, XmATTACH_FORM);
  XtSetArg(chars[4], XmNshadowThickness, 2);
  XtSetArg(chars[5], XmNheight, 75);
  res_frame = new XMFrame("PresFrame", *fourth_tab, chars, 6);

  XtSetArg(chars[1], XmNtopAttachment, XmATTACH_FORM);
  XtSetArg(chars[3], XmNradioAlwaysOne, True);
  XtSetArg(chars[4], XmNradioBehavior,  True);
  res_mgr = new XMRowColumn("PresMgr", *res_frame, chars, 5);
  res_mgr->SetOrientation(XmHORIZONTAL);
  res_mgr->SetPacking(XmPACK_TIGHT);

  one_to_one = new XMToggleButton("onetoone", *res_mgr);
  one_to_one->Label("1:1    ");
  one_to_two = new XMToggleButton("twotoone", *res_mgr);
  one_to_two->Label("1:2    ");
  one_to_four = new XMToggleButton("fourtoone", *res_mgr);
  one_to_four->Label("1:4    ");
  one_to_eight = new XMToggleButton("eighttoone", *res_mgr);
  one_to_eight->Label("1:8");

  Printer_Options_Data.t_file = t_file;
  Printer_Options_Data.t_cmd  = t_cmd;
  Printer_Options_Data.list_type = list_type;

  t_file->SetAttribute(XmNsensitive, (long int)False);
  list_type->SetAttribute(XmNsensitive, (long int)False);

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

  char *newcmd;

  /*  If the Ok button was hit then process it: */

  if(button == dlg->ok()) {

    /* Deal with the textuals: */

    newcmd  = dlg->printcmd();
    strcpy(printcmd, newcmd);
    strcpy(dflt_print_opts->print_cmd, printcmd);
    XtFree(newcmd);
   
    ptype = dlg->printertype();

    // Save the defaults file:

    Xamine_SaveDefaultProperties();
  }

  /* Pop down the dialog */

  dlg->UnManage();
}

/*
  void TabsActionCallback(XMWidget* w, XtPointer user, XtPointer call)
  
    This is the callback when the "OK" button is pressed on the "Print"
    tabs dialog widget. It invokes the Xamine_Print function to generate
    the printout.
*/
void TabsActionCallback(XMWidget* w, XtPointer user, XtPointer call)
{
  XMPushButton *button       = (XMPushButton *)w;
  PrintSpectrumDialog *dlg   = (PrintSpectrumDialog*) user;

  char* newcmd;

  /*  If the Ok button was hit then process it: */

  if(button == dlg->ok()) {

    /* Deal with the textuals: */
    newcmd  = dlg->getcmd();
    strcpy(printcmd, newcmd);
    XtFree(newcmd);
    if(!Xamine_SetDfltPrintOpts(w, dlg, call)) {
      printf("Warning: Unable to write default printer options to disk\n");
      perror("Xamine_SetDfltPrintOpts");
    }
    Xamine_Print(w, dlg, call);
  }
  
  /* Pop down the dialog */

  dlg->UnManage();
}

/*
  void Xamine_PrintAllCallback
   
    This function is invoked when the "Print All" toggle button is selected.
    It "desensitizes" the title, axis names and lengths entry boxes.
*/
void PrintAllCallback(XMWidget *w, XtPointer user, XtPointer call)
{
  XMText* rows  = (XMText*)(Print_Options.t_rows);
  XMText* cols  = (XMText*)(Print_Options.t_cols);
  XMText* title = (XMText*)(Print_Options.t_title);
  XMText* xname = (XMText*)(Print_Options.t_name_x);
  XMText* yname = (XMText*)(Print_Options.t_name_y);
  XMText* xlen  = (XMText*)(Print_Options.t_length_x);
  XMText* ylen  = (XMText*)(Print_Options.t_length_y);
  XMToggleButton* tbt = (XMToggleButton*)(Print_Options.ten_by_ten);
  XMToggleButton* tbf = (XMToggleButton*)(Print_Options.ten_by_fifteen);
  XMToggleButton* us  = (XMToggleButton*)(Print_Options.user_specified);
  XMToggleButton* dp  = (XMToggleButton*)(Print_Options.draw_palette);
  XMToggleButton* dc  = (XMToggleButton*)(Print_Options.draw_contours);

  // If there are any 2d spectra on the display, we allow the user to
  // select 'draw color palette' from spectrum options...
  XtArgVal is2d = False;
  int nr = Xamine_Panerows();
  int nc = Xamine_Panecols();
  for(int r = 0; r < nr; r++) {
    for(int c = 0; c < nc; c++) {
      win_attributed *at = Xamine_GetDisplayAttributes(r,c);
      if(at != NULL) {
	if((xamine_shared->gettype(at->spectrum()) == twodbyte) ||
	   (xamine_shared->gettype(at->spectrum()) == twodword)) {
	  is2d = True;
	  break;
	}
      }
    }
  }

  rows->SetAttribute(XmNsensitive, (long int)True);
  cols->SetAttribute(XmNsensitive, (long int)True);
  title->SetAttribute(XmNsensitive, (long int)False);
  xname->SetAttribute(XmNsensitive, (long int)False);
  yname->SetAttribute(XmNsensitive, (long int)False);
  xlen->SetAttribute(XmNsensitive, (long int)False);
  ylen->SetAttribute(XmNsensitive, (long int)False);
  tbt->SetAttribute(XmNsensitive, (long int)False);
  tbf->SetAttribute(XmNsensitive, (long int)False);
  us->SetAttribute(XmNsensitive, (long int)False);
  dp->SetAttribute(XmNsensitive, (long int)is2d);
  dc->SetAttribute(XmNsensitive, (long int)False);
}

/*
  void PrintSelCallback

    This is called when the "Print Selected" toggle button is selected.
    It sensitizes the spectrum title, axis name and length entry boxes.
*/
void PrintSelCallback(XMWidget* w, XtPointer user, XtPointer call)
{
  XMText* rows  = (XMText*)(Print_Options.t_rows);
  XMText* cols  = (XMText*)(Print_Options.t_cols);
  XMText* title = (XMText*)(Print_Options.t_title);
  XMText* xname = (XMText*)(Print_Options.t_name_x);
  XMText* yname = (XMText*)(Print_Options.t_name_y);
  XMText* xlen  = (XMText*)(Print_Options.t_length_x);
  XMText* ylen  = (XMText*)(Print_Options.t_length_y);
  XMToggleButton* tbt = (XMToggleButton*)(Print_Options.ten_by_ten);
  XMToggleButton* tbf = (XMToggleButton*)(Print_Options.ten_by_fifteen);
  XMToggleButton* us  = (XMToggleButton*)(Print_Options.user_specified);
  XMToggleButton* dp  = (XMToggleButton*)(Print_Options.draw_palette);
  XMToggleButton* dc  = (XMToggleButton*)(Print_Options.draw_contours);

  // If the selected spectrum is a 2d spectra, we allow the user to
  // select 'draw color palette' and 'draw contours' from spectrum options...
  XtArgVal is2d = False;
  win_attributed *at = Xamine_GetSelectedDisplayAttributes();
  if((xamine_shared->gettype(at->spectrum()) == twodbyte) ||
     (xamine_shared->gettype(at->spectrum()) == twodword))
    is2d = True;

  rows->SetAttribute(XmNsensitive, (long int)False);
  cols->SetAttribute(XmNsensitive, (long int)False);
  title->SetAttribute(XmNsensitive, (long int)True);
  xname->SetAttribute(XmNsensitive, (long int)True);
  yname->SetAttribute(XmNsensitive, (long int)True);
  xlen->SetAttribute(XmNsensitive, (long int)True);
  ylen->SetAttribute(XmNsensitive, (long int)True);
  tbt->SetAttribute(XmNsensitive, (long int)True);
  tbf->SetAttribute(XmNsensitive, (long int)True);
  us->SetAttribute(XmNsensitive, (long int)True);
  dp->SetAttribute(XmNsensitive, (long int)is2d);
  dc->SetAttribute(XmNsensitive, (long int)is2d);  
}

/*
  void PresetSizeCallback

    When a preset size is selected by the user, the ability to enter a size
    is disabled.
*/
void PresetSizeCallback(XMWidget* w, XtPointer user, XtPointer call)
{
  XMText* xlen = (XMText*)(Print_Options.t_length_x);
  XMText* ylen = (XMText*)(Print_Options.t_length_y);
  XMToggleButton* tbt = (XMToggleButton*)(Print_Options.ten_by_ten);
  XMToggleButton* tbf = (XMToggleButton*)(Print_Options.ten_by_fifteen);
  XMToggleButton* us  = (XMToggleButton*)(Print_Options.user_specified);

  if(tbt->GetState() || tbf->GetState()) {
    xlen->SetAttribute(XmNsensitive, (long int)False);
    ylen->SetAttribute(XmNsensitive, (long int)False);
  }
  else if(us->GetState()) {
    xlen->SetAttribute(XmNsensitive, (long int)True);
    ylen->SetAttribute(XmNsensitive, (long int)True);
  }
}

/*
  void ToPrinterCallback

    Selecting "To Printer" disables the ability to select an output file path
    and format type.
*/
void ToPrinterCallback(XMWidget* w, XtPointer user, XtPointer call)
{
  XMText* file = (XMText*)(Printer_Options_Data.t_file);
  XMText* cmd  = (XMText*)(Printer_Options_Data.t_cmd);
  XMScrolledList* sl = (XMScrolledList*)(Printer_Options_Data.list_type);

  file->SetAttribute(XmNsensitive, (long int)False);
  cmd->SetAttribute(XmNsensitive, (long int)True);
  sl->SetAttribute(XmNsensitive, (long int)False);
}

/*
  void ToFileCallback

    Selecting the "To File" button disables the ability to set the print
    command
*/
void ToFileCallback(XMWidget* w, XtPointer user, XtPointer call)
{
  XMText* file = (XMText*)(Printer_Options_Data.t_file);
  XMText* cmd  = (XMText*)(Printer_Options_Data.t_cmd);
  XMScrolledList* sl = (XMScrolledList*)(Printer_Options_Data.list_type);
  
  file->SetAttribute(XmNsensitive, (long int)True);
  cmd->SetAttribute(XmNsensitive, (long int)False);
  sl->SetAttribute(XmNsensitive, (long int)True);
}

/*
  void DrawContoutsCallback

    Selecting "Draw Contours" allows the user to set the contour increment
*/
void DrawContoursCallback(XMWidget* w, XtPointer user, XtPointer call)
{
  XMText* inc = (XMText*)(Contours_Data.t_contour_inc);
  XMToggleButton* tb = (XMToggleButton*)(Contours_Data.tb);

  if(tb->GetState())
    inc->SetAttribute(XmNsensitive, (long int)True);
  else
    inc->SetAttribute(XmNsensitive, (long int)False);
}

void UseDefaultTicsCallback(XMWidget* w, XtPointer user, XtPointer call)
{
  XMText* xmaj = (XMText*)(Print_Options.t_xmajor);
  XMText* xmin = (XMText*)(Print_Options.t_xminor);
  XMText* ymaj = (XMText*)(Print_Options.t_ymajor);
  XMText* ymin = (XMText*)(Print_Options.t_yminor);

  xmaj->SetAttribute(XmNsensitive, (long int)False);
  xmin->SetAttribute(XmNsensitive, (long int)False);
  ymaj->SetAttribute(XmNsensitive, (long int)False);
  ymin->SetAttribute(XmNsensitive, (long int)False);
}

void SpecifyTicsCallback(XMWidget* w, XtPointer user, XtPointer call)
{
  XMText* xmaj = (XMText*)(Print_Options.t_xmajor);
  XMText* xmin = (XMText*)(Print_Options.t_xminor);
  XMText* ymaj = (XMText*)(Print_Options.t_ymajor);
  XMText* ymin = (XMText*)(Print_Options.t_yminor);

  xmaj->SetAttribute(XmNsensitive, (long int)True);
  xmin->SetAttribute(XmNsensitive, (long int)True);
  ymaj->SetAttribute(XmNsensitive, (long int)True);
  ymin->SetAttribute(XmNsensitive, (long int)True);
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
    dialog->AddCallback(XtNdestroyCallback, NullPointer, (XtPointer)&dialog);
  }
  
  /* Make the dialog contents match the current defaults. */

  
  dialog->printcmd(Xamine_GetPrintCommand());
  dialog->printertype(Xamine_GetPrinterType());

  /* Manage the dialog to pop it up */

  dialog->Manage();

}

/*
** Functional Description:
**   Xamine_PrintSpectrumDialog:
**     Prompts for the print spectrum setup dialog.
**     The dialog is NOT cached for multiple re-use because that wasn't working
**     very well.
** Formal Parameters:
**    XMWidget *w:
**      The widget to be used as the parent for the dialog widget subtree.
**   All other parameters are the same as for a callback but are ignored.
*/
void Xamine_PrintSpectrumDialog(XMWidget* w, XtPointer user, XtPointer call)
{
  // Make sure the dialog is deleted.
  if(ps_dialog) {
    delete ps_dialog;
    ps_dialog = NULL;
  }
  ps_dialog = new PrintSpectrumDialog("PrintSpectrum", w, 
				      "Print Spectrum Options");
  ps_dialog->AddOkCallback(TabsActionCallback, ps_dialog);
  ps_dialog->AddCancelCallback(TabsActionCallback, ps_dialog);
  ps_dialog->AddHelpCallback(Xamine_display_help, &help_setup);
  ps_dialog->SetModal(XmDIALOG_FULL_APPLICATION_MODAL);

  win_attributed* pAttributes = Xamine_GetSelectedDisplayAttributes();
  char xlabel[72];
  char ylabel[72];
  int nSpectrum = pAttributes->spectrum();
  int fDefaultsExist = (dflt_print_opts != NULL);
  if(!dflt_print_opts) {
    dflt_print_opts = new DefaultPrintOptions;
  }
  ps_dialog->setnum(dflt_print_opts->num);
  
  switch(xamine_shared->gettype(nSpectrum)) {
  case onedword:
  case onedlong: {
    win_1d* pAttrib = (win_1d*)pAttributes;

    // If this is a 1d, we can't draw contours or a color palette!
    ps_dialog->setdraw_palette(false);
    ps_dialog->setdraw_contours(false);
    ps_dialog->setcontour_val(false);

    // If it's mapped, give default values to the x and y axis names
    // (provided that we are printing a single spectrum, not all spectra)
    if((pAttrib->ismapped()) && (ps_dialog->getnum() == printsel)) {
      xamine_shared->getxlabel_map(xlabel, nSpectrum);
      if(pAttrib->isflipped()) {
	ps_dialog->setyname(xlabel);
	ps_dialog->setxname("");
      } else {
	ps_dialog->setxname(xlabel);
	ps_dialog->setyname("");
      }
    }
    else {
      ps_dialog->setxname("");
      ps_dialog->setyname("");
    }
    break;
  }
  case twodbyte:
  case twodword: {
    win_2d* pAttrib = (win_2d*)pAttributes;
    ps_dialog->setdraw_palette(true);  // this says the palette can be drawn
    ps_dialog->setdraw_contours(true); // this says the contours can be drawn
    ps_dialog->setcontour_val(false);  /* this says the contour text box is 
					  not sensitive at construction. */

    // If this is a mapped spectrum, then give values to the x and y
    // axis labels (provided we are printing a single spectrum)
    if((pAttrib->ismapped()) && (ps_dialog->getnum() == printsel)) {
      xamine_shared->getxlabel_map(xlabel, nSpectrum);
      xamine_shared->getylabel_map(ylabel, nSpectrum);
      if(pAttrib->isflipped()) {
	ps_dialog->setyname(xlabel);
	ps_dialog->setxname(ylabel);
      } else {
	ps_dialog->setxname(xlabel);
	ps_dialog->setyname(ylabel);
      }
    }
    else {
      ps_dialog->setxname("");
      ps_dialog->setyname("");
    }
    break;
  }
  }
  
  // Default to using colored lines on superimposed spectra (not symbols)
  ps_dialog->setuse_symbols(false);

  int nr = Xamine_Panerows();
  int nc = Xamine_Panecols();
  int nSuperposCount = 0;
  for(int r = 0; r < nr; r++) {
    for(int c = 0; c < nc; c++) {
      win_attributed *at = Xamine_GetDisplayAttributes(r, c);
      if(at) {
	if(xamine_shared->gettype(at->spectrum()) == onedword ||
	   xamine_shared->gettype(at->spectrum()) == onedlong) {
	  win_1d* pAttrib = (win_1d*)at;
	  SuperpositionList SuperList = pAttrib->GetSuperpositions();
	  nSuperposCount = SuperList.Count();
	  if(nSuperposCount) break;
	}
      }
    }
    if(nSuperposCount) break;
  }
  if(nSuperposCount)
    ps_dialog->setuse_symbols(true);  // this means that symbols can be drawn
  
  /* Make the dialog contents match the current defaults. */
  char rows[10];
  char cols[10];
  sprintf(rows, "%d", Xamine_Panerows());
  sprintf(cols, "%d", Xamine_Panecols());
  char ci[10];
  int fs = pAttributes->getfsval();
  sprintf(ci, "%d", (fs / 4));

  ps_dialog->allow_geom(false);
  ps_dialog->allow_tics(false);
  ps_dialog->settics(deflt);

  ps_dialog->setlayout(dflt_print_opts->layout);


  
  ps_dialog->setrows(dflt_print_opts->rows);
  ps_dialog->setcols(dflt_print_opts->cols);

  ps_dialog->settitle
    (const_cast<char*>(Xamine_GetSpectrumTitle().c_str()));
  
  char xlen[10];
  char ylen[10];
  sprintf(xlen, "%f", dflt_print_opts->xlen);
  sprintf(ylen, "%f", dflt_print_opts->ylen);
  ps_dialog->setxlen(xlen);
  ps_dialog->setylen(ylen);

  ps_dialog->setspecified();
  if(fs / 4) {
    ps_dialog->setcontour_inc(ci);
  }

  ps_dialog->setcmd(dflt_print_opts->print_cmd);
  ps_dialog->setdest(dflt_print_opts->dest);
  ps_dialog->setlist_type(dflt_print_opts->file_type+1);
  ps_dialog->setres(dflt_print_opts->res);
  ps_dialog->setfile(const_cast<char*>(Xamine_GetOutputFilename().c_str()));

  // If defaults exist in the defaults file, set the appropriate
  // spectrum options toggle switch states.
  
  switch(xamine_shared->gettype(nSpectrum)) {
  case twodbyte:
  case twodword:
    ps_dialog->set_palette(dflt_print_opts->color_pal);
    ps_dialog->setdraw_palette(1);
    ps_dialog->set_contours(dflt_print_opts->contours);
    break;
  case onedword:
  case onedlong:
    ps_dialog->set_symbols(dflt_print_opts->symbols);
    break;
  }
  ps_dialog->set_time(dflt_print_opts->time_stamp);

  /* Manage the dialog to pop it up */

  ps_dialog->Manage();
}

/*
  This is the callback if Gri is not installed
*/
void Xamine_NoGriDialog(XMWidget* w, XtPointer user, XtPointer call)
{

  // Not much for now
  fprintf(stderr, "Printing not available.");
  fprintf(stderr, " You must install the Gri plotting packge!\n");
}


/*
  Xamine_SetDfltPrintOpts(XMWidget* w, XtPointer user, XtPointer call)

  This function sets the default values specified by the user, so they
  can be reused the next time the "Print Spectrum Options" dialog is
  opened.
*/
int Xamine_SetDfltPrintOpts(XMWidget* w, XtPointer user, XtPointer call)
{
  if(!dflt_print_opts) {
    dflt_print_opts = new struct DefaultPrintOptions;
    if(!dflt_print_opts)
      return FALSE;
  }

  PrintSpectrumDialog *dlg    = (PrintSpectrumDialog*) user;
  dflt_print_opts->layout     = dlg->getlayout();
  dflt_print_opts->num        = dlg->getnum();
  dflt_print_opts->dest       = dlg->getdest();
  dflt_print_opts->res        = dlg->getres();
  string sType = string(dlg->gettype());
  if(sType == "JPEG, 72 dpi (*.jpg)") {
    dflt_print_opts->file_type = 1;
  }
  else if(sType == "JPEG, 300 dpi (*.jpg)") {
    dflt_print_opts->file_type = 2;
  }
  else if(sType == "Portable Network Graphics, 72 dpi (*.png)") {
    dflt_print_opts->file_type = 3;
  }
  else if(sType == "Portable Network Graphics, 300 dpi (*.png)") {
    dflt_print_opts->file_type = 4;
  }
  else {
    dflt_print_opts->file_type = 0;
  }
  dflt_print_opts->xlen       = (dlg->getxlen() / 2.54);
  dflt_print_opts->ylen       = (dlg->getylen() / 2.54);
  dflt_print_opts->time_stamp = dlg->getdraw_time();
  dflt_print_opts->color_pal  = dlg->getdraw_palette();
  dflt_print_opts->contours   = dlg->getdraw_contours();
  dflt_print_opts->symbols    = dlg->getuse_symbols();
  strcpy(dflt_print_opts->print_cmd, dlg->getcmd());
  strcpy(dflt_print_opts->rows, dlg->getrows());
  strcpy(dflt_print_opts->cols, dlg->getcols());
  
  // Save the print options in the defaults file
  Xamine_SaveDefaultProperties();

  return TRUE;
}

struct DefaultPrintOptions* Xamine_GetDefaultPrintOptions()
{
  return dflt_print_opts;
}

void Xamine_SetDefaultPrintOptions(struct DefaultPrintOptions* p)
{
  dflt_print_opts = p;
}
