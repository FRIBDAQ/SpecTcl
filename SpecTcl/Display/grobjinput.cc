/*
** Facility:
**   Xamine - NSCL Display program.
** Abstract:
**   grobjinput.cc
**     This file implements the non-trivial methods of the ObjectInput class.
** Author:
**    Ron FOx
**    NSCL
**    MIchigan State University
**    East Lansing, MI 48824-1321
*/

/*
** Include files:
*/
#include "XMManagers.h"
#include "XMDialogs.h"
#include "XMSeparators.h"
#include "grobjinput.h"

/*
** Functional Description:
**     ObjectInput:
**       This is the class constructor.  We invoke the anscestral constructors
**       and fill in the work area with a form, a separator and a text
**       field widget.
**       Management is left to someone else since they should first fill
**       in the client area form prior to managing the hierarchy.
** Formal Parameters:
**    XMWidget *parent:
**       Pointer to the parent widget object.
**    char *name:
**       Pointer to the widget name string.
**    char **help_text
**       Pointer to the help text list, NULL means no help available.
*/
ObjectInput::ObjectInput(XMWidget *parent, char *name, char **help_text) :
       GraphicalInput(parent, name, help_text)
{
  /* Instantiate the widgets:   */

  name_prompt = new XMTextField("Name_input", *work_area);
  name_label  = new XMLabel("Name_Label", *work_area, "Object Name");
  separator   = new XMSeparator("Separator", *work_area);
  point_prompts = new XMForm("Client_region", *work_area);

  /* Set up the client area form widget.  This consists of just pasting 
  ** the form to the upper part of the work area.
  */

  work_area->SetTopAttachment(*point_prompts,   XmATTACH_FORM);
  work_area->SetLeftAttachment(*point_prompts,  XmATTACH_FORM);
  work_area->SetRightAttachment(*point_prompts, XmATTACH_FORM);

  /*  Set up the separator.  The separator is set to horizontal orientation
  **  it is set to be a single line.
  **  It is then pasted to the work area below the form.
  */
  separator->SetOrientation(XmHORIZONTAL);
  separator->SetShadowType(XmSINGLE_LINE);
 
  work_area->SetTopAttachment(*separator,       XmATTACH_WIDGET);
  work_area->SetTopWidget(*separator,          *point_prompts);
  work_area->SetLeftAttachment(*separator,      XmATTACH_FORM);
  work_area->SetRightAttachment(*separator,     XmATTACH_FORM);

  /* The name label is pasted to the form just below the separator.
  ** The name prompt is pasted below it.
  */
  work_area->SetTopAttachment(*name_label,      XmATTACH_WIDGET);
  work_area->SetTopWidget(*name_label,         *separator);
  work_area->SetLeftAttachment(*name_label,     XmATTACH_FORM);
  work_area->SetRightAttachment(*name_label,    XmATTACH_FORM);

  work_area->SetTopAttachment(*name_prompt,     XmATTACH_WIDGET);
  work_area->SetTopWidget(*name_prompt,         *name_label);
  work_area->SetLeftAttachment(*name_prompt,    XmATTACH_FORM);
  work_area->SetRightAttachment(*name_prompt,   XmATTACH_FORM);
  work_area->SetBottomAttachment(*name_prompt,  XmATTACH_FORM);

}
