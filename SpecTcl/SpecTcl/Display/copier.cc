/*
** Facility:
**    Xamine support functions/classes
** Abstract:
**    copier.cc:
**      This file implements non-trivial methods of the copier class.
**      The copier class is a graphical user interface method to copy
**      items from one list to another list of things.  It is implemented
**      as a pair of scrolled lists connected by an arrow button which
**      can point either left or right.  By default, the arrow button
**      points to the right.  The idea is that the user selects one or
**      more items in a source list, and one or more items in a destination
**      list, and then presses the arrow button to initiate the copy in the
**      direction of the arrow.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/
static char *sccsinfo="@(#)copier.cc	8.1 6/23/95 ";

/*
** Includes:
*/
#include <stdio.h>
#include "copier.h"		/* Should include all the other widget stuff */

/*
** Functional Description:
**   Copy_Relay:
**    This function is called when the copy button is pressed.
**    our mission is to call the Copy method once for each source and
**    destination pair.  This is done source depth first (e.g. for each
**    soruce copy to each destination).
** Formal Parameters:
**    XMWidget *button:
**      Widget ID of the pushbutton.  We use it to get the arrow direction.
**    XtPointer userd:
**      USer data, in this case the pointer to the 'this' widget.
**    XtPointer calld:
**      Pointer to motif callback info... useless to us.
*/
static void Copy_Relay(XMWidget *button, XtPointer userd, XtPointer calld)
{
  XMArrowButton *ab = (XMArrowButton *)button;
  Copier_Form   *us = (Copier_Form *)userd;

  char **left_list;
  char **right_list;
  char **from;
  char **to;
  int  left_count;
  int  right_count;
  int  from_count;
  int  to_count;

  Copier_Arrow_Direction dir;
  unsigned char mdir;

  /* Get the left and right selection lists and sizes:  */

  left_count = us->GetLeftSelectedList(&left_list);
  right_count= us->GetRightSelectedList(&right_list);

  /* Depending on the direction of the arrow widget, these are assigned to
  ** the from and to items
  */
  mdir = ab->Direction();

  switch (mdir) {
  case XmARROW_UP:
  case XmARROW_DOWN:
  default:
    fprintf(stderr, ">>>Copier has invalid direction, defaulting to ->\n");
  case XmARROW_RIGHT:
    dir = right;
    from = left_list;
    to   = right_list;
    from_count = left_count;
    to_count   = right_count;
    break;
  case XmARROW_LEFT:
    dir = left;
    from = right_list;
    to   = left_list;
    from_count = left_count;
    to_count   = right_count;
    break;
  }

  

  /* Now we iterate among all the from and two targets and do the copy
  ** function.
  */
  int src;
  for(src = 0; src < from_count; src++) {
    for(int dst = 0; dst < to_count; dst++) {
      if( (from[src] != NULL) && (to[dst] != NULL))
      if(!us->CopyItem(from[src], to[dst], dir)) /* Invoke the copy method. */
	goto done;
    }
  }

  /*  Free the strings that were allocated for us and return. */
 done:
  for(src = 0; src < from_count; src++) 
    if(from[src] != NULL)
      XtFree(from[src]);		/* Delete the character strings. */
  for(int dst = 0; dst < to_count; dst++)
    if(to[src] != NULL)
      XtFree(to[dst]);		/* For both the from and to lists. */
  XtFree((char *)from);			/* Delete the pointers too. */
  XtFree((char *)to);			/* Delete the pointers. */
}

/*
** Functional Description:
**   Copier_Form:
**     Constructor for the Copier_Form class.  Builds a copier widget into
**     a Form parent.
** Formal Parameters:
**   XMForm *parent:
**      Parent form object pointer.
**   char *leftlbl:
**      Label to put on top of the left side scrolled selection widget.
**   char *rightlbl:
**      Label to put on top of the right side scrolled selection widget.
** NOTE:
**    The default is a rightward facing arrow.
*/
Copier_Form::Copier_Form(XMForm *parent, char *leftlbl, char *rightlbl)
{
  left_label = new XMLabel("Left_Label", *parent, leftlbl);
  right_label= new XMLabel("Right_Label", *parent, rightlbl);
  left_l     = new XMScrolledList("Left_List", *parent);
  right_l    = new XMScrolledList("Right_list",*parent);
  button     = new XMArrowButton("Copy_Trigger", *parent, 
			    Copy_Relay, this);

  button->PointRight();

  Configure(parent);
}

