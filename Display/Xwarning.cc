/*
** This program is started up if a user runs a program which uses an old
** version of the Xamine library (can happen since only static versions are
** supplied.  We display an X-11 error message box and on exit from the
** Xmain loop, start up the 'old' image of Xamine.
*/
#include <stdio.h>

#include "XMWidget.h"
#include "XMManagers.h"
#include "XMLabel.h"
#include "XMPushbutton.h"

#ifdef VMS
#define XAMINE_IMAGE "XAMINE_DIR:Xamine_OLD.EXE"
#else
#define XAMINE_IMAGE "/daq/bin/Xamine.old"
#endif
#ifndef Linux
extern "C"
{
  void exit(int);
  int execve(char *path, char **argv, XtPointer envp);
}
#endif
void done(XMWidget *w, XtPointer ud, XtPointer cd)
{
  char *arg = NULL;
  execve(XAMINE_IMAGE, &arg, ud);
  exit(0);
}
int main(Cardinal argc, char **argv, char **envp)
{
  Arg args[10];

  XtSetArg(args[0], XmNheight, 50);
  XtSetArg(args[1], XmNwidth,  500);
  XtSetArg(args[2], XmNminHeight, 50);
  XtSetArg(args[3], XmNminWidth, 100);

  XMApplication top("XWarn", &argc, argv,
		    NULL, 0, NULL,
		    args, 4
		    ); // Top level shell and app context.
  XMForm        layout("LayoutForm", top);  // Form for text and button.
  XMPushButton  exbutton("Continue", layout, done, (XtPointer)envp);
  XMLabel       msg("MsgText", layout,
    "You must re-link your program to use the current version of Xamine");

  /* Set up the form:   */

  layout.SetLeftAttachment(msg, XmATTACH_FORM);
  layout.SetRightAttachment(msg, XmATTACH_FORM);
  layout.SetTopAttachment(msg,   XmATTACH_FORM);

  layout.SetBottomAttachment(exbutton, XmATTACH_FORM);
  layout.SetTopAttachment(exbutton, XmATTACH_WIDGET);
  layout.SetTopWidget(exbutton, msg);
  layout.SetLeftAttachment(exbutton, XmATTACH_POSITION);
  layout.SetLeftPosition(exbutton, 40);

  /* Make windows for the widget tree and paste them to the display */

  exbutton.Manage();
  msg.Manage();
  layout.Manage();

  top.Realize();

  top.Begin();

  
}
