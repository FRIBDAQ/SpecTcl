/*
** Facility:
**   Xamine support functions.
** Abstract:
**   copier.h:
**     This file provides the public interfaces to a class which facilitates
**     the construction of copier dialogs.  A copier consists of a 
**     pair of list widgets connected by an arrow pushbutton.
**     The arrow push button can face either to the left or right.
**     The idea is that you select one or more entries on both list widgets
**     and then punch the arrow to get the copy done.
**     The class defined is built to be a base class for a class that has
**     been refined for the specific work to be done.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
**   June 24, 1994
**
** SCCS info:
**   @(#)copier.h	8.1 6/23/95 
*/
#ifndef _COPIER_H
#define _COPIER_H
#include "XMPushbutton.h"
#include "XMManagers.h"
#include "XMList.h"
#include "XMLabel.h"
enum Copier_Arrow_Direction {
                              left, 
			      right 
			      };

class Copier_Form {
 public:
  /* Constructors and Desctructors: */

  Copier_Form(XMForm *Parent, char *leftlbl = "From", char *rightlbl = "To");
  virtual ~Copier_Form() {
    delete left_l;
    delete right_l;
    delete button;
    delete left_label;
    delete right_label;
  }

    /* Controls */

  void Configure(XMForm *parent);		/* Set the form configuration */

  void Manage() {
    left_label->Manage();
    right_label->Manage();
    left_l->Manage();
    button->Manage();
    right_l->Manage();
  }
  void UnManage() {
    left_label->UnManage();
    right_label->UnManage();
    left_l->UnManage();
    right_l->UnManage();
    button->UnManage();
  }

  /* Expose internals: */

  XMScrolledList  *GetLeftListWidget()   { return left_l; }
  XMScrolledList  *GetRightListWidget()  { return right_l; }
  XMArrowButton *GetButtonWidget() { return button; }

  /* Control Functions: */

  void SetButtonDirection(Copier_Arrow_Direction dir)
    {
      switch(dir) {
      case left:
	button->PointLeft();
	break;
      default:			/* Default bad value to right. */
      case right:
	button->PointRight();
	break;
      }
    }
  void ClearLeftList()     { left_l->ClearItems(); }
  void ClearRightList()    { right_l->ClearItems(); }
  void AddToLeftList(char *item)   { left_l->AddItem(item); }
  void AddToRightList(char *item)  { right_l->AddItem(item); }
  void SetLeftList(int count, char **items);
  void SetRightList(int count, char **items);
  void SetLeftSelectionPolicy(unsigned char policy) {
    left_l->SetAttribute(XmNselectionPolicy, (int)policy);
  }
  void SetRightSelectionPolicy(unsigned char policy) {
    right_l->SetAttribute(XmNselectionPolicy, (int)policy);
  }

  /* Inquiry functions */

  int GetLeftSelectedListCount() {
    int nitems;
   
    left_l->GetAttribute(XmNselectedItemCount, &nitems);
    return nitems;
  }
  int GetRightSelectedListCount() {
    int nitems;

    right_l->GetAttribute(XmNselectedItemCount, &nitems);
    return nitems;
  }

  int GetLeftSelectedList(char ***elements);
  int GetRightSelectedList(char ***elements);


  /* Action functions which are pure in this class: */

  virtual int CopyItem(char *from, char *to, 
			Copier_Arrow_Direction dir) = 0;

 protected:
  XMScrolledList *left_l;
  XMScrolledList *right_l;
  XMArrowButton  *button;
  XMLabel        *left_label;
  XMLabel        *right_label;
};

#endif


