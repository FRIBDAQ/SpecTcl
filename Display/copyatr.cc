/*
** Facility:
**   Xamine -- NSCL display program.
** Abstract:
**   copyatr.cc:
**     This file implements the copy attributes functions.  Copy attributes
**     is an Xamine function which allows you to copy the pane attributes
**     of the selected pane to one or more destination panes.
** Author:
**    Ron Fox
**    NSCL
**    Michigan State University
**   East Lansing, MI 48824-1321
*/


/*
** Include files:
*/
#include  <stdio.h>
#include  <string.h>

#include "dispwind.h"
#include "panemgr.h"
#include "checklist.h"
#include "compatspec.h"

#include "superpos.h"
#include "XMDialogs.h"
#include "XMManagers.h"
#include "XMCallback.h"
#include "compatspec.h"
#include "errormsg.h"
#include "helpmenu.h"
#include "refreshctl.h"
/*
** Local storage:
*/
static char *mode[] = { "Sampled",
			"Summed",
			"Averaged"
		      };
static char *rend2[] = {
                         "Scatter",
			 "Boxes",
			 "Color",
			 "Contour",
			 "Surface",
			 "Lego"
			 };
static char *rend1[] = {
                         "Smoothed",
			 "Histogram",
			 "Lines",
			 "Points"
			 };

/* Classes which are in the dialog:
*/

class AxisLabels : public CheckList /* Axis label part of the form. */
{
 public:
  AxisLabels(XMWidget *parent, win_attributed *atr);
  ~AxisLabels();

  void Update(win_attributed *atr);	          /* Update the toggles status */
  void CopyAttributes(win_attributed *in,      /* Copy in to out based on checks */
		 win_attributed *out);		 

};

class SpectrumTitles : public CheckList	/* Spectrum titles part of the pic. */
{
 public:
  SpectrumTitles(XMWidget *parent, win_attributed *atr);
  ~SpectrumTitles();

  void Update(win_attributed *atr);	          /* Update the toggles status */
  void CopyAttributes(win_attributed *in,      /* Copy in to out based on checks */
		 win_attributed *out);		 

};

class DisplayAttributes : public CheckList
{
 public:
  DisplayAttributes(XMWidget *parent, win_attributed *atr);
  ~DisplayAttributes();

  void Update(win_attributed *atr);	          /* Update the toggles status */
  void CopyAttributes(win_attributed *in,      /* Copy in to out based on checks */
		 win_attributed *out);		 

};

class TwodAttributes : public CheckList
{
 public:
  TwodAttributes(XMWidget *parent, win_2d *atr);
  ~TwodAttributes();

  void Update(win_2d *atr);	          /* Update the toggles status */
  void CopyAttributes(win_2d  *in,      /* Copy in to out based on checks */
		 win_2d *out);		 

};

class OnedAttributes : public CheckList
{
 public:
  OnedAttributes(XMWidget *parent, win_1d *atr);
  ~OnedAttributes();

  void Update(win_1d *atr);	          /* Update the toggles status */
  void CopyAttributes(win_1d *in,      /* Copy in to out based on checks */
		 win_1d *out);		 

};

class AttributeCopy : public XMForm
{
 public:
  AttributeCopy(XMWidget *parent, win_attributed *atr);
  ~AttributeCopy();

  void Manage();
  void UnManage();

  void Update(win_attributed *atr);
  void CopyAttributes(win_attributed *from, win_attributed *to);
  void ClearDestinationList();
  int  GetDestinationList(char ***spectra);

 protected:
  XMForm                   *properties;        /* Checkboxes in here. */
  AxisLabels               *LabelAttributes;
  SpectrumTitles           *TitleAttributes;
  DisplayAttributes        *DisplayConfig;
  TwodAttributes           *TwodSpecific;
  OnedAttributes           *OnedSpecific;
  DisplayedCompatibleSpectrumList   *CopyTo;
  XMPushButton             *UpdateButton;
  void ArrangeWidgets();	/* Re paste the widgets. */
  Boolean                   oned;
};

class AttributeCopyDialog : public XMCustomDialog
{
 public:
  AttributeCopyDialog(char *name, XMWidget *parent, char *title,
		      win_attributed *atr);
  ~AttributeCopyDialog();
  
  void OkPressed(XMWidget *w, XtPointer ud, XtPointer cd);
  void CancelPressed(XMWidget *w, XtPointer ud, XtPointer cd);
  void HelpPressed(XMWidget *w, XtPointer ud, XtPointer cd);
  void Manage();
  void UnManage();
  void Update(win_attributed *atr);
 protected:
  XMCallback<AttributeCopyDialog> Okcb;
  XMCallback<AttributeCopyDialog> Applycb;
  XMCallback<AttributeCopyDialog> Cancelcb;
  XMCallback<AttributeCopyDialog> Helpcb;

  AttributeCopy *WorkAreaContents;

};

/*
** Forward function references:
*/

static void CopyAttributes(AttributeCopy *form); /* Copy selected pane attribs.  */

