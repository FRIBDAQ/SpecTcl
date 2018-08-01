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

#ifndef  XMDIALOGS_H
#include "XMDialogs.h"
#endif

#ifndef XMLIST_H
#include "XMList.h"
#endif
#include <string>
#include <vector>


/*!
   XMMultiSelectionList is a bit of a misnomer. What it actually is is a custom dialog
   whose work area is just a list box.  This supports access to the customization
   resources of the list box directly.. specifically, the selection policy.

   You can use this dialog if you want a seletion list dialog that supports selecting
   several items from the list (unlike the 'normal' selection list dialog.

*/
class  XMMultiSelectionList : public XMCustomDialogBox
{
 protected:
  XMList*    m_pList;		// The selection list.

 public:
  XMMultiSelectionList(char* name, XMWidget& parent, char* title);
  virtual ~XMMultiSelectionList();

  // Expose the list box:
 public:
  XMList*  getList();		// In case what's provided below is not sufficient:

  void AutoSelect(Boolean enable=True);
  void SetDoubleClickTime(int ms = 100);
  void SetRows(int rows);
  void SetScrollPolicy(int policy = XmAS_NEEDED);
  void SetSelectionPolicy(int policy = XmSINGLE_SELECT);

  int GetListCount();
  XmStringTable GetListValues();

  void setItems(std::vector<std::string> items);

  int GetSelectedListCount();
  XmStringTable GetSelectedItems();


};
