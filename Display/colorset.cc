/*
** Facility:
**
**     Xamine - Color map building tool
**
** Abstract:
**
** Set the color maps for a display program.
** Sliders control the color, 
** Buttons control the color map entry.
** A pop-up dialog controls the range of values associated with a button.
**
** Usage:
**   colorset planes
**     Where planes is the numeric argument indicating the number of color
**     planes to set up.
**
** Author:
**    Ron Fox
**    NSCL
**    Michigan State University
**    East Lansing, MI 48824-1321
**
*/
#if 0
#ifdef unix
#pragma implementation "XMCallback.h"
#endif
#endif


/*
** Include files required:
*/
				/* Standard system includes. */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
				/* Motif/X11 includes. */
#include <Xm/Label.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>
#include <Xm/Separator.h>
#include <Xm/Text.h>
				/* XM C++ includes. */
#include "XMDialogs.h"
#include "XMWidget.h"
#include "XMPushbutton.h"
#include "XMMenus.h"
				/* Utility includes  */
#include "helpmenu.h"
#include "errormsg.h"
#include "procede.h"
#include "colormgr.h"
/*
** Preprocessor level symbols
*/

#define MAX_COLORS 256

#ifdef unix			
#define DEFAULT_FILEMASK "./*.ctbl" /* Unix default file search  for rd/wr */
#define ICON_FILE "/daq/etc/colorset.ico"   /* Unix Icon def file. */
#endif

#ifdef VMS			
#define DEFAULT_FILEMASK "[]*.ctbl" /* VMS default file search  for rd/wr */
#define ICON_FILE "DAQ_LIB:colorset.ico"  /* VMS Icon def file. */
#endif
#define DEFAULT_EXT ".ctbl"
/* 
** Data structure definitions:
*/
struct color_entry {
                     int low, hi; /* Low/Hi range of values. */
		     int red,green,blue; /* Color entries */
		     unsigned long pixel; /* The pixel number assigned to map. */
		     XMPushButton *button; /* Widget ID of the push button. */
		   };
/*
** Derived classes:
*/

class ColorDialog : public XMPromptDialog
{
 private:
  XMWidget *manager;	/* Row column manager widget to add to prompt */

  XMManagedWidget *redlabel;	/* Labels for the sliders. */
  XMManagedWidget *bluelabel;
  XMManagedWidget *greenlabel;

  XMManagedWidget *redslider;	/* Sliders. */
  XMManagedWidget *blueslider;
  XMManagedWidget *greenslider;

  XMManagedWidget *toplabel;
  XMManagedWidget *top_level; /* Prompt for bottom level.   */

 public:

  /* Destructor must delete all the children widgets.  */

  ~ColorDialog() {
    delete top_level;
    delete greenslider;
    delete blueslider;
    delete redslider;
    delete greenlabel;
    delete bluelabel;
    delete redlabel;
    delete toplabel;
    delete manager;
  }

  ColorDialog(XMWidget &parent, 
	      void (*cb)(XMWidget *,
			 XtPointer,
			 XtPointer) = NULL,
	      XtPointer cbd = NULL) : XMPromptDialog("Color_Button",
						     parent, "Lower Limit",
						     cb, cbd) {
		XmString label;

		this->UnManage();
		manager = new XMWidget("Slidermgr", xmRowColumnWidgetClass, id);
		manager->SetAttribute(XmNorientation, XmVERTICAL);
		manager->SetAttribute(XmNpacking, XmPACK_COLUMN);
		manager->SetAttribute(XmNnumColumns, 1);

		/* Red slider/label */

		redlabel = new XMManagedWidget("Red", xmLabelWidgetClass, *manager);
		label    = XmStringCreateSimple("Red Intensity");
		redlabel->SetAttribute(XmNlabelString, label);
		XmStringFree(label);
		redslider = new XMManagedWidget("RedSlider", xmScaleWidgetClass, *manager);
		redslider->SetAttribute(XmNorientation, XmHORIZONTAL);
		redslider->SetAttribute(XmNprocessingDirection, XmMAX_ON_RIGHT);
		redslider->SetAttribute(XmNshowValue, True);

		/* Green slider/label */

		greenlabel = new XMManagedWidget("Green", xmLabelWidgetClass, *manager);
		label    = XmStringCreateSimple("Green Intensity");
		greenlabel->SetAttribute(XmNlabelString, label);
		XmStringFree(label);
		greenslider = new XMManagedWidget("GreenSlider", xmScaleWidgetClass, 
						  *manager);
		greenslider->SetAttribute(XmNorientation, XmHORIZONTAL);
		greenslider->SetAttribute(XmNprocessingDirection, XmMAX_ON_RIGHT);
		greenslider->SetAttribute(XmNshowValue, True);

		/* Blue slider/label */


		bluelabel = new XMManagedWidget("Blue", xmLabelWidgetClass, *manager);
		label    = XmStringCreateSimple("Blue Intensity");
		bluelabel->SetAttribute(XmNlabelString, label);
		XmStringFree(label);
		blueslider = new XMManagedWidget("BlueSlider", xmScaleWidgetClass,
						 *manager);
		blueslider->SetAttribute(XmNorientation, XmHORIZONTAL);
		blueslider->SetAttribute(XmNprocessingDirection, XmMAX_ON_RIGHT);
		blueslider->SetAttribute(XmNshowValue, True);

		/* Top level label/prompt */

		toplabel  = new XMManagedWidget("TopLabel", xmLabelWidgetClass, *manager);
		label = XmStringCreateSimple("Upper Limit");
		toplabel->SetAttribute(XmNlabelString, label);
		XmStringFree(label);
		top_level = new XMManagedWidget("Top", xmTextWidgetClass, *manager);
		top_level->SetAttribute(XmNrows, 1);
		top_level->SetAttribute(XmNcolumns, 5);
		top_level->SetAttribute(XmNmaxLength, 5);
		
		manager->Manage();
	      }
  /*
  **  get/set the slider values: 
  */
  void RedSlider(int lvl) { redslider->SetAttribute(XmNvalue, lvl); }
  void GreenSlider(int lvl) { greenslider->SetAttribute(XmNvalue, lvl); }
  void BlueSlider(int lvl) { blueslider->SetAttribute(XmNvalue, lvl); }