/*
** The functions on this page are methods for the AxisLabels class.
** That is a form contaning a checklist.  The checklist allows
** you to check for copying attributes associated with axis labelling.
** It also shows the current values of these attributes.
*/

/*
** Functional Description:
**   AxisLabels::AxisLabels:
**     Instantiates an axis label checklist form.
** Formal Parameters:
**   XMWidget *parent:
**      Parent of the widget tree that is a checklist.
**   win_attributed *atr:
**      attribute set to use when constructing the actual status
**      fields.
*/
AxisLabels::AxisLabels(XMWidget *parent, win_attributed *atr) :
       CheckList("AxisLabels", parent, "Axis Labels", 3)
{
  /* Set the button titles:  */

  SetButtonString(0, "Axes        ");
  SetButtonString(1, "Ticks       ");
  SetButtonString(2, "Tick Labels ");

  State(0, True);
  State(1, True);
  State(2, True);

  Update(atr);


}
/*
** Destructor: 
*/
AxisLabels::~AxisLabels() {}

/*
** AxisLabels::Update:
**  Update the state of the labels.
** Formal Parameters:
**   win_attributed *atr:
**     The window attributes object.
*/
void AxisLabels::Update(win_attributed *atr)
{
  SetLabelString(0, (char*)(atr->showaxes()  ? "On" : "Off"));
  SetLabelString(1, (char*)(atr->showticks() ? "On" : "OFF"));
  SetLabelString(2, (char*)(atr->labelaxes() ? "On" : "OFF"));

}
/*
** AxisLabels::CopyAttributes:
**   Copy axis attributes as dictated by toggles.
** Formal Parameters:
**   win_attributed *in:
**      Input attributes (source)
**   win_attributed *out:
**      Output attributes (destination)
*/
void AxisLabels::CopyAttributes(win_attributed *in, win_attributed *out)
{
  if(State(0)) {
    if(in->showaxes()) out->axeson();
    else               out->axesoff();
  }

  if(State(1)) {
    if(in->showticks())  out->tickson();
    else                 out->ticksoff();
  }

  if(State(2)) {
    if(in->labelaxes()) out->axis_labelson();
    else                 out->axis_labelsoff();
  }
}

/*
** The functions on this page implement the methods of the 
** SpectrumTitles class.  That class provides a user interface and
** mechanisms for copying the spectrum title label attributes from one
** spectrum pane to another selectively.
*/

/*
** Functional Description:
**    SpectrumTitles::SpectrumTitles:
**       The Constrcutor for the class.  We instantiate the CheckList
**       label it's toggle switches and force an update setting the
**       text in the values fields.
** Formal Parameters:
**   XMWidget *parent:
**      Parent widget of the checklist composite widget.
**   win_attributed *atr:
**      Attributes of the pane we're copying from.
*/
SpectrumTitles::SpectrumTitles(XMWidget *parent, win_attributed *atr) :
       CheckList("SpectrumTitles", parent, "Spectrum Titles", 6)
{
  SetButtonString(0, "Name          ");
  SetButtonString(1, "Number        ");
  SetButtonString(2, "Description   ");
  SetButtonString(3, "Peak Info     ");
  SetButtonString(4, "Update time   ");
  SetButtonString(5, "Object labels ");

  State(0, True);
  State(1, True);
  State(2, True);
  State(3, True);
  State(4, True);
  State(5, True);

  Update(atr);
}

/*  Destructor: */

SpectrumTitles::~SpectrumTitles() {}

/*
** Functional Description:
**   SpectrumTitles::Update:
**    updates the status strings in the checklist.
** Formal Parameters:
**   win_attributed *atr:
**     attributes block.
*/
void SpectrumTitles::Update(win_attributed *atr)
{
  SetLabelString(0, (char*)(atr->showname() ? "On" : "Off"));
  SetLabelString(1, (char*)(atr->shownum()  ? "On" : "Off"));
  SetLabelString(2, (char*)(atr->showdescrip() ? "On" : "Off"));
  SetLabelString(3, (char*)(atr->showpeak() ? "On" : "Off"));
  SetLabelString(4, (char*)(atr->showupdt() ? "On" : "Off"));
  SetLabelString(5, (char*)(atr->showlbl()  ? "On" : "Off"));

}
/*
** Functional Description:
**   SpectrumTitles::CopyAttributes:
**      This function initiates a controlled copy of the title attributes
**      from an input pane to an output pane. The State of the check boxes
**      is used to filter what gets copied.
** Formal Parameters;
**   win_attributed *in:
**     input attributes block.
**   win_attributed *out:
**     output attributes block.
*/
void SpectrumTitles::CopyAttributes(win_attributed *in, win_attributed *out)
{
  if(State(0)) {
    if(in->showname()) out->dispname();
    else               out->hidename();
  }
  if(State(1)) {
    if(in->shownum()) out->dispid();
    else              out->hideid();
  }
  if(State(2)) {
    if(in->showdescrip()) out->dispdescr();
    else                  out->hidedescr();
  }
  if(State(3)) {
    if(in->showpeak())  out->disppeak();
    else                out->hidepeak();
  }
  if(State(4)) {
    if(in->showupdt())  out->dispupd();
    else                out->hideupd();
  }
  if(State(5)) {
    if(in->showlbl())  out->labelobj();
    else               out->unlabelobj();
  }

}

