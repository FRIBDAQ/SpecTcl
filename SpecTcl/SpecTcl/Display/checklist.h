/*
** Facility:
**   Xamine - NSCL display program X/Motif support classes.
** Abstract:
**   checklist.h:
**     This file defines the checklist and related classes for
**     client modules.
**     Checklists allow you to build vertical lists of check boxes
**     (e.g. for attributes), control their labelling and manage them
**     as a group.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
**   @(#)checklist.h	8.1 6/23/95 
*/

#ifndef _CHECKLIST_H
#define _CHECKLIST_H

/*
** Include files required:
*/
#include "XMManagers.h"
#include "XMPushbutton.h"
#include "XMLabel.h"

/*
** The CheckElement class is a single check (toggle) button paired with
**  a label.  The idea is that you can label the button with a fixed
**  label (the button's label resource) and then vary the other label
**  independently.  They layout is:
**     [ ]  Button Label    Additional Label Widget
*/

class CheckElement : public XMForm {
 public:
  /* Constructors and destructors: */

  CheckElement(char *name,	       /* 's name. */
	       XMWidget *parent,       /* Form's parent. */
	       Boolean state = False,  /* Button initial state. */
	       char *blabel = "Check", /* Button initial label string. */
	       char *label = NULL);    /* Button additional label string  */
  ~CheckElement();

  /* Manipulators: */

  void SetButtonString(char *newlabel);
  void SetLabelString(char  *newlabel = NULL);

  Boolean State();		/* Get button state */
  void    State(Boolean set);	/* Set button state */

  void Manage();		/* Display widgets. */
  void UnManage();		/* UnDisplay widgets. */
  void Enable()  { button->Enable(); }
  void Disable() { button->Disable(); }

 private:
  XMToggleButton *button;	/* Toggle button widget pointer. */
  XMLabel        *label;	/* Additional title label. */


};

class CheckList : public XMForm {
 public:
  /*  Constructors and destructors: */

  CheckList(char *name,		/* Form's name. */
	    XMWidget *parent,	/* Form's parent widget */
	    char *title,	/* Title string at top of form. */
	    int button_count,	/* Number of buttons. */
	    char **buttonstrings = NULL,	/* Initial button names */
	    char **labelstrings  = NULL);       /* Initial label strings  */
  ~CheckList();

  /* Manipulators: */

  void SetButtonString(int num, /* Button number from zero. */
		       char *str); /* Button title string. */
  void SetLabelString(int num,	/* Button number from zero. */
		      char *str); /* Button extra label string. */

  Boolean State(int num);	/* Get state of button num */
  void State(int num,		/* Set state of button num */
	     Boolean state);	/* to 'state' */

  void Disable();
  void Enable();

  void Manage();		/* Make widgets visible. */
  void UnManage();		/* Make widgets invisible. */
    
 private:
  XMLabel *label;		/* Title at top of checklist */
  int     nchecks;		/* Number of checkmarks.     */
  CheckElement **checks;	/* List of checkmarks.       */
};

#endif