  int RedSlider() {
    int val;

    redslider->GetAttribute(XmNvalue, &val);
    return val;
  }
  int GreenSlider() {
    int val;

    greenslider->GetAttribute(XmNvalue, &val);
    return val;
  }
  int BlueSlider() {
    int val;
  
    blueslider->GetAttribute(XmNvalue, &val);
    return val;
  }

  /* 
  ** Get/set the levels:
  */
  void UpperLevel(int lvl);
  void LowerLevel(int lvl);


  int UpperLevel() {
    char *string;
    int  val;
    int  stat;

    string = XmTextGetString(top_level->getid());
    stat = sscanf(string,"%d", &val);
    XtFree(string);
    return (stat == 1) ? val : -1;
  }
  int LowerLevel() { 
    char *string;
    int  val;
    int  stat;

    string = XmTextGetString(XmSelectionBoxGetChild(id, XmDIALOG_TEXT));

    stat = sscanf(string,"%d", &val);
    XtFree(string);
    return (stat == 1) ? val : -1;

  }
  void AddSliderCallback(void (*cb)(XMWidget *w, XtPointer, XtPointer), XtPointer client = NULL) {
    greenslider->AddCallback(XmNdragCallback, cb, client);
    greenslider->AddCallback(XmNvalueChangedCallback, cb, client);

    redslider->AddCallback(XmNdragCallback, cb, client);
    redslider->AddCallback(XmNvalueChangedCallback, cb, client);

    blueslider->AddCallback(XmNdragCallback, cb, client);
    blueslider->AddCallback(XmNvalueChangedCallback, cb, client);
  }
};
void ColorDialog::UpperLevel(int lvl) {  
  char     value[30];

  sprintf(value, "%d", lvl);
  XmTextSetString(top_level->getid(), value);
  
}
void ColorDialog::LowerLevel(int lvl) { 
  char value[30];
  
    sprintf(value, "%d", lvl);
  SetText(value);
}

/*
**  Program global data:
*/
int         changed = False;
int         table_size;
color_entry button_table[MAX_COLORS];
int         current_button = 0;
ColorDialog *color_prompt = NULL;
XMWidget    *shell_manager;
XMWidget    *button_manager;
XMApplication *application;
Colormap    app_colormap;	/* The application's color map. */
unsigned long pixels[MAX_COLORS];
XColor       white;

/*
** External references.
*/
extern "C" {
  void exit(int);

/*
** Forward references:
*/
void BuildButtons(int mapsize, XMWidget *manager);

}

/*
**  This section contains help texts etc: 
*/

char *about_text[] = {
  "   colorset X1.0 --   Color table definition program\n",
  "   Ron Fox\n",
  "   NSCL\n",
  "   Michigan State University\n",
  "   East Lansing, Michigan 48824-1321\n",
    NULL
  };

static Xamine_help_client_data about_help = { "About", NULL, about_text };


char *main_help_text[]  = {
  "   Colorset is a program which allows you to set up a customized color table\n",
  " The main window pane contains a File menu which can be used to read in or write out\n",
  " color table files, or to exit the program\n\n",

  "   In addition to the menubar, the main window contains a set of buttons.  Each\n",
  "button represents a color table entry.  The numbers in the button represent the\n",
  "percentage of full scale that will be covered by that button.  The bottom number is\n",
  "the low end of the range, the top number the high end of the range.  The button's\n",
  "background color shows the current value of the associated color table entry.\n\n",

  "   Clicking on one of the range buttons allows you to alter the full scale range\n",
  "covered by that button and the red, green, blue values of the color table entry\n",
  NULL
  };
static Xamine_help_client_data main_help = {"Help", NULL, main_help_text };

char *help_colorsetting_text[] = {
  "  This dialog allows you to set up an entry in the color table.  The two text entry\n",
  "fields let you set up the percentage of full scale covered by the selected entry\n",
  "of the color table.  The three sliders let you set the color table values.  As you\n",
  "move the sliders around, the background of the color table button will change to\n",
  "reflect your current color choice\n\n",

  "   The dialog buttons function as follows\n\n",
 
  "         OK     - Applies the current values of the dialog fields to the color\n",
  "                  table button and dismisses the dialog\n\n",

  "         Cancel - Dismisses the dialog but leaves the color table entry unchanged\n\n",

  "         Help   - Displays this help text dialog\n",
  NULL
  };
Xamine_help_client_data help_colorsetting = {"Color_help", NULL, help_colorsetting_text };

char *exit_help_text[] = {
  "  You have made changes to the color table which have not yet been saved\n",
  "If you exit now, then you will loose these changes.  The Write menu entry\n",
  "in the file menu will allow you to save color table changes.\n\n",
  "   The dialog buttons function as follows\n\n",
  "      Really Exit   - Exits the program without saving the color table\n",
  "      Don't Exit    - Cancels the exit and returns to the program\n",
  "      Help          - Displays this text\n",
  NULL
  };
Xamine_help_client_data exit_help = { "Exit_help", NULL, exit_help_text };