/*
** The functions on this page are methods for the DisplayAttributes class.
** this class allows one to do a filtered copy of one pane display attributes
** values to another.
*/

/*
** Functional Description:
**   DisplayAttributes::DisplayAttributes:
**     Instantiates a DisplayAttributes object.
** Formal Parameters:
**   XMWidget *parent:
**      The parent widget.
**   win_attributed:
**      The attributes of the source.
*/
DisplayAttributes::DisplayAttributes(XMWidget *parent, win_attributed *atr) : 
       CheckList("DisplayAttributes", parent, "Display Attributes",
		 7)
{

  /* Label the toggles */

  SetButtonString(0, "Axis Orientation ");
  SetButtonString(1, "Reduction        ");
  SetButtonString(2, "Scale            ");
  SetButtonString(3, "Counts axis      ");
  SetButtonString(4, "Floor            ");
  SetButtonString(5, "Ceiling          ");
  SetButtonString(6, "Auto update      ");

  /* Initialize the default filter */

  State(2, True);
  State(3, True);
  State(4, True);
  State(5, True);
  State(6, True);

  /* Set the status strings */

  Update(atr);
}

/* Destructor: */

DisplayAttributes::~DisplayAttributes() {}

/*
** Functional Description:
**   DisplayAttributes::Update:
**    This function updates the status text associated with a
**    a DisplayAttributes object.
** Formal Parameters:
**   win_attributed *atr:
**      The attribute block to use to describe the status.
*/
void DisplayAttributes::Update(win_attributed *atr)
{
  char value[32];
  
  SetLabelString(0, (char*)(atr->isflipped() ? "Flipped" : "Normal"));

  strcpy(value, mode[atr->getreduction()]);
  SetLabelString(1, value);

  if(atr->manuallyscaled()) 
    sprintf(value, "%d", atr->getfsval());
  else
    strcpy(value, "Auto");
  SetLabelString(2, value);
	 
  SetLabelString(3, (char*)(atr->islog() ? "Log" : "Linear"));

  if(atr->hasfloor()) 
    sprintf(value, "%d", atr->getfloor());
  else
    strcpy(value, "Disabled");
  SetLabelString(4, value);

  if(atr->hasceiling())
    sprintf(value, "%d", atr->getceiling());
  else
    strcpy(value, "Disabled");
  SetLabelString(5, value);

  if(atr->update_interval() != 0) 
    sprintf(value, "%d", atr->update_interval());
  else
    strcpy(value, "Disabled");
  SetLabelString(6, value);


  
}
/*
** Functional Description:
**   DisplayAttributes:CopyAttributes:
**     This function coies a filtered set of attributes from the
**     source pane to the destination pane.
** Formal Parameters:
**    win_attributed *in:
**      Input attribute set.
**    win_attributed *out:
**      Destination attribute set.
*/
void DisplayAttributes::CopyAttributes(win_attributed *in, win_attributed *out)
{
  if(State(0)) {		/* Flip axes: */
    out->setflip(in->isflipped());
  }

  if(State(1)) {		/* Reduction mode. */
    out->setreduce(in->getreduction());
  }

  if(State(2)) {		/* Scaling. */
    if(in->manuallyscaled()) out->setfs(in->getfs());
    else
      out->autoscale();
  }

  if(State(3)) {		/* Log/lin */
    out->setlog(in->islog());
  }

  if(State(4)) {		/* Floor value. */
    if(in->hasfloor())
      out->setfloor(in->getfloor());
    else
      out->nofloor();
  }

  if(State(5)) {		/* Ceiling value */
    if(in->hasceiling())
      out->setceiling(in->getceiling());
    else
      out->noceiling();
  }

  if(State(6)) {		/* Autoupdate */
    if(in->update_interval() == 0)
      out->noautoupdate();
    else
      out->update_interval(in->update_interval());
  }
}


/*
** Methods on this page implement the behavior of the TwodAttributes
** checklist 'widget'.  This user interface component allows you to 
** perform filtered copies of the attribute block elements specific
** to panes containing 2-d spectra.
*/

/*
** Functional Description:
**   TwodAttributes::TwodAttributes:
**     Instantiates a checklist for twod attributes.
** Formal Parameters:
**   XMWidget *parent:
**     Parent widget for the checklist.
**   win_2d *atr:
**     Attribute block used to stock the status parts of the checklist.
*/
TwodAttributes::TwodAttributes(XMWidget *parent, win_2d *atr) :
       CheckList("2-d", parent, "2-d Attributes", 3)
{
  SetButtonString(0, "Rendition        ");
  SetButtonString(1, "Expansion Limits ");
  SetButtonString(2, "Projection       ");

  State(0, True);
  State(1, True);
 

  Update(atr);
}

