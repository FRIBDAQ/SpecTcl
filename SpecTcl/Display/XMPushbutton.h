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

#ifndef XMPushbutton_H
#define XMPushbutton_H

#include <Xm/PushB.h>
#include <Xm/CascadeB.h>
#include <Xm/ToggleB.h>
#include <Xm/ArrowB.h>
#include "XMWidget.h"

/*
** XMButton is the base class from which all Motif button types are
** derived.  There is no such thing in Motif however.
*/
class XMButton : public XMManagedWidget
                   {
		   public:
		     /* Turn button on/of. */

		     virtual void Enable()
		       { 
			 SetAttribute(XmNsensitive, (XtArgVal)True); 
		       }
		     virtual void Disable()
		       {
			 SetAttribute(XmNsensitive, (XtArgVal)False); 
		       }

		     /* Set label values... virtual to support button with */
		     /* PIXMAP subtype                                     */

		     virtual void Label(XmString label)
		       {
			 SetAttribute(XmNlabelString, label);
		       }
		     virtual void Label(String label)
		       {
			 XmString s;
			 s = XmStringCreateLtoR(label, 
						XmSTRING_DEFAULT_CHARSET);
			 Label(s);
			 XmStringFree(s);
		       }
		     /* Set the button trigger mnemonic: */

		     virtual void SetMnemonic(KeySym k)
		       {
			 SetAttribute(XmNmnemonic, k);
		       }

		     /* Constructors: */

		     XMButton(char *n, WidgetClass c, Widget parent)
		       : XMManagedWidget(n, c, parent)
			 {}
		     XMButton(char *n, WidgetClass c, XMWidget &parent)
		       : XMManagedWidget(n, c, parent)
			 {}
		     XMButton(Widget w) : XMManagedWidget(w)
		       {}

		     void SetAccelerator(char *translation, char *prompt)
		       {
			 XmString acctext = XmStringCreateSimple(prompt);
			 XtVaSetValues(id,
				       XmNaccelerator, translation,
				       XmNacceleratorText, acctext,
				       NULL);
			 XmStringFree(acctext);
		       }
		   };

/*
** The push button class is an ordinary push button to activate a function
*/
class XMPushButton : public XMButton
                   {
                   public:
		     /* Constructors... make the widget */

		     XMPushButton(char *n, Widget parent, 
				  void (*cb)(XMWidget *, XtPointer, XtPointer)
				  = NULL,
				  XtPointer cd = NULL):
		       XMButton(n, xmPushButtonWidgetClass, parent)
		       {
			 Enable();
			 if(cb)
			   XMWidget::AddCallback(XmNactivateCallback, cb, cd);
		       }
		     XMPushButton(char *n, XMWidget &parent, 
				  void (*cb)(XMWidget *, XtPointer, XtPointer)
				  = NULL,
				  XtPointer cd=NULL):
			      XMButton(n, xmPushButtonWidgetClass, parent)
		       {
			 Enable();
			 if(cb)
			   XMWidget::AddCallback(XmNactivateCallback, cb, cd);
		       }
		     XMPushButton(Widget w) : XMButton(w) {}
		     /* Add additional call backs: */
		   
		     Callback_data *AddCallback(void (*cb)(XMWidget *, 
					    XtPointer, 
					    XtPointer) = NULL,
				 XtPointer cd = NULL)
		       { return XMWidget::AddCallback(XmNactivateCallback, cb, cd); }
		      
		   };

/* This derivation of Cascade button supports pull right menuing via
** additional methods.
*/

class XMCascadeButton : public XMButton
                   {
		   public:
		     /* Describe associated cascaded menus: */
		     
		     void SetAssociatedMenu(XMWidget &w)
		       {
			 SetAttribute(XmNsubMenuId, w.getid());
		       }
		     void SetAssociatedMenu(Widget w)
		       {
			 SetAttribute(XmNsubMenuId, w);
		       }
		     
		     /* Set the keystroke  trigger mnemonic */

	     
		     /* Constructors... make the widget */

		     XMCascadeButton(char *n, Widget parent, 
				  void (*cb)(XMWidget *, XtPointer, XtPointer)
				  = NULL,
				  XtPointer cd = NULL):
		       XMButton(n, xmCascadeButtonWidgetClass, parent)
		       {
			 Enable();
			 if(cb)
			   XMWidget::AddCallback(XmNactivateCallback, cb, cd);
		       }
		     XMCascadeButton(char *n, XMWidget &parent, 
				  void (*cb)(XMWidget *, XtPointer, XtPointer)
				  = NULL,
				  XtPointer cd=NULL):
		       XMButton(n, xmCascadeButtonWidgetClass, parent)
		       {
			 Enable();
			 if(cb)
			   XMWidget::AddCallback(XmNactivateCallback, cb, cd);
		       }
		     XMCascadeButton(Widget w) : XMButton(w) {}
		    
		     /* Add additional call backs: */
		   
		     Callback_data *AddCallback(void (*cb)(XMWidget *, 
					    XtPointer, 
					    XtPointer) = NULL,
				 XtPointer cd = NULL)
		       { return XMWidget::AddCallback(XmNactivateCallback, 
						      cb, cd); }

		   };