char *file_help_text[] = {
  "  You are being prompted for the name of a file to read or write.\n",
  "The list in the middle-left part of the selection box is a list of the\n",
  "directories which are immediately accessible from this directory\n",
  "The list in the middle-right part of the selection box is a list of the\n",
  "files in the current  directory.  The text field at the top of the box determines\n",
  "which files will be shown.  It can contain wild cards.  The text box \n",
  "towards the bottom part of the box allows you to type in a filename.\n\n",
  "Mouse actions are as follows:\n\n",
  "   double click directory      - Move to that directory\n",
  "   double click file           - Read/write that file\n",
  "          click file           - Load filename into the filename text box\n",
  "          click filter text box- Allow you to change the filter string\n",
  "          click filename box   - Allow you to change the filename\n",
  "          click OK             - Accepts the current filename\n",
  "          click Filter         - Apply the new filter string\n",
  "          click Cancel         - Abort the operation\n",
  "          click Help           - Bring up this help text\n",
  NULL
  };

Xamine_help_client_data file_help = { "File_Help", NULL, file_help_text };

/*
** Functions to convert pct to color and back..
** requires that white be set up.
*/

int pcttored(int pct)
{
  return (int)((float)white.red*(float)pct/100.0);
}
int pcttogreen(int pct)
{
  return (int)((float)white.green*(float)pct/100.0);
}
int pcttoblue(int pct)
{
  return (int)((float)white.blue*(float)pct/100.0);
}

int redtopct(int red)
{
  return (int)(((float)red/(float)white.red)*100.0);
}
int greentopct(int green)
{
  return (int)(((float)green/(float)white.green)*100.0);
}
int bluetopct(int blue)
{
  return (int)(((float)blue/(float)white.blue)*100.0);
}

/*
** Functional Description:
**   GetFgColor:
**     This function selects an appropriate foreground color for a color cell.
**     I had thought that the Xm routine I used works to do this but the
**     results are nonetheless sometimes black on black or white on white.
**     We operate by using a reference widget other than the one we're
**     modifying.  The game is to chose to draw in either fg or bg,
**     whichever has a cartesian distance farthest from the 
**     selected color:
** Formal Parameters:
**   XMWidget *w:
**     Some widget with a 'normal' background.
**   Colormap cmap:
**     color map associated with colorset colors.
**   Pixel cellcolor:
**     The cell color table entry to be used for the bg.
** Returns:
**   Pixel - pixel value to use for fg.
*/
Pixel GetFgColor(XMWidget *w, Colormap cmap, Pixel cellcolor)
{
  /* First get the color map fg and bg pixels and their rgb values.
  ** To prevent overflows we use floating point r,g,b's.
  */
  Pixel  fg,bg;
  float  fgr,fgb,fgg, bgr,bgb,bgg;
  XColor acolor;
  Colormap wcmap;

  w->GetAttribute(XmNforeground, &fg);
  w->GetAttribute(XmNbackground, &bg);
  w->GetAttribute(XmNcolormap,   &wcmap);
  acolor.pixel = fg;

  XQueryColor(XtDisplay(w->getid()),  wcmap, &acolor);
  fgr = (float)acolor.red;   fgb = (float)acolor.blue;
  fgg = (float)acolor.green;

  acolor.pixel = bg;
  XQueryColor(XtDisplay(w->getid()), wcmap, &acolor);
  bgr = (float)acolor.red;   bgb = (float)acolor.blue;
  bgg = (float)acolor.green;

  /* Now get the color values for the proposed new background color: */
  
  float r,g,b;
  acolor.pixel = cellcolor;
  XQueryColor(XtDisplay(w->getid()), cmap, &acolor);
  r = (float)acolor.red;    b  = (float)acolor.blue;
  g = (float)acolor.green;

  /* Compute the distances (square of distances actually: */

  float dfg, dbg;
  dfg = (fgr - r)*(fgr - r)   +
        (fgb - b)*(fgb - b)   +
	(fgg - g)*(fgg - g);
  dbg = (bgr - r)*(bgr - r)   +
        (bgb - b)*(bgb - b)   +
	(bgg - g)*(bgg - g);

  if(dfg > dbg) return fg;
  return bg;
}

/*
** Functional Description:
**   SetWindowLabel:
**     This function modifies the label of a window given any widget which is
**     a descendant of that shell.
** Formal Parameters:
**  Widget w:
**   Descendent widget id.
** char *string:
**  New window title
*/
void SetWindowLabel(Widget w, char *s)
{
  while(!XtIsVendorShell(w)) w = XtParent(w);

  XtVaSetValues(w, 
		XmNtitle, s,
		NULL);

}

/*
** Functional Description:
**    SetIcon    - Sets the icon for the shell widget. 
** Formal Parameters:
**    Widget w:
**       Some widget in the hierarchy.
**    char *filename:
**       Name of the file containing the icon.
*/
void SetIcon(Widget w, char *filename)
{
  Pixmap icon;

  /* Load the pixmap */

  icon = XmGetPixmap(XtScreen(w), filename, BlackPixelOfScreen(XtScreen(w)),
		     WhitePixelOfScreen(XtScreen(w)));
  if(icon == XmUNSPECIFIED_PIXMAP)
    return;			/* Pixmap not found is a no-op */

  /* Locate the vendor shell: */

  while (!XtIsVendorShell(w)) w = XtParent(w);

  /* Set the icon pixmap resource: */

  XtVaSetValues(w, XmNiconPixmap, icon, NULL);
}
  