/* Destructor: */

TwodAttributes::~TwodAttributes() {}

/*
** Functional Description:
**   TwodAttributes::Update:
**     Updates the status section of the checklist.
** Formal Parameters:
**   win_2d *atr:
**      The attributes block which contains the status of the pane.
*/
void TwodAttributes::Update(win_2d *atr)
{
  char label[100];

  SetButtonString(0, rend2[atr->getrend()]);

  if(atr->isexpanded()) {
    sprintf(label, "(%d,%d) - (%d,%d)", 
	    atr->xlowlim(), atr->ylowlim(),
	    atr->xhilim(), atr->yhilim());
  }
  else {
    strcpy(label, "Not Expanded");
  }
  SetButtonString(1, label);

  SetButtonString(2,"Unimplemented");
 
}
/*
**  Functional Description:
**     TwodAttributes::CopyAttributes:
**       This method does a filtered copy of the
**       2-d attributes from an input attribute block
**       to an output attribute block.
** Formal Parameters:
**    win_2d *in:
**      Input attributes block.
**    win_2d *out:
**      Output attributes block.
*/
void TwodAttributes::CopyAttributes(win_2d *in, win_2d *out)
{
  if(State(0)) {
    out->setrend(in->getrend());
  }

  if(State(1)) {
    if(in->isexpanded()) {
      out->expand(in->xlowlim(), in->xhilim(),
		  in->ylowlim(), in->yhilim());
    }
    else {
      out->unexpand();
    }
  }

  if(State(2)) {
    /* Projections are not supported at this time.  */
  }
}


/*
** Functional Description:
**    OnedAttributes::OnedAttributes:
**      Constructs a checklist for the 1-d specific spectrum attributes.
**      The checklist will allow the user to create a filtered copy
**      list of these attributes.
** Formal Parameters:
**   XMWidget *parent:
**     Parent of the checklist.
**   win_1d *atr:
**     The spectrum attributes.
*/
OnedAttributes::OnedAttributes(XMWidget *parent, win_1d *atr) :
       CheckList("OnedAttribs", parent, "1-d Attributes", 3)
{
  SetButtonString(0, "Rendition");
  SetButtonString(1, "Expansion");
  SetButtonString(2, "Superposition");

  State(0, True);
  State(1, True);

  Update(atr);
}

/*
** Destructor:
*/
OnedAttributes::~OnedAttributes() {}


/*
** Functional Description:
**   OnedAttributes::Update:
**      This function updates the status part of the 1-d attributes checklist.
** Formal Parameters:
**   win_1d *atr:
**     Pointer to the attributes object which defines this spectrum.
*/
void OnedAttributes::Update(win_1d *atr)
{
  char label[100];
  SuperpositionList  slist = atr->GetSuperpositions();

  SetLabelString(0, rend1[atr->getrend()]);

  if(atr->isexpanded()) {
    sprintf(label, "%d - %d", atr->lowlimit(), atr->highlimit());
  }
  else
    strcpy(label, "Not Expanded");
  SetLabelString(1, label);

  if(slist.Count() > 0) {
    sprintf(label, "%d spectra", slist.Count());
  }
  else 
    strcpy(label, "None");
  SetLabelString(2, label);
}
/*
** Functional Description:
**   OnedAttributes::CopyAttributes
**    This method allows a checklisted copy of the 1-d specific attributes
**    of a spectrum.
** Formal Parameters:
**   win_1d *in:
**     Set of input attributes to copy.
**   win_1d *out:
**     output attribute set to copy them to.
*/
void OnedAttributes::CopyAttributes(win_1d *in, win_1d *out)
{
  if(State(0)) {		/* Copy rendition. */
    out->setrend(in->getrend());
  }

  if(State(1)) {		/* Copy Expansion */
    if(in->isexpanded()) {
      out->expand(in->lowlimit(), in->highlimit());
    }
    else {
      out->unexpand();
    }
  }

  if(State(2)) {		/* Copy superposition list. */
    SuperpositionList sin  = in->GetSuperpositions();
    SuperpositionListIterator siter(sin);

    SuperpositionList sout = out->GetSuperpositions();

    sout.Clear();		/* Start by clearing the output list. */

    for(int i = 0; i < sin.Count(); i++) {
      Superposition s = siter.Next();
      sout.Add(s.Spectrum());
    } 
  }
}
/*
** The next several functions define the methods of the 
** Attribute copy form.  This form contains all of the attribute copiers,
** the chooser list, and an update button arranged in some attractive manner.
** The autonomous behavior is to perform updates, when the update button
** is pushed.  Available behavior is to update, and to perform the actual
** copy operations.
*/
/*
** AtrCopy_UpdateRelay:
**   This function is a callback relay which is used to relay the
**   update button to the Update method:
**
** Formal Parameters:
**   XMWidget *button:
**      The button that was pressed to call us.
**   XtPointer User_data:
**      User data which is actually a pointer to an instance of 
**      AttributeCopy.
**   XtPointer call_data:
**     XmPushButtonCallbackStruct data structure pointer.
*/

