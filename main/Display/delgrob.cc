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
*/static const char* Copyright = "(C) Copyright Michigan State University 1994, All rights reserved";
/*
** Facility:
**   Xamine -- NSCL Display program.
** Abstract:
**   delgrob.cc:
**     This file contains code which prompts for and deletes a graphical
**     object.  This is done by presenting a Selection box dialog which
**     contains all of the objects on the currently selected spectrum.
**     The user then either selects an entry from the box or types it in.
** Author:
**   ROn Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/

/*
** Include files:
*/
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>


#include "XMDialogs.h"
#include "XMMultiSelectionList.h"

#include "dispgrob.h"
#include "dispwind.h"
#include "helpmenu.h"
#include "delgrob.h"
#include "grobjmgr.h"
#include "panemgr.h"
#include "errormsg.h"
#include "refreshctl.h"
/*
** External references:
*/
extern char *upcase(char *s);

using namespace std;

/*
** Below we define the help text for the dialog:
*/
static const char *help_text[] = {
  "  This dialog deletes graphical objects.  The list at the top of the\n",
  "work area is the set of deletable graphical objects defined on the\n",
  "spectrum at the time this dialog was popped up.  To refresh the list\n",
  "request the delete function again. You may select several graphical\n",
  "objects to delete.\n",
  "   The buttons at the bottom of the dialog (action area) have the\n",
  "following meanings: \n\n",
  "     Ok       - Delete the indicated graphical object and dismiss the\n",
  "                dialog.\n",
  "     Apply    - Delete the indicated graphical object and do not dismiss\n",
  "                the dialog.\n",
  "     Cancel   - Don't delete anything and dismiss the graphical object\n",
  "     Help     - Display this text\n",
  NULL
  };

static Xamine_help_client_data help = { "Delete_grobj_help",
					NULL,
					help_text
					};
/*
** Below are type flag characters used to identify individual object types
*/
static char typechars[] = " CSBCMSM";
/*
** Keep track of the current spectrum too:
*/
static int specid;
/*

*/

/*
** Make a specific class derived fromt he Multi Selection list for
** Selecting the set of grobs to delete:
*/

class MultiGrobSelectionList : public XMMultiSelectionList
{
  int m_spectrum;
public:
  MultiGrobSelectionList(XMWidget& parent);
  virtual ~MultiGrobSelectionList();

public:
  void stockList(int spectrumId);

  // Callback handlers:

  virtual void OkPressed(XMWidget* pWid, XtPointer cli, XtPointer call);
  virtual void ApplyPressed(XMWidget* pWid, XtPointer cli, XtPointer call);
  virtual void CancelPressed(XMWidget* pWid, XtPointer cli, XtPointer call);

protected:
  
  void scheduleDeletion();	// Delete this object in an idle process.
  void deleteSelectedObjects();	// Delete the selected grobjs.

private:
  static Boolean deleteCallback(XtPointer theObject);
};


//////////////////////////////////////////////////////////////////////////////////////
//
// Implementation of the dialog object:
//

/*!
 Construction:  Just construct the base class with some pre-defined values for 
 the name and title:
 and set the selection mode to extended:

 @param parent XMWidget& - Widget that is the parent of this megawidget.
*/
MultiGrobSelectionList::MultiGrobSelectionList(XMWidget& parent) :
  XMMultiSelectionList( const_cast<char*>("GrobjChooser"), parent, 
			const_cast<char*>("Select objects to delete"))
{
  SetSelectionPolicy(XmEXTENDED_SELECT);
  SetHelpText(help_text);
}

/*!
  Destruction just chains to the parent class destructors:
*/
MultiGrobSelectionList::~MultiGrobSelectionList()
{}

/*!
   Stock the list box with the set of graphical objects defined on the specified spectrum:
   @param spectrumId - Id of the spectrum whose objects will be listed:

*/
void
MultiGrobSelectionList::stockList(int spectrumId)
{
  vector<string> objectNames;	// Names of the objects.

  // Get the names of the objects defined on this spectrum:
  
  m_spectrum = spectrumId;
  int nObjects = Xamine_GetSpectrumObjectCount(m_spectrum);


  grobj_generic** objects = new grobj_generic*[nObjects];
  Xamine_GetSpectrumObjects(m_spectrum, objects, nObjects, True);

  for (int i= 0; i < nObjects; i++) {
    grobj_name aname;
    objects[i]->getname(aname);
    objectNames.push_back(string(aname));
  }

  // Set them as the list box entries:

  setItems(objectNames);

  // Delete the dynamic storage:

  delete []objects;


}

