#include <config.h>
#include "XMPushbutton.h"

XMButton::XMButton(char *n, WidgetClass c, Widget parent)
  : XMManagedWidget(n, c, parent)
{}

XMButton::XMButton(char *n, WidgetClass c, XMWidget &parent)
  : XMManagedWidget(n, c, parent)
{}

XMButton::XMButton(Widget w) : XMManagedWidget(w)
{}

void
XMButton::Enable()
{
  SetAttribute(XmNsensitive, (XtArgVal)True); 
}

void
XMButton::Disable()
{
  SetAttribute(XmNsensitive, (XtArgVal)False);
}

void 
XMButton::Label(XmString label)
{
  SetAttribute(XmNlabelString, label); 
}

void
XMButton::Label(String label)
{
  XmString s;
  s = XmStringCreateLtoR(label, 
			 XmSTRING_DEFAULT_CHARSET);
  Label(s);
  XmStringFree(s);
}

void
XMButton::SetMnemonic(KeySym k)
{
  SetAttribute(XmNmnemonic, k); 
}

void
XMButton::SetAccelerator(char *translation, char *prompt)
{
  XmString acctext = XmStringCreateSimple(prompt);
  XtVaSetValues(id,
		XmNaccelerator, translation,
		XmNacceleratorText, acctext,
		NULL);
  XmStringFree(acctext);
}


XMPushButton::XMPushButton(char *n, Widget parent, 
			   void (*cb)(XMWidget *, XtPointer, XtPointer),
			   XtPointer cd ):
  XMButton(n, xmPushButtonWidgetClass, parent)
{
  Enable();
  if(cb)
    XMWidget::AddCallback(XmNactivateCallback, cb, cd);
}

XMPushButton::XMPushButton(char *n, XMWidget &parent, 
			   void (*cb)(XMWidget *, XtPointer, XtPointer),
			   XtPointer cd):
  XMButton(n, xmPushButtonWidgetClass, parent)
{
  Enable();
  if(cb)
    XMWidget::AddCallback(XmNactivateCallback, cb, cd);
}

XMPushButton::XMPushButton(Widget w) : XMButton(w) {}

Callback_data*
XMPushButton::AddCallback(void (*cb)(XMWidget *, 
				     XtPointer, 
				     XtPointer),
			  XtPointer cd)
{
  return XMWidget::AddCallback(XmNactivateCallback, cb, cd);
}


XMCascadeButton::XMCascadeButton(char *n, Widget parent, 
				 void (*cb)(XMWidget *, XtPointer, XtPointer),
				 XtPointer cd):
  XMButton(n, xmCascadeButtonWidgetClass, parent)
{
  Enable();
  if(cb)
    XMWidget::AddCallback(XmNactivateCallback, cb, cd);
}

XMCascadeButton::XMCascadeButton(char *n, XMWidget &parent, 
				 void (*cb)(XMWidget *, XtPointer, XtPointer),
				 XtPointer cd):
  XMButton(n, xmCascadeButtonWidgetClass, parent)
{
  Enable();
  if(cb)
    XMWidget::AddCallback(XmNactivateCallback, cb, cd);
}

XMCascadeButton::XMCascadeButton(Widget w) : XMButton(w) {}

void
XMCascadeButton::SetAssociatedMenu(XMWidget &w)
{
  SetAttribute(XmNsubMenuId, w.getid());
}

void
XMCascadeButton::SetAssociatedMenu(Widget w)
{
  SetAttribute(XmNsubMenuId, w);
} 

Callback_data*
XMCascadeButton::AddCallback(void (*cb)(XMWidget *, 
					XtPointer, 
					XtPointer) ,
			     XtPointer cd)
{ 
  return XMWidget::AddCallback(XmNactivateCallback, cb, cd); 
}


