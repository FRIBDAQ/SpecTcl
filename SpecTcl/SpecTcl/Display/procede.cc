/*
** Facility:
**   Xamine utility function:
** Abstract:
**   procede.cc  - There are times in programming X/Motif where you'd really
**                 like to just stop and wait for the answer to a question
**                 before proceding... normally you'd have to do a modal dialog
**                 and set up a callback procedure and all of that sort of stuff
**                 The code in this module follows the example 5-11 in the
**                 O'Reilly Motif programmer's manual to set up a modal/blocking
**                 dialog without multiplying callback routines.
** Author:
**  Ron FOx
**  NSCL
**  Michigan State University
**  East Lansing, MI 48824-1321
*/
#ifdef unix
#pragma implementation "XMCallback.h"
#endif
static char *sccsinfo="@(#)procede.cc	8.1 6/23/95 \n";


/*
** Include files:
*/

#include "XMDialogs.h"

/*
** #define level definitions:
*/

#define YES        1
#define NO         2
#define UNANSWERED 0

/*
** Local static storage:
*/
int answer = UNANSWERED;


/*
** Functional Description:
**   PromptCallback:
**      This function responds to cancel or ok button presses on question
**      callback that was initiated by Prompt.
** Formal Parameters:
**   XMWidget *w:
**     The dialog question widget.
**   XtPointer cd:
**     Pointer to where to put the reply
**   XtPointer callback:
**     Pointer to an XmAnyCallbackStruct from which we figure out which
**     button was pressed.
*/
void PromptCallback(XMWidget *w, XtPointer cd, XtPointer callback)
{
  int *a = (int *)cd;
  XmAnyCallbackStruct *why = (XmAnyCallbackStruct *)callback;

  switch(why->reason) {
  case XmCR_OK:
    *a = YES;
    break;
  case XmCR_CANCEL:
    *a = NO;
    break;
  default:
    *a = UNANSWERED;
    break;
  }
}

/*
** Functional Description:
**   Procede:
**     This function performs the X/Motif equivalent of a Yes/No prompt.
** Formal Parameters:
**   XMWidget *parent:
**      Parent widget for the prompt window.
**   char *prompt:
**      The Prompt text.
** Returns:
**    TRUE   - if OK accepted.
**    FALSE  - if Cancel accepted.
*/

int Procede(XMWidget *parent, char *prompt)
{
  XtAppContext app;

  XMQuestionDialog question("Yes_or_no", *parent,
			    prompt,
			    PromptCallback,
			    (XtPointer)&answer); /* Set up the dialog */
  question.AddCancelCallback(PromptCallback, (XtPointer)&answer); /* Add the cancel cb. */
  question.SetModal(XmDIALOG_FULL_APPLICATION_MODAL);

  /*  The code below is a mini XtAppMainLoop simulator */
  answer = UNANSWERED;

  app = XtWidgetToApplicationContext(question.getid());
  while(answer == UNANSWERED) {
    XtAppProcessEvent(app, XtIMAll);
    XSync(XtDisplay(question.getid()),0);
  }

  /* Drop the dialog down. */

  question.UnManage();
  XSync(XtDisplay(question.getid()), 0);
  XmUpdateDisplay(question.getid());

  return (answer == YES);
}