/*!
   Handles the Ok button:
   - Call deleteSelectedObjects
   - Schedule our deletion (Ok dismisses the dialog)
   @param pWid (XMWidget*)  - Pointer to the encapsulated widget (I think this is the ok button).
   @param cli (XtPointer)   - Client data (not meaningful I think).
   @param call (XtPointer)  - Call data.  Not so meaningful.
*/
void
MultiGrobSelectionList::OkPressed(XMWidget* pWid, XtPointer cli, XtPointer call)
{
  deleteSelectedObjects();
  scheduleDeletion();
}
/*!
  Handles the apply button. Same as the 
  OkPressed member but does not schedule deletion:
*/
void
MultiGrobSelectionList::ApplyPressed(XMWidget* pWid, XtPointer cli, XtPointer call)
{
  deleteSelectedObjects();
}
/*!
  Cancel callback, on the other hand just schedules deletion.
*/
void
MultiGrobSelectionList::CancelPressed(XMWidget* pWid, XtPointer cli, XtPointer call)
{
  scheduleDeletion();
}

/*
** Set up a work procedure that deletes this object.  THis is done to ensure that
** nothing on the call stack is actively referencing the objects or its member data
** when it's being deleted.
*/
void
MultiGrobSelectionList::scheduleDeletion()
{
  XtAppContext context = XtWidgetToApplicationContext(this->getid());
  XtAppAddWorkProc(context, MultiGrobSelectionList::deleteCallback, 
		   reinterpret_cast<XtPointer>(this));
}
/*
 *  Called to delete the set of objects that are currently selected in the list box.
 */
void
MultiGrobSelectionList::deleteSelectedObjects()
{
  // Delete each object that's selected:

  int             numSelected  = GetSelectedListCount();
  XmStringTable   selections   = GetSelectedItems();


  for (int i=0; i < numSelected; i++) {

    char* name;
    XmStringGetLtoR(selections[i], XmFONTLIST_DEFAULT_TAG, &name);
    int             nObjects     = Xamine_GetSpectrumObjectCount(m_spectrum);
    grobj_generic** objects      = new grobj_generic*[nObjects];
    Xamine_GetSpectrumObjects(m_spectrum, objects, nObjects, True);


    for (int j=0; j < nObjects; j++) {
      grobj_name objName;
      objects[j]->getname(objName);
      if (strcmp(name, objName) == 0) {
	Xamine_DeleteGrobj(m_spectrum, objects[j]->getid());
	break;
      }

    }

    delete []objects;

    XtFree(name);
  }
  // Restock the list: 

  stockList(m_spectrum);

  // Redraw the spectrum window:

  Xamine_RedrawSelectedPane();
}

/*
** Callback for to delete the dialog in a work proc.
*/
Boolean
MultiGrobSelectionList::deleteCallback(XtPointer theObject)
{
  MultiGrobSelectionList* pObject = reinterpret_cast<MultiGrobSelectionList*>(theObject);
  pObject->UnManage();
  delete pObject;
  return True;
}

////////////////////////////////////////////////////////////////////////////////////////////
//

/*!
   Called in response to the delete object menu entry. All we need to do is create the 
   MultiGrobSelectionList object, manage it and let nature take its course.
   @param widget (XMWidget*) - Encasulated widget of the menu entry.
                              Used to parent the dialog.		      
   @param cd (XtPointer) - Client data, unused.
   @param ud (XtPointer) - User data includes  the event e.g.(also ignored).

*/
void
Xamine_DeleteObject(XMWidget* widget, XtPointer ud, XtPointer cd)
{
  XMWidget*       pManager = Xamine_Getpanemgr();
  win_attributed* pAtt     = Xamine_GetSelectedDisplayAttributes();
  if (pAtt == NULL) {
    Xamine_error_msg(pManager, 
		     "There is no spectrum in this pane");
    return;
  }

  MultiGrobSelectionList* pObject = new MultiGrobSelectionList(*widget);
  pObject->stockList(pAtt->spectrum());
  pObject->Manage();
}
