/*
** Facility:
**   Xt/Motif support.
** Abstract:
**   XMCallback.h:
**     This file defines a template class called the XMCallback
**     class.  The class implements an encapsulated behavior
**     for Xt callbacks.  XM widgets can incoroprate callbacks
**     and register them to allow delivery of Xt callbacks to 
**     class functions without the rigmarole of additional relay
**     functions (outside of the callback class itself that is).
**     Since compilers known to me require textual inclusion of
**     template functions in the client files, this
**     file also includes the file 
**     XMCallback.c the implementation file.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
**   @(#)XMCallback.h	8.1 6/23/95 
** NOTE:
**   Inspired by the article C++ callback J. Darren James
**   X Journal Nov./Dec. 1993
*/

#ifndef XMCALLBACK_H
#define XMCALLBACK_H
#ifdef unix
#pragma interface
#endif
#include "XMWidget.h"
#include <stdio.h>
#include <string.h>
#define TEMP_TYPEDEF
template <class T>
class  XMCallback {
 public:
  typedef void (T::*T_Action)(XMWidget *,
			      XtPointer,
			      XtPointer);

  /* Constructors and destructor functions */

  XMCallback(T *object) {
                           instance = object; 
			   callbackreason = NULL;
			   callbackaction = 
                                  (T_Action)
			                    NULL;
			   callbackuserd  = NULL; /* Constructor */
			   wid            = NULL;
			   self           = this;
			   callbackset    = False;
			 }

  XMCallback() {}
  ~XMCallback() { UnRegister(); }		/* Destructor. */

  /* Functions below register a callback and unregister it: */
  /* Note that destructor implies an unregister if already registered */
  void
  Register(XMWidget *w,		/* Widget to register callback on. */
	   String   reason,	/* Which callback to register.     */
	   void (T::*callback)(XMWidget *,
			  XtPointer,
			  XtPointer),  /* Member function to invoke.      */
	   XtPointer userdata) {	/* User data to pass to callback.  */

    /* First build the data needed to register/unregister/relay the callback
     */
    
    wid = w;
    memcpy(&callbackaction, &callback, sizeof(callback));
    callbackuserd  = userdata;
    callbackreason = (String) new char[strlen(reason)+1];
    strcpy(callbackreason, reason);
    
    /* Now Register the callback with Xt:  */
    
    XtAddCallback(wid->getid(),
		  callbackreason,
		  &XMCallback<T>::_Dispatch_Callback_Relay,
		  (XtPointer)self);
    callbackset = True;
  }
  
  void
    UnRegister() {			/* Un-register most recent callback  */

      if(callbackset) {
	XtRemoveCallback(wid->getid(),
			 callbackreason,
			 &XMCallback<T>::_Dispatch_Callback_Relay,
			 (XtPointer)self);
	
	delete []callbackreason;	/* Delete dynamic storage. */
	
	callbackreason = NULL;
	callbackaction = 
                        (T_Action)
			 NULL;
	callbackuserd  = NULL;
	wid            = NULL;
	callbackset = False;
      }
    }
  void
    Relay(XtPointer calld) {	/* Relay the callback to instance    */
      (instance->*(callbackaction))(wid, callbackuserd, calld);
    }
 private:

  T *instance;			/* Pointer to enclosing class' instance  */
  
  /* The following are stored from Register calls and used for both the
  ** Callback relay and the unregister:
  */

  String callbackreason;	/* Stored callback reason. */
  T_Action callbackaction;
  int      callbackset;
  XtPointer callbackuserd;	/* User data to pass to method function */
  XMWidget *wid;		/* Widget callback is on.           */
  XMCallback<T> *self;                 /* self pointer */

  /* The following static function is what actually gets attched to the
  ** XtCallbacks list of the widget:
  */
  static void _Dispatch_Callback_Relay(Widget w, XtPointer ud,
				       XtPointer cd){
    XMCallback<T> *us = (XMCallback<T> *)ud;
    us->Relay(cd);
  }
};


#endif
