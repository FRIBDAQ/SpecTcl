/*
** Facility:
**   Xamine -- NSCL display program.
** Abstract:
**   lblfont.cc:
**     This file implements a dialog to select the font size used to label
**     graphical objects. 
**       The dialog is implemented as an object with a work area consisting
**     of a label which is written in the currently selected font.
**     Two arrow buttons, up and down allow you to increase and decrease the
**     font size.  Ghosting is done to make sure that you cannot run off
**     the range of legitimate fonts.
**        The action area of the dialog contains buttons OK, Apply, Cancel
**     and help.  These buttons operate on the setting selected as follows:
**       Apply   - Update all spectra in the current pane set using the
**                 new fonts for labels.
**       OK      - Same as apply, but first unmanage the dialog.
**       Cancel  - Don't make any changes.
**       Help    - Display a help dialog.
** Author:
**    Ron Fox
**    NSCL
**    Michigan State University
**    East Lansing, MI 48824-1321
*/

/*
** Include files:
*/
#include <stdio.h>

#include "XMDialogs.h"
#include "XMPushbutton.h"
#include "XMLabel.h"

#include "refreshctl.h"
#include "grobjdisplay.h"
#include "helpmenu.h"
#include "lblfont.h"
#include "dfltmgr.h"
#include "text.h"
/*
** Help text and help data structure for the widget help:
*/

static char *help_text[] = {
  "   This dialog box allows you to set the size of the graphical object label\n",
  "text.  The top area contains a text string and a pair of arrow buttons.\n",
  "Pressing the up arrow button increases the font size, while pressing the\n",
  "the down button decreases the font size.  The text string changes in size\n",
  "as you press buttons in order to show you what the resulting lables will\n",
  "look like.\n\n",
  "   When you have selected the font size you want, then you should click on\n",
  "one of the buttons in the bottom of the dialog.  These buttons have the\n",
  "usage:\n\n",
  "   OK    - Apply the change and dismiss the dialog.\n",
  "   APPLY - Apply the change and keep the dialog displayed\n",
  "   CANCEL- Do not make any changes, dismiss the dialog.\n",
  "   HELP  - Display this help text.\n",
  NULL
  };

static Xamine_help_client_data help = { "Font_HELP", NULL, help_text };

/*
** The class below implements the font selection chooser dialog.
*/

class FontChooser : public XMCustomDialog
{
 public:
  /*  Constructor and desctructor: */

  FontChooser(XMWidget *parent, char *name);
  ~FontChooser();

  /* Manipulators: */

  void Manage();

  void EnableUp(Boolean enable);	/* Enable up buton. */
  void EnableDn(Boolean enable);        /* Enable down button  */

  /*  Call back handlers: */

  void SizeUp();		/* Increase the font size. */
  void SizeDn();		/* Decrease the font size. */
  void SetFont(int newidx);	/* Set new font index      */

  void Okrtn();		        /* Accept button. */
  void Applyrtn();			/* OK button.     */
  void Cancelrtn() { UnManage(); } /* Cancel button  */
 protected:
  void SetGhosting();		/* Adjust up/down ghosting depending on state */
 private:
  /* State variables. */

  int  font_index;
  int  max_fontidx;

  /* Subwidget  pointers */

  XMArrowButton *up, *down;
  XMLabel       *label;

  Callback_data *okcb, *applycb, *cancelcb, *helpcb;
  Callback_data *upcb, *downcb;

};

/*
**  Below we have a font chooser pointer which will hold the chooser
**  used to run the dialog to set the label font size.
*/
static FontChooser *dialog = NULL;

/*
** The functions on this page relay Motif callbacks to method functions of
** the FontChooser class.  All of these functions have the following
** Formal Parameters:
**   XMWidget *button:
**     The widget pointer for the button that was pushed.
**   XtPointer user_data:
**     The use client data to the callback.  In all cases this is a pointer
**     to the instance of the FontChooser to which the callback belongs.
**  XtPointer call_data:
**     Mostly unused.  This contains Motif callback data from the button
**     callback.
*/

void Ok_Relay(XMWidget *button,
	     XtPointer user_data, /* Relay the Ok button. */
	     XtPointer call_data)
{
  FontChooser *fc = (FontChooser *)user_data;
  fc->Okrtn();
}

