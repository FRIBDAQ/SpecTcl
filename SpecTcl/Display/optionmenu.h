/*
** Facility:
**    Xamine -- NSCL Display program.
** Abstract:
**    optionmenu.h  - This include file contains definitions of the
**                    functions which prompt for default spectrum display
**                    options.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
**    @(#)optionmenu.h	8.1 6/23/95 
*/
#ifndef _OPTIONMENU_H
#define _OPTIONMENU_H
#include "XMWidget.h"
#include "XMText.h"
#include "XMPushbutton.h"
#include "XMLabel.h"
#include "XMScale.h"
#include "XMManagers.h"
#include "dispwind.h"

void Xamine_SetDefaultAxis(XMWidget *w, XtPointer user, XtPointer call);
void Xamine_SetDefaultTitling(XMWidget *w, XtPointer user, XtPointer call);
void Xamine_SetDefaultAttributes(XMWidget *w, XtPointer user, XtPointer call);
void Xamine_SetDefaultReduction(XMWidget *w, XtPointer user, XtPointer call);
void Xamine_Set1dDefaultRendition(XMWidget *w, XtPointer user, XtPointer call);
void Xamine_Set2dDefaultRendition(XMWidget *w, XtPointer user, XtPointer call);
void Xamine_SetDefaultRefresh(XMWidget *w, XtPointer user, XtPointer call);
/*
** These functions are not meant to be used by clients other than those 
** implementing classes below.
*/
void Xamine_ShowAxisCallback(XMWidget *w, XtPointer user_d, XtPointer call_d);
void Xamine_EnsureNumerics(XMWidget *w, XtPointer user_d, XtPointer call_d);
void Xamine_AttribEnableCallback(XMWidget *w, XtPointer user_d, 
				 XtPointer call_d);
/*
**  The classes below are used in setting the axis label characteristics
*/

/*
** AxisForm is assumed to be used within an XMForm Derived class to
** prompt for axis labelling parameters.
*/
class AxisForm
{
 protected:
  XMToggleButton *shwaxes;
  XMToggleButton *shwticks;
  XMToggleButton *shwlabels;


 public:
  /*
  ** Constructors:
  */
  AxisForm(char *name, XMForm &parent);
  /*
  ** Destructors.
  */
  virtual ~AxisForm() {
    delete shwaxes;
    delete shwticks;
    delete shwlabels;
  }

  /* Adjust the state of the showticks/showlabels toggles based on showaxes */

  void AdjustEnables() { if(!shwaxes->GetState()) {
                            shwticks->Disable();
			    shwlabels->Disable();
			  }
			 else {
			   shwticks->Enable();
			   shwlabels->Enable();
			 }
		       }
  /* Inquire about toggle button states */

  Boolean ShowAxes()         { return shwaxes->GetState(); }
  Boolean ShowTicks()        { return shwticks->GetState(); }
  Boolean ShowLabels()       { return shwlabels->GetState(); }

  /* Set toggle states */

  void ShowAxes(Boolean state)  { shwaxes->SetState(state);
				  AdjustEnables();
				}
  void ShowTicks(Boolean state) { shwticks->SetState(state); }
  void ShowLabels(Boolean state) { shwlabels->SetState(state); }

  /* Add callbacks for the toggles if desired: */

  Callback_data *AddShowAxesCallback(void (*cb)(XMWidget *, XtPointer, XtPointer),
			   XtPointer user_d = NULL)
    { return shwaxes->AddCallback(cb, user_d); }
  Callback_data *AddShowTicksCallback(void (*cb)(XMWidget *, XtPointer, XtPointer),
			    XtPointer user_d = NULL)
    { return shwticks->AddCallback(cb, user_d); }
  Callback_data *AddShowLabelsCallback(void (*cb)(XMWidget *, XtPointer, XtPointer),
			     XtPointer user_d = NULL)
    { return shwlabels->AddCallback(cb, user_d); }

};

