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
#include <config.h>
#include "XMMultiSelectionList.h"

using namespace std;

/*!
   Constructor:
   \param name    - Name of the new widget.
   \param parane  - Reference to parent widget.
   \param title   - Widget title.

*/
XMMultiSelectionList::XMMultiSelectionList(char* name, XMWidget& parent, char* title) :
  XMCustomDialogBox(name, parent, title)
{
  string  listname(name);
  XMForm* wa(WorkArea());

  listname += "_list";
  m_pList = new XMList(const_cast<char*>(listname.c_str()), *wa);
  wa->SetTopAttachment(*m_pList, XmATTACH_FORM);
  wa->SetLeftAttachment(*m_pList, XmATTACH_FORM);
  wa->SetRightAttachment(*m_pList, XmATTACH_FORM);
  wa->SetBottomAttachment(*m_pList, XmATTACH_FORM);
		
  m_pList->Manage();		// So parent manage will already have us managed.

}


/*!
  Destroy the widget.
*/
XMMultiSelectionList::~XMMultiSelectionList()
{
  delete m_pList;
}


/////////////////////////////////////////////////////////////////////////////////
//
// The member functions below just operate on the list:
//

/*!
  Return the list widget itself in case the remaining list operations don't provide
  what's needed:
  @return XMList*
  @retval Pointer to the encapsulated list widget.
*/
XMList*  
XMMultiSelectionList::getList()
{
  return m_pList;
}
/*!
  Set the number of rows of visible items.  This should be set prio to managing
  the dialog, as it affect the geometry calculations.
  @param rows (int)  Numer of desired rows.
*/

void 
XMMultiSelectionList::SetRows(int rows)
{
  m_pList->SetRows(rows);
}

/*!
  Set the scrolling policy.  This determines when and why the list box
  will grow scroll bars.  The deaftult XmAS_NEEDED grows scroll bars when the
  size of the box is too big for the area it's allotted.
  @param policy (int = XmAS_NEEDED) scroll bar policy.
*/
void 
XMMultiSelectionList::SetScrollPolicy(int policy)
{
  m_pList->SetScrollPolicy(policy);
}

/*!
   Set the selection policy.  Supplying this for the dialog is the real reason we want
   this

   @param policy (int = XmSINGLE_SELECT) The new policy. This can be one of:
                 XmSINGLE_SELECT  - Only one item can be concurrently selected.
		 XmMULTIPLE_SELECT - Any number of items can be selected click toggles selection
                                    state without modifying the selection state of any other.
		 XmBROWSE_SELECT  - Only one item canb e selected but you can drag
                                    to change which
		 XmEXTENDED_SELECT - Multiple itmes can be selected via shift drag, shift click etc.
*/

void 
XMMultiSelectionList::SetSelectionPolicy(int policy)
{
  m_pList->SetSelectionPolicy(policy);
}
/*!
   Get the number of items in the list..this is the number of items that can be
   selected from, not the number that are selected.
   @return int
   @retval Number of items in the list.
*/
int 
XMMultiSelectionList::GetListCount()
{
  return m_pList->GetListCount();
}
/*!
  Get the set of values that are in the list.  This is the set of values available,
  not the selected set.
  @return XmStringTable
  @retval The 'list of strings' directly in the table.  This should therefore not be 
          released by the caller.
*/
XmStringTable 
XMMultiSelectionList::GetListValues()
{
  return m_pList->GetListValues();
}
/*!
   Return the number of items currently selected in the list.
   @return int
   @retval Selected item count (anywhere from 0 to GetListCount).
*/
int 
XMMultiSelectionList::GetSelectedListCount()
{
  return m_pList->GetSelectedListCount();
}
/*!
   @return XmStringTable
   @retval The set of selected items.  This is a pointer into to resources of the widget
           so don't free the pointer.
*/
XmStringTable 
XMMultiSelectionList::GetSelectedItems()
{
  return m_pList->GetSelectedItems();
}
/*!
   Sets the current list of items to the ordered vector of strings passed in.
   @param items (std::vector<std::string>) The ites to put in the listbox.
*/
void 
XMMultiSelectionList::setItems(std::vector<std::string> items)
{
  Widget w = m_pList->getid();

  // First empty the list:

  XmListDeleteAllItems(w);

  // Marshall the items into a set of XmString pointers:

  XmString* pStrings = new XmString[items.size()];
  
  for(int i=0; i < items.size(); i++) {
    pStrings[i] = XmStringCreateLocalized(const_cast<char*>(items[i].c_str()));
  }
  XmListAddItems(w, pStrings, items.size(), 0);

  // Release string storage:

  for (int i=0; i< items.size(); i++) {
    XmStringFree(pStrings[i]);
  }

  delete []pStrings;
}