/*
** Functional Description:
**    ReadColorTable:
**        This function reads in a color tabel from a file.
**  Formal Parameters:
**        FILE *fp:
**           File pointer open on the file.
** Returns:
**    Size of the new color table for success, -1 if failed.
*/
int ReadColorTable(FILE *fp)
{
  int entries;			/* Number of table entries. */
  char button_label[20];
  unsigned long planes;

  /* First read the number of entries.  and check for errors. */

  if(fscanf(fp, "%d\n", &entries) != 1) {
    return -1;
  }
  if(entries <= 0) 
    return -1;

  /* Kill the current set of buttons then build a new set: */


  delete color_prompt;
  color_prompt = NULL;
  int i;
  for(i = 0; i < table_size; i++) {

    delete button_table[i].button;
  }
  XFreeColors(XtDisplay(shell_manager->getid()), app_colormap,
	      pixels, table_size, 0);
  if(XAllocColorCells(XtDisplay(shell_manager->getid()), app_colormap,
		      False, &planes, 0, pixels, entries) == 0) {
    Procede(shell_manager, "Insufficient color resources unable to continue");
    exit(-1);
  }
  delete button_manager;
  button_manager = new XMManagedWidget("Buttons", xmRowColumnWidgetClass, 
				       *shell_manager);

  BuildButtons(entries, button_manager);

  /* Now read in the rest of the color table... we're screwed if there are */
  /* errors at this point -BUGBUGBUG- Do more error checkingo n readin     */

  for(i = 0; i < entries; i ++) {
    XColor        colorcell;

    fscanf(fp, "%d %d %d %d %d\n", 
	   &button_table[i].low, &button_table[i].hi,
	   &button_table[i].red, &button_table[i].green, &button_table[i].blue);
    sprintf(button_label, "%d\n\n%d", button_table[i].hi, button_table[i].low);
    button_table[i].button->Label(button_label);
    colorcell.red = pcttored(button_table[i].red);
    colorcell.green= pcttogreen(button_table[i].green);
    colorcell.blue = pcttoblue(button_table[i].blue);
    colorcell.pixel= button_table[i].pixel = pixels[i];
    colorcell.flags = DoRed | DoGreen | DoBlue;
    XStoreColor(XtDisplay(shell_manager->getid()), app_colormap, &colorcell);
  }
  /*
  **  Re-calculate and set the colors of the button backgrounds:
  */
  for(i = 0; i < entries; i++) {
    Pixel foreground, top_shadow, bottom_shadow, select;

    XmGetColors(XtScreen(shell_manager->getid()), app_colormap,
		pixels[i], &foreground, &top_shadow, &bottom_shadow,
		&select);
    foreground = GetFgColor(shell_manager, app_colormap,
			    pixels[i]);
    button_table[i].button->SetAttribute(XmNbackground, button_table[i].pixel);
    button_table[i].button->SetAttribute(XmNtopShadowColor, top_shadow);
    button_table[i].button->SetAttribute(XmNbottomShadowColor, bottom_shadow);
    button_table[i].button->SetAttribute(XmNselectColor, select);
    button_table[i].button->SetAttribute(XmNarmColor, select);
    button_table[i].button->SetAttribute(XmNborderColor, foreground);
    button_table[i].button->SetAttribute(XmNforeground, foreground);
  }
		
  /*
  ** Re-manage the widgets:
  */
  shell_manager->Manage();
  return entries;
}

/*
** Functional Description:
**    WriteColorTable:
**       This function actually writes the colortable to a file.
** Formal Parameters:
**    FILE *fp:
**      File pointer open on the file.
*/
void WriteColorTable(FILE *fp)
{
  fprintf(fp, "%d\n", table_size);	/* Lead off with the table size. */
  for(int i = 0; i < table_size; i++)
    fprintf(fp, "%d %d %d %d %d\n",
	    button_table[i].low, button_table[i].hi,
	    button_table[i].red, button_table[i].green, button_table[i].blue);

}   

/*
** Functional Description:
**    ReadFile:
**      This function is called when the user selects a file from the Read
**      file prompt.  If the reason was OK, we open the file and attempt to
**      read it.  If the reason was nomatch then we complain.
**      If the file is read, then we clear the changed flag and set the
**      window banner title to Colorset -- filename.
** Formal Parameters:
**  XMWidget *w:
**    Pointer to the widget object which invoked us... the file selection dialog
**  XtPointer cd:
**    Client data, in this case the XMApplication pointer.
**  XtPointer cbd:
**    Callback reason data, a pointer to XmFileSelectionBoxCallbackStruct
*/
void ReadFile(XMWidget *w, XtPointer cd, XtPointer cbd)
{
  XMApplication *app = (XMApplication *)cd;
  XmFileSelectionBoxCallbackStruct *why = 
    (XmFileSelectionBoxCallbackStruct *)cbd;

  char *filename;
  FILE *infile;
  char newlabel[1024];
  int  entries;

  switch (why->reason) {
  case XmCR_NO_MATCH:		/* No Such file.. */
  case XmCR_OK:			/* File exists. */
    XmStringGetLtoR(why->value, XmSTRING_DEFAULT_CHARSET, &filename);

    /* If there is no "." in the filename, then we add the default
     ** extension:
     */
    if(strrchr(filename, '.') == NULL) {
      char *newfilename = XtMalloc(strlen(filename) + strlen(DEFAULT_EXT) + 1);
      if(newfilename == NULL) {
	Xamine_error_msg(w, 
			 "XtMallocFailed in default extenstion processing");
	return;
      }
      strcat(strcpy(newfilename, filename), DEFAULT_EXT);
      XtFree(filename);
      filename = newfilename;
    }

    infile = fopen(filename, "r");
    sprintf(newlabel, "Colorset -- %s", filename);
    XtFree(filename);
    if(infile == NULL) {
      Xamine_error_msg(w, "Unable to open selected file");
      return;
    } 
    application->UnRealize();
    entries = ReadColorTable(infile);
    application->Realize();
    fclose(infile);
    if(entries <= 0) {
      Xamine_error_msg(w, "Failed to correctly read selected file");
      return;
    }
    changed = False;
    table_size = entries;
    current_button = 0;
    SetWindowLabel(app->getid(), newlabel);
    break;    
  default:
    fprintf(stderr,"Invalid callback reason in ReadFile %d\n", why->reason);
    return;
  }
  w->UnManage();


}

