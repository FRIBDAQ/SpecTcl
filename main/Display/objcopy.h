/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


/*
** Facility:
**   Xamine  - NSCL Display program.
** Abstract:
**   objcopy.h:
**     This file contains definitions for stuff associated with the
**     double list copier class and the graphical object copier dialog
**     function.
** Author:
**    Ron Fox
**    NSCL
**    Michigan State University
**    East Lansing, MI 48824-1321
**    @(#)objcopy.h	8.1 6/23/95 
*/

#ifndef OBJCOPY_H
#define OBJCOPY_H
#include "copier.h"

/*
**  This class implements a copier with an added set of buttons.
**  The extra buttons are Select All  buttons under each of the
**  list widgets an an Update button in the middle.  The class autonomously
**  handles the Select All buttons and virtualizes the functionality of
**  the Update button.
**  The Copy function in the parent class (copier_form) is virtualized
**  with a no-op function so that specific classes can provide appropriate
**  behavior.
*/
class Copy_Multiselect : public Copier_Form 
{
 public:
  /* Instantiators  and destructors: */

  Copy_Multiselect(XMForm *top, XMForm *bottom, 
		   const char *left_lbl = "Left:",
		   const char *right_lbl = "Right:");
  virtual ~Copy_Multiselect() {
    delete SelectLeft;
    delete SelectRight;
    delete Update;
  }

  /* Control methods: */

  void Manage() {
    Copier_Form::Configure(t);
    Copier_Form::Manage();
    SelectLeft->Manage();
    SelectRight->Manage();
    Update->Manage();
    t->Manage();
    b->Manage();
  }
  void UnManage() {
    Copier_Form::UnManage();
    SelectLeft->UnManage();
    SelectRight->UnManage();
    Update->UnManage();
    b->UnManage();
    t->UnManage();
  }

  /* virtualizations:  */

  virtual int CopyItem(char *from, char *to, Copier_Arrow_Direction dir) 
    { return 0; }
  virtual void UpdateLeft()  = 0;	/* Called to update contents of left list. */
  virtual void UpdateRight() = 0;	/* Called to update contents of right list. */
 protected:
  XMForm       *t, *b;
  XMPushButton *SelectLeft;
  XMPushButton *SelectRight;
  XMPushButton *Update;
 private:
};

void Xamine_CopyObject(XMWidget *button, XtPointer user_d, XtPointer call_d);

#endif