/*
** AttributeForm fills in the parent form with prompts for the 
** spectrum attributes.
*/
class AttributeForm
{
 protected:			/* Widgets given in t to b l to r order */
  XMToggleButton *flipped;
  XMToggleButton *logscale;
  XMToggleButton *autoscale;
  XMText         *fullscale;
  XMLabel        *fullscale_label;
  XMToggleButton *lowlevel_enable;
  XMText         *lowlevel;
  XMLabel        *lowlevel_label;
  XMToggleButton *hilevel_enable;
  XMText         *hilevel;
  XMLabel        *hilevel_label;
 public:
  /*  Constructors and destructors: */
 
  AttributeForm(char *name, XMForm &parent);
  virtual ~AttributeForm() {
    delete flipped;
    delete logscale;
    delete autoscale;
    delete fullscale;
    delete fullscale_label;
    delete lowlevel_enable;
    delete lowlevel;
    delete lowlevel_label;
    delete hilevel_enable;
    delete hilevel;
    delete hilevel_label;
  }

  /* Get values of the widgets:  */

  Boolean Flipped()         { return flipped->GetState(); }
  Boolean LogScale()        { return logscale->GetState(); }
  Boolean AutoScale()       { return autoscale->GetState(); }
  char   *FullScale()       { return fullscale->GetText(); }
  Boolean LowLevelEnabled() { return lowlevel_enable->GetState(); }
  char   *LowLevel()        { return lowlevel->GetText(); }
  Boolean HiLevelEnabled()  { return hilevel_enable->GetState(); }
  char   *HiLevel()         { return hilevel->GetText();  }


  /* Enable/disable the various pairs of text/labels  */

  void SetFullScaleSensitivity(Boolean enabled) {
    fullscale->SetAttribute(XmNeditable, enabled);
    fullscale->SetAttribute(XmNsensitive, enabled);
    fullscale->SetAttribute(XmNcursorPositionVisible, enabled);
    fullscale_label->SetAttribute(XmNsensitive, enabled);
  }
  void SetLowLevelSensitivity(Boolean enabled) {
    lowlevel->SetAttribute(XmNeditable, enabled);
    lowlevel->SetAttribute(XmNsensitive, enabled);
    lowlevel->SetAttribute(XmNcursorPositionVisible, enabled);
    lowlevel_label->SetAttribute(XmNsensitive, enabled);
  }
  void SetHiLevelSensitivity(Boolean enabled) {
    hilevel->SetAttribute(XmNeditable, enabled);
    hilevel->SetAttribute(XmNsensitive, enabled);
    hilevel->SetAttribute(XmNcursorPositionVisible, enabled);
    hilevel_label->SetAttribute(XmNsensitive, enabled);
  }

  /* Adjust the ghosting depending on the state of the toggle button widgets: */

  void AdjustGhosting() {
    SetFullScaleSensitivity(!AutoScale());
    SetLowLevelSensitivity(LowLevelEnabled());
    SetHiLevelSensitivity(HiLevelEnabled());
  }

  /* Set the values of the widgets:  */

  void Flipped(Boolean state)     { flipped->SetState(state); }
  void LogScale(Boolean state)    { logscale->SetState(state); }
  void AutoScale(Boolean state)   { autoscale->SetState(state); }
  void FullScale(char *digits)    { fullscale->SetText(digits); }
  void LowLevelEnabled(Boolean state) { lowlevel_enable->SetState(state); }
  void LowLevel(char *digits)     { lowlevel->SetText(digits); }
  void HiLevelEnabled(Boolean state) { hilevel_enable->SetState(state); }
  void HiLevel(char *digits)      { hilevel->SetText(digits); }

  /* Allow callbacks to be set on the text widgets to validate input */

  Callback_data *AddFullScaleValidationCallback(void (*cb)(XMWidget *, 
						 XtPointer, XtPointer),
				      XtPointer user_d = NULL)
    { return fullscale->AddCallback(XmNmodifyVerifyCallback, cb, user_d); }

