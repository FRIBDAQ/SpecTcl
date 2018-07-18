/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

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

#ifndef XMWIDGET_H
#define XMWIDGET_H

/*
** Include files:
*/
#include <strings.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>

#ifndef __STL_LIST
#include <list>
#ifndef __STL_LIST
#define __STL_LIST
#endif
#endif

typedef char XMWidgetName[32];

struct Callback_data;

class XMApplication 
{
protected:
  XtAppContext  application;
  Widget       toplevel_shell;
  XMWidgetName app_class;


public:
  XMApplication(const char *cl, Cardinal *argc, char **argv,
		XrmOptionDescList options = NULL, 
		Cardinal noptions = 0,
		const char **fallback_resources = NULL,
		ArgList args = NULL, Cardinal num_args=0);

  void Begin();	/* Instantiant everything and do main loop */

  XtAppContext GetContext();
  Widget getid();
  void Manage();
  void UnManage();
  void Realize();
  void UnRealize();

  void SetAttribute(String attribute, XtArgVal value);
  void SetAttribute(String attribute, void *value);
  void GetAttribute(String attribute, void *value);
  void GetAttribute(String attribute, XtArgVal value);
};


class XMWidget
{
private:
  std::list<Callback_data*>  m_callbacks;

protected:
  Widget id;
  XMWidgetName name;
  void Create(const char *n, WidgetClass cl, Widget parent,
	      ArgList l, Cardinal num_args);

public:

  /* Constructors and Destructors */
  XMWidget(const char *n);
  XMWidget(Widget w); 
  XMWidget(const char *n, WidgetClass cl, XMApplication &parent,
	   ArgList l=NULL, Cardinal num_args=0);
  XMWidget(const char *n, WidgetClass cl, Widget parent,
	   ArgList l=NULL, Cardinal num_args=0);
  XMWidget(const char *n, WidgetClass cl, XMWidget &parent,
	   ArgList l=NULL, Cardinal num_args=0);
  virtual ~XMWidget();
		  
  /* Get basic attributes: */
		  
  Widget getid();
  Widget getparent();
  const char *getname() const ;
		  
  /* Get/Modify X resources: */

  Window getWindow();
  void SetAttribute(String attribute, XtArgVal value);
  void SetAttribute(String attribute, void *value);
  void GetAttribute(String attribute, void *value);
  void GetAttribute(String attribute, XtArgVal value);
		  
  Callback_data *AddCallback(String reason, 
			     void (*proc)(XMWidget *, XtPointer, XtPointer),
			     XtPointer data = NULL);
  void RemoveCallback(String reason,
		      void (*proc)(XMWidget*, XtPointer, XtPointer),
		      XtPointer data);
		  
  /* Management: */
		  
  void Map();
  void UnMap();
  void Manage();
  void UnManage();
  void Realize();
  void UnRealize();

		  
};


class XMManagedWidget : public XMWidget
{
 public:
  XMManagedWidget(const char *n);
  XMManagedWidget(const char *n, WidgetClass cl, Widget parent,
		  ArgList l=NULL, Cardinal num_args=0);
  XMManagedWidget(const char *n, WidgetClass cl, XMWidget &parent,
		  ArgList l=NULL, Cardinal num_args=0);
  XMManagedWidget(Widget w);
  virtual ~XMManagedWidget();
};


struct Callback_data 
{ 
  XMWidget      *object;     
  void          (*function)(XMWidget *,
			    XtPointer,
			    XtPointer); 
  XtPointer     client_data; 
  String	reason;
};

extern void XMRemoveCallback (Callback_data *cb);

void XMUnmanageChild (XMWidget *, XtPointer, XtPointer);
void XMDestroyWidget (XMWidget *, XtPointer, XtPointer);		  



extern void NullPointer(XMWidget* pWidget, 
			XtPointer pClientData,
			XtPointer pEvent);

#endif
