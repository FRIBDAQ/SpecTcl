/*
** Facility:
**   C++ library support for Motif.
** Abstract:
**   XMManagers.cc  - This file contains code for methods in the manager
**                    class which is not suitable for in-lining.
** Author:
**   Ron FOx
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/
static char *sscsinfo = "@(#)XMManagers.cc	8.1 6/23/95 \n";


/*
** Include files:
*/

#include "XMManagers.h"


/*
** Method Description:
**   XMMainWindow::SetAreas
**      This function tells a main window object which widgets are which.
**      The Main Window widget supports the following windows:
**         menubar        -  A menu bar at the top of the window.
**         commandwindow  -  A widget for taking commands.
**         scrollbars     -  Horizontal and vertical scrollbars for the
**                           work region.
**         workregion     -  A region in which real work gets done.
**         message        - A region for displaying messages.
** Formal Parameters:
**    NOTE: Any parameter can be null indicating that there is not a
**          corresponding widget.
**  XMWidget *menubar:
**      The menu bar widget.
**  XMWidget *commandwindow:
**      The window containing command data.
**  XMWidget *verticalscroller:
**  XMWidget *horizontalscroller:
**       The scroll bars for the work region.
*/
void XMMainWindow::SetAreas(XMWidget *menubar, XMWidget *commandwindow,
			    XMWidget *horizontalscroller,
			    XMWidget *verticalscroller,
			    XMWidget *workregion,
			    XMWidget *messagewidget) {
  Widget menu, command, horiz, vertic, work;

  menu = command = horiz = 
    vertic = work = NULL;	/* Assume for now none are present */

  /* Setup for and call the XmMainWindowSetAreas function by getting */
  /* all of the supplied widget ids.                                 */

  if(menubar)            menu     = menubar->getid();
  if(commandwindow)      command  = commandwindow->getid();
  if(horizontalscroller) horiz    = horizontalscroller->getid();
  if(verticalscroller)   vertic   = verticalscroller->getid();
  if(workregion)         work     = workregion->getid();

  XmMainWindowSetAreas(this->getid(), menu, command, horiz, vertic, work);

  /* If necessary, set the XmNmessageWindow resource: */

  if(messagewidget) SetAttribute(XmNmessageWindow, messagewidget->getid());
}
			       