/*
** Functional Description:
**    PromptReadFile:
**       This function is called when the user selects READ from the file
**       menu.  We put up a file selection box widget which lets the user
**       choose which of the color map files they'd like to load.
**       If the color table is in the changed state we first ask for permission
**       to procede.
** Formal Parameters:
**  XMWidget *w:
**    The widget which invoked us... most likely the READ menu button.
**  XtPointer cd:
**    Client data... in this case a pointer to the top_level application
**    object.
**  XtPointer cbd:
**     Callback data.. unused.
*/
void PromptReadFile(XMWidget *w, XtPointer cd, XtPointer cbd)
{
  static XMFileListDialog *prompt = NULL;

  /* Make sure the mean it if things have changed */

  if(changed)
    if(!Procede(w, "Current color table has not been saved\nProcede?"))
      return;

  if(!prompt) {
    prompt = new XMFileListDialog("Read_Colortable",
				  *w, DEFAULT_FILEMASK);
    prompt->GetHelpButton()->Enable();
    prompt->AddOkCallback(ReadFile, cd);
    prompt->AddNoMatchCallback(ReadFile, cd);
    prompt->AddCancelCallback(XMUnmanageChild);
    prompt->GetHelpButton()->AddCallback(Xamine_display_help, 
					 (XtPointer)&file_help);
    prompt->RestrictChoices();
  }
  else {
    prompt->DoSearch(DEFAULT_FILEMASK);
    prompt->Manage();
  }
}

/*
** Functional Description:
**    WriteFile:
**       This function is the OK Callback for attempts to write a file.
**       If the file exists, we ask for confirmation.
**       If the file cannot be opened for write, then we put up an error dialog.
**       If the file is new and writeable, then we open the file, write the
**       current color table entry and change the window main label to the
**       colorset -- filename.
** Formal Parameters:
**    XMWidget *w:
**       The file selection box widget which called us.
**    XtPointer cd:
**        Client data which is actually an XMWidget * pointer to the top level
**        widget.
**     XtPointer calldata:
**        Pointer to an XmFileSelectionBoxCallbackStruct which describes
**        why the callback occured in the first place.
*/
void WriteFile(XMWidget *w, XtPointer cd, XtPointer calldata)
{
  XMApplication *top_level  = (XMApplication *)cd;
  FILE *outfile;
  char *filename;
  char window_label[1024];

  XmFileSelectionBoxCallbackStruct *why = 
      (XmFileSelectionBoxCallbackStruct *)calldata;

  switch(why->reason) {
  case XmCR_OK:			/* File exists. */
    if(!Procede(w, "File exists Ok to overwrite? ")) 
      return;
  case XmCR_NO_MATCH:		/* File doesn't exist (yet) */
    XmStringGetLtoR(why->value, XmSTRING_DEFAULT_CHARSET, &filename);

    /* If there is no "." in the filename, then we add the default
     ** extension:
     */
    if(strrchr(filename, '.') == NULL) {
      char *newfilename = XtMalloc(strlen(filename) + strlen(DEFAULT_EXT) + 1);
      if(newfilename == NULL) {
	Xamine_error_msg(w, 
			 "XtMallocFailed in default extenstion processing");
	return;
      }
      strcat(strcpy(newfilename, filename), DEFAULT_EXT);
      XtFree(filename);
      filename = newfilename;
    }
    sprintf(window_label, "Colorset -- %s", filename);
    outfile  = fopen(filename, "w");
    XtFree(filename);
    if(outfile == NULL) {
      Xamine_error_msg(w, "Unable to open selected file for write");
      return;
    }
    WriteColorTable(outfile);
    fclose(outfile);
    changed = 0;
    SetWindowLabel(top_level->getid(), window_label);
    break;
  default:			/* Shouldn't happen.  */
    fprintf(stderr, "Unexpected callback reason in WriteFile %d\n", why->reason);
    return;
  }
  w->UnManage();

}


/*
** Functional Description:
**   PromptWriteFile:
**     This function puts up a file write prompt box.
**     The file write prompt box is a file selection dialog widget
**     The file mask is *.ctbl.  Ok is pointed to the WriteFile callback
**     which will actually do the file write.
** Formal Parameters:
**   XMWidget *w:
**     The widget that caused the callback.
**   XtPointer cd:
**     Client data for the callback.  In this case, the client data is actually
**     the widget pointer of the top level widget.  This is because when
**     the file is written we'll change the banner to reflect the 'current file'
**     so we'll need to pass that pointer to the Ok callback.
**   XtPointer callback:
**     Unused callback data.
*/
void PromptWriteFile(XMWidget *w, XtPointer cd, XtPointer callback)
{
  static XMFileListDialog *prompt = NULL;

  if(!prompt) {
    prompt = new XMFileListDialog("Write_Colortable",
				  *w, DEFAULT_FILEMASK);

    prompt->GetHelpButton()->Enable();
    prompt->AddOkCallback(WriteFile, cd);
    prompt->AddNoMatchCallback(WriteFile, cd);
    prompt->AddCancelCallback(XMUnmanageChild);
    prompt->GetHelpButton()->AddCallback(Xamine_display_help, 
					 (XtPointer)&file_help);
    prompt->RestrictChoices();
  }
  else {
    prompt->DoSearch(DEFAULT_FILEMASK);
    prompt->Manage();
  }
  
}

/*
** Functional Description:
**     CancelUpdate  - This function cancels a color modification.
**                     The previous color values are restored to the
**                     button_table r,g,b's and the hi,low are also not changed.
** Formal Parameters:
**   XMWidget *wid:
**      The dialog widget ... we'll unmanage it:
**   XtPointer client:
**      Client data, actually a pointer to an entry in the button_table.
**   XtPointer callback:
**      Unused callback data.
**
*/
void CancelUpdate(XMWidget *wid, XtPointer client, XtPointer callback)
{
  color_entry *entry = *(color_entry **)client;

  entry->button->SetAttribute(XmNshowAsDefault, (XtArgVal)0); /* Turn off emphasis shadow. */
  if(wid != NULL)wid->UnManage();		/* Make dialog disappear */

  /*
  ** Restore the color map entry to the initial state: 
  */
  XColor centry;

  centry.red = pcttored(entry->red);
  centry.green = pcttogreen(entry->green);
  centry.blue  = pcttoblue(entry->blue);
  centry.flags = DoRed | DoBlue | DoGreen;
  centry.pixel = entry->pixel;
  XStoreColor(XtDisplay(entry->button->getid()), app_colormap, &centry);

  XSync(XtDisplay(entry->button->getid()),0);
  XmUpdateDisplay(entry->button->getid());


}

