#include "XMWidget.h"

/*
** Implementation of functions from class XMApplication
*/

XMApplication::XMApplication(char *cl, Cardinal *argc, char **argv,
			     XrmOptionDescList options = NULL, 
			     Cardinal noptions = 0,
			     const char **fallback_resources = NULL,
			     ArgList args = NULL, Cardinal num_args=0)
{
  strcpy(app_class, cl);
  toplevel_shell = XtAppInitialize(&application,
				   app_class, options,
				   noptions,
				   (int *)
				   argc, argv,
				   (String *)
				   fallback_resources,
				   args, num_args);
}

void
XMApplication::Begin()
{
  if(!XtIsRealized(toplevel_shell))
    XtRealizeWidget(toplevel_shell); 
  XtAppMainLoop(application); 
}

XtAppContext
XMApplication::GetContext() { return application; }

Widget
XMApplication::getid() { return toplevel_shell; }

void
XMApplication::Manage() 
{
  XtManageChild(toplevel_shell);
}

void
XMApplication::UnManage()
{
  XtUnmanageChild(toplevel_shell);
}

void
XMApplication::Realize()
{
  XtRealizeWidget(toplevel_shell);
}

void
XMApplication::UnRealize()
{
  XtUnrealizeWidget(toplevel_shell);
}

void
XMApplication::SetAttribute(String attribute, XtArgVal value) 
{ 
  XtVaSetValues(toplevel_shell, attribute, value, NULL);
}

void 
XMApplication::SetAttribute(String attribute, void *value)
{ 
  XtVaSetValues(toplevel_shell, attribute, value, NULL);
}

void 
XMApplication::GetAttribute(String attribute, void *value)
{ 
  XtVaGetValues(toplevel_shell, attribute, value, NULL);
}

void
XMApplication::GetAttribute(String attribute, XtArgVal value)
{ 
  XtVaGetValues(toplevel_shell, attribute, value, NULL);
}


/*
** Implementation of functions for class XMWidget 
*/

XMWidget::XMWidget(char *n)
{
  strcpy(name, n);
  /* Null constructor to allow full override. */
}

XMWidget::XMWidget(Widget w)
{
  id = w;
  strcpy(name, XtName(id));
}

XMWidget::XMWidget(char *n, WidgetClass cl, XMApplication &parent,
		   ArgList l=NULL, Cardinal num_args=0)
{
  Create(n, cl, parent.getid(), l, num_args);
}

XMWidget::XMWidget(char *n, WidgetClass cl, Widget parent,
		   ArgList l=NULL, Cardinal num_args=0)
{
  Create(n, cl, parent, l, num_args);
}

XMWidget::XMWidget(char *n, WidgetClass cl, XMWidget &parent,
		   ArgList l=NULL, Cardinal num_args=0)
{
  Create(n, cl, parent.getid(), l, num_args);
}

XMWidget::~XMWidget()
{
  XtDestroyWidget(id);
}

Widget
XMWidget::getid() { return id; }

Widget
XMWidget::getparent() { return XtParent(id); }

char*
XMWidget::getname() { return name; }

void
XMWidget::SetAttribute(String attribute, XtArgVal value) 
{
  XtVaSetValues(id, attribute, value, NULL);
}

void
XMWidget::SetAttribute(String attribute, void *value)
{
  XtVaSetValues(id, attribute, value, NULL);
}

void
XMWidget::GetAttribute(String attribute, void *value)
{
  XtVaGetValues(id, attribute, value, NULL); 
}

void
XMWidget::GetAttribute(String attribute, XtArgVal value)
{
  XtVaGetValues(id, attribute, value, NULL);
}

Callback_data*
XMWidget::AddCallback(String reason,
		      void (*proc)(XMWidget *, XtPointer, XtPointer),
		      XtPointer data = NULL)
{ 
  extern Callback_data *XMAddCallback(XMWidget *, String, 
				      void (*)(XMWidget *,
					       XtPointer, XtPointer),
				      XtPointer);
  return XMAddCallback(this, reason, proc, data);
}

void
XMWidget::Map() { XtMapWidget(id); }

void
XMWidget::UnMap() { XtUnmapWidget(id); }

void
XMWidget::Manage() { XtManageChild(id); }

void 
XMWidget::UnManage() { XtUnmanageChild(id); }

void
XMWidget::Realize() { XtRealizeWidget(id); }

void
XMWidget::UnRealize() { XtUnrealizeWidget(id); }

void
XMWidget::Create(char *n, WidgetClass cl, Widget parent,
		 ArgList l, Cardinal num_args)
{
  strcpy(name,n);
  id = XtCreateWidget(name, cl, parent, l, num_args);
}

/*
** Implementation of functions from class XMManagedWidget
*/

XMManagedWidget::XMManagedWidget(char *n) :
  XMWidget(n)
{ /* Null default constructor to allow full override */ }

XMManagedWidget::XMManagedWidget(char *n, WidgetClass cl, Widget parent,
				 ArgList l=NULL, Cardinal num_args=0) :
  XMWidget(n, cl, parent, l, num_args)
{ 
  Manage();
}

XMManagedWidget::XMManagedWidget(char *n, WidgetClass cl, XMWidget &parent,
				 ArgList l=NULL, Cardinal num_args=0) :
  XMWidget(n, cl, parent, l, num_args)
{
  Manage();
}

XMManagedWidget::XMManagedWidget(Widget w) :
  XMWidget(w)
{ }

XMManagedWidget::~XMManagedWidget()
{
  XtDestroyWidget(getid());
}

