/*
** Facility:
**   Xamine -- NSCL display program.
** Abstract:
**   select2.cc:
**     This file contains implementations for the Select2 widget class.
**     Select2 is a widget which prompts for two values. Each value has a label
**     and a delete button.  The dialog has the standard Ok, Apply, Cancel and
**     Help buttons.  The user supplies the semantics of the box via callbacks.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/
static char *sccsinfo = "@(#)select2.cc	8.1 6/23/95 ";

/*
** Include files:
*/
#include <stdio.h>

#include "XMWidget.h"
#include "XMDialogs.h"
#include "XMLabel.h"
#include "XMText.h"
#include "XMPushbutton.h"
#include "helpmenu.h"
#include "select2.h"

/*
** Functional Description:
**   Xamine_Select2CommonCallback:
**      This function is the common callback function for the Select2
**      dialog.  It receives a callback descriptor record and, if the
**      calback function is not void, calls it passing in the user data field.
** Formal Parameters:
**    XMWidget *w:
**       The actual widget within the dialog that caused us to be called
**       (ignored).
**    XtPointer user_data:
**        The pointer to the call back description record.
**    XtPointer call_data:
**        Callback specific call data (passed on to the user callback).
*/
void Xamine_Select2CommonCallback(XMWidget *w, XtPointer user_data, 
				  XtPointer call_data)
{
  Select2_CallbackStruct *cbs = (Select2_CallbackStruct *)user_data;

  if(cbs->callback) {		/* Relay the callback if defined.  */
    cbs->callback(cbs->thewid, cbs->user_data, call_data);
  }
}

/*
** Functional Description:
**    Xamine_Select2SetValue:
**       This function takes a text widget and preloads it with
**       a numerical value.  Text widgets normally contain character
**       string information so we do the text conversion here.
** Formal Parameters:
**   XMTextField *t:
**      Pointer to a text field widget.
**   unsigned int v:
**      Value to set.
*/
void Xamine_Select2SetValue(XMTextField *t, unsigned int v)
{
  char buf[20];
  sprintf(buf, "%u", v);
  t->SetText(buf);
}
/*
** Functional Description:
**   Xamine_Select2SetValue2:
**     Set two space separated integers as the value of the
**     associated text field.
** Formal Parameters:
**   XMTextField *t:
**      Pointer to the text field widget.
**   unsigned int x,y:
**      The values to set.
*/
void Xamine_Select2SetValue2(XMTextField *t, unsigned  x, 
			     unsigned  y)
{
  char txt[20];			/* Text buffer for encoded data. */

  sprintf(txt, "%d %d", x,y);
  t->SetText(txt);
}

/*
** Functional Description:
**   Xamine_Select2GetValue:
**      This function is used by the GetPointx methods.  It takes a text
**      widget, gets the string it contains and converts it to an
**      integer value.  If the string results in a negative number or
**      is not convertable, the result will be -1 otherwise the result will
**      be zero or a positive number.
** Formal Parameters:
**     XMTextField *t:
**        The text widget to get the value from .
*/
int Xamine_Select2GetValue(XMTextField *t)
{
  char *text_value;
  int  scanf_status;
  int  value;

  /* Get the text value and convert it. */

  text_value = t->GetText();
  scanf_status = sscanf(text_value, "%d", &value);
 
  /* Only you can prevent memory leaks... return the memory to Xt */

  XtFree(text_value);

  /* Adjust the status for errors and negative values */

  if( (scanf_status != 1) || (value < 0)) value = -1;
  return value;
}

/*
** Functional Description:
**   Xamine_Select2Get2Values:
**     This function gets a pair of values from an input text field.
**     The values are integers greater than zero.
** Formal Parameters:
**    XMTextField *t:
**      Text field to get the values from
**    int *x, *y:
**      Pointers to buffers to hold the values.
** Returns:
**   True  - Success.
**   False - Failure
*/
int Xamine_Select2Get2Values(XMTextField *t, int *x, int *y)
{
  char *text_value;
  int  scanf_status;
  int  value;

  /* Get the text value and convert it. */

  text_value = t->GetText();
  scanf_status = sscanf(text_value, "%d %d", x,y);
 
  /* Only you can prevent memory leaks... return the memory to Xt */

  XtFree(text_value);

  /* Adjust the status for errors and negative values */

  if( (scanf_status != 2))
    return False;
  if((*x < 0) || (*y < 0)) 
    return False;
  return True;

}