/*
** Functional Description:
**    Propagate      - This function propagates a color cell boundary
**                     value change as needed to the rest of the color
**                     cells. The intent is to make sure that the color
**                     cells maintain a contiguous coverage of the
**                     full scale.
** Formal Parameters:
**    color_entry *here:
**       Pointer to the entry of the color table which has just changed
**    int  number:
**       Number of the color table.
*/
void Propagate(color_entry *here, int number)
{
  int index;
  color_entry *entry;
  int         hi,lo;
  char        label[30];

  /* Propagate down towards zero.  */


  lo = here->low;
  index = number - 1;
  entry = here-1;

  while(index >= 0) {
    entry->hi = lo;
    if(entry->low > entry->hi) { /* Must keep trickling down. */
      entry->low = lo;		/* Keep hi >= low. */
      sprintf(label, "%d\n\n%d", 
	      entry->hi, entry->low);
      entry->button->Label(label);
      lo = entry->low;
    }
    else {			/* No further need to trickle down. */
      sprintf(label, "%d\n\n%d",
	      entry->hi, entry->low);
      entry->button->Label(label);
      break;
    }
    index--;
    entry--;
  }
  if(button_table[0].low != 0) { /* Don't allow a hole to creep in. */
    button_table[0].low = 0;
    sprintf(label, "%d\n\n%d",
	    button_table[0].hi, button_table[0].low);
    button_table[0].button->Label(label);
  }

  /* Now we propagate upwards towards the last one: */
  /* The logic here is very similar to the one above */

  entry = here + 1;
  hi    = here->hi;
  index = number+1;

  while(index < table_size) {
    entry->low = hi;
    if(entry->hi < entry->low) {
      entry->hi = hi;
      sprintf(label,"%d\n\n%d", entry->hi, entry->low);
      entry->button->Label(label);
      hi = entry->hi;
    }
    else {
      sprintf(label, "%d\n\n%d", entry->hi, entry->low);
      entry->button->Label(label);
      break;
    }
    index++;
    entry++;
  }
  if(button_table[table_size-1].hi != 100) {
    button_table[table_size-1].hi = 100;
    sprintf(label, "%d\n\n%d",
	    button_table[0].hi, button_table[0].low);
    button_table[0].button->Label(label);
  }
}

/* Functional Description:
**    UpdateColors   - This function updates the colors of a button from the 
**                     Current values of the color dialog widget.
** Formal Parameters:
**   XMWidget *w:
**     Widget pointer which is really a ColorDialog *
**   XtPointer client:
**     client data pointer which is in fact a pointer to an entry in the button_table
**   XtPointer callback
**     Unused
*/
void UpdateColors(XMWidget *w, XtPointer client, XtPointer callback)
{
  ColorDialog *prompt = (ColorDialog *)w;
  color_entry *entry  = *(color_entry **)client;

  int hi, lo;

  char label[20];


  /* Validate the range values: */

  hi = prompt->UpperLevel();
  lo = prompt->LowerLevel();

  if( (hi < 0) || (hi > 100) ||
      (lo < 0) || (lo > 100) ||
      (lo > hi)) {
    prompt->UpperLevel(entry->hi);
    prompt->LowerLevel(entry->low);
    w->Manage();		/* Keep the widget around to try again. */
    Xamine_error_msg(w, 
		     "The hi and lo values must be in the range [0-100] with lo <= hi");
    return;
  }

  /* Indicate that the table will have changed */

  changed = True;

  /* Update the database entries */

  entry->red   = prompt->RedSlider();
  entry->green = prompt->GreenSlider();
  entry->blue  = prompt->BlueSlider();
  entry->hi    = hi;
  entry->low   = lo;

  /* Update the background color: */

  XColor centry;

  centry.red = pcttored(entry->red);
  centry.green = pcttogreen(entry->green);
  centry.blue  = pcttoblue(entry->blue);
  centry.flags = DoRed | DoBlue | DoGreen;
  centry.pixel = entry->pixel;
  XStoreColor(XtDisplay(w->getid()), app_colormap, &centry);

  /* Figure out the new other colors for the entry: */
  Pixel top, bottom, select, fg;

  XmGetColors(XtScreen(w->getid()), 
	      app_colormap, centry.pixel, &fg, &top, &bottom, &select);
  fg =  GetFgColor(w, app_colormap, centry.pixel);

  entry->button->SetAttribute(XmNtopShadowColor, top);
  entry->button->SetAttribute(XmNbottomShadowColor, bottom);
  entry->button->SetAttribute(XmNselectColor, select);
  entry->button->SetAttribute(XmNarmColor, select);
  entry->button->SetAttribute(XmNborderColor, fg);
  entry->button->SetAttribute(XmNforeground, fg);
  

  /* Update the button label: */

  sprintf(label, "%d\n\n%d", entry->hi, entry->low);
  entry->button->Label(label);
  Propagate(entry, current_button); /* Propagate boundary changes. */
  




}

