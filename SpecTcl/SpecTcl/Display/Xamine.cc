/*
** Facility:
**   Xamine  - NSCL Display program.
** Abstract:
**    Xamine.cc:
**      This file contains main code to initialize Xamine, the widget hierarchy
**      of the base user interface and to drop into the Xt main dispatch loop.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/
static char *sccsinfo="@(#)Xamine.cc	8.3 7/23/98 \n";

/*
** Include files:
*/

#include <stdio.h>
#include <string.h>
#ifdef unix
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#endif
#ifdef VMS
#include <types.h>
#endif
#include <new.h>
#include <Xm/DrawingA.h>
#include <Xm/ScrolledW.h>

#include "XMWidget.h"
#include "XMPushbutton.h"
#include "XMMenus.h"
#include "XMDialogs.h"
#include "XMManagers.h"
#include "dispshare.h"
#include "dispwind.h"
#include "panemgr.h"
#include "menusetup.h"
#include "buttonsetup.h"
#include "pkgmgr.h"
#include "colormgr.h"
#include "dfltmgr.h"
#include "gateio.h"
#include "clirequests.h"
#include "prccheck.h"
#include "spccheck.h"
#include "trackcursor.h"

//
//   For OSF, we need to turn off the UAC warnings:
//   We'll do this for both parent and us:
//
#ifdef OSF1
#include <sys/sysinfo.h>
#include <sys/proc.h>
#include <machine/hal_sysinfo.h>
#endif

/*
** Definitions:
*/
static caddr_t usage;
/*
** Define where to find Xamine's catchy icon file.
*/

#ifdef VMS
#define ICON_FILENAME "DAQ_LIB:XAMINE.ICO"
typedef int pid_t;
extern "C" {
pid_t getppid();
}
#endif
#ifdef unix
#ifndef HOME
#define ICON_FILENAME "/daq/etc/Xamine.ico"
#endif
#endif

#ifdef ultrix
extern "C" {
  char *sbrk(int);
}
#endif

/*
** Functional Description:
**   MemGone:
**     Called when memory is exhausted
*/
#ifdef VMS
extern "C" {
void __NEW_HANDLER()
#else
static void MemGone()
#endif
{
  int a;
  fprintf(stderr, "Xamine -- Memory allocation failed... coredumping\n");
  fflush(stderr);		// Ensure we see the output.
  a = *(int *)(NULL);
}
#ifdef VMS
}
#endif

/*
** Functional Description:
**   Monitor:
**      This function is called periodically to monitor changes
**      in state.  We check to see that the parent process is still alive.
**      We also check to see if we should trigger updates on spectra
**      which have changed that are displayed.
** Formal Parameters:
**    XtPointer app:
**      Application data, in this case a pointer to the top level widget.
**    XtIntervalId id:
**      Time interval I.D.      
**
*/
static void Monitor(XtPointer app, XtIntervalId *id)
{
  XMApplication *top = (XMApplication *)app;

  /*
  **  If our parent is not alive, then we exit.
  */

  pid_t p = getppid();
  if(!ProcessAlive(p)) {
    fprintf(stderr, "Xamine -parent process exited\n");
    exit(0);
  }

  /*
  ** Refresh any panes displaying spectra which have changed character since
  ** the last update.
  */

  CheckSpectra();

  /* Repropagate ourselves. */

  XtAppAddTimeOut(top->GetContext(),
		  1000, Monitor, top);
}


/*
** Functional Description:
**    SetIcon    - Sets the icon for the shell widget. 
** Formal Parameters:
**    Widget w:
**       Some widget in the hierarchy.
**    char *filename:
**       Name of the file containing the icon.
*/
void SetIcon(Widget w, char *filename)
{
  Pixmap icon;

  /* Load the pixmap */

  icon = XmGetPixmap(XtScreen(w), filename, BlackPixelOfScreen(XtScreen(w)),
		     WhitePixelOfScreen(XtScreen(w)));
  if(icon == XmUNSPECIFIED_PIXMAP)
    return;			/* Pixmap not found is a no-op */

  /* Locate the vendor shell: */

  while (!XtIsVendorShell(w)) w = XtParent(w);

  /* Set the icon pixmap resource: */

  XtVaSetValues(w, XmNiconPixmap, icon, NULL);
}