static void AtrCopy_UpdateRelay(XMWidget *button, XtPointer user_data,
				XtPointer call_data)
{
  AttributeCopy *us = (AttributeCopy *)user_data;
  win_attributed *atr = Xamine_GetSelectedDisplayAttributes();
  us->Update(atr);
}
/*
** AttributeCopy::AttributeCopy:
**    Constructor/initializer for the Attribute Copy class.
** Formal Parameters:
**   XMWidget *parent:
**     Parent widget of the widget tree we're producing.
**   win_attributed *atr:
**     Pane attributes of the source pane.
*/
AttributeCopy::AttributeCopy(XMWidget *parent, win_attributed *atr) :
       XMForm("AttributeCopy", *parent)
{

  win_1d atr1;
  win_2d atr2;

  /* Create the subwidgets:    */

  properties              = new XMForm("Checkboxes", *this);
  LabelAttributes         = new AxisLabels(properties, atr);
  TitleAttributes         = new SpectrumTitles(properties, atr);
  DisplayConfig           = new DisplayAttributes(properties, atr);
  TwodSpecific    = new TwodAttributes(properties, &atr2);
  OnedSpecific    = new OnedAttributes(properties, &atr1);

  CopyTo          = new DisplayedCompatibleSpectrumList("Speclist",
							this,
							"Copy to",
							atr->spectrum());
  UpdateButton   = new XMPushButton("Update", *this, 
				    AtrCopy_UpdateRelay, (XtPointer)this);

  ArrangeWidgets();
  oned = atr->is1d();
  Update(atr);
}
/*
** Functional Description:
**   AttributeCopy::ArrangeWidgets:
**     This method arranges the widgets on the form surface.
**     The arrangement depends on the type of spectrum being displayed
** Formal Parameters:
*/
void AttributeCopy::ArrangeWidgets()
{

  /* Paste them up to the form:    */

  SetFractionBase(100);
  properties->SetFractionBase(100);		/* Nice fine grid. */

  properties->SetLeftAttachment(*LabelAttributes, XmATTACH_FORM); /* Label attribs. */
  properties->SetTopAttachment(*LabelAttributes,  XmATTACH_FORM);
  properties->SetRightAttachment(*LabelAttributes, XmATTACH_POSITION);
  properties->SetRightPosition(*LabelAttributes, 48); /* Attach to the midline. */

  properties->SetLeftAttachment(*TitleAttributes, XmATTACH_FORM);
  properties->SetTopAttachment(*TitleAttributes, XmATTACH_WIDGET);
  properties->SetTopWidget(*TitleAttributes, *LabelAttributes);
  properties->SetRightAttachment(*TitleAttributes, XmATTACH_POSITION);
  properties->SetRightPosition(*TitleAttributes, 48);
  properties->SetBottomAttachment(*TitleAttributes, XmATTACH_FORM);

  properties->SetLeftAttachment(*OnedSpecific,  XmATTACH_POSITION);
  properties->SetLeftPosition(*OnedSpecific,    52);
  properties->SetTopAttachment(*OnedSpecific,   XmATTACH_FORM);
  properties->SetRightAttachment(*OnedSpecific, XmATTACH_FORM);

  properties->SetLeftAttachment(*TwodSpecific,  XmATTACH_POSITION);
  properties->SetLeftPosition(*TwodSpecific,    52);
  properties->SetRightAttachment(*TwodSpecific, XmATTACH_FORM);
  properties->SetTopAttachment(*TwodSpecific, XmATTACH_WIDGET);
  properties->SetTopWidget(*TwodSpecific, *OnedSpecific);

  properties->SetLeftAttachment(*DisplayConfig, XmATTACH_POSITION);
  properties->SetLeftPosition(*DisplayConfig,  52);
  properties->SetTopAttachment(*DisplayConfig, XmATTACH_WIDGET);
  properties->SetTopWidget(*DisplayConfig, *TwodSpecific);
  properties->SetRightAttachment(*DisplayConfig, XmATTACH_FORM);
  properties->SetBottomAttachment(*DisplayConfig, XmATTACH_FORM);

  SetTopAttachment(*properties, XmATTACH_FORM);
  SetLeftAttachment(*properties, XmATTACH_FORM);
  SetRightAttachment(*properties, XmATTACH_FORM);

  SetLeftAttachment(*CopyTo,  XmATTACH_FORM);
  SetRightAttachment(*CopyTo, XmATTACH_FORM);
  SetTopAttachment(*CopyTo, XmATTACH_WIDGET);
  SetTopWidget(*CopyTo, *properties);

  SetTopAttachment(*UpdateButton, XmATTACH_WIDGET);
  SetTopWidget(*UpdateButton, *CopyTo);
  SetBottomAttachment(*UpdateButton, XmATTACH_FORM);
  SetLeftAttachment(*UpdateButton, XmATTACH_POSITION);
  SetLeftPosition(*UpdateButton,  33);

  /* Manage all the widgets except the parent form to get geometry 
  ** Computations done in advance:
  */

  LabelAttributes->Manage();
  TitleAttributes->Manage();
  DisplayConfig->Manage();
  TwodSpecific->Manage();
  OnedSpecific->Manage();
  properties->Manage();
  CopyTo->Manage();
  UpdateButton->Manage();
}
/*
** Destructor -- gets rid of the widgets we cretaed:
*/
AttributeCopy::~AttributeCopy()
{
  delete LabelAttributes;
  delete TitleAttributes;
  delete DisplayConfig;
  delete TwodSpecific;
  delete OnedSpecific;
  delete CopyTo;
  delete UpdateButton;
  delete properties;
}