/*
** Functional Description:
**   Xamine_Select2::Create:
**      This method contains the common creation code which is invoked by
**      constructors after the basic XMCustomDialog constructor has been
**      invoked.  This function fills in the action area adds an optional
**      help callback and sets up the initial management except for the final
**      outermost widget (left for the Manage, UnManage methods to deal with).
** Formal Parameters:
**    XMForm &work_area
**     The form which is the parent of this widget.
*/
void Xamine_Select2::Create(XMForm &work_area)
{
  /*
  ** Fill in the Select2_CallbackStruct structures to indicate that there's
  ** no callback.
  */
  Point1.thewid   = pthis;
  Point1.callback = NULL;
  Point1.user_data= NULL;

  Point2.thewid   = pthis;
  Point2.callback = NULL;
  Point2.user_data= NULL;

  Delete1.thewid    = pthis;
  Delete1.callback  = NULL;
  Delete1.user_data = NULL;

  Delete2.thewid    = pthis;
  Delete2.callback  = NULL;
  Delete2.user_data = NULL;

  /*
  ** Fill in th work_area form with our widgets. 
  */
  work_area.SetFractionBase(3);
      /* First build the label column... use default labels for now: */

  pt1_label = new XMLabel("Point1_Label", work_area, "Limit 1 : ");
  work_area.SetTopAttachment(*pt1_label,      XmATTACH_FORM);
  work_area.SetLeftAttachment(*pt1_label,     XmATTACH_FORM);
  work_area.SetRightAttachment(*pt1_label,    XmATTACH_POSITION);
  work_area.SetRightPosition(*pt1_label,      1);

  pt2_label = new XMLabel("Point2_Label", work_area, "Limit 2 : ");
  work_area.SetTopAttachment(*pt2_label,      XmATTACH_WIDGET);
  work_area.SetTopWidget(*pt2_label,          *pt1_label);
  work_area.SetLeftAttachment(*pt2_label,     XmATTACH_FORM);
  work_area.SetBottomAttachment(*pt2_label,    XmATTACH_FORM);
  work_area.SetRightAttachment(*pt2_label,    XmATTACH_POSITION);
  work_area.SetRightPosition(*pt2_label,      1);

  /* Next build the input text field region column. */

  pt1_value = new XMTextField("Point1", work_area, 6);
  work_area.SetTopAttachment(*pt1_value,      XmATTACH_FORM);
  work_area.SetLeftAttachment(*pt1_value,     XmATTACH_POSITION);
  work_area.SetLeftPosition(*pt1_value,       1);
  work_area.SetRightAttachment(*pt1_value,    XmATTACH_POSITION);
  work_area.SetRightPosition(*pt1_value,      2);

  pt2_value = new XMTextField("Point2", work_area, 6);
  work_area.SetTopAttachment(*pt2_value,      XmATTACH_WIDGET);
  work_area.SetTopWidget(*pt2_value,          *pt1_value);
  work_area.SetLeftAttachment(*pt2_value,     XmATTACH_POSITION);
  work_area.SetLeftPosition(*pt2_value,       1);
  work_area.SetBottomAttachment(*pt2_value,    XmATTACH_FORM);
  work_area.SetRightAttachment(*pt2_value,    XmATTACH_POSITION);
  work_area.SetRightPosition(*pt2_value,      2);

  /* Set up the activation callbacks for the text regions: */

  pt1_value->AddActivateCallback(Xamine_Select2CommonCallback,
				 &Point1);
  pt1_value->AddCallback(XmNlosingFocusCallback,
			 Xamine_Select2CommonCallback,
			 &Point1);
  pt2_value->AddActivateCallback(Xamine_Select2CommonCallback,
				 &Point2);
  pt2_value->AddCallback(XmNlosingFocusCallback,
			 Xamine_Select2CommonCallback,
			 &Point2);

  /* Build the Delete button column.   */

  pt1_delete = new XMPushButton("Delete1", work_area, 
				Xamine_Select2CommonCallback,
				&Delete1);
  work_area.SetTopAttachment(*pt1_delete,      XmATTACH_FORM);
  work_area.SetLeftAttachment(*pt1_delete,     XmATTACH_POSITION);
  work_area.SetLeftPosition(*pt1_delete,       2);
  work_area.SetRightAttachment(*pt1_delete,    XmATTACH_FORM);

  pt2_delete = new XMPushButton("Delete2", work_area,
				Xamine_Select2CommonCallback,
				&Delete2);
  work_area.SetTopAttachment(*pt2_delete,      XmATTACH_WIDGET);
  work_area.SetTopWidget(*pt2_delete,          *pt1_delete);
  work_area.SetLeftAttachment(*pt2_delete,     XmATTACH_POSITION);
  work_area.SetLeftPosition(*pt2_delete,       2);
  work_area.SetBottomAttachment(*pt2_delete,   XmATTACH_FORM);
  work_area.SetRightAttachment(*pt2_delete,    XmATTACH_FORM);

  /* Manage the work area pieces in preparation to manage the whole shooting
  ** match:
  */


  work_area.Manage();


}

/*
** Functional Description:
**   Xamine_Select2::~Xamine_Select2
**     This function is a destructor for the Xamine_Select2 class.
**     All widgets that we created get destroyed.
*/
Xamine_Select2::~Xamine_Select2()
{
  delete pt1_label;
  delete pt1_value;
  delete pt1_delete;
 
  delete pt2_label;
  delete pt2_value;
  delete pt2_delete;
}
