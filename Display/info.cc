/*
** Facility:
**    Xamine -- NSCL Display program.
** Abstract:
**    info.cc:
**      This file contains code to respond to the info button.  The info
**      button displays information about the currently selected spectrum
**      pane.  Note that if logging is turned on, then information is
**      written to the selected log file.  If logging fails, then an
**      error dialog  is popped up for the user.
** Author:
**   ROn FOx
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/
static char *sccsinfo = "@(#)info.cc	2.2 1/28/94 ";

/*
** Include files:
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "XMWidget.h"
#include "XMDialogs.h"
#include "XMText.h"

#include "info.h"
#include "logging.h"
#include "panemgr.h"
#include "errormsg.h"
#include "dispwind.h"
#include "dispgrob.h"
#include "dispshare.h"
#include "grobjmgr.h"
#include "spcdisplay.h"

/*
** Constant definitions:
*/
static const int Columns = 80;		/* 80 column text scroller. */
static const int Rows    = 24;		/* 24 lines of text showing. */
static const char *gtype[] = {
                                "Undf.",
				"Cut",
				"Sum",
				"Band",
				"Cont.",
				"Mark",
				"Sum",
				"Mark"
			      };

/*
** Referenced globals:
*/
extern spec_shared *xamine_shared;
/*
** Locally defined classes:
*/
class InfoDisplay : public XMCustomDialog {
 protected:
  XMText *text;			/* The text display region. */
 public:
  InfoDisplay(char *name, XMWidget *parent);
  ~InfoDisplay() {
    delete text;
  }
  void SetText(char *t) { text->SetText(t); }
};

/*
** Local storage:
*/

static InfoDisplay *dialog_widget = NULL; /* Dialog widget handle.  */

/*
** Functional Description:
**    InfoDisplay::InfoDisplay:
**       This function is a constructor for the InfoDisplay widget class.
**       We are inheriting general behavior of the XMCustomDialog class and
**       specialize it by filling in the work area with an XMText widget.
**       The Apply, Cancel, and Help buttons are unmanaged, and the
**       label of the Ok button is changed to Dismiss in keeping with
**       the general form of Information Dialogs. However this is not an
**       information dialog, but a possibly longlived window so no dialog
**       class symbol is shown.
*/
InfoDisplay::InfoDisplay(char *name, XMWidget *parent) :
       XMCustomDialog(name, *parent, name)
{
  /* First unmanage all of the buttons we don't need: */

  Apply->UnManage();
  Cancel->UnManage();
  Help->UnManage();
 
  /* Relabel the ok button so that it's function is a bit clearer */

  Ok->Label(" Dismiss ");

  /* Create the work area by filling in the work_area widget with a */
  /* text widget:                                                   */

  Arg  scrollargs[10];

  XtSetArg(scrollargs[0], XmNrows, Rows);
  XtSetArg(scrollargs[1], XmNcolumns, Columns);
  XtSetArg(scrollargs[2], XmNeditable, False);
  XtSetArg(scrollargs[3], XmNeditMode, XmMULTI_LINE_EDIT);
  XtSetArg(scrollargs[4], XmNcursorPositionVisible, False);

  Widget text_wd = XmCreateScrolledText(work_area->getid(),
					"InfoText", scrollargs, 5);
  text = (XMText *)(new XMWidget(text_wd));


  /* Format the work area by attaching the text widget's scrolling window
  ** Parent to all of the walls of the form.  We Also set up the scrolling
  ** policy.
  */
  Widget sw = XtParent(text_wd);
  XtVaSetValues(sw, 
		XmNtopAttachment,   XmATTACH_FORM,
		XmNleftAttachment,  XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		XmNbottomAttachment,XmATTACH_FORM,
		NULL);
  text->Manage();		/* Put text under form's geometry mgmnt */
  XtManageChild(sw);                 /* Manage the scrolled widget too       */
}

