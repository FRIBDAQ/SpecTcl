/*
** Facility:
**   Xamine -- NSCL Display program.
** Abstract:
**   buttondlg.cc:
**     This file contains code to implement the classes which are used
**     to construct the client's button box.  The client's button box is
**     implemented as a top level shell containing a form into which the
**     client buttons are placed.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/

/*
** Include files:
*/

#include <string.h>

#include "XMManagers.h"
#include "XMPushbutton.h"
#include "XMShell.h"
#include "buttondlg.h"

#include <Xm/ScrolledW.h>
#include <Xm/MwmUtil.h>


/*
** The next pages implement methods in the ButtonBox class.  
** The ButtonBox class is the manager of all of the client buttons.
** It holds the button box top level shell widget, the form and the
** list of buttons which are inside it.
*/

/*
** Functional Description:
**   ButtonBox:
**      Constructor for the button box class.
**  Formal Parameters:
**     XMWidget *parent:
**        Parent widget.
**     String name:
**        Name attached to the shell (used for icon labelling too).
**     Cardinal row, col:
**        Number of rows and columns to create for the shell.
*/
ButtonBox::ButtonBox(XMWidget *parent, String name, Cardinal row, Cardinal col)
{

  /* First the easy stuff... save the number of rows and columns
  */
  columns = col;
  rows    = row;

  /* Now create the top level shell:  */

  Box = new TopLevelShell(name, parent);

  /* If the Motif window manager is running, then turn off the 
  ** menu box so that nobody can kill the widget.
  */

  if(XmIsMotifWMRunning(Box->getid())) {
    int decorations;
    Widget id = Box->getid();

    XtVaGetValues(id, XmNmwmDecorations, &decorations, NULL);
    decorations &= (MWM_DECOR_BORDER |
		    MWM_DECOR_RESIZEH |
		    MWM_DECOR_TITLE   |
		    MWM_DECOR_MAXIMIZE |
		    MWM_DECOR_MINIMIZE);
    XtVaSetValues(id, XmNmwmDecorations, decorations,
		    NULL);
  }

  XtVaSetValues(Box->getid(), XmNheight, row*20,
		    XmNwidth, col*60,
		    NULL);

  /* Set up the form to hold the buttons inside the shell: */

  ButtonLayout = new XMForm("ButtonLayout", *Box);
  ButtonLayout->SetFractionBase(rows*columns);

  /* Create the Button list  */

  Buttons = new pButton[row*columns];
  for(Cardinal i = 0; i < rows*columns; i++) {
    Buttons[i] = (Button *) NULL;
  }

  /* Make the shell visible: */

  ButtonLayout->Manage();
  Box->Realize();



}

/*
** Functional Description:
**   ButtonBox::~ButtonBox:
**      Destructor for the button box class.  We destroy the defined buttons,
**      the form and the top level shell widget.
*/
ButtonBox::~ButtonBox()
{
  /*
  ** Kill off the buttons in the button list.
  */
  for(Cardinal i = 0; i < rows*columns; i++) {
    if(Buttons[i] != (Button *)NULL) {
      delete Buttons[i];
    }
  }
  /* Next Kill off the form and the shell: */

  delete ButtonLayout;
  delete Box;

}

/*
** Functional Description:
**   ButtonBox::SetButton:
**     Adds a button to the button box. Since we're policy free we handle
**     redefinition by just unmanaging and overwriting the old
**     button.  This places responsibility on the client for fancy stuff.
**     This is all done
**     with everything unmanaged to allow the resize to occur.
** Formal Parameters:
**    Button *button:
**       Pointer to the button to add to the list.
**    Cardinal row, col:
**       Coordinates of the button in the form... these number from zero.
*/
void ButtonBox::SetButton(Button *button, Cardinal row, Cardinal col)
{
  if( (row < rows) && (col < columns)) {
    ButtonLayout->UnManage();	// Will remanage in a bit...
    
    /* Unmanage the buttons... */
    Cardinal i;
    for(i = 0; i < rows*columns; i++) { // This will also unmanage the
      if(Buttons[i] != (Button *)NULL) {	 // prior button if there was one.
	Buttons[i]->UnManage();
      }
    }
    /* Set the new button and manage everything: */
    
    XMButton *b = button->GetButton();
    Buttons[Index(row, col)] = button;
    ButtonLayout->SetLeftAttachment(*b, XmATTACH_POSITION);
    ButtonLayout->SetTopAttachment(*b, XmATTACH_POSITION);
    ButtonLayout->SetLeftPosition(*b, col*rows);
    ButtonLayout->SetTopPosition(*b, row*columns);
    
    for(i = 0; i < rows*columns; i++) {
      if(Buttons[i] != (Button *)NULL) {
	Buttons[i]->Manage();
      }
    }
    ButtonLayout->Manage();
  }
}

