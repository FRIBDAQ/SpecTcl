/*
** Facility:
**   Xamine -- NSCL display program.
** Abstract:
**   guintegrate.cc:
**     This file implements the graphical user interface to the 
**     Integrate function.  The Integrate funtion integrates all summing
**     regions that are defined on the selected spectrum.  The results
**     of the integration are:
**       1. Popped up into a scrolling text dialog widget with a Dismiss
**          button at the bottom.  The text is continually added until
**          the dismiss occurs at which time it is deleted.  Thus 
**          the results of several integrations can be accumulated into
**          the window for comparison by just not hitting Dismiss.
**          Viewpoint position is always maintained towards the bottom of
**          the window.
**       2. If logging is enabled, then the integrations are also sent
**          out to the logfile.
**          Time stamps and headers help to identify the integrations.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/

/*
** Include files:
*/
#include <stdio.h>
#include <string.h>

#include "XMDialogs.h"
#include "XMText.h"
#include "errormsg.h"
#include "guintegrate.h"
#include "integrate.h"
#include "logging.h"
#include "dispshare.h"
#include "dispgrob.h"
#include "grobjmgr.h"
#include "panemgr.h"
/*
** Constants:
*/
#define GAMMA  2.354		/* Number of sigmas in FWHM for Gaussian. */
#define SCROLL_WINDOW_CHARS 5000 /* Max chars allowed in scrolled window. */
/*
** External References: 
*/
extern spec_shared *xamine_shared; /* Shared spectrum region. */

/*
**  The class definition below is a specialization of the
**  Custom dialog class and contains a Scrolled text widget.  It is built as
**  a scrolled text widget in the work area and a Dismiss button in the
**  action area.
*/
class IntegrationDisplay : public XMCustomDialog {
 protected:
  XMScrolledText *text;
  Callback_data  *cd;
 public:
  /* Construction and desctruction: */

  IntegrationDisplay(char *name, XMWidget &parent, int rows=20,
		     int cols=40);
  ~IntegrationDisplay() {
    XMRemoveCallback(cd);
    delete text;
  }
  /* Manipulation: */

  void ClearText()         { text->ClearText(); }
  void AddText(char *s)    { text->AddText(s); }

  /* Callbacks: */
 
  void Dismiss() {
    text->ClearText();
    UnManage();
  }

};
/*
** The object pointer below holds a handle to the dialog
*/
static IntegrationDisplay *dialog = NULL;

/*
**  This function is called when the dialog is being destroyed:
*/
static void Destroy_callback(XMWidget* pWidget, XtPointer ud, XtPointer cd)
{
  delete dialog;
  dialog = (IntegrationDisplay*)NULL;

}

/*
** Functional Description:
**   Dismiss_callback:
**     This function relays the dismiss button callback to the
**     Dismiss method.
** Formal Parameters:
**   XMWidget *w:
**      Widget (subwidget within the object) which triggered the callback.
**   XtPointer ud:
**      User data, in this case a pointer to the object which executed the
**      callback.
**   XtPointer cd:
**      Pointer to call data which depends on what *w was.
*/
static void Dismiss_callback(XMWidget *w, XtPointer ud, XtPointer cd)
{
  IntegrationDisplay *obj  = (IntegrationDisplay *)ud;

  obj->Dismiss();

}

/*
** Functional Description: 
**   IntegrationDisplay:
**     This function constructs the integration display widget. 
**     See the description at the file header for information about the
**     structure of this widget.
** Formal Parameters:
**   char *name:
**      Name of the widget.
**   XMWidget &parent:
**      Parent of this widget.
**   int rows:
**      Number of rows of text.
**   int cols:
**      Number of columns of text.
*/
IntegrationDisplay::IntegrationDisplay(char *name, XMWidget &parent,
				       int rows, int cols):
       XMCustomDialog(name, parent, "Integration Results")
{
  /* First create the scrolled text widget and place it in the
  ** work_area form:
  */
  Arg args[20];

  XtSetArg(args[0], XmNeditable, False);
  XtSetArg(args[1], XmNeditMode, XmMULTI_LINE_EDIT);
  XtSetArg(args[2], XmNcursorPositionVisible, False);
  text = new XMScrolledText(name, *work_area, rows, cols,
			    args, 3);
  text->SetMaxLength(SCROLL_WINDOW_CHARS);


  work_area->SetLeftAttachment(text->Scroller(),     XmATTACH_FORM);
  work_area->SetTopAttachment(text->Scroller(),      XmATTACH_FORM);
  work_area->SetRightAttachment(text->Scroller(),    XmATTACH_FORM);
  work_area->SetBottomAttachment(text->Scroller(),   XmATTACH_FORM);

  /*  Next we eliminate all but the Ok Button and label it Dismiss */

  Apply->UnManage();
  Cancel->UnManage();
  Help->UnManage();
  Ok->Label("Dismiss");

  /* Now attach the callback relay to the Ok button so that 
  ** we have the behavior almost neatly encapsulated in the object.
  */

  cd = AddOkCallback(Dismiss_callback, this);

  /* Manage the srolled text widget: */

  XtManageChild(text->getid());
  XtManageChild(text->Scroller());
  work_area->Manage();
}

