/*
** Facility:
**   Xamine - NSCL display program widget support classes.
** Abstract:
**   checklist.cc
**     This file contains implementations of the checklist class methods.
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


#include "XMManagers.h"
#include "XMPushbutton.h"
#include "XMLabel.h"
#include "checklist.h"

/*
** Type definitions:
*/

typedef CheckElement *pCheckElement;

/*
** Functional Description:
**   CheckElement::CheckElement
**     This function is called to instantiate a check box element.
**     A check box element consists of a Form containing (in one horizontal
**     row) a toggle button (square like a check box), it's title and
**     an extra label.
** Formal Parameters:
**   char *name:
**     Name of the composite widget (used for form name).
**   XMWidget *parent:
**     Parent widget for the check box element (will parent the 
**     enclosing form).
**   Boolean state:
**     The initial state of the toggle button.  Defaults to False.
**   char *blabel:
**     The button label: Defaults to "Check"
**   char *labels:
**     The text string for the extra label, defaults to NULL in which
**     case no label will be set.
*/
CheckElement::CheckElement(char *name, XMWidget *parent, Boolean state,
			   char *blabel, char *labels) : 
       XMForm(name, *parent)
{

  /* Instantiate the subwidgets and set their properties. */

  button = new XMToggleButton("checkbox", id);
  button->Box();		/* Use square button. */
  button->SetState(state);
  button->Enable();
  button->Label(blabel);

  label  = new XMLabel("Label", id, labels);
  if(label) 
    label->SetLabel(labels);

  /* Now layout the widgets in the form:  */

  SetLeftAttachment(*button, XmATTACH_FORM);
  SetTopAttachment(*button,  XmATTACH_FORM);
  SetBottomAttachment(*button, XmATTACH_FORM);

  SetLeftAttachment(*label, XmATTACH_WIDGET);
  SetLeftWidget(*label, *button);
  SetTopAttachment(*label, XmATTACH_FORM);
  SetRightAttachment(*label, XmATTACH_FORM);
  SetBottomAttachment(*label, XmATTACH_FORM);
}

/*
** Functional Description:
**   CheckElement::~CheckElement:
**      Destructor for a check element.
*/
CheckElement::~CheckElement()
{
  delete button;
  delete label;

  /* Form is deleted by parent class destructor  */

}

/*
** Functional Description:
**   CheckElement::SetButtonString:
**     Modifies the button label string.
**   Formal Parameters:
**      char *newlabel:
**         Pointer to new label string.  Note that if this is null we'll
**         put in an empty string on the caller's behalf.
*/
void CheckElement::SetButtonString(char *newlabel)
{
  if(newlabel)
    button->Label(newlabel);
  else
    button->Label("");
}

/*
** Functional Description:
**   CheckElement::SetLabelString:
**     Set the label's label string.
** Formal Parameters:
**    char *newlabel:
**      The new label string.  If NULL, we provide a null string for the
**      caller.
*/
void CheckElement::SetLabelString(char *newlabel)
{
  if(newlabel) 
    label->SetLabel(newlabel);
  else
    label->SetLabel("");

}

/*
** Functional Description:
**   CheckElement::State:
**     Get/return the state of the toggle button
** Formal Parameters:
**    Boolean state:
**      New state.
** Returns:
**    Current state.
*/
void CheckElement::State(Boolean state)
{
  button->SetState(state);
}

Boolean CheckElement::State()
{
  return button->GetState();
}

/*
** Functional Description:
**   CheckElement::Manage:
**       Enable display of subwidgets and self.
*/
void CheckElement::Manage()
{
  button->Manage();
  label->Manage();
  XMForm::Manage();
}

/*
** Functional Description:
**   CheckElement::UnManage:
**      Disable geometry management/display of subwidgets and self.
*/
void  CheckElement::UnManage()
{
  button->UnManage();
  label->UnManage();
  XMForm::UnManage();
}