/*
** Functional Description:
**   ButtonBox::ClrButton:
**      This function removes a button from the button list.
**      Note that since we're policy independent, we don't second guess the
**      client's long term intentions for the button.  We just unmanage
**      it can remove it from the list.  The client must dispose of the
**      button themselves.
** Formal Parameters:
**   int row, int col:
**      Coordintates of the button in the button box.
** Returns:
**    Pointer to the button... this is NULL if there was no button defined.
*/
Button * ButtonBox::ClrButton(Cardinal row, Cardinal col)
{
  Button *b = GetButton(row, col);

  if(b != (Button *) NULL) {	// Needed since indices could have been out
    Buttons[Index(row, col)] = (Button *)NULL; // of range.
  }

  return b;
}

/*
** Functional Description:
**   ButtonBox::GetButton:
**      This function returns the pointer to a button in the button list.
** Formal Parameters:
**    Cardinal row, col:
**      The coordinates of the button in the button box button list.
** Returns:
**     Button pointer or NULL if the button is not defined or the indices are
**     out of range.
*/
Button * ButtonBox::GetButton(Cardinal row, Cardinal col)
{
  Button * b;

  if((row < rows) && (col < columns)) {
    b = Buttons[Index(row, col)];
  }
  else {
    b = (Button *) NULL;
  }
  return b;

}

/*
** Functional Description:
**    ButtonBox::EnableButton:
**      This function turns a button on which may or may not have been
**      disabled by the client.
** Formal Parameters:
**    Cardinal row, col:
**      Coordinates of the button to enable/disable.
*/
void ButtonBox::EnableButton(Cardinal row, Cardinal col)
{
  Button * b = GetButton(row, col); // Get the button pointer.

  if(b != (Button *)NULL) {	// Make sure the button exists too:
    b->Enable();		// Before trying to disable it.
  }

}
/*
** Functional Description:
**    ButtonBox::Disable:
**       Disable the selected button.
** Formal Parameters:
**   Cardinal r, c:
**      Coordinates of the button to affect.
*/
void ButtonBox::DisableButton(Cardinal row, Cardinal col)
{
  Button *b = GetButton(row, col);

  if(b != (Button *)NULL) {
    b->Disable();
  }
}

/*
**   Functions in the next few pages implement the methods of the Button
**   class.  The button class implements a generic base class for client
**   buttons.  The Button class is not intended to be directly instantiated.
**   Instead the user derives a class from it to generate a button with the
**   desired actual behavior on top of the behavior and abstractions offered
**   by the Button Class.
*/

/*
** Functional Description:
**   Button:Button:
**      This is the constructor for the base class.  We initialize the
**      code to a default value (0) and set the availability.  The
**      remainder of the work must be done by the subclass.
** Formal Parameters:
**    XMWidget *p:
**       The button's parent widget.
**    ButtonSensitivity a:
**       Button's initial sensitivity state.
*/
Button::Button(XMWidget *p, XtPointer userd) 
{
  parent = p;
  button = NULL;
  enabled= T;
}

/*
** Functional Description:
**    Button::~Button:
**       This method deletes a button.  We do this by calling the destructor
**       for the button widget.  This should cover the desired behavior
**       for most push buttons.
*/
Button::~Button()
{
  if(button != NULL)
    delete button;

}

/*
** Functional Description:
**   Button::SetLabel:
**     This function labels a button.  If the button is nonexistent, then
**     this is a no-op.
** Formal Parametrs:
**     ButtonTitle label:
**       The button's new label.
*/
void Button::SetLabel(ButtonTitle label)
{
  if(button != (XMButton *)NULL) {
    button->Label(label);
  }
}

/*
** Functional Description:
**    Button::GetLabel:
**      This function retrieves the label from a button.  If the button
**      does not exist, then an empty string is returned.
** Formal Parameters:
**    ButtonTitle label:
**      Buffer to hold the button label.
*/
void Button::GetLabel(ButtonTitle label)
{
  XmString cpnd;
  String   asciz;

  memset(label, 0, sizeof(ButtonTitle));


  if(button != (XMButton *)button) {
    
    
    button->GetAttribute(XmNlabelString, &cpnd);
    XmStringGetLtoR(cpnd, XmSTRING_DEFAULT_CHARSET, &asciz);
    XmStringFree(cpnd);
    strncpy(label, asciz, (sizeof(ButtonTitle) - 1));

  }
    
}

