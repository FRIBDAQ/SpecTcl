/*
** Facility:
**   Xamine C++ support for Motif.
** Abstract:
**   XMcallback.cc   - This file contains callback handling support for
**                     C++ systems using Motif.  The main problem we must
**                     solve is that the Motif callback mechanism calls
**                     routines via:
**                     rtn(Widget id, XtPointer client_data, XtPointer reason)
**                     We really would like to see this look like:
**                     rtn(XMWidget object, XtPointer client_data, 
**                         XtPointer reason)
**                     The mechanism we choose is to build a dynamic
**                     client data structure which contains the this pointer,
**                     the callback address, and the original client data.
**                     Then we set up a callback dispatcher as the Motif
**                     callback which then calls the user's callback routine
**                     with the desired arguments.
**                     
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
**/

/*
** Includes:
*/
#include <errno.h>
#include <stdio.h>
#include "XMWidget.h"
#ifndef Linux
extern "C" {
             void exit(int);

	   }
#endif


/*
** Functional Description:
**    XMDispatchCallback  - This function is the callback function that
**                          is registered as the motif callback.  We
**                          use the client data argument to call an object
**                          oriented callback function.
** Formal Parameters:
**    Widget  w:
**      The widget ID of the object that's the source of the callback.
**    XtPointer cbd:
**      Actually a pointer to a Callback_data structure which describes the
**      desired callback.
**    XtPointer reason:
**      Reason for callback, supplied by Motif.
*/

static void XMDispatchCallback(Widget w, XtPointer cbd, XtPointer reason)
{
  Callback_data *descrip = (Callback_data *)cbd;

  (descrip->function)(descrip->object, descrip->client_data, reason);
}


/*
** Functional Description:
**    XMAddCallback        - Adds a callback for an XM object:
** Arguments:
**    XMWidget *object:
**       The widget for which we add the callback.
**    String reason:
**       The Motif callback reason.
**    XtCallbackProc proc:
**       The user's callback procedure.
**    XtPointer data:
**       The users's client data
** Returns:
**   Pointer to callback data structure which can be used as a handle
**   to XMRemoveCallback().
*/
Callback_data *XMAddCallback(XMWidget *object, String reason,
		   void (*proc)(XMWidget *, XtPointer, XtPointer),
		   XtPointer data)
{
  Callback_data *our_cd;

  /* First create a callback data structure and then fill it in with
  ** the arguments in the list:
  */

  our_cd = new Callback_data;
  if(our_cd == NULL) {
    perror("Fatal error in XMAddCallback -- unable to allocate Callback_data");
    exit(-1);
  }
  our_cd->object   = object;
  our_cd->function = proc;
  our_cd->client_data = data;
  our_cd->reason   = new char[strlen(reason)+1];
  strcpy(our_cd->reason, reason);

  /* Next register the callback prior to returning */

  XtAddCallback(object->getid(), reason, 
		(XtCallbackProc)XMDispatchCallback, (XtPointer)our_cd);
  return our_cd;
}

/* 
** Functional Description:
**   XMRemoveCallback:
**      This function removes a callback given a pointer to it's callback
**      data handle.
** Formal Parameters:
**      Callback_data *cbd:
**        Pointer to the dynamic callback data structure.
*/
void XMRemoveCallback(Callback_data *cbd)
{
  /* 
  ** Remove the callback function:
  */
  XtRemoveCallback(cbd->object->getid(), cbd->reason, 
		   (XtCallbackProc)XMDispatchCallback, 
		   (XtPointer)cbd);
  /*
  ** Then delete the callback data structure since it was the source of
  ** memory leaks else where.
  */
  delete [] cbd->reason;
  delete cbd;			/* Prevent memory leaks. */

}

/** Functional Description:
**    XMUnmanageChild  - A callback that can be specified to unmanage a child
**                       widget.
** Formal Parameters:
**    XMWidget *wid:
**       Widget of callback.
**    XtPointer client_data, callback_data:
**       Ignored data pointers.
*/
void XMUnmanageChild(XMWidget *wid, XtPointer client_data, 
		     XtPointer callback_data)
{
  wid->UnManage();
}

/*
** Functional Description:
**   XMDestroyWidget  - This function is intended to be used as a callback
**                      when the desired action is to destroy the widget
**                      object.
** Formal Parameters:
**   XMWidget *w  - The widget to destroy
**   XtPointer cd - Client data ignored.
**   XtPointer cb - Callback data ignored.
*/
void XMDestroyWidget(XMWidget *w, XtPointer cd, XtPointer cb)
{
  delete w;
}