/*
** Functional Description:
**   Format1d:
**     This function formats a 1-d spectrum integration. 
**     the format is done in such a way as to line up with the columns
**     of the header most of the time.
** Formal Parameters:
**   IntegrationDisplay *d:
**     Text dialog to which we're adding the text... it's also added to the
**     log file if logging is on.
**   grobj_generic *g:
**     Points to the graphical object.  We'll need to get the name and
**     Id from it.
**   float centroid, fwhm, area:
**     The peak statistics that we're formatting.
*/
static void Format1d(IntegrationDisplay *d, grobj_generic *g, 
		     float centroid, float fwhm, float area)
{
  char txt[1024];		/* A nice big formatting buffer. */
  grobj_name n;

  sprintf(txt, "%4d %27s  %8.2f     %8.2f         %f\n",
	  g->getid(),
	  g->getname(n),
	  centroid, fwhm, area);
  d->AddText(txt);		/* Add to the dialog. */
  if(Xamine_logging) {
    Xamine_log.ContinueMessage(txt);
  }
  
}

/*
** Functional Description:
**   Format2d:
**     This function formats and displays a 2-d integration.
**     The format is identical to that of the 1-d integration except that
**     the centroid and FWHM's have x and y components which are displayed in
**     the form (x,y). Again, if logging is enabled, output is also added
**     to the log file using ContinueMessage.
** Formal Parameters:
**   IntegrationDisplay *d:
**      The dialog widget which we display in.
**   grobj_generic *g:
**      The graphical object that's being integrated.  We need the Id and
**      name from this.
**   float cx,cy:
**      The X and y coordinates of the centroid.
**   float fx,fy:
**      The X and Y fwhm's.
**   float a:
**      The number of counts in the summing region.
*/
static void Format2d(IntegrationDisplay *d, grobj_generic *g,
		     float cx, float cy, float fx, float fy, float a)
{
  char txt[1024];		/* Nice large formatting buffer. */
  grobj_name n;			/* Holds the object name.        */

  /* Format the output line: */

  sprintf(txt, "%4d %27s  (%6.1f,%6.1f) (%6.1f,%6.1f) %f\n",
	  g->getid(), g->getname(n),
	  cx,cy, fx,fy, a);

  /* Add the line to the dialog: */

  d->AddText(txt);

  /* If logging is on then also log: */

  if(Xamine_logging) {
    Xamine_log.ContinueMessage(txt);
  }
}

/*
** Functional Description:
**   Integrate:
**     This function performs and reports on a single integration.
**     The integration is formatted into a string and added to the dialog
**     text.  If logging is on, then it is also appended to the log file.
** Formal Parameters:
**   IntegrationDisplay *d:
**     Dialog to add text to.
**   grobj_generic *g:
**     Graphical object which must by now be a summing region.
**   int specno:
**     Spectrum we must integrate.
**   spec_type typ:
**     Type of spectrum being integrated.
*/
static void Integrate(IntegrationDisplay *d, grobj_generic *g, 
		      int specno, int typ)
{

  /* First we branch on spectrum type... to separate 1-d from 2-d: */

  if( (typ == onedlong) || (typ == onedword)) {	/* 1-d integration. */
    float centroid;
    float fwhm;
    float area;
    if((g->type() != summing_region_1d) && (g->type() != cut_1d)) {
      Xamine_error_msg(Xamine_Getpanemgr(),
	      "Mis-match between grobj type and spectrum type in Integrate");
      return;
    }
    if(typ == onedlong) {
      Integrate_1dl I((unsigned int *)xamine_shared->getbase(specno), 
		      (grobj_sum1d *)g);
      I.Perform();
      centroid = I.GetCentroid();
      fwhm     = I.GetStdDev() * GAMMA;
      area     = I.GetVolume();
    }
    else {
      Integrate_1dw I((unsigned short *)xamine_shared->getbase(specno),
		      (grobj_sum1d *)g);
      I.Perform();
      centroid = I.GetCentroid();
      fwhm     = I.GetStdDev() * GAMMA;
      area     = I.GetVolume();
    }
    Format1d(d, g, centroid, fwhm, area);
  }
  else if( (typ == twodword) || (typ == twodbyte)){ /* 2-d integration. */
    float area;
    float xc;
    float yc;
    float xfwhm;
    float yfwhm;

    if( (g->type() != summing_region_2d) &&
        (g->type() != contour_2d)  ) {
      Xamine_error_msg(Xamine_Getpanemgr(),
		       "Invalid summing region ignored in Integrate");
      return;
    }
    if(typ == twodword) {
      Integrate_2dw I((unsigned short *)xamine_shared->getbase(specno), 
		      xamine_shared->getxdim(specno),
		      (grobj_sum2d *)g);
      I.Perform();
      area   = I.GetVolume();
      xc     = I.GetXCentroid();
      yc     = I.GetYCentroid();
      xfwhm  = I.GetXStdDev()  * GAMMA;
      yfwhm  = I.GetYStdDev()  * GAMMA;
    }
    else {
      Integrate_2db I((unsigned char *)xamine_shared->getbase(specno),
		      xamine_shared->getxdim(specno),
		      (grobj_sum2d *)g);
      I.Perform();
      area = I.GetVolume();
      xc   = I.GetXCentroid();
      yc   = I.GetYCentroid();
      xfwhm= I.GetXStdDev() * GAMMA;
      yfwhm= I.GetYStdDev() * GAMMA;

    }
    Format2d(d,g, xc, yc, xfwhm, yfwhm, area);
  }
  else {                                       /* Bad news here. */
    Xamine_error_msg(Xamine_Getpanemgr(),
		    "Invalid spectrum type in Integrate ignored");
    return;
  }
}