/*
** Functional Description:
**  FormatSpectrumInfo:
**    This function returns a string that describes the spectrum.
**    The string is formatted as follows:
**      Spectrum: nnn
**      Title:    nnn
**      Type:     xxx
**      Channels  x [by y] using nnnn bytes
** Formal Parameters:
**    const int specno:
**      Number of the spectrum to get information about.
** Returns:
**    Pointer to a text string describing the spectrum.
**    NULL implies an allocation error.  If Spectrum is not defined,
**    then we return a shorter string indicating that fact.
*/
char *FormatSpectrumInfo(const int specno)
{
  char *specinfo = (char *)XtMalloc(200); /* 200 chars should be enough. */
  spec_title     title;

  assert(undefined == 0);
  assert(onedlong == 4);
  assert(onedword == 2);
  assert(twodword == 3);
  assert(twodbyte == 1);

  static char *spctype[] = { "Undefined", /* String names for each spec type */
			     "2-d Byte per channel",
			     "1-d Word per channel",
			     "2-d Word per channel",
			     "1-d Longword per channel"
			     };
  static int spcmult[]   = { 0, 1, 2, 
			       2, 4 }; /* Bytes per chan for spectypes */

  if(specinfo == NULL) return specinfo;

  switch(xamine_shared->gettype(specno)) {
  case onedlong:
  case onedword:
    sprintf(specinfo,
 "Spectrum : %d\nTitle    : %s\nType     : %s\nChannels : %d using %d bytes\n",
	    specno, xamine_shared->getname(title, specno),
	    spctype[xamine_shared->gettype(specno)],
	    xamine_shared->getxdim(specno),
    xamine_shared->getxdim(specno)*spcmult[xamine_shared->gettype(specno)]);
    break;
  case twodword:
  case twodbyte:
    sprintf(specinfo,
 "Spectrum : %d\nTitle    : %s\nType     : %s\nChannels : %d by %d using %d bytes \n",
	    specno, xamine_shared->getname(title, specno),
	    spctype[xamine_shared->gettype(specno)],
	    xamine_shared->getxdim(specno),
	    xamine_shared->getydim(specno),
            xamine_shared->getxdim(specno)*xamine_shared->getydim(specno)*
	    spcmult[xamine_shared->gettype(specno)]);
    break;
  default:
    sprintf(specinfo, "Spectrum %d is undefined\n", specno);
    break;
  }
  return specinfo;
}

