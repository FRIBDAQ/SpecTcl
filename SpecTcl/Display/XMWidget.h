/*
** Facility:
**   Xamine - NSCL display program C++ widget support
** Abstract:
**    XMWidget.h   - This include file defines an Xt Widget class.
**                   This class is the superclass for all
**                   Widget classes created in this library.
** Author:
**   Ron Fox
**   NSCL 
**   Michigan State University
**   East Lansing, MI 48824-1321
** Revision info:
**   @(#)XMWidget.h	8.3 5/20/96 
*/

#ifndef _XMWidget_H
#define _XMWidget_H

#ifdef unix
#pragma interface
#endif

/*
** Include files:
*/
#ifdef unix
#include <strings.h>
#endif
#ifdef VMS
#include <string.h>
#endif
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>

typedef char XMWidgetName[32];

struct Callback_data;



class XMApplication 
               {
               protected:
		 XtAppContext  application;
		 Widget       toplevel_shell;
		 XMWidgetName app_class;
	       public:
		 XMApplication(char *cl, Cardinal *argc, char **argv,
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
		 void Begin()	/* Instantiant everything and do main loop */
		   {
		     if(!XtIsRealized(toplevel_shell))
			XtRealizeWidget(toplevel_shell); 
		     XtAppMainLoop(application); 
		   }
		 XtAppContext GetContext() { return application; }
		 Widget getid() { return toplevel_shell; }
		 void Manage()   {XtManageChild(toplevel_shell); }
		 void UnManage() {XtUnmanageChild(toplevel_shell); }
		 void Realize()  {XtRealizeWidget(toplevel_shell); }
		 void UnRealize() {XtUnrealizeWidget(toplevel_shell); }

		  void SetAttribute(String attribute, XtArgVal value) 
		    { XtVaSetValues(toplevel_shell, attribute, value, NULL); }
		  void SetAttribute(String attribute, void *value)
		    { XtVaSetValues(toplevel_shell, attribute, value, NULL); }
		  
		  void GetAttribute(String attribute, void *value)
		    { XtVaGetValues(toplevel_shell, attribute, value, NULL); }
		  void GetAttribute(String attribute, XtArgVal value)
		    { XtVaGetValues(toplevel_shell, attribute, value, NULL); }
		  

	       };
	      
class XMWidget {
               protected:
                  Widget id;
		  XMWidgetName name;
		  void Create(char *n, WidgetClass cl, Widget parent,
			      ArgList l, Cardinal num_args)
		    { 
		      strcpy(name,n);
		      id = XtCreateWidget(name, cl, parent, l, num_args);
		    }
               public:

		  /* Constructors and Destructors */
		  XMWidget(char *n)
		    {
		      strcpy(name, n);
		      /* Null constructor to allow full override. */
		    }
		  XMWidget(Widget w) 
		    {
		      id = w;
		      strcpy(name, XtName(id));
		    }
		  XMWidget(char *n, WidgetClass cl, XMApplication &parent,
			   ArgList l=NULL, Cardinal num_args=0)
		    {
		      Create(n, cl, parent.getid(), l, num_args);
		    }		      
		  XMWidget(char *n, WidgetClass cl, Widget parent,
			   ArgList l=NULL, Cardinal num_args=0)
		    {
		      Create(n, cl, parent, l, num_args);
		    }
		  XMWidget(char *n, WidgetClass cl, XMWidget &parent,
			   ArgList l=NULL, Cardinal num_args=0)
		    {
		      Create(n, cl, parent.getid(), l, num_args);
		    }
		  virtual ~XMWidget() { XtDestroyWidget(id); }
		  
		  /* Get basic attributes: */
		  
		  Widget getid()         { return id; }
		  Widget getparent()     { return XtParent(id); }
		  char *getname() { return name; }
		  
		  /* Get/Modify X resources: */
		  void SetAttribute(String attribute, XtArgVal value) 
		    { XtVaSetValues(id, attribute, value, NULL); }
		  void SetAttribute(String attribute, void *value)
		    { XtVaSetValues(id, attribute, value, NULL); }
		  
		  void GetAttribute(String attribute, void *value)
		    { XtVaGetValues(id, attribute, value, NULL); }
		  void GetAttribute(String attribute, XtArgVal value)
		    { XtVaGetValues(id, attribute, value, NULL); }
		  
		  Callback_data *AddCallback(String reason, 
				   void (*proc)(XMWidget *, XtPointer, XtPointer),
				   XtPointer data = NULL)
		    { 
		      extern Callback_data *XMAddCallback(XMWidget *, String, 
						          void (*)(XMWidget *,
							  XtPointer, XtPointer),
						          XtPointer);
		      return XMAddCallback(this, reason, proc, data); }
		  
		  /* Management: */
		  
		  void Map()    { XtMapWidget(id);   }
		  void UnMap()  { XtUnmapWidget(id); }
		  void Manage() { XtManageChild(id); }
		  void UnManage() { XtUnmanageChild(id); }
		  void Realize() { XtRealizeWidget(id); }
		  void UnRealize() { XtUnrealizeWidget(id); }
		  
		};
class XMManagedWidget : public XMWidget
                {
		public:
		  XMManagedWidget(char *n) : XMWidget(n)
		    {

		      /* Null default constructor to allow full override */
		    }
		  XMManagedWidget(char *n, WidgetClass cl, Widget parent,
				  ArgList l=NULL, Cardinal num_args=0) :
				    XMWidget(n, cl, parent, l, num_args)
				      {  Manage(); }
		  XMManagedWidget(char *n, WidgetClass cl, XMWidget &parent,
				 ArgList l=NULL, Cardinal num_args=0) :
				   XMWidget(n, cl, parent, l, num_args)
				     { Manage(); }
		  XMManagedWidget(Widget w) : XMWidget(w)
		    {}
		};

struct Callback_data { 
                       XMWidget       *object;     
		       void           (*function)(XMWidget *,
						  XtPointer,
						  XtPointer); 
		       XtPointer      client_data; 
		       String      reason;
		     };

extern void XMRemoveCallback(Callback_data *cb);
void XMUnmanageChild(XMWidget *, XtPointer, XtPointer);
void XMDestroyWidget(XMWidget *, XtPointer, XtPointer);		  

#endif