  Callback_data *AddLowLevelValidationCallback(void (*cb)(XMWidget *,
						XtPointer, XtPointer),
				     XtPointer user_d = NULL)
    { return lowlevel->AddCallback(XmNmodifyVerifyCallback, cb, user_d); }

  Callback_data *AddHiLevelValidationCallback(void (*cb)(XMWidget *,
					       XtPointer, XtPointer),
				    XtPointer user_d = NULL)
    { return hilevel->AddCallback(XmNmodifyVerifyCallback, cb, user_d); }

  /* Allow callbacks to be set on all of the toggle buttons: */

  Callback_data *AddAutoScaleCallback(void (*cb)(XMWidget *, XtPointer, XtPointer),
		       XtPointer user_d = NULL) {
    return autoscale->AddCallback(cb, user_d);
  }
  Callback_data *AddLowLevelEnableCallback(void (*cb)(XMWidget *, XtPointer, XtPointer),
			    XtPointer user_d = NULL) {
    return lowlevel_enable->AddCallback(cb, user_d);
  }
  Callback_data *AddHiLevelEnableCallback(void (*cb)(XMWidget *, XtPointer, XtPointer),
			   XtPointer user_d = NULL) {
    return hilevel_enable->AddCallback(cb, user_d);
  }
  
};
/*
** ReduceForm class fills in a form with a prompter for the pixel
** reduction method.
*/
class ReduceForm
{
 protected:
  XMRowColumn *radio_box;
  XMToggleButton *Sampled;
  XMToggleButton *Averaged;
  XMToggleButton *Summed;
 public:
  /*
  **  Constructors and destructors:
  */
  ReduceForm(char *name, XMForm &parent);
  virtual ~ReduceForm() {
    delete Summed;
    delete Averaged;
    delete Sampled;
    delete radio_box;
  }
  /* 
  ** Management -- we need to get our manager dealt with too: 
  */
  void Manage() {
    Sampled->Manage();
    Averaged->Manage();
    Summed->Manage();
    radio_box->Manage();
  }
  void UnManage() {
    Sampled->UnManage();
    Averaged->UnManage();
    Summed->UnManage();
    radio_box->UnManage();
  }
  /*
  **  Manipulators:
  */
  void State(reduction_mode value);	/* Set the state */
  reduction_mode State();	/* Get the state */

};

/*
** Rend1dForm - This class lays out a prompter for 1-d spectrum rendition
**              selection within a parent form widget object.
*/
class Rend1dForm {
 protected:
  XMRowColumn *radio_box;
  XMToggleButton *Histogram;
  XMToggleButton *Lines;
  XMToggleButton *Points;
 public:
  /* 
  ** Constructors and destructors:
  */
  Rend1dForm(char *name, XMForm &parent);
  virtual ~Rend1dForm() {
    delete Histogram;
    delete Lines;
    delete Points;
  }
  /* Manipulators: */

  rendition_1d State();		     /* Return state of the toggles. */
  void State(rendition_1d newstate); /* Set new state of the toggles */

  /* Management... we need to handle the additional manager */

  void Manage() {
    radio_box->Manage();
  }
  void UnManage() {
    radio_box->UnManage();
  }
};

/*
** Rend2dForm -- This class builds the behavior of a prompter for 
**               2-d renditions inside a parent form.
*/
class Rend2dForm {
 protected:
  XMRowColumn    *radio_box;	/* work area radio-box manager. */
  XMToggleButton *Color;	/* Color rendition toggle button */
  XMToggleButton *Scatter;	/* Scatter plot toggle button. */
  XMToggleButton *Boxes;	/* Box plot toggle button.     */
 public:
  /*
  ** Constructors and destructors:
  */
  Rend2dForm(char *name, XMForm &w);
  virtual ~Rend2dForm() {
    delete Color;
    delete Scatter;
    delete Boxes;
    delete radio_box;
  }
  /*
  ** Manipulators
  */
  rendition_2d State();		    /* Get dialog state. */
  void State(rendition_2d newstate); /* Set dialog initial state. */
  void SetColor(Boolean state) { /* Modify dialog depending on color cap. */
    if(state) {
      Color->Manage();
    }
    else {
      Color->UnManage();
    }
  }