/*
** A toggle button includes an ON/OFF indicator.
*/
class XMToggleButton: public XMButton
                    {
		    public:
		      /* Constructors: */

		      XMToggleButton(char *n, Widget parent,
		           void (*cb)(XMWidget *, XtPointer, XtPointer) = NULL,
				     XtPointer cd = NULL) :
			  XMButton(n, xmToggleButtonWidgetClass, parent)
		       {
			 Enable();
			 if(cb)
			   XMWidget::AddCallback(XmNvalueChangedCallback, 
						 cb, cd);
		       }
		      XMToggleButton(char *n, XMWidget &parent,
		           void (*cb)(XMWidget *, XtPointer, XtPointer) = NULL,
				     XtPointer cd = NULL) :
			  XMButton(n, xmToggleButtonWidgetClass, parent)
		       {
			 Enable();
			 if(cb)
			   XMWidget::AddCallback(XmNvalueChangedCallback, cb, cd);
		       }
		      XMToggleButton(Widget w) : XMButton(w) {}

			/* Specialized toggle button methods */

		    
		     /* Add additional call backs: */
		   
		      Callback_data *AddCallback(void (*cb)(XMWidget *, 
					    XtPointer, 
					    XtPointer) = NULL,
				 XtPointer cd = NULL)
			{ return XMWidget::AddCallback(XmNvalueChangedCallback,
						       cb, cd); }

		      void ShowIndicator() /* Display the toggle */
			{ SetAttribute(XmNindicatorOn, (XtArgVal)True); }
		      void HideIndicator() /* Hide the toggle */
			{ SetAttribute(XmNindicatorOn, (XtArgVal)False); }
		      void Diamond()      /* Set indicator to diamond */
			{ SetAttribute(XmNindicatorType, 
				       (XtArgVal)XmONE_OF_MANY); }
		      void Box() /* Set toggle indicator to box. */
			{ SetAttribute(XmNindicatorType,
				       (XtArgVal)XmN_OF_MANY);   }

		      void SetState(Boolean state) 
			{ SetAttribute(XmNset, state); }
		      void Set()          /* Set toggle to true. */
			{ SetAttribute(XmNset, (XtArgVal)True);}
		      void UnSet()        /* Set toggle to false */
			{ SetAttribute(XmNset, (XtArgVal)False); }
		      Boolean GetState()  /* Get toggle state */
			{ Boolean value;
			  GetAttribute(XmNset, &value);
			  return value;
			}
		    };

/*
** An arrow button is a button with an arrow that points left right up or down
** The arrow is the only label allowed so we override the labelling functions
** as well as a few other inappropriate functions.
*/
class XMArrowButton : public XMButton
{
 public:
  /* Constructors/destructors */

  XMArrowButton(char *n, Widget parent, 
		void (*cb)(XMWidget *, XtPointer, XtPointer) = NULL,
		XtPointer cd = NULL) :
		  XMButton(n, xmArrowButtonWidgetClass, parent)
		    {
		      Enable();
		      if(cb) 
		         XMWidget::AddCallback(XmNactivateCallback, cb, cd);
		    }
  XMArrowButton(char *n, XMWidget &parent,
		void (*cb)(XMWidget *, XtPointer, XtPointer) = NULL,
		XtPointer cd = NULL) :
		  XMButton(n, xmArrowButtonWidgetClass, parent)
		    {
		      Enable();
		      if(cb)
		         XMWidget::AddCallback(XmNactivateCallback, cb, cd);

		    }
  /* Additional manipulators: */

  Callback_data *AddCallback(void (*cb)(XMWidget *, XtPointer, XtPointer)=NULL,
			     XtPointer cd = NULL)
    {
      return XMWidget::AddCallback(XmNactivateCallback, cb, cd);
    }
  void PointLeft() {
    SetAttribute(XmNarrowDirection, (XtArgVal)XmARROW_LEFT);
  }
  void PointRight() {
    SetAttribute(XmNarrowDirection, (XtArgVal)XmARROW_RIGHT);
  }
  void PointUp() {
    SetAttribute(XmNarrowDirection, (XtArgVal)XmARROW_UP);
  }
  void PointDown() {
    SetAttribute(XmNarrowDirection, (XtArgVal)XmARROW_DOWN);
  }

  unsigned char Direction() {
    unsigned char dir;

    GetAttribute(XmNarrowDirection, &dir);

    return dir;
  }

  /* Overrides of virtual functions in the base class */

  virtual void Label(XmString label) {}
  virtual void Label(String label)   {}
  virtual void SetMnemonic(KeySym k) {}
};
#endif
