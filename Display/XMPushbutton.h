/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2008

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/
/*
** Facility:
**   Xamine MOTIF support for C++
** Abstract:
**   XMPushbutton.h   - This include file defines the PushButton class
**                      as a specialization of the ManagedWidget class.
**                      The Pushbutton widget is intended as a base class
**                      for various AppXXPushbuttons.  It includes:
**                       A method which is called when the button is pushed.
**                       Methods for enabling and disabling the button.
**                       A method for changing the label on the button.
** Author:
**    Ron Fox
**    NSCL
**    Michigan State University
**    East Lansing, MI 48824-1321
** Revision:
**     @(#)XMPushbutton.h	8.1 6/23/95 
*/

#ifndef XMPUSHBUTTON_H
#define XMPUSHBUTTON_H

#include <Xm/PushB.h>
#include <Xm/CascadeB.h>
#include <Xm/ToggleB.h>
#include <Xm/ArrowB.h>

#ifndef XMWIDGET_H
#include "XMWidget.h"
#endif

/*
** XMButton is the base class from which all Motif button types are
** derived.  There is no such thing in Motif however.
*/
class XMButton : public XMManagedWidget
{
 public:
  /* Turn button on/of. */

  virtual void Enable();
  virtual void Disable();

  /* Set label values... virtual to support button with */
  /* PIXMAP subtype                                     */

  virtual void Label(XmString label);
  virtual void Label(const char* label);
		      
  /* Set the button trigger mnemonic: */
  
  virtual void SetMnemonic(KeySym k);
  
  /* Constructors: */
  
  XMButton(const char *n, WidgetClass c, Widget parent);
  XMButton(const char *n, WidgetClass c, XMWidget &parent);
  XMButton(Widget w);
  
  void SetAccelerator(const char *translation, const char *prompt);
};

/*
** The push button class is an ordinary push button to activate a function
*/
class XMPushButton : public XMButton
{
private:
  void Initialize( void (*cb)(XMWidget *, XtPointer, XtPointer),
		   XtPointer cd );
 public:
  /* Constructors... make the widget */
  
  XMPushButton(const char *n, Widget parent, 
	       void (*cb)(XMWidget *, XtPointer, XtPointer)
	       = NULL,
	       XtPointer cd = NULL);
  XMPushButton(const char *n, XMWidget &parent, 
	       void (*cb)(XMWidget *, XtPointer, XtPointer)
	       = NULL,
	       XtPointer cd=NULL);
  XMPushButton(Widget w);
  /* Add additional call backs: */
  
  Callback_data *AddCallback(void (*cb)(XMWidget *, 
					XtPointer, 
					XtPointer) = NULL,
			     XtPointer cd = NULL);
};

/* This derivation of Cascade button supports pull right menuing via
** additional methods.
*/

class XMCascadeButton : public XMButton
{
private:
  void Initialize(void (*cb)(XMWidget *, 
			     XtPointer, 
			     XtPointer) ,
		  XtPointer cd);
 public:
  /* Describe associated cascaded menus: */
  
  void SetAssociatedMenu(XMWidget &w);
  void SetAssociatedMenu(Widget w);
  
  /* Set the keystroke  trigger mnemonic */
  
  
  /* Constructors... make the widget */
  
  XMCascadeButton(const char *n, Widget parent, 
		  void (*cb)(XMWidget *, XtPointer, XtPointer)
		  = NULL,
		  XtPointer cd = NULL);
  XMCascadeButton(const char *n, XMWidget &parent, 
		  void (*cb)(XMWidget *, XtPointer, XtPointer)
		  = NULL,
		  XtPointer cd=NULL);
  XMCascadeButton(Widget w);
  
  /* Add additional call backs: */
  
  Callback_data *AddCallback(void (*cb)(XMWidget *, 
					XtPointer, 
					XtPointer) = NULL,
			     XtPointer cd = NULL);
  
};
/*
** A toggle button includes an ON/OFF indicator.
*/
class XMToggleButton: public XMButton
{
private:
  void
  Initialize(void (*cb)(XMWidget *, XtPointer, XtPointer) ,
			    XtPointer cd);
 public:
  /* Constructors: */
  
  XMToggleButton(const char *n, Widget parent,
		 void (*cb)(XMWidget *, XtPointer, XtPointer) = NULL,
		 XtPointer cd = NULL);
  XMToggleButton(const char *n, XMWidget &parent,
		 void (*cb)(XMWidget *, XtPointer, XtPointer) = NULL,
		 XtPointer cd = NULL);
  XMToggleButton(Widget w);
  
  /* Specialized toggle button methods */
  
  
  /* Add additional call backs: */
  
  Callback_data *AddCallback(void (*cb)(XMWidget *, 
					XtPointer, 
					XtPointer) = NULL,
			     XtPointer cd = NULL);
  
  void ShowIndicator(); /* Display the toggle */
  void HideIndicator(); /* Hide the toggle */
  void Diamond();      /* Set indicator to diamond */
  void Box(); /* Set toggle indicator to box. */
  void SetState(Boolean state); 
  void Set();          /* Set toggle to true. */
  void UnSet();        /* Set toggle to false */
  Boolean GetState();  /* Get toggle state */
};

/*
** An arrow button is a button with an arrow that points left right up or down
** The arrow is the only label allowed so we override the labelling functions
** as well as a few other inappropriate functions.
*/
class XMArrowButton : public XMButton
{
private:
  void Initialize(void (*cb)(XMWidget *, XtPointer, XtPointer) ,
		   XtPointer cd );
 public:
  /* Constructors/destructors */

  XMArrowButton(const char *n, Widget parent, 
		void (*cb)(XMWidget *, XtPointer, XtPointer) = NULL,
		XtPointer cd = NULL);
  XMArrowButton(const char *n, XMWidget &parent,
		void (*cb)(XMWidget *, XtPointer, XtPointer) = NULL,
		XtPointer cd = NULL);

  /* Additional manipulators: */

  Callback_data *AddCallback(void (*cb)(XMWidget *, XtPointer, XtPointer)=NULL,
			     XtPointer cd = NULL);
  void PointLeft();
  void PointRight();
  void PointUp();
  void PointDown();

  unsigned char Direction();

  /* Overrides of virtual functions in the base class */

  virtual void Label(XmString label);
  virtual void Label(const char*  label);
  virtual void SetMnemonic(KeySym k);
};
#endif