/*
** Functional Description:
**    FormatAttributeInfo:
**       This function produces a formatted description of the
**       display attribute information.
** Formal Parameters:
**    win_attributed *attribs:
**       Pointer to window display attributes.
** Returns:
**    Pointer to the formatted string.
**    or NULL If it could not be allocated.
*/
static char *FormatAttributeInfo(win_attributed *attribs)
{

  if(attribs == NULL) return NULL;
  if(xamine_shared->gettype(attribs->spectrum()) == undefined) return NULL;

  char line[80];		/* Text line we're building up. */
  char *text = XtMalloc(2000);	/* Should be big enough. */
  if(text == NULL) return NULL;


  static char *reduction_text[] = { "Sampled",
				    "Summed",
				    "Averaged" };
  static char *r1dtext[] = { "Smoothed",
			     "Histogram",
			     "Lines",
			     "Points" };

  static char *r2dtext[] = { "Scatter",
			     "Boxes",
			     "Color",
			     "Contour",
			     "Surface",
			     "Lego" };

  text[0] = 0;			/* Set up for strcats... */

  /* Format superposition info:   */

  if(attribs->is1d()) {
    win_1d *a1 = (win_1d *)attribs;
    SuperpositionList &sl = a1->GetSuperpositions();
    if(sl.Count() > 0) {
      sprintf(line, "Superimposed with: \n  ID       Name\n");
      strcat(text, line);
      SuperpositionListIterator sli(sl);
      while(!sli.Last()) {
	spec_title name;
	int spno = (sli.Next()).Spectrum();
	sprintf(line, " %4d %s\n", spno, xamine_shared->getname(name, spno));
	strcat(text, line);
      }
    }
  }

  /* First we format the dimensionality independent attributes: */

      /* Axis attributes: */

  if(attribs->showaxes()) {
    sprintf(line, "\nAxes are on, Ticks are %s, Tick labels are %s\n",
	    attribs->showticks() ? "On" : "OFF",
	    attribs->labelaxes() ? "On" : "Off");
    strcat(text, line);
  }
  else {
    strcat(text, "Axes are off\n");
  }
  /* Title pieces: */

  strcat(text, "Title elements: ");
  if(attribs->showname()) strcat(text, "Spectrum Name ");
  if(attribs->shownum())  strcat(text, "Spectrum Number ");
  if(attribs->showdescrip()) strcat(text, "Description ");
  if(attribs->showpeak()) strcat(text, "Peak Information ");
  if(attribs->showupdt()) strcat(text, "Last update time ");
  if(attribs->showlbl())  strcat(text, "Graphical object labels ");
  strcat(text, "\n");

  /* Axis orientation */

  if(attribs->isflipped()) {
    strcat(text, " X and Y axes have been flipped\n");
  }
  /* Reduction mode: */

  sprintf(line, "Reduction mode is %s\n", 
	  reduction_text[attribs->getreduction()]);
  strcat(text, line);

  /* Scaling attributes: */

  sprintf(line, "Scaling is %s with current full scale at %d counts\n",
	  attribs->manuallyscaled() ? "Manual" : "Auto",
	  attribs->getfsval());
  strcat(text, line);

  /* Counts axis type: */

  sprintf(line, "Counts axis is %s \n", attribs->islog() ? "Log" : "Lin");
  strcat(text, line);

  /* Show floor/ceiling */

  if(attribs->hasfloor()) {
    sprintf(line, "Counts floor is %d\n", attribs->getfloor());
    strcat(text, line);
  }
  if(attribs->hasceiling()) {
    sprintf(line, "Counts ceiling is %d\n", attribs->getceiling());
    strcat(text, line);
  }
  /* The rest depends on the spectrum dimensionality */

  if(attribs->is1d()) {
    win_1d *a = (win_1d *)attribs;
    sprintf(line, "Spectrum rendition is %s\n",
	    r1dtext[a->getrend()]);
    strcat(text, line);
    if(a->isexpanded()) {
      sprintf(line, "Spectrum is expanded between %d and %d\n",
	      a->lowlimit(), a->highlimit());
      strcat(text,line);
    }
  }
  else {
    win_2d *a = (win_2d *)attribs;
    sprintf(line, "Spectrum rendition is %s\n",
	    r2dtext[a->getrend()]);
    strcat(text, line);
    if(a->isexpanded()) {
      sprintf(line, "Spectrum is expanded between (%d,%d) and (%d,%d)\n",
	      a->xlowlim(), a->ylowlim(), 
	      a->xhilim(), a->yhilim());
      strcat(text, line);
    }
  }
  return text;
}
/*
** Functional Description:
**    FormatGrobjInfo:
**     This function returns a pointer to a string which describes each of
**     the graphical objects which are attached to a spectrum.
**     Objects are sorted by object type and (most likely given the way the
**     database is maintained by object id.
** Formal Parameters:
**     int specno:
**        The id of the spectrum we're producing information for.
** Returns:
**     Pointer to a dynamically allocated character string which contains
**     the information we're providing.  The caller must free this string to
**     avoid memory leaks.
*/    
static char *FormatGrobjInfo(int specno)
{
  char *result;
  char element[20];
  grobj_generic *objects[GROBJ_MAXOBJECTS];
  int object_count = Xamine_GetSpectrumObjects(specno, 
					       objects,
					       GROBJ_MAXOBJECTS,
					       True);

  /* We take two passes through this... first we size the names of all
  ** of the objects.  If there are none, then we return NULL.
  */

  int name_size = sizeof(element) * 8;
  if(object_count == 0) return NULL;
  int i;
  for(i = 0; i < object_count; i++) {
    grobj_name name;
    name_size += strlen(objects[i]->getname(name)) + 80;
  }

  /* Allocate the data for graphical objects.  */

  result = (char *)XtMalloc(name_size);

  /* Describe the markers.   */

  strcpy(result, 
	 "\n-------- Graphical Objects ---------\n\nMarkers:\n  ID  Name");
  for(i = 0; i < object_count; i++) {
    if((objects[i]->type() == marker_1d) || 
       (objects[i]->type() == marker_2d)) {
      grobj_name name;
      int        id = objects[i]->getid();;
      objects[i]->getname(name);
      sprintf(element, "\n%04d  ", id);
      strcat(result, element);
      strcat(result, name);
    }
  }
  strcat(result, "\n");

  /* Describe the summing regions  */

  strcat(result,"\nSumming Regions:\n  ID   Name");
  for(i = 0; i < object_count; i++) {
    if((objects[i]->type() == summing_region_1d) || 
       (objects[i]->type() == summing_region_2d)) {
      grobj_name name;
      int        id = objects[i]->getid();;
      objects[i]->getname(name);
      sprintf(element, "\n%04d  ", id);
      strcat(result, element);
      strcat(result, name);
    }
  }
  strcat(result, "\n");


  /* Return the string. */

  return result;
}