/*
** Functional Description:
**  FormatIntegrationText:
**    This function appends text to an integration dialog which represents
**    the integrations over all summing regions defined in the selected
**    spectrum.  The text is also appended to the log file if
**    logging is turned on.
** Formal Parameters:
**   IntegrationDisplay *d:
**      Pointer to the dialog.
*/
static void FormatIntegrationText(IntegrationDisplay *d)
{
  /* First we get the currently displayed spectrum information:   */

  win_attributed *att = Xamine_GetSelectedDisplayAttributes();
  if(att == NULL) 
    return;	           /* No output if no display. */

  int specno          = att->spectrum();
  spec_type spectype  = xamine_shared->gettype(specno);
  if(spectype == undefined)
    return;			/* No Output if no spectrum.  */

  /* Get the count of graphical objects and the object list. 
  */

  int nobjects = Xamine_GetSpectrumObjectCount(specno);
  int ngates  =  Xamine_GetSpectrumGateCount(specno);
  int nints;
  if(att->is1d()) 
    nints = nobjects+ngates;
  else
    nints = nobjects+ngates;

  if(nints == 0) {
    return;
  }
  grobj_generic *objects[GROBJ_MAXOBJECTS];
  Xamine_GetSpectrumObjects(specno,
			    objects,
			    GROBJ_MAXOBJECTS, True);

  /* Format the header.  Put it in the dialog and if logging is on, put
  ** it in the log file too:
  */

  char buffer[1024];
  sprintf(buffer, 
	  " Integrations for spectrum %d\n\n%s%s",
	  specno,
  " Id           Name                   Centroid       FWHM          Area\n",
  "Summing regions: \n");
  if(Xamine_logging) {
    Xamine_log.LogMessage(buffer);
  }
  d->AddText(buffer);
  int i;
  for( i = 0; i < nobjects; i++) {
    if( (objects[i]->type() == summing_region_1d) ||
        (objects[i]->type() == summing_region_2d)) {
      Integrate(dialog, objects[i], specno, spectype);
    }
  }

  if(!att->is1d()) {
    sprintf(buffer,"Contours:\n");
    if(Xamine_logging)
      Xamine_log.LogMessage(buffer);
    d->AddText(buffer);

    nobjects = Xamine_GetSpectrumGateCount(specno);
    Xamine_GetSpectrumGates(specno, objects, GROBJ_MAXOBJECTS, True);

    for(i = 0; i < nobjects; i++) {
      if(objects[i]->type() == contour_2d) {
	Integrate(dialog, objects[i], specno, spectype);
      }
    }
  }
  else {
    sprintf(buffer, "Cuts: \n");
    if(Xamine_logging)
      Xamine_log.LogMessage(buffer);
    d->AddText(buffer);
    nobjects = Xamine_GetSpectrumGateCount(specno);
    Xamine_GetSpectrumGates(specno, objects, GROBJ_MAXOBJECTS, True);
    for(i = 0; i < nobjects; i++) {
      if(objects[i]->type() == cut_1d) {
        Integrate(dialog, objects[i], specno, spectype);
      }
    }
  }

}

/*
** Functional Desription:
**   Xamine_Integrate:
**     This function handles the graphical user interface issues of
**     performing integrations.  See the file header comments for some
**     detailed information about what GUI looks like for integrations.
** Formal Parameters:
**     XMWidget *wid:
**       Pointer to the widget which caused us to pop up.
**     XtPointer ud,cd:
**       user and call data which are both ignored by this function but 
**       provided by motif and it's underlings.
*/   
void Xamine_Integrate(XMWidget *wid, XtPointer ud, XtPointer cd)
{
  /* If necessary, instantiate the widget:  */

  if(dialog == NULL) {
    dialog = new IntegrationDisplay("Integration", *wid, 20, 80);
    dialog->AddCallback(XtNdestroyCallback, Destroy_callback);
  }

  /* Put the integration text into the widget and log file if necessary  */

  FormatIntegrationText(dialog);

  /* Pop up the widget if necessary: */

  dialog->Manage();
} 