int main(Cardinal argc, char **argv)
{
#ifdef OSF1
  {
    int buf[4];			// Buffer to contain sysinfo requests.
    buf[0] = SSIN_UACPARNT;	// This and next turn off UAC print for parent
    buf[1] = UAC_NOPRINT;
    buf[2] = SSIN_UACPROC;	// This and the next turn off our UAC print.
    buf[3] = UAC_NOPRINT;

    if(setsysinfo(SSI_NVPAIRS, buf, 2, 0, 0) < 0) { // Not fatal.. just noisy
      perror("-OSF Specific- Failed to turn off UAC printing");
    }

  }
#endif

  setsid();			// Form new session.
  XMApplication top("Xamine", &argc, argv); /* Top level/init ap. */
  XMMainWindow main_win("MainWindow", top, NULL, 0); /* Main window widget. */
  XMForm       work_area("WorkArea", main_win);
  XMForm       panes("PaneManager",  work_area);
  XMWidget    *status_area = Xamine_SetupLocator(&work_area);

  /*
  ** Set up the work area with the panes on top and the status form on the
  ** Bottom.
  */

  work_area.SetFractionBase(100);
  work_area.SetTopAttachment(panes,    XmATTACH_FORM);
  work_area.SetLeftAttachment(panes,   XmATTACH_FORM);
  work_area.SetRightAttachment(panes,  XmATTACH_FORM);

  work_area.SetLeftAttachment(*status_area,   XmATTACH_FORM);
  work_area.SetBottomAttachment(*status_area, XmATTACH_FORM);
  work_area.SetRightAttachment(*status_area, XmATTACH_FORM);
  work_area.SetTopAttachment(*status_area, XmATTACH_POSITION);
  work_area.SetTopPosition(*status_area, 95);

  work_area.SetBottomAttachment(panes,   XmATTACH_POSITION);
  work_area.SetBottomPosition(panes,  94);

  Arg arg[10];
  XtSetArg(arg[0], XmNscrollingPolicy, XmAUTOMATIC);
  XtSetArg(arg[1], XmNscrollBarDisplayPolicy, XmAS_NEEDED);
  XtSetArg(arg[2], XmNresizable, False);
  XMWidget *cmd_area = new XMWidget("button_bar", 
				     xmScrolledWindowWidgetClass, main_win,
				     arg, 3);


  XMMenuBar *mb;

  /* We need to set up the mechanism for getting Xt notification of */
  /* gate events from the users.  This will also create the appropriate */
  /* pipe/mailboxes to communicate between Xamine and the client.   */

  Xamine_SetupRequestNotification(XtWidgetToApplicationContext(top.getid()),
		      (XtInputCallbackProc)Xamine_ProcessClientRequests);

  /* Initialize access to the shared memory spectra. */

  Xamine_initspectra();

  /* Set up the icon for the application */
#ifdef HOME
  char IconName[100];
  sprintf(IconName,"%s/Etc/Xamine.ico" ,HOME);
  SetIcon(main_win.getid(), IconName);
#else
  SetIcon(main_win.getid(), ICON_FILENAME);
#endif
  /* Set up the user interface */

  mb = Xamine_setup_menus(&main_win); /* Set up the menu bar. */

  Xamine_SetupButtonBar(cmd_area);
  Xamine_Initialize_panedb(&panes); /* Set up the spectrum display panes */


  main_win.SetAreas(mb, cmd_area, NULL, NULL, &work_area); 
  main_win.SetAttribute(XmNcommandWindowLocation, XmCOMMAND_BELOW_WORKSPACE);


  XMWidget *panemgr = Xamine_Getpanemgr();
  panemgr->Manage();
  panes.Manage();
  status_area->Manage();
  work_area.Manage();
  cmd_area->Manage();
  main_win.Manage();

  Xamine_EnableBasePackage();
  Xamine_DisableMultiwindowPackage();
  Xamine_DisableSpectrumPresentPackage();
  Xamine_DisableSpectrumSelectedPackage();
  Xamine_Disable1dSelectedPackage();
  Xamine_Disable2dSelectedPackage();


  /* Set up the colormap if any: */

  top.Realize();
  Xamine_InitColors(&main_win);

  /* Read default pane properties and modify as needed for the tube */

  Xamine_ReadDefaultProperties();
  /*
  **   BUGBUGBUG -- Here's something I can't figure out...
  **                Default properties get updated on all changes
  **                This can cause flips between BW/Color renditions
  **                depending on what's running... 
  **                the code below effectively ignores the users's default
  **                rendition.
  **                possibly later, we'll have two files, one for color,
  **                one for B/W.
  */

  if(!Xamine_ColorDisplay()) 
    Xamine_SetDefault2DRendition(scatter);
  else
    Xamine_SetDefault2DRendition(color);

  /* Start the application */

  usage = (caddr_t)sbrk(0);		/* Get current stack/heap break point. */
  XtAppAddTimeOut(top.GetContext(),
		  1000,
		  Monitor,
		  &top);
#ifndef VMS
  set_new_handler(MemGone);
#endif
  top.Begin();			/* Start processing events */
}
 