  /*  We need to tack on some additional management to handle the 
  **  radio box manager:
  */

  void Manage() {
    radio_box->Manage();
  }

  void UnManage() {
    radio_box->UnManage();
  }
};

/*
** TitleForm -- This class creates a title attributes prompter within
**              a parent form.
*/
class TitleForm
{
 protected:
  XMToggleButton *showname;
  XMToggleButton *shownumber;
  XMToggleButton *showdesc;
  XMToggleButton *showmaxpeak;
  XMToggleButton *showupdate;
  XMToggleButton *showobjects;
 public:
  /*  Constructors and destructors: */

  TitleForm(char *name, XMForm &parent);
  virtual ~TitleForm()
    { delete showname;
      delete shownumber;
      delete showdesc;
      delete showmaxpeak;
      delete showupdate;
      delete showobjects;
    }

  void Manage() {
    XtManageChild(XtParent(showname->getid()));
  }
  void UnManage() {
    XtUnmanageChild(XtParent(showname->getid()));
  }

  /*  Object manipulators to get and set the state of the toggles */

  Boolean ShowName()        { return showname->GetState();    }
  Boolean ShowNumber()      { return shownumber->GetState();  }
  Boolean ShowDesc()        { return showdesc->GetState();    }
  Boolean ShowMaxPeak()     { return showmaxpeak->GetState(); }
  Boolean ShowUpdate()      { return showupdate->GetState();  }
  Boolean ShowObjects()     { return showobjects->GetState(); }

  void ShowName(Boolean state)   { showname->SetState(state); }
  void ShowNumber(Boolean state) { shownumber->SetState(state); }
  void ShowDesc(Boolean state)   { showdesc->SetState(state); }
  void ShowMaxPeak(Boolean state) {showmaxpeak->SetState(state); }
  void ShowUpdate(Boolean state) { showupdate->SetState(state); }
  void ShowObjects(Boolean state) { showobjects->SetState(state); }

  /* Add callbacks to the toggle buttons.  While at the time this */
  /* object is being created this is not necessary, the methods are */
  /* added for completeness.                                        */

  Callback_data *AddShowNameCallback(void (*cb)(XMWidget *, XtPointer, XtPointer), 
		      XtPointer user_d = NULL)
    { return showname->AddCallback(cb, user_d); }

  Callback_data *AddShownumberCallback(void (*cb)(XMWidget *, XtPointer, XtPointer), 
		      XtPointer user_d = NULL)
    { return shownumber->AddCallback(cb, user_d); }

  Callback_data *AddShowDescCallback(void (*cb)(XMWidget *, XtPointer, XtPointer), 
		      XtPointer user_d = NULL)
    { return showdesc->AddCallback(cb, user_d); }

  Callback_data *AddShowMaxPeakCallback(void (*cb)(XMWidget *, XtPointer, XtPointer), 
		      XtPointer user_d = NULL)
    { return showmaxpeak->AddCallback(cb, user_d); }

  Callback_data *AddShowUpdateCallback(void (*cb)(XMWidget *, XtPointer, XtPointer), 
		      XtPointer user_d = NULL)
    { return showupdate->AddCallback(cb, user_d); }

  Callback_data *AddShowObjectsCallback(void (*cb)(XMWidget *, XtPointer, XtPointer), 
		      XtPointer user_d = NULL)
    { return showobjects->AddCallback(cb, user_d); }

};

/*
** RefreshForm  - This class provides a prompter for auto-refreshing spectra.
*/
class RefreshForm
{
 protected:
   XMScale   *interval;
 public:
   /* Constructors and destructors: */

   RefreshForm(char *name, XMForm &parent);

   virtual ~RefreshForm() {
     delete interval;
   }
   int Value() {
     return interval->Value();
   }
   void Value(int v) {
     interval->Value(v);
   }
   void Manage() { interval->Manage(); }
   void UnManage() {interval->UnManage(); }
};
#endif