/*
** Functional Description:
**   Button::Enable:
**      Enable the button.  This means making the button sensitive to user
**      input.
**   Button::Disable:
**      Disable the button.  This make sthe button insensitive to user
**      input.
*/
void Button::Enable()
{
  if(button != (XMButton *)NULL) {
    button->Enable();
  }
  enabled = T;
}
void Button::Disable()
{
  if(button != (XMButton *)NULL) {
    button->Disable();
  }
  enabled = F;
}

/*
** Functional Description:
**   Button::Manage:
**      This method makes the button visible.
**   Button::UnManage:
**       This method makes the button invisible.
** Both methods are no-ops if the button is undefined.
*/
void Button::Manage()
{
  if(button != (XMButton *)NULL) {
    button->Manage();
  }
}
void Button::UnManage()
{
  if(button != (XMButton *)NULL) {
    button->UnManage();
  }
}

/*
** The functions on the following pages implement a push button widget
** class.
**   This is a button which accepts a single and has no state.  Again, 
** all of these classes are intended to be generalized for the application's
** use so the button press callback function is initially nuled out.
** The constructor provides a button callback user argument which is attached
** to the callback.
*/

/*
** Functional Description:
**   PushButton::PushButton:
**     The constructor of the pushbutton class. 
**     The base class initializer sets up the bulk of the stuff
**     We must, however, create a new instance of the XMPushButton widget
**     and register the callback for ButtonPresscb with the user's 
**     client data.
** Formal Parameters:
**    XMWidget *p:
**      The button's parent widget
**    XtPointer userd:
**      User client data passed in to the push button callback.
*/
PushButton::PushButton(XMWidget *p, XtPointer userd) :
   Button(p, userd) ,
   pushed(this)
{
  button = new XMPushButton("PushButton", *parent);
  pushed.Register(button, XmNactivateCallback, 
		 &PushButton::ButtonPresscb, userd);
}

/*
** Functional Description:
**   PushButton::~PushButton:
**     This destroys a pushbutton.  Main thing we need to do is
**     1) Unregister the callback.
**     2) Destroy the pushbutton widget.
*/
PushButton::~PushButton()
{
  pushed.UnRegister();
  delete button;
  button = NULL;
}


/*
** Functional Description:
**   PushButton::ButtonPresscb:
**     This is the callback which is activated on a button push.
**     The user is assumed to be abstracting the class to provide
**     actual behavior here.  We're just a placeholder and therefore
**     just return.
**  Formal Parameters:
**     XMWidget *wid:
**        Pointer to the widget object that fired us off.
**     XtPointer Userd:
**        User data (arbitrary)
**     XtPointer Buttond:
**        Button activation callback structure.
*/
void PushButton::ButtonPresscb(XMWidget *wid, XtPointer Userd, 
			       XtPointer Buttond)
{}


/*
** Functional Description:
**   ToggleButton::ToggleButton:
**     This method instantiates a toggle button widget with
**     encapsulated callback.  The callback function is the
**     virtual function ButtonPresscb which is intended to be
**     superceded by derived classes.
** Formal Parameters:
**   XMWidget *wid:
**     XMWidget *parent:
**       The parent widget of the toggle buton.
**     XtPointer calldata:
**       data which will be attached to the press callback and
**       passed in to the callback function.
*/
ToggleButton::ToggleButton(XMWidget *parent, XtPointer calldata) :
              Button(parent, calldata),
	      pushed(this)
{
  button = new XMToggleButton("ToggleButton", *parent);
  pushed.Register(button, XmNvalueChangedCallback,
		 &ToggleButton::ButtonPresscb, calldata);
  state  = F;
}

/*
** Functional Description:
**   ToggleButton::~ToggleButton:
**     Destroys an existing toggle button.
*/
ToggleButton::~ToggleButton()
{
  pushed.UnRegister();
  
}

/*
** Functgional descriptoin:
**   ToggleButton::GetState,
**   ToggleButton::SetState:
**      Set or retrieve the value of the state variable.
** Formal Parameters:
**    Logical new_state:
**      New state for the button for SetState.
** Returns:
**    GetState returns the current internal state of the button.
*/
void ToggleButton::SetState(Logical new_state)
{
  button->SetAttribute(XmNset, new_state);
  state = new_state;
}
Logical ToggleButton::GetState()
{
  return state;
}

/*
** Functional Description:
**   ToggleButton::ButtonPresscb:
**     This function is a virtual function which is intended to be overridden
**     by descendents of this class.  It is called when the togglel button's
**     state changes.
**       Default behavior is a no-op.
*/
void ToggleButton::ButtonPresscb(XMWidget *wid, XtPointer userd, 
				 XtPointer calld)
{
}