/*
** Functional Description:
**   CheckList::CheckList:
**     Constructor called when an instance of a CheckList is created.
**  Formal Parameters:
**    char *name:
**       Name of the checklist - applied to the XMForm that does geometry
**       management for the checklist.
**    XMWidget *parent:
**       Parent widget of the widget tree that's makes up a Checklist.
**    char *title:
**       The title string at the top of the form.
**    int button_count:
**       Number of entries in the checklist.
**    char **buttonstrings:
**       If non null, an array of button_count strings.  The non-null
**       entries are labels for corresponding buttons.
**    char **labelstrings:
**       If non null, an array of button_count strings.  The non null ones
**       are labels for the extra label strings in the button checklists.
*/
CheckList::CheckList(char *name,   XMWidget *parent,
		     char *title,  int button_count,
		     char **buttonstrings, char **labelstrings) :
       XMForm(name, *parent)
{
  nchecks = button_count;
  /*
  ** First create the title name and paste it up to the form.
  */
  XMWidget *above;
  if(title) {
    label = new XMLabel("Label", id, title);
  }
  else {
    label = new XMLabel("Label", id, "");
  }
  above = label;		/* Widget above current Check el't. in pasting */
  SetLeftAttachment(*label, XmATTACH_FORM);
  SetTopAttachment(*label, XmATTACH_FORM);
  SetRightAttachment(*label, XmATTACH_FORM);

  /* Now create all the push buttons:   */

  checks = new pCheckElement[nchecks];
  int button;
  for(button = 0; button < nchecks; button++) {
    char bname[32];
    sprintf(bname, "Button_%d", button);
    CheckElement *c = new CheckElement(bname, this);
    SetLeftAttachment(*c, XmATTACH_FORM);
    SetRightAttachment(*c, XmATTACH_FORM);
    SetTopAttachment(*c, XmATTACH_WIDGET);
    SetTopWidget(*c, *above);
    above = c;
    checks[button] = c;
  }
  SetBottomAttachment(*above, XmATTACH_FORM); /* Glue last to form at bottom */

  /*
  ** If there's a set of button strings, then label the buttons.
  */

  if(buttonstrings) {
    for(button = 0; button < nchecks; button++) {
      if(buttonstrings[button]) {
	checks[button]->SetButtonString(buttonstrings[button]);
      }
    }
  }

  /* If there's a set of label strings, then setup the labels. */

    if(labelstrings) {
      for(button = 0; button < nchecks; button++) {
	if(buttonstrings[button]) {
	  checks[button]->SetLabelString(labelstrings[button]);
	}
      }
    }
}
/*
** Functional Description:
**   CheckList::~CheckList:
**      Destroy an instance of a checklist.
*/
CheckList::~CheckList()
{
  delete label;
  for(int i = 0; i < nchecks; i++)
    delete checks[i];
}

/*
** Functional Description:
**   CheckList::SetButtonString:
**     Set the label string for a button.
** Formal Parameters:
**   int num:
**     Number of the button to set.
**   char *string:
**      New string.  If null, we supply a blank.
*/
void CheckList::SetButtonString(int num, char *string)
{
  /* Do nothing for out of range button number */

  if(num < 0) return;
  if(num >= nchecks) return;

  checks[num]->SetButtonString(string);
}
/*
** Functional Description:
**   CheckList::SetLabelString:
**     Set the string of the extra label in a given check element
** Formal Parameters:
**   int num:
**     The button number.
**   char *str:
**     New label string.
*/
void CheckList::SetLabelString(int num, char *str)
{
 /* Do nothing for out of range button number */

  if(num < 0) return;
  if(num >= nchecks) return;

  checks[num]->SetLabelString(str);

}

/*
** Functional Description:
**   CheckList::State:
**     Set/get the state of a button.
**  Formal Parameters:
**    int num:
**      Number of the button.
**    Boolean state:
**      New state of button.
** Returns:
**   Current button state.
*/
void CheckList::State(int num, Boolean state)
{
 /* Do nothing for out of range button number */

  if(num < 0) return;
  if(num >= nchecks) return;

  checks[num]->State(state);

}
Boolean CheckList::State(int num) /* Returns False for bad guys too. */
{
 /* Do nothing for out of range button number */

  if(num < 0) return False;
  if(num >= nchecks) return False;

  return checks[num]->State();
}

/*
** Functional Description:
**   Manage:
**     Manages the pieces of the composite widget:
*/
void CheckList::Manage()
{
  label->Manage();
  for(int i = 0; i < nchecks; i++) 
    checks[i]->Manage();

  XMForm::Manage();
}
void CheckList::UnManage()
{
  label->UnManage();
  for(int i = 0; i < nchecks; i++)
    checks[i]->UnManage();

   XMForm::UnManage();
}


/*
** Functional Description:
**    Disable:
**      Disable all checkboxes.
*/
void CheckList::Disable()
{
   for(int i = 0; i < nchecks; i++)
     checks[i]->Disable();
}
/*
** Functional Description:
**    Enable:
**      Enable all checkboxes.
*/
void CheckList::Enable()
{
   for(int i = 0; i < nchecks; i++)
     checks[i]->Enable();
}