/*
** Managment methods:
**   Manage   -- Manages the widgets (Makes visible)
** UnManage   -- Unmanages the widgets (Makes invisible).
*/
void AttributeCopy::Manage()
{
  XMForm::Manage();
}
void AttributeCopy::UnManage()
{
  XMForm::UnManage();
}

/*
** Functional Description:
**   AttributeCopy::Update:
**     This method updates all the widget boxes.
**     to do this is a bit more complex than might seem.  The
**    dimensionality of the spectrum might have changed since we were
**    last updated which in turn will require us to shuffle the 
**    Type specific attributes and some widget relationships around.
**  Formal Parameters:
**    win_attributed *atr:
**      Window pane attributes.
*/
void AttributeCopy::Update(win_attributed *atr)
{

  oned = atr->is1d();


  /* Now call all the update methods: */

  LabelAttributes->Update(atr);
  TitleAttributes->Update(atr);
  DisplayConfig->Update(atr);
  if(oned) {
    OnedSpecific->Update((win_1d *)atr);
  }
  else {
    TwodSpecific->Update((win_2d *)atr);
  }
  CopyTo->NewSpectrum(atr->spectrum());	/* This also updates. */


  if(oned) { 
    TwodSpecific->Disable();
    OnedSpecific->Enable();
  }
  else     {
     OnedSpecific->Disable();
     TwodSpecific->Enable();
  }
}
/*
** Functional Description:
**   AttributeCopy::CopyAttributes:
**     This function copies attributes from one pane to another given
**     the two window definitions.  The copy is filtered by the checklists
**     for each attribute set.
** Formal Parameters:
**    win_attributed *from:
**      Source pane.
**    win_attributed *to:
**      Destination pane.
*/
void AttributeCopy::CopyAttributes(win_attributed *from , win_attributed *to)
{
  LabelAttributes->CopyAttributes(from, to);
  TitleAttributes->CopyAttributes(from, to);
  DisplayConfig->CopyAttributes(from, to);
  if(!oned) TwodSpecific->CopyAttributes((win_2d *)from, 
						(win_2d *)to);
  if(oned) OnedSpecific->CopyAttributes((win_1d *)from, 
						(win_1d *)to);

}

/*
** Functional Description:
**   AttributeCopy::ClearDestinationList:
**    This function clears all items from the selection list.  Items which
**    are selected are deselected.  I think/hope that using this function
**    at unmanagement time will kill off some of the AXP Problems.
*/
void AttributeCopy::ClearDestinationList()
{
  CopyTo->Clear();
}
/*
** Functional Description:
**   AttributeCopy::GetDestinationList:
**      Get the list of destination spectra for the copy.
**      This is a matter of getting the list of selected spectra and
**      one by one converting them to character strings.  
**      The strings are located in the widget and therefore the 
**      caller should not delete them when done.
** Formal Parametrs:
**   char ***spectra:
**     Pointer which will be filled in with the table of spectrum names
**     that were selected.
** Returns:
**    Number of items selected.   Note that  the client must be prepeared to 
**    handle null pointers in the spectra list since partial returns are possible.
*/
int AttributeCopy::GetDestinationList(char ***spectra)
{
  int item_count;
  char **list;
  *spectra = (char **) NULL;	/* Assume everything failed. */

  CopyTo->GetSelections(&list, &item_count);

  if(item_count == 0) return 0;	/* Nothing selected.   */

  if(list  == NULL) return 0;


  /* and return the final list to the caller, whatever it might contain */

  *spectra = list;
  return item_count;
}