/*
** Functional Description:
**   FormatGateInfo
**     This function formats the gate description text.
** Formal Parameters:
**   int specno:
**     Spectrum to format text for.
** Returns:
**   NULL -- no gate information.
**   char *- pointer to the gate description text.   The caller
**           must XtFree this storage
*/
static char*
FormatGateInfo(int specno)
{
  char *result;
  char element[20];
  grobj_generic *objects[GROBJ_MAXOBJECTS];
  int object_count = Xamine_GetSpectrumGates(specno, 
					       objects,
					       GROBJ_MAXOBJECTS,
					       True);

  /* We take two passes through this... first we size the names of all
  ** of the objects.  If there are none, then we return NULL.
  */

  int name_size = sizeof(element) * 8;
  if(object_count == 0) return (char *)NULL;
  int i;
  for(i = 0; i < object_count; i++) {
    grobj_name name;
    name_size += strlen(objects[i]->getname(name)) + 80;
  }

  /* Allocate the data for graphical objects text..  */

  result = (char *)XtMalloc(name_size);

  /* Describe the markers.   */

  strcpy(result,
	 "\n-------- Gates ---------\n\nType   ID  Name");
  for(i = 0; i < object_count; i++) {
    grobj_name name;
    int        id = objects[i]->getid();;
    objects[i]->getname(name);
    sprintf(element, "\n%5s  %04d  ",
	    gtype[objects[i]->type()],
	    id);
    strcat(result, element);
    strcat(result, name);
  }
  strcat(result, "\n");

  /* Describe the peak markers: */

  strcat(result, "\nPeak markers:\n  ID   Position    Width    Comment");
  for(i = 0; i < object_count; i++) {
    if(objects[i]->type() == peak1d) {
      grobj_name name;
      int id = objects[i]->getid();
      objects[i]->getname(name);
      float fC = ((grobj_Peak1d*)objects[i])->Centroid();
      float fW = ((grobj_Peak1d*)objects[i])->Width();
      sprintf(element, "\n%04d  %6.2f    %6.2f   %s",
	      id, fC, fW, name);
      strcat(result, element);
    }
  }
  strcat(result, "\n");


  /* Return the string. */

  return result;
}