/*
** Functional Description:
**   Copier_Form::Configure:
**    This function configures the copier form  It's evidently needed
**    for some non Motif window managers that botch the configuration
**    prior to management.
** Formal Parameters:
**    XMForm *parent:  Parent of the copier widget.
*/
void Copier_Form::Configure(XMForm *parent)
{


  parent->SetFractionBase(7);

  /* Label the left label  at the top */

  parent->SetTopAttachment(*left_label,  XmATTACH_FORM);
  parent->SetLeftAttachment(*left_label, XmATTACH_POSITION);
  parent->SetRightAttachment(*left_label, XmATTACH_NONE);
  parent->SetLeftPosition(*left_label,   0);

  /* Label the right label at the top: */

  parent->SetTopAttachment(*right_label,  XmATTACH_FORM);
  parent->SetLeftAttachment(*right_label, XmATTACH_POSITION);
  parent->SetRightAttachment(*right_label,XmATTACH_NONE);
  parent->SetLeftPosition(*right_label,   4);
  

  /* Place the left list box:  */

  Widget leftw = left_l->GetScrolledWindow();

  parent->SetConstraint(leftw, XmNleftAttachment, XmATTACH_POSITION);
  parent->SetConstraint(leftw, XmNtopAttachment,  XmATTACH_POSITION);
  parent->SetConstraint(leftw, XmNrightAttachment, XmATTACH_POSITION);
  parent->SetConstraint(leftw, XmNbottomAttachment, XmATTACH_POSITION);
  parent->SetConstraint(leftw, XmNleftPosition,    (XtArgVal)0);
  parent->SetConstraint(leftw, XmNrightPosition,  (XtArgVal)2);
  parent->SetConstraint(leftw, XmNtopPosition,     (XtArgVal)1);
  parent->SetConstraint(leftw, XmNbottomPosition,  (XtArgVal)6);

  /* Place the arrow button.  */

  parent->SetLeftAttachment(*button,  XmATTACH_POSITION);
  parent->SetTopAttachment(*button,   XmATTACH_POSITION);
  parent->SetRightAttachment(*button, XmATTACH_NONE);
  parent->SetBottomAttachment(*button,XmATTACH_NONE);
  parent->SetLeftPosition(*button,   3);
  parent->SetTopPosition(*button, 3);

  /* Place the right list box:  */

  Widget rightw = right_l->GetScrolledWindow();
  parent->SetConstraint(rightw, XmNleftAttachment, XmATTACH_POSITION);
  parent->SetConstraint(rightw, XmNtopAttachment,  XmATTACH_POSITION);
  parent->SetConstraint(rightw, XmNrightAttachment, XmATTACH_POSITION);
  parent->SetConstraint(rightw, XmNbottomAttachment, XmATTACH_POSITION);
  parent->SetConstraint(rightw, XmNleftPosition,    4);
  parent->SetConstraint(rightw, XmNtopPosition,     1);
  parent->SetConstraint(rightw, XmNrightAttachment, 6);
  parent->SetConstraint(rightw, XmNbottomPosition,  6);
			  

}

/*
** Functional Description:
**   SetList:
**     This is a local function to set the contents of a list widget
**     to the specified list of character strings.  This implementation
**     minimizes storage costs at the cost of time by adding each item
**     one at a time.
** Formal Parameters:
**   XMListBaseClass *list:
**     The list widget to affect.
**   int nitems:
**     Number of items to set.
**   char **items:
**     list of text items to set.
*/
static void SetList(XMListBaseClass *list, int nitems, char **items)
{
  list->ClearItems();
  for(;nitems > 0; nitems--) {
    list->AddItem(*items++);
  }
}
/*
** Functional Description:
**   Copier_Form::SetLeftList:
**     Sets the items in the left most list.
**   Copier_Form::SetRightList:
**     Sets the items in the right most list.
** Formal Parameters:
**    int nitems:
**      Number of items.
**    char **items:
**      set of items to add.
*/
void Copier_Form::SetLeftList(int nitems, char **items)
{
  SetList(left_l, nitems, items);
}

void Copier_Form::SetRightList(int nitems, char **items)
{
  SetList(right_l, nitems, items);
}

/*
** Functional Description:
**   GetSelectionList:
**     This function gets the list of selected elements from a designated
**     list.  It is a local function meant to be called only by methods
**     defined in this file.
** Formal Parameters:
**   XMListBaseClass *list:
**     The list from which we're pulling the selected list of stuff.
**   char ***elements:
**     Pointer to a list of text strings that were selected.  The
**     user must free the set of pointers returned using XtFree (not delete)
**     since they will be allocated via the X-Toolkit through compound string
**     conversion functions.
** Returns:
**    Number of entries selected.
**    NOTE:  The elements pointer will be null if this is zero.
*/
static int GetSelectionList(XMListBaseClass *list, char ***elements)
{
  int nelts;
  char **els = NULL;

  list->GetAttribute(XmNselectedItemCount, &nelts);
  if(nelts > 0) {

    /* Allocate an array of pointers:   */

    els = (char **)XtMalloc(sizeof(char *)*nelts);
    if(els != NULL) {
      XmStringTable selections;
      list->GetAttribute(XmNselectedItems, &selections);
      for(int i =0; i < nelts; i++) { /* We do our best on failures. */
	char *el = NULL;
	XmStringGetLtoR(selections[i], XmSTRING_DEFAULT_CHARSET, &el);
	els[i] = el;
      }
    }
  }

  *elements = els;
  return nelts;
}
/*
** Functional Description:
**   Copier_Form::GetLeftSelectedList:
**     Retrieve the selected element list from the left list.
**   Copier_Form::GetRightSelectedList:
**     Retrieve the selected element list from the right list.
** Formal Parameters:
**    char ***list:
**     List of selection texts.
** Returns:
**    Number of selections.  Note... some could conceivably be NULL.
**    You must XtFree the non-null ones as well as the list of pointers.
*/
int Copier_Form::GetLeftSelectedList(char ***list)
{
  return GetSelectionList(left_l, list);
}

int Copier_Form::GetRightSelectedList(char ***list)
{
  return GetSelectionList(right_l, list);
}