/*
** Functional Description:
**  ChangeColor    - This function is a call back which is invoked when a slider changes.  It
**                   is intended to dynamically alter the color of the button as the user slides the
**                   sliders.
** Formal Parameters:
**   XMWidget *w:
**     The invoking widget.  In this case actually a ColorDialog widget.
**   XtPointer client:
**     Client data in this case actually a pointer to a color_entry for the button being changed.
**   XtPointer cbd:
**     Callback data which in this case is ignored (we get the slider values calling ColorDialog methods
** NOTE:
**   No permanent changes are made to the internal color database... only the X color map entries are
**   altered.  This allows the cancel button to revert to prior color settings.
*/
void ChangeColor(XMWidget *w, XtPointer client, XtPointer cbd)
{
  color_entry *entry   = *(color_entry **)client;

  XColor centry;
  Pixel  fg, top, bottom, select;

  /* First, set up the color entry and set the map: */

  centry.red   = pcttored(color_prompt->RedSlider());
  centry.blue  = pcttoblue(color_prompt->BlueSlider());
  centry.green = pcttogreen(color_prompt->GreenSlider());
  centry.pixel = entry->pixel;
  centry.flags = DoRed | DoGreen | DoBlue;
  XStoreColor(XtDisplay(entry->button->getid()), app_colormap, &centry);

  /* Second correct the other colors in the widget if needed: */

  XmGetColors(XtScreen(entry->button->getid()), app_colormap, centry.pixel,
	      &fg, &top, &bottom, & select);
  fg = GetFgColor(w, app_colormap, centry.pixel);

  entry->button->SetAttribute(XmNforeground, fg);
  entry->button->SetAttribute(XmNtopShadowColor, top);
  entry->button->SetAttribute(XmNbottomShadowColor, bottom);
  entry->button->SetAttribute(XmNselectColor, select);
  entry->button->SetAttribute(XmNarmColor, select);
  entry->button->SetAttribute(XmNborderColor, fg);

  /* That's it!  */

}

/*
** Functional Description:
**   pb_callback   - This function is called when one of the push buttons is 
**                   pressed.  We generate a dialog which has a red/green/blue slider
**                   preloaded with the colors of the color table entry.
**                   We generate two text entry fields pre-loaded with the low/high
**                   end values for the button.
**                   The following buttons then exist:
**                        OK     - Accepts the new hi/lo/r/g/b values.
**                        CANCEL - Reverts to prior values of hi/lo/r/g/b.
**                        HELP   - Puts up a descriptive help dialog.
** Formal Parameters:
**   XMWidget *wid   - The widget pointer of the button pressed.
**   XtPointer cli   - The client data pointer which in this case is the button number.
**   XtPointer cbd   - The callback data in this case unused.
*/
void pb_callback(XMWidget *wid, XtPointer cli, XtPointer cbd)
{
  int button_number = (int)cli;
  XMPushButton *help;
  static  color_entry *entry;

  entry = &button_table[current_button];

  CancelUpdate(color_prompt, (XtPointer)&entry, NULL); /* Restore the old colors and stuff. */


  current_button = button_number;
  entry = &button_table[current_button];
  wid->SetAttribute(XmNshowAsDefault, (Dimension)1);

  if(color_prompt == NULL) {
    color_prompt = new ColorDialog(*wid, UpdateColors, 
				   (XtPointer)&entry);
    color_prompt->AddCancelCallback(CancelUpdate, 
				    (XtPointer)&entry);
    color_prompt->AddSliderCallback(ChangeColor, 
				    (XtPointer)&entry); /* Dynamically alter colors with sliders. */
    help = color_prompt->GetHelpButton();
    help->AddCallback(Xamine_display_help, (XtPointer)&help_colorsetting);
    help->Enable();

  }

  color_prompt->RedSlider(entry->red);
  color_prompt->GreenSlider(entry->green);
  color_prompt->BlueSlider(entry->blue);

  color_prompt->UpperLevel(entry->hi);
  color_prompt->LowerLevel(entry->low);

  color_prompt->Manage();

}

/*
** Functional Description:
**   BuildButtons  - Build the array of color buttons
**                   label is --\n#\n-- where -- indicates that the bottom/top
**                   range of values have not been set.  # is the color map 
**                   entry.
** Formal Parameters:
**    int  mapsize  - Number of color map entries.
**    XMWidget *manager - Row column widget manager.
**/

void BuildButtons(int mapsize, XMWidget *manager)
{
  char name[20];
  char txt[100];
  float increment;
  int   columns;

  /* We build an array into a horizontally oriented rc widget which we set
  ** to hold 8 buttons per row.
  */
  manager->SetAttribute(XmNorientation, XmHORIZONTAL);
  manager->SetAttribute(XmNpacking, XmPACK_COLUMN);
  manager->SetAttribute(XmNadjustLast, (XtArgVal)False);
  manager->SetAttribute(XmNspacing, (XtArgVal)0);
  columns = mapsize/16;
  columns = columns ? columns : 1; /* Must be at least 1 column. */
  manager->SetAttribute(XmNnumColumns, columns);

  increment = 100.0/((float)mapsize);

  for(int i = 0; i < mapsize; i++) {
    XMPushButton *button;
    XmString     label;
    unsigned long defpixel;
    XColor        defcolor;

    button_table[i].low = (int)((i)*(increment)+0.5);
    button_table[i].hi  = (int)(((i+1)*increment)+0.5);
    button_table[i].pixel = pixels[i]; /* Select a pixel. */


    sprintf(name, "Button-%d", i);  /* Widget name. */
    sprintf(txt,  "%d\n\n%d", 
	    button_table[i].hi,
	    button_table[i].low); /* Button label */
    label = XmStringCreateLtoR(txt, XmSTRING_DEFAULT_CHARSET);

    button = new XMPushButton(name, *manager, pb_callback, (XtPointer)i);
    button->SetAttribute(XmNlabelString, label);
    XmStringFree(label);
    button->SetAttribute(XmNuserData, (XtPointer)i);
    button->SetAttribute(XmNdefaultButtonShadowThickness, (Dimension)1);
    button->GetAttribute(XmNbackground, &defpixel);
    button->SetAttribute(XmNbackground, button_table[i].pixel);
    defcolor.pixel = defpixel;
    XQueryColor(XtDisplay(button->getid()), app_colormap, &defcolor);
    button_table[i].red = redtopct(defcolor.red);
    button_table[i].blue= bluetopct(defcolor.blue);
    button_table[i].green= greentopct(defcolor.green);
    defcolor.pixel = button_table[i].pixel;
    XStoreColor(XtDisplay(button->getid()), app_colormap, &defcolor);

    button_table[i].button = button; /* Set the table value. */
  }

   
}