XMToggleButton::XMToggleButton(char *n, Widget parent,
			       void (*cb)(XMWidget *, XtPointer, XtPointer) ,
			       XtPointer cd) :
  XMButton(n, xmToggleButtonWidgetClass, parent)
{
  Enable();
  if(cb)
    XMWidget::AddCallback(XmNvalueChangedCallback, 
			  cb, cd);
}

XMToggleButton::XMToggleButton(char *n, XMWidget &parent,
			       void (*cb)(XMWidget *, XtPointer, XtPointer) ,
			       XtPointer cd) :
  XMButton(n, xmToggleButtonWidgetClass, parent)
{
  Enable();
  if(cb)
    XMWidget::AddCallback(XmNvalueChangedCallback, cb, cd);
}

XMToggleButton::XMToggleButton(Widget w) : XMButton(w) {}

void
XMToggleButton::ShowIndicator() /* Display the toggle */
{ 
  SetAttribute(XmNindicatorOn, (XtArgVal)True);
} 

void
XMToggleButton::HideIndicator()
{
  SetAttribute(XmNindicatorOn, (XtArgVal)False);
}

void
XMToggleButton::Diamond()
{ 
  SetAttribute(XmNindicatorType, 
	       (XtArgVal)XmONE_OF_MANY);
}

void
XMToggleButton::Box()
{ 
  SetAttribute(XmNindicatorType,
	       (XtArgVal)XmN_OF_MANY);
}

void
XMToggleButton::SetState(Boolean state)
{ 
  SetAttribute(XmNset, state); 
}

void
XMToggleButton::Set()
{
  SetAttribute(XmNset, (XtArgVal)True);
}

void
XMToggleButton::UnSet()
{ 
  SetAttribute(XmNset, (XtArgVal)False);
}

Boolean
XMToggleButton::GetState()
{ 
  Boolean value;
  GetAttribute(XmNset, &value);
  return value;
}

Callback_data*
XMToggleButton::AddCallback(void (*cb)(XMWidget *, 
				       XtPointer, 
				       XtPointer) ,
			    XtPointer cd )
{ 
  return XMWidget::AddCallback(XmNvalueChangedCallback, cb, cd);
}


XMArrowButton::XMArrowButton(char *n, Widget parent, 
			     void (*cb)(XMWidget *, XtPointer, XtPointer) ,
			     XtPointer cd ) :
  XMButton(n, xmArrowButtonWidgetClass, parent)
{
  Enable();
  if(cb) 
    XMWidget::AddCallback(XmNactivateCallback, cb, cd);
}

XMArrowButton::XMArrowButton(char *n, XMWidget &parent,
			     void (*cb)(XMWidget *, XtPointer, XtPointer) ,
			     XtPointer cd ) :
  XMButton(n, xmArrowButtonWidgetClass, parent)
{
  Enable();
  if(cb)
    XMWidget::AddCallback(XmNactivateCallback, cb, cd);
  
}

Callback_data*
XMArrowButton::AddCallback(void (*cb)(XMWidget *, XtPointer, XtPointer),
			   XtPointer cd)
{
  return XMWidget::AddCallback(XmNactivateCallback, cb, cd);
}

void
XMArrowButton::PointLeft()
{
  SetAttribute(XmNarrowDirection, (XtArgVal)XmARROW_LEFT);
}

void
XMArrowButton::PointRight()
{
  SetAttribute(XmNarrowDirection, (XtArgVal)XmARROW_RIGHT);
}

void
XMArrowButton::PointUp()
{
  SetAttribute(XmNarrowDirection, (XtArgVal)XmARROW_UP);
}

void
XMArrowButton::PointDown()
{
  SetAttribute(XmNarrowDirection, (XtArgVal)XmARROW_DOWN);
}

unsigned char
XMArrowButton::Direction()
{
  unsigned char dir;
  GetAttribute(XmNarrowDirection, &dir);
  return dir;
}

void
XMArrowButton::Label(XmString label)
{}

void
XMArrowButton::Label(String label)
{}

void
XMArrowButton::SetMnemonic(KeySym k)
{}