/*
** The following pages contain methods which implement the AttributeCopyDialog
** class.  This is a custom dialog which handles the copying of window props
** from one window to another set of windows.
** Note that this class also serves as a realistic test of the 
** XMCallback class.
*/
/*
** Functional Description:
**   AttributeCopyDialog::AttributeCopyDialog:
**      This is a constructor for the attribute copy dialog.
**      The main thing we need to do is to instantiate the work area which is
**      an AttributeCopy object and then set the callbacks for the action
**      area buttons.
** Formal Parameters:
**   char *name:
**     Name of the widget trea hierarchy base widget.
**   XMWidget *parent:
**     Parent of the dialog widget tree.
**   char *title:
**     Title of the dialog window decoration.
**   win_attributed *atr:
**     Pointer to attributes of window whose attributes will be copied.
*/
AttributeCopyDialog::AttributeCopyDialog(char *name, XMWidget *parent, 
					  char *title,
					 win_attributed *atr) :
       XMCustomDialog(name, *parent, title) ,
       Okcb(this) ,
       Applycb(this) ,
       Cancelcb(this) ,
       Helpcb(this)
{
  /* Create the work area compound widget and glue it to the sides of the
  ** work area form: 
  */

  WorkAreaContents = new AttributeCopy(work_area, atr);
  work_area->SetLeftAttachment(*WorkAreaContents, XmATTACH_FORM);
  work_area->SetRightAttachment(*WorkAreaContents, XmATTACH_FORM);
  work_area->SetTopAttachment(*WorkAreaContents, XmATTACH_FORM);
  work_area->SetBottomAttachment(*WorkAreaContents, XmATTACH_FORM);

  /* Now install the callbacks, Ok and apply are bound to the same function
  ** The flag parameter is True if the dialog should be dismissed after
  ** the work is done.
  */

	Okcb.Register(Ok, XmNactivateCallback, &AttributeCopyDialog::OkPressed,
		(XtPointer)True);
  Applycb.Register(Apply, XmNactivateCallback, &AttributeCopyDialog::OkPressed,
		    (XtPointer)False);

  Cancelcb.Register(Cancel, XmNactivateCallback, &AttributeCopyDialog::CancelPressed,
		     (XtPointer)NULL);
  Helpcb.Register(Help, XmNactivateCallback, &AttributeCopyDialog::HelpPressed,
		   (XtPointer)NULL);
}
/*
** Functional Description:
**   AttributeCopyDialog::~AttributeCopyDialog:
**     Unregisters the callbacks and destroys the widget.
*/
AttributeCopyDialog::~AttributeCopyDialog()
{
  delete WorkAreaContents;

  Okcb.UnRegister();
  Applycb.UnRegister();
  Cancelcb.UnRegister();
  Helpcb.UnRegister();

}
/*
** Functional Description:
**    AttributeCopyDialog::Manage   - Make dialog visible.
**    AttributeCopyDialog::
UnManage - Make dialog invisible.
*/

void AttributeCopyDialog::Manage()
{
  WorkAreaContents->Manage();
  XMCustomDialog::Manage();

}
void AttributeCopyDialog::UnManage()
{
  WorkAreaContents->UnManage();
  XMCustomDialog::UnManage();
}

/*
** Functional Description:
**    AttributeCopyDialog::Update:
**      This function updates the attribute copy member.
** Formal Parameters:
**    win_attributed *atr:
**     The attributes block pointer for the window.
*/
void AttributeCopyDialog::Update(win_attributed *atr)
{
  if(atr != NULL) {
    UnManage();
    XtAppContext ap = XtWidgetToApplicationContext(id);
    XSync(XtDisplay(id), False);
    while(XtAppPending(ap) )
      XtAppProcessEvent(ap, XtIMAll);
    WorkAreaContents->Update(atr);
    WorkAreaContents->Update(atr);   // Seems needed for AXP VMS. ?!?
    Manage();
  }
  else {
    Xamine_error_msg(this, "Spectrum not valid anymore");
    UnManage();
  }
}
/*
** Functional Description:
**    AttributeCopyDialog::CancelPressed:
**       This method is called in response to pressing the Cancel button.
**       We simply unmanage ourselves.
*/
void AttributeCopyDialog::CancelPressed(XMWidget *b, 
					XtPointer ud, XtPointer cd)
{
  UnManage();
}

/*
** The stuff below is used to describe the help dialog that can be popped
** up from this dialog
*/
static char *AtrCpyHelp_Text[] = {
  "    This dialog allows you to copy the display attributes of a window\n",
  "to one or panes containing compatible spectra.   The top part of\n",
  "the work area of this dialog consists of a number of check boxes and \n",
  "status strings.  Checking the box (by clicking in it) indicates that you \n",
  "want the characteristic to be copied.  Unchecking it (by clicking) means\n",
  "that the destination pane should retain the values of that attribute\n",
  "The status string indicates what the current value of that attribute is\n",
  "in the source pane\n",
  "    The Lower half of the work area contains a list of the names of \n",
  "compatible spectra which are currently displayed in visible panes.  You\n",
  "can add spectra to the list by shift-clicking them or remove them from the\n",
  "list in the same way.  By manipulating this list you determine which panes\n",
  "the selected attributes will be copied to\n",
  "    At the bottom of the work area a single button labelled 'Update' allows\n",
  "you to update the current attributes and the set of compatible spectra\n",
  "you may need to operate this button if you change the selected window\n",
  "or you modify the set of spectra displayed.\n\n",
  "    When you have chosen the set of attributes to copy and specified the\n",
  "set of panes which will be destinations, you must click one of the buttons\n",
  "in the action area to perform the copy.  These buttons have the following\n",
  "meanings:\n\n",
  "    Apply     - Perform the copy, leave the dialog in place\n",
  "    Ok        - Perform the copy, dismiss the dialog\n",
  "    Cancel    - Dismiss the dialog without performing any copies\n",
  "    Help      - Display this help text dialog\n",
  NULL
  };