/*
** Functional Description:
**   FormatInfo:
**     This function returns the info text in a dynamically allocated
**     string.  It is up to the caller to delete the storage allocated
**     using XtFree.
*/
static char *FormatInfo()
{
  char *text;
  int  size = 0;

  /* First thing we need to do is get the attributes block of the 
  ** currently selected spectrum.  This has a lot of stuff in it,
  ** like the current spectrum which are needed in subsequent subinfo calls.
  */

  win_attributed *att = Xamine_GetSelectedDisplayAttributes();
  if(att == NULL) {
    char *t = " Pane does not contain a spectrum ";
    text = XtMalloc(strlen(t)+1);
    if(text != NULL) strcpy(text, t);
    return t;
  }
  int sp = att->spectrum();

  /* If the spectrum is undefined, then mention that and clear the spectrum. */

  if(Xamine_SpectrumType(sp) == undefined) {
    char *t = " Pane does not contain a spectrum ";
    text = XtMalloc(strlen(t)+1);
    if(text != NULL) strcpy(text, t);
    Xamine_ClearSelectedPane(Xamine_Getpanemgr(), NULL, NULL);
    return t;
  }
  /* Get the text associated with the shared memory info in the spectrum.
  */
  char *spec_text = FormatSpectrumInfo(att->spectrum());
  if(spec_text != NULL) size += strlen(spec_text);

  /* Get text associated with displayed attributes: */

  char *att_text = FormatAttributeInfo(att);
  if(att_text != NULL) size += strlen(att_text);

  /* Get text associated with graphical objects: */

  char *grob_text = FormatGrobjInfo(att->spectrum());
  if(grob_text != NULL) size += strlen(grob_text);

  char *gate_text = FormatGateInfo(att->spectrum());
  if(gate_text != NULL) size+= strlen(gate_text);
  /* At this time we know the size of the output text, allocate it: */

  text = (char *)XtMalloc(size + 1);
  if(text != NULL) {
    text[0] = 0;		/* Start with an empty string: */
    if(spec_text != NULL) {
      strcat(text, spec_text);
      XtFree(spec_text);
    }
    if(att_text != NULL) {
      strcat(text, att_text);
      XtFree(att_text);
    }
    if(grob_text != NULL) {
      strcat(text, grob_text);
      XtFree(grob_text);
    }
    if(gate_text != NULL) {
      strcat(text, gate_text);
      XtFree(gate_text);
    }
  }
  return text;
}

/*
** Functional Description:
**   UnManage:
**     Unmanages a dialog.. called as a callback pinned to an action area 
**     button.
** Formal Parameters:
**     XMWidget *wid:
**       Pointer to widget invoking.
**     XtPointer user_d:
**       Pointer to widget to unmanage.
**     XtPointer call_d:
**       UnUsed.
*/
static void UnManage(XMWidget *wid, XtPointer user_d, XtPointer call_d)
{
  InfoDisplay *unm = (InfoDisplay *)user_d;

  unm->UnManage();
}

/*
** Functional Description:
**   Xamine_DisplayInfo:
**     This function is a callback procedure invoked by either the Info
**     button or the Info menu item.  It produces a description of the
**     spectrum, and other ancillary data, displays it to the tube, and if
**     logging is enabled logs it to a file as well.
**     The spectrum description is formatted as a Custom dialog with
**     the work area taken up by a text widget that contains the
**     description of the spectrum.   vertical scrollers are present to
**     allow the text to exceed the dedicated screen area and nonetheless be
**     visible.
**       The work area includes an Ok button which is labelled DISMISS and
**     removes the text widget from the tube (unmanages it).
*/
void Xamine_DisplayInfo(XMWidget *parent, XtPointer client_d, XtPointer call_d)
{
  char *information;
  /*
  ** If the dialog widget does not exist, then create it:
  */

  if(!dialog_widget) {
    dialog_widget = new InfoDisplay("Info_Display", parent);
    dialog_widget->AddOkCallback(UnManage, dialog_widget);	/* OK unmanages. */
  }
  /*
  ** Format the information:
  */

  information = FormatInfo();
  if(information == NULL) {
    Xamine_error_msg(dialog_widget, 
		     "Could not get any information about the selected pane");
    return;
  }

  /* Fill the dialog widget with the text, this puts the text in the 
  ** text widget part of the work area.
  */

  dialog_widget->SetText(information);
  dialog_widget->Manage();

  /* If logging is on, then log the information to file: */

  if(Xamine_logging)
    Xamine_log.LogMessage(information);

  /* The information string was dynamically allocated, so we free it here */

  XtFree(information);
}