void Apply_Relay(XMWidget *button,
		 XtPointer user_data, /* Relay apply button. */
		 XtPointer call_data)
{
  FontChooser *fc = (FontChooser *)user_data;
  fc->Applyrtn();
}

void Cancel_Relay(XMWidget *button,
		  XtPointer user_data, /* Relay cancel button. */
		  XtPointer call_data)
{
  FontChooser *fc = (FontChooser *)user_data;
  fc->Cancelrtn();
}

void Up_Relay(XMWidget *button,
	      XtPointer user_data, /* UP arrow button. */
	      XtPointer call_data)
{
  FontChooser *fc = (FontChooser *)user_data;
  fc->SizeUp();
}

void Dn_Relay(XMWidget *button,
	      XtPointer user_data, /* Down button. */
	      XtPointer call_data)
{
  FontChooser *fc = (FontChooser *)user_data;
  fc->SizeDn();
}


/*
** Functional Description:
**    FontChooser::FontChooser:
**       This function is a constructor for the font chooser dialog.
**       We need to build a font list for the label widget, and layout
**       all of the children widgets in the form of the custom dialog
** Formal Parameters:
**    XMWidget *parent:
**       Widget which should parent the dialog shell widget of the
**       custom dialog.
**    char *title:
**       The title string at the top of the widget... also used as the widget 
**       name.
*/
FontChooser::FontChooser(XMWidget *parent, char *title) :
       XMCustomDialog(title, *parent, title)
{
  Display *d  = XtDisplay(parent->getid());

  /* Set the font index and max_fontidx member elements: */

  font_index = Xamine_GetObjectLabelIndex(d);
  max_fontidx= Xamine_GetFontCount(d);

  /* Now build the font list. We'll need it for the label widget.     */


  char fname[80];		/* Font alias names. */
  XmFontList flist =
    XmFontListCreate(Xamine_GetFontByIndex(d, 0),
		     "Font0");
  for(int i = 1; i < max_fontidx; i++) {
    sprintf(fname, "Font%d", i);
    flist = 
      XmFontListAdd(flist, 
		    Xamine_GetFontByIndex(d, i),
		    fname);
  }
  /* Create the subwidgets of the work area form.    */

  Arg  arglist[2];

  XtSetArg(arglist[0], XmNfontList, flist);

  up     = new XMArrowButton("Size_UP", *work_area);
  down   = new XMArrowButton("Size_DN", *work_area);
  label  = new XMLabel("Label", *work_area, 
		       "Sample Label",
		       arglist, 1); /* The arglist sets the list of fonts. */
  
  /* Paste the widgets to the work area.  You might wonder why we don't  
  ** attempt to set the label font at this time.  The reason is that
  ** the widget will not be visible until managed.  The widget might be cached
  ** and pre-created, so it makes sense to get the final font right for the
  ** label at management time and then to maintain it using the arrow keys
  ** while the widgets are visible.
  */
  work_area->SetFractionBase(100); /* Work in pct of display. */


  /* Label is attached to the form on the left top and bottom and
  ** then it extends to 80% of the form
  */
  work_area->SetTopAttachment(*label, XmATTACH_FORM);
  work_area->SetLeftAttachment(*label, XmATTACH_FORM);
  work_area->SetBottomAttachment(*label, XmATTACH_FORM);
  work_area->SetRightAttachment(*label, XmATTACH_POSITION);
  work_area->SetRightPosition(*label, 80);
 

  work_area->SetLeftAttachment(*up, XmATTACH_POSITION);
  work_area->SetLeftPosition(*up,   85);
  work_area->SetBottomAttachment(*up, XmATTACH_POSITION);
  work_area->SetBottomPosition(*up, 48);
  up->PointUp();		/* Point the up arrow up. */

  work_area->SetLeftAttachment(*down, XmATTACH_POSITION);
  work_area->SetLeftPosition(*down, 85);
  work_area->SetTopAttachment(*down, XmATTACH_POSITION);
  work_area->SetTopPosition(*down, 52);
  down->PointDown();		/* Point the down arrow down. */


  /* Now we need to set up the callbacks:  */

  okcb = AddOkCallback(Ok_Relay, (XtPointer)this);
  applycb = AddApplyCallback(Apply_Relay, (XtPointer)this);
  cancelcb = AddCancelCallback(Cancel_Relay, (XtPointer)this);
  helpcb = AddHelpCallback(Xamine_display_help, (XtPointer)&::help);

  upcb = up->AddCallback(Up_Relay, this);
  downcb = down->AddCallback(Dn_Relay, this);
}

