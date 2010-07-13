#include <config.h>
#include "XMWidget.h"
using namespace std;
/*
** Implementation of functions from class XMApplication
*/

XMApplication::XMApplication(const char *cl, Cardinal *argc, char **argv,
			     XrmOptionDescList options , 
			     Cardinal noptions,
			     const char **fallback_resources,
			     ArgList args, Cardinal num_args)
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

XMWidget::XMWidget(const char *n)
{
  strcpy(name, n);
  /* Null constructor to allow full override. */
}

XMWidget::XMWidget(Widget w)
{
  id = w;
  strcpy(name, XtName(id));
}

XMWidget::XMWidget(const char *n, WidgetClass cl, XMApplication &parent,
		   ArgList l, Cardinal num_args)
{
  Create(n, cl, parent.getid(), l, num_args);
}

XMWidget::XMWidget(const char *n, WidgetClass cl, Widget parent,
		   ArgList l, Cardinal num_args)
{
  Create(n, cl, parent, l, num_args);
}

XMWidget::XMWidget(const char *n, WidgetClass cl, XMWidget &parent,
		   ArgList l, Cardinal num_args)
{
  Create(n, cl, parent.getid(), l, num_args);
}

XMWidget::~XMWidget()
{
  XtDestroyWidget(id);

  // delete the callback data structures that were dynamically created 
  // XMAddCallback

  list<Callback_data*>::iterator i = m_callbacks.begin();
  while (i != m_callbacks.end()) {
    Callback_data* pcb = *i; 
    delete []pcb->reason;	// This is dynamically allocated.
    delete pcb;			// delete the callback struct itself.
    i++;
  }
  // std::list can clean up after itself now.

  m_callbacks.clear();
}

Widget
XMWidget::getid() { return id; }

Widget
XMWidget::getparent() { return XtParent(id); }

const char*
XMWidget::getname() const
{ 
   return name; 
}

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
		      XtPointer data)
{ 
  extern Callback_data *XMAddCallback(XMWidget *, String, 
				      void (*)(XMWidget *,
					       XtPointer, XtPointer),
				      XtPointer);
  Callback_data* pData = XMAddCallback(this, reason, proc, data);
  m_callbacks.push_back(pData);
  return pData;

}
//
// To remove a callback we have to search our 
// callback list for one that matches then call
// XMRemoveCallback and get it out of our callback list.
// XMRemoveCallback will release the resources used by
// the Callback_data struct.
//
void
XMWidget::RemoveCallback(String reason,
			 void (*proc)(XMWidget*, XtPointer, XtPointer),
			 XtPointer data)
{
  list<Callback_data*>::iterator i = m_callbacks.begin();
  while (i != m_callbacks.end()) {
    Callback_data* cbd   = *i;

    // compare the simple fields before bothering with the strcmp on the reason:

    if ((this == cbd->object)    &&  (proc == cbd->function) && (data == cbd->client_data)) {
      if (strcmp(reason, cbd->reason) == 0) {

	// perfect match:

	XMRemoveCallback(cbd);	// This deletes cbd and the string
	m_callbacks.erase(i);	// Get rid of the list element.
	delete []cbd->reason;
	delete cbd;		// Get rid of the dynamic storage.
	return;

      }
    }

    i++;
  }
  // As with Xt, there's no penalty for removing a nonexistent callback.
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
XMWidget::Create(const char *n, WidgetClass cl, Widget parent,
		 ArgList l, Cardinal num_args)
{
  strcpy(name,n);
  id = XtCreateWidget(name, cl, parent, l, num_args);
}

/*
** Implementation of functions from class XMManagedWidget
*/

XMManagedWidget::XMManagedWidget(const char *n) :
  XMWidget(n)
{ /* Null default constructor to allow full override */ }

XMManagedWidget::XMManagedWidget(const char *n, WidgetClass cl, Widget parent,
				 ArgList l, Cardinal num_args) :
  XMWidget(n, cl, parent, l, num_args)
{ 
  Manage();
}

XMManagedWidget::XMManagedWidget(const char *n, WidgetClass cl, XMWidget &parent,
				 ArgList l, Cardinal num_args) :
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