/*
** Functional Description:
**   ExitCallback:
**      This function is called when the Exit menu entry is selected.
**      If the color table file was not changed since the last file write,
**      we just exit.  If the color table file did change since the last
**      File write, then we pop up a warning dialog to indicate that 
**      The user might want to save the file first.
** Formal Parameters:
**    XMWidget *w:
**       The widget id of the guy that started this mess off.
**    XtPointer client:
**       Client data pointer which is really the changed flag.
**    XtPointer callback:
**       Callback data structure (unused).
*/
void ExitCallback(XMWidget *w, XtPointer client, XtPointer callback)
{
  int warn = *(int *)client;
  static int f = False;
  static XMWarningDialog *ExitWarning = NULL;

  if(warn) {
    if(!ExitWarning) {
      ExitWarning = new XMWarningDialog("Exit_Warning", *w,
		  "If you Exit now you will loose unsaved color table changes",
					ExitCallback, (XtPointer)&f);
      ExitWarning->LabelOkButton("Really Exit");
      ExitWarning->LabelCancelButton("Don't Exit");
      ExitWarning->GetCancelButton()->Enable();
      ExitWarning->GetHelpButton()->Enable();
      ExitWarning->GetHelpButton()->AddCallback(Xamine_display_help, 
						(XtPointer)&exit_help);
    }
    ExitWarning->Manage();
    return;
  }
  else
    exit(0);
}

/*
** Functional Description:
**    ReadInitialFile:
**      This function sets up the initial colorset color table.
**      We assume that Xamine color tables are being read in and
**      read in the colortable that Xamine would if it had the number
**      of planes we're working with:
** Formal Parameters:
**    int planes:
**       Number of color table planes.
*/
static void ReadInitialFile(int planes)
{
  FILE *fp = Xamine_OpenColorTable((unsigned int)planes);
  if(fp == NULL) {
    perror("Could not read initial color table file");
    exit(errno);
  }

  if(ReadColorTable(fp) < 0) {
    perror("Could not read in initial color table");
    exit(errno);
  }

}

/*
**  Main program entry point
*/
int main(Cardinal argc, char **argv)
{
  XMApplication top("Xamine-test", &argc, argv); /* Top level/init ap. */
  /*  The window is divided into a set of vertically oriented sliders and */
  /*  a set of horizontally oriented buttons.                             */
  /*  All managed by the row/column widget MainManager                    */

  XMWidget rc("MainManager", xmRowColumnWidgetClass, top);
  XMMenuBar menubar("Menu_bar", rc, 2);



  /* Figure out the color map size:  */
  if(argc < 2) {
    fprintf(stderr, "Usage:\n\t%s\n\n\t%s\n",
	    "colorset planes",
	    "Where planes is the number of color planes to set up\n");
    exit(-1);
  }

  int planes = atoi(argv[1]);
  if( (planes <= 0) || (planes > 6)) {
    fprintf(stderr, 
	    "colorset -- The planes parameter must be between 1 and 6\n");
    exit(-1);
  }
  table_size = 1;
  for(int i = 0; i < planes; i++) table_size *= 2;

  /*
  ** Get the color map ID we'll be using: 
  */
  unsigned long planemsk;

  XtVaGetValues(top.getid(), XmNcolormap, &app_colormap, NULL);
  if(XAllocColorCells(XtDisplay(top.getid()), app_colormap, 
		       False, &planemsk, 0, pixels, table_size) == 0) {
    fprintf(stderr,"Unable to allocate sufficient color resources\n");
    exit(-1);
  }
  /*
  ** Figure out what max intensities are:
  */

  XColor vwhite;
  XLookupColor(XtDisplay(top.getid()), app_colormap, "white", &vwhite, &white);

  /* Set up the application icon: */

  SetIcon(top.getid(), ICON_FILE);

  /*
  **  Set up the menus:
  */

  XMPulldown    *pd;
  XMPushButton *pb;


  /* File menu */

  pd = menubar.AddPulldown("File", 4);
  pb = pd->AddMenuButton("Read...", PromptReadFile, (XtPointer)&top);
  pb = pd->AddMenuButton("Write...", PromptWriteFile, (XtPointer) &top);
  pd->AddSeparator();
  pb = pd->AddMenuButton("Exit", ExitCallback, (XtPointer)&changed);

  /* Help Menu */

  pd = menubar.AddHelpPulldown("Help", 3);
  pb = pd->AddMenuButton("About", Xamine_display_help, (XtPointer)&about_help);

  pb = pd->AddMenuButton("Help", Xamine_display_help, (XtPointer)&main_help);
  pd->AddSeparator();

  menubar.Manage();



  /* Build the buttons: */
  /* Kill off the current set of buttons and build a new set */

  button_manager = new XMManagedWidget("Buttons", xmRowColumnWidgetClass, rc);
  BuildButtons(table_size, button_manager);
  shell_manager  = &rc;

  /* Read in the initial color table file: */

  ReadInitialFile(planes);

  /* Start up the user interface */

  rc.SetAttribute(XmNadjustLast, (XtArgVal)False);
  rc.Manage();

  application = &top;
  XtVaSetValues(top.getid(), XmNallowShellResize, True, NULL);
  top.Begin();			/* Start processing events */
}
