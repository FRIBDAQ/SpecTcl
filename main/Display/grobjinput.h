/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2008

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
**    Xamine -- NSCL Display program.
** Abstract:
**    grobjinput.h
**      This file contains definitions which are common to all
**      graphical object input callback subsystems.  In particular
**      we define a custom dialog which is descended from GraphicalInput.
**      The dialog work area contains three widgets.  The first two and bottom
**      most are a separator and a text field widget which can be used to type in the
**      name of the graphical object being entered.
**       The second is a form which contains no subwidgets.  The intent is
**      that this class will be further specialized for each type of
**      graphical input required.  The specialization will typically cause
**      further prompting widgets to be displayed in the form.  The layout
**      is:
**               +-------------------------------+
**               |   form which specialized class|
**               |   fills in.                   |
**               +-------- separator ------------+
**               |  Name text field.             |
**               +-------------------------------+
**        The only behavior added to the GraphicalInput class is access to
**      the name type-in field, and it's title.
** Author:
**    Ron FOx
**    NSCL
**    Michigan State University
**    East Lansing, MI 48824-1321
** SCCS Information:
**    @(#)grobjinput.h	8.1 6/23/95 
*/
#ifndef GROBJINPUT_H
#define GROBJINPUT_H
#include "XMManagers.h"
#include "XMDialogs.h"
#include "XMText.h"
#include "XMSeparators.h"
#include "XMLabel.h"
#include "grafinput.h"

class ObjectInput : public GraphicalInput {
 protected:
  XMTextField *name_prompt;
  XMSeparator *separator;
  XMForm      *point_prompts;
  XMLabel     *name_label;
 public:
  /* Constructors and destructors: */

  ObjectInput(XMWidget *parent, const char *name, const char **help_text = NULL);
  ~ObjectInput() {
    delete name_prompt;
    delete name_label;
    delete separator;
    delete point_prompts;
  }

  /* Manipulators of the text field:  */

  void SetText(const char *text) { name_prompt->SetText(text); }
  char *GetText()          { return name_prompt->GetText(); }

  /* Management:                      */

  void Manage() {
    point_prompts->Manage();
    name_prompt->Manage();
    name_label->Manage();
    separator->Manage();
    XMCustomDialog::Manage();
  }
  void UnManage() {
    point_prompts->UnManage();
    name_prompt->UnManage();
    separator->UnManage();
    XMCustomDialog::UnManage();  
  }

  /* We need to give implementations to these pure virtual ABC functions */
  virtual void ClearState() {}
  virtual void DrawPoints(XMWidget *pane, Xamine_RefreshContext *ctx = NULL)
    {}
  virtual void ClearPoints(XMWidget *pane, 
			   Xamine_RefreshContext *ctx = NULL) {}
  virtual void ClearDialog() {}
  virtual void AddPoint(point &pt) {}
  virtual void DelPoint() {}
  virtual Boolean DoAccept() {return false;}
};


#endif