/*
** Functional Description:
**   ~FontChooser:
**      This function is a destructor for the font chooser dialog class.
*/
FontChooser::~FontChooser()
{
  delete upcb;
  delete downcb;

  delete okcb;
  delete applycb;
  delete cancelcb;
  delete helpcb;

  delete down;
  delete up;
  delete label;

}

/*
** Functional Description:
**   FontChooser::Manage:
**     This method manages the font chooser dialog.  Besides the simple
**     work of managing children and dialog, we also get the current
**     font label index and rewrite the label text using it.
*/
void FontChooser::Manage()
{
  int f = Xamine_GetObjectLabelIndex(XtDisplay(label->getid()));
  SetFont(f);


  /* Now manage the widget tree:  */

  up->Manage();
  down->Manage();
  label->Manage();
  XMCustomDialog::Manage();
  

}

/*
** Functional Description:
**   FontChooser::EnableUp
**   FontChooser::EnableDn
** 
**     Enables or disables the up and down arrow buttons.
** Formal Parameters:
**
**    Boolean enable:
**      True to enable the buttons, False to disable them.
*/
void FontChooser::EnableUp(Boolean enable)
{
  if(enable) {
    up->Enable();
  }
  else {
    up->Disable();
  }
}

void FontChooser::EnableDn(Boolean enable)
{
  if(enable) {
    down->Enable();
  }
  else {
    down->Disable();
  }
}

/*
** Functional Description:
**   FontChooser::SetFont
**      Sets the absolute index of the font.  No change is made if the 
**      new index is illegal.  Up/Down arrow ghosting is adjusted.
** Formal Parameters:
**    int newidx:
**      New font index.
*/
void FontChooser::SetFont(int newidx)
{
  if( (newidx >= 0) && (newidx < max_fontidx)) {
    char font[20];		/* Font nameid. */
    XmString labelstring;

    font_index = newidx;
    sprintf(font, "Font%d", newidx);
    labelstring = XmStringCreateLtoR("Sample Label", font);
    label->SetLabel(labelstring);
    XmStringFree(labelstring);
  }
 
  SetGhosting();		/* Adjust arrow ghosting. */
}

/*
** Functional Description:
**   FontChooser::SizeUp:
**   FontChooser::SizeDn:
**     Increase/Decrease the size of the label font one notch.
**     ASSUMPTION:   The largest font has smallest index.
*/
void FontChooser::SizeUp()
{
  SetFont(font_index-1);

}
void FontChooser::SizeDn()
{
  SetFont(font_index+1);

}

/*
** Functional Description:
**  FontChooser::Ok:
**    This method is called in response to the Ok button being pressed.
*/
void FontChooser::Okrtn()
{
  Applyrtn();			/* Apply the current choice */
  UnManage();			/* Pop down the dialog.     */
}

/*
** Functional Description:
**   FontChooser::Apply:
**      Called to apply the current font_index to the 
**      graphical object labels. An update all is  forced... 
*/
void FontChooser::Applyrtn()
{
  Xamine_SetObjectLabelIndex(XtDisplay(label->getid()),
			     font_index);

  Xamine_UpdateAll(this, NULL, NULL); /* Update all spectra. */

  /* Set the defaults:   */

  Xamine_SaveDefaultProperties();

}

/*
** Functional Description:
**   FontChooser::SetGhosting:
**     This function sets the ghosting of the arrow buttons appropriately
**     to prevent the user from selecting fonts that are outside the legitimate
**     font range.
*/
void FontChooser::SetGhosting()
{
  EnableUp((font_index != 0));
  EnableDn(((font_index+1) != max_fontidx));
}

/*
** Functional Description:
**   Xamine_SetGrobjLblFont:
**     This function is called to invoke a dialog to set the 
**     Graphical object font size.  The dialog is cached for future use.
**     This function is assumed to be invoked from a menu or quick button.
** Formal Parameters:
**   XMWidget *w:
**     Widget Id of the invoker.
**  XtPointer user_data:
**     unused user data.
**  XtPointer call_data:
**     Unused callback data.
*/
void Xamine_SetGrobjLblFont(XMWidget *w, XtPointer ud, XtPointer cd)
{
  if(!dialog) {
    dialog = new FontChooser(w, "Object_Fonts");
  }

  dialog->Manage();
}