static Xamine_help_client_data help = { "Copy_Help",
					  NULL,
					  AtrCpyHelp_Text
					  };

/*
** Functional Description:
**   AttributeCopyDialog::HelpPressed:
**     This function responds to the help button.  We just call 
**     Xamine_display_help to display the help text.
** Formal Parameters:
**    XMWidget *button:
**      Handle to the help button widget object.
**    XtPointer User_data:
**      Unused user data.
**    XtPointer call_data:
**      Unused call data.
*/
void AttributeCopyDialog::HelpPressed(XMWidget *button, XtPointer user_data,
				      XtPointer call_data)
{
  Xamine_display_help(button, (XtPointer)&::help, (XtPointer)NULL);
}
/*
** Functional Description:
**   AttributeCopyDialog::OkPressed:
**     This function is called in response to the Ok or Apply buton being pressed
**     There are two cases.  The only difference in operation is whether or not
**     the dialog is dismissed after all the copying is done (it is if Ok was
**     pressed).  We distinguish between the cases through the User data which
**     is True if the dialog needs to be dismissed after we're done operating.
** Formal Parameters:
**    XMWidget *button:
**      Pointer to the button that fired us off.
**    XtPointer user_d:
**      Actually a boolean which indicates if the we should dismiss the dialog
**      upon completion of the copy operation.
**    XtPointer call_d:
**      Call data that's ignored.
*/
void AttributeCopyDialog::OkPressed(XMWidget *buton, XtPointer user_d, 
				    XtPointer call_d)
{
  int     dmis    = (int)user_d;
  Boolean dismiss = (Boolean)dmis;

  /* Perform the copy operation. */

  CopyAttributes(WorkAreaContents);

  /* If necessary, dismiss the dialog: */

  if(dismiss) UnManage();

}

/*
** Functional Description:
**   CopyAttributes:
**     This function actually does the attribue copy.  We fetch the
**     selection list from the destination list.  We also get the attributes
**     block of the currently selected pane.  Then for each spectrum which
**     has a pane we copy the attributes into the requested pane.  The
**     requested pane is determined by decoding it from the front part
**     of the text string in the selection list.  This is assumed to be
**     of the form:
**       (px, py)  [snum]   sname
**     px     - Pane x coordinates.
**     py     - Pane y coordinates.
**     snum   - Number of the spectrum.
**     sname  - Name of the spectrum.
** Formal Parameters:
**    AttributeCopy *form:
**      Pointer to the form that contains the destination list.
*/
static void CopyAttributes(AttributeCopy *form)
{
  win_attributed *src = Xamine_GetSelectedDisplayAttributes();
  if(src == NULL) return;
  int srcspec = src->spectrum();

  char **spectrum_list;

  int spectrum_count = form->GetDestinationList(&spectrum_list);
  if(spectrum_count == 0) return;

  /* Copy each one... */
 
  for(int i = 0; i < spectrum_count; i++) {
    int row, col;
    sscanf(spectrum_list[i], "(%02d,%02d)", &col, &row);
    win_attributed *dst = Xamine_GetDisplayAttributes(row, col);
    if(dst != NULL) {
      int destspec = dst->spectrum();
      if(Xamine_IsCompatible(srcspec, destspec)) {
	form->CopyAttributes(src, dst);
	Xamine_RedrawPane(col, row);
      }
    }
  }
 
}

/*
** This page contains the code to respond to the copy attributes request.
*/

static AttributeCopyDialog *dialog = NULL;

/*
** Functional Description:
**   Xamine_CopyPaneAttrbutes:
**     This function is called in response to the attributes copy dialog.
**     If the dialog has not been instantiated, we instantiate it.
**     If it has been, then we just manage and update it.
** Formal Parameters:
**    XMWidget *parent:
**      The widget whos callback invoked us (probably a menu button, but
**      we ignore this anyway).
**    XtPointer ud:
**      User data.. we ignore this too.
**   XtPointer cd:
**      Call data, also ignored.
*/
void Xamine_CopyPaneAttributes(XMWidget *parent, XtPointer ud, XtPointer cd)
{
  win_attributed *atr = Xamine_GetSelectedDisplayAttributes();
  if(atr == NULL) {
    Xamine_error_msg(parent, "Selected spectrum is empty");
  }
  if(!dialog) {
    dialog = new AttributeCopyDialog("Copy_ATR", parent, "Copy Attributes",
				     atr);
  }
  dialog->Update(atr);
  dialog->Manage();
}
