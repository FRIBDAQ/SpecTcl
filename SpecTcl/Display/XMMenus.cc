/*
** Facility:
**   Xamine MOTIF C++ support.
** Abstract:
**   XMMenus.cc   - This file contains method code to support the XMMenus
**                  classes.  These class allows the client to construct
**                  menu items a bit more easily than direct motif calls.
**                  Methods for the following  classes are present:
**                             XMPulldown - Pulldown menu.
**                             XMMenuBar  - Menu bar class.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** Version information:
**    @(#)XMMenus.cc	8.1 6/23/95 
*/

/*
** Include files:
*/
#include <stdio.h>
#include <Xm/Separator.h>
#include "XMMenus.h"

/*
** Declarations:
*/

extern "C" {
             void exit(int);
	   }


/*
** Method description:
**    BuildMenu   - This private method builds a pulldown menu.
**                  At this point, nothing has been done since we've invoked
**                  the default constructor of the XMManaged widget which
**                  just copies the widget name.
** Formal Parameters:
**    Cardinal max_items:

**      The maximum number of items in the menu.  This includes buttons,
**      submenus and yes, even separators.
**    Widget parent:
**      The parent widget for the menu... this should either be another menu
**      or a menu bar.
**    ArgList l:
**      A posibly null argument list of resource modifiers.
**    Cardinal num_args:
**      A possibly zero number entries in l.
*/
void XMPulldown::BuildMenu(Cardinal max_items, Widget parent,
			   ArgList l, Cardinal num_args)
{

  /* Attempt to get the required storage: */

  if(max_items == 0) {
    fprintf(stderr, "Error from XMPulldown constructor... max_items = 0\n");
    exit(-1);
  }
  menu_count     = 0;
  max_menu_items = max_items;
  menu_items = new XMMenuItem[max_items];
  if(menu_items == NULL) {
    fprintf(stderr, "Error from XMPulldown constructor...%s\n",
	    "Unable to allocate menu_items");
    exit(-1);
  }
  for(int i = 0; i < max_menu_items; i++) {
    menu_items[i].item = NULL;
    menu_items[i].type = Unused;
  }

  /* Create the pulldown menu: */

  id = XmCreatePulldownMenu(parent, name, l, num_args);

  /* Now we create a cascade button which is linked to the pulldown via 
  ** the XmNsubMenuId resource.
  */
  pd_button = new XMCascadeButton(name, parent);
  pd_button->Label(name);
  pd_button->SetAssociatedMenu(id);
  pd_button->SetMnemonic(name[0]);
			
  if(pd_button == NULL) {
    fprintf(stderr,
         "XMPulldown constructor error...Could not make new cascade button\n");
  }
}

/*
** Method Description:
**   ~XMPulldown      - Destructor for the pulldown menu class.
**                      We must destroy the cascade button widget as well
**                      as all the widgets in the menu list.
*/
XMPulldown::~XMPulldown()
{
  delete pd_button;		/* Destroy the cascade button. */


  /* Destroy all the widgets and submenus: */

  for(int idx = 0; idx < menu_count; idx++) {
    XMManagedWidget *mw;
    XMPushButton    *pb;
    XMToggleButton  *tb;
    XMPulldown      *pd;
    switch(menu_items[idx].type) {
    case Unused:
      break;
    case Separator:
      mw = (XMManagedWidget *)menu_items[idx].item;
      delete mw;
      break;
    case Button:
      pb = (XMPushButton *)menu_items[idx].item;
      delete pb;
      break;
    case ToggleButton:
      tb = (XMToggleButton *)menu_items[idx].item;
      delete tb;
      break;
    case Submenu:
	pd = (XMPulldown *)menu_items[idx].item;
      delete pd;
      break;
    default:
      fprintf(stderr,
	      "Error in XMMenu destructor... invalid item type\n");
      break;
    }
  }
  /* Destroy the book keeping storage: */

  delete []menu_items;

}

/*
** Method Description:
**   XMPulldown::AddMenuButton    - This method adds a pushbutton menu entry to the
**                      pulldown menu.  This is done at the end of the
**                      current list of menu items.
** Formal Parameters:
**   char *n:
**     Name of the button, also used as the label string.
**   void (*callback)(XMWIdget, XtPointer, XtPointer):
**     Callback to be associated with the activation of the button.
**     NULL for no callback.
**   XtPointer client_data:
**     Possibly null client data for the callback.
**   ArgList l:
**     Possibly null argument list of resource overrides for the pushbutton.
**     Most commonly used to set up the button mnemonic, or to control the
**     initial sensitivity of the button.
**   Cardinal num_args:
**     Possibly zero argument list size.  
*/
XMPushButton *XMPulldown::AddMenuButton(char *n, 
					void (*callback)(XMWidget *, 
							 XtPointer,
							 XtPointer),
					XtPointer client_data,
					ArgList l,
					Cardinal num_args)
{
  XMPushButton *pb;
  /*  First ensure that there's room: */

  if(menu_count >= max_menu_items) {
    fprintf(stderr,
	    "Error in XMPulldown::AddmenuButton - Maximum menu size exceeded\n"
	    );
    exit(-1);
  }
  menu_items[menu_count].item = pb = new XMPushButton(n,
						      *this,
						      callback,
						      client_data);
  if(pb == NULL) {
    fprintf(stderr,
	    "Error in XMPulldown::AddmenuButton - unable to new pushbutton\n");
    exit(-1);
  }
  /*  Set the item list properties: */

  for(int i = 0; i < num_args; i++) {
    pb->SetAttribute(l[i].name, l[i].value);
  }
  menu_items[menu_count].type = Button;

  /* Count one more entry and return the widget object pointer. */

  menu_count++;
  return pb;
}

/*
** Method Description:
**   XMToggleButton *XMPulldown::AddMenuToggleButton  - 
**                                          This function adds a toggle button
**                                          to the pulldown menu.
** Formal Parameters:
**   char *n:
**     Name of the button and default label.
**   void (*callback)(XMWidget *, XtPointer, XtPointer):
**     Pointer to the callback or NULL If there isn't to be one.
**   XtPointer client_data:
**     Call back function client data or NULL if there isn't any.
**   ArgList l:
**     Possibly NULL set of override resources.
**   Cardinal num_args:
**     Possibly zero count of override resources
** Returns:
**    Pointer to toggle button widget object.
*/
XMToggleButton *XMPulldown::AddMenuToggleButton(char *n, 
						void (*callback)(XMWidget *,
								 XtPointer,
								 XtPointer),
						XtPointer client_data,
						ArgList l, 
						Cardinal num_args)
{
  XMToggleButton *pb;
  /*  First ensure that there's room: */

  if(menu_count >= max_menu_items) {
    fprintf(stderr,
	    "Error in XMPulldown::AddmenuToggleButton - Maximum menu size exceeded\n"
	    );
    exit(-1);
  }
  menu_items[menu_count].item = pb = new XMToggleButton(n,
							*this,
							callback,
							client_data);
  if(pb == NULL) {
    fprintf(stderr,
	    "Error in XMPulldown::AddmenuButton - unable to new pushbutton\n");
    exit(-1);
  }
  /*  Set the item list properties: */

  for(int i = 0; i < num_args; i++) {
    pb->SetAttribute(l[i].name, l[i].value);
  }
  menu_items[menu_count].type = ToggleButton;

  /* Count one more entry and return the widget object pointer. */

  menu_count++;
  return pb;
}
						

/*
** Method Description:
**   XMPulldown::AddSeparator   - This method adds a separator object to a menu.
**                    A separator looks like a horizontal line that spans the
**                    width of the menu.
** Returns:
**   Pointer to the created object widget.
*/
XMWidget *XMPulldown::AddSeparator()
{
  XMManagedWidget *sep;
  char n[30];

  /* Make sure there's room: */

  if(menu_count >= max_menu_items) {
    fprintf(stderr,
	    "Error in XMPullDown::AddSeparator. Too many menu items\n");
    exit(-1);
  }

  /* Now create the widget: */

  sprintf(n, "Sep_%d", menu_count);
  menu_items[menu_count].item = sep = 
                                new XMManagedWidget(n,
						    xmSeparatorWidgetClass,
						    *this);
  if(sep == NULL) {
    fprintf(stderr,
	    "Error in XMPullDown::AddSeparator.. Failed new XMManagedWidget\n");
    exit(-1);
  }
  /* Set the entry type and return to the caller: */

  menu_items[menu_count].type = Separator;
  menu_count++;

  return sep;

}

/*
** Method Description:
**    XMPulldown::AddSubmenu  - This method adds a submenu to the parent
**                              menu.  The submenu is a pull right menu.
**                              This is done by creating a new Pulldown
**                              with 'this' as the parent.
** Formal Parameters:
**   char *n:
**     Pulldown name and button label.
**   int size:
**     Maximum number of items in the menu
**   ArgList l:
**     Possibly null list of resource modifiers.
**   Cardinal num_args:
**     Possibly zero count of the argument list.
** Returns:
**   Pointer to the created widget object.
*/
XMPulldown *XMPulldown::AddSubmenu(char *n, int size, ArgList l, Cardinal num_args)
{
  XMPulldown *pd;

  /* Check that we won't be exceeding bounds on menu size: */

  if(menu_count >= max_menu_items) {
    fprintf(stderr,
	    "Error in XMPulldown::AddSubmenu - menu size exceeded\n");
    exit(-1);
  }
  /*  Generate the new pulldown: */

  menu_items[menu_count].item = pd = 
                                new XMPulldown(n, *this, size, l, num_args);
  if(pd == NULL) {
    fprintf(stderr,
	    "Error in XMPulldown::AddSubmenu - new XMPulldown failed\n");
    exit(-1);

  }
  /* Set type, increment the size and return the pulldown */

  menu_items[menu_count].type = Submenu;
  menu_count++;

  return pd;
}

/*
** Method Description:
**    XMPulldown::FindMenuItem  - Locates a menu item in a Pulldown.
**                                If there are descendents submenus,
**                                they too are searched.  Note that the
**                                name could be that of a submenu legally.
** Formal Parameters:
**   char *n:
**     Name of the item to find.
** Returns:
**   Pointer to the entry located or
*/
XMMenuItem *XMPulldown::FindMenuItem(char *n)
{
  XMMenuItem *here, *found;

  here = menu_items;
  for(int i = 0; i < menu_count; i ++,here++) {
    XMPulldown *sm;
    switch(here->type) {
    case Unused:
      break;			/* No associated widget. */
    case Separator:
    case Button:
    case ToggleButton:
      if(strcmp(n, here->item->getname()) == 0)
	return here;
      break;
    case Submenu:		/* Search recursively: */
      {
	sm = (XMPulldown *)here->item;
	found = sm->FindMenuItem(n);
	if(found) return found;
      }
      break;
    default:
      fprintf(stderr,
	      "Error XMPulldown::FindMenuItem invalid menu item type\n");
      exit(-1);
    }
  }
  return NULL;
}

/*
** Method Description:
**    XMPulldown::GetNextMenuItem   - This function returns the next menu
**                                   item in an interative retrieval.
** Returns:
**   Pointer to the menu entry or NULL if at the end of the list.
*/
XMMenuItem *XMPulldown::GetNextMenuItem()
{
  XMMenuItem *here;

  if(menu_cursor < menu_count) {
    here = &menu_items[menu_cursor];
    menu_cursor++;
  }
  else {
    here = NULL;
  }
  return here;
}

/*
** Method Description:
**    XMMenuBar::mbCreate     - This method is used by the creation
**                              operators (constructors) of the menubar class.
**                              We do the following:
**                                Initialize attribute values.
**                                Allocate storage for the menubar book-keeping
**                                   fields.
**                                Create the menubar widget.
**                             Note, while we inherit from XMManagedWidget,
**                             we have not yet been created as the constructor
**                             has called a null constructor which has only copied
**                             the widget name into the name field.
** Formal Parameters:
**  Widget parent:
**     Parent widget id.
**  Cardinal num_menus:
**     Maximum number of pull down menus the bar will support.
**  ArgList l:
**     Possibly null list of resource overrides.
**  Cardinal num_args:
**     Possibly zero count of elements in l.
*/
typedef XMPulldown *pdpointer;
void  XMMenuBar::mbCreate(Widget parent, Cardinal num_menus, ArgList l,
			  Cardinal num_args)
{
  /* Initialize the attribute fields */

  menu_count = 0;
  max_menu_items = num_menus;
  help_pulldown = NULL;		/* No help pulldown yet. */

  /* Allocate storage for the pulldown list: */

  menu_items = new pdpointer[max_menu_items];
  if(menu_items == NULL) {
    fprintf(stderr,
	    "XMMenuBar constructor error.. could not allocate menu list\n");
    exit(-1);
  }
  for(int i = 0; i < max_menu_items; i++)
    menu_items[i] = NULL;

  /* Create the menu bar: */

  id = XmCreateMenuBar(parent, name, l, num_args);

}

/*
** Method Description:
**   ~XMMenuBar   - Destructor for the menu bar class.
**                  We must delete all the pulldowns that have been
**                  added and we must free the storage taken up by the
**                  pulldown widget list.
*/
XMMenuBar::~XMMenuBar()
{
  

  /* Kill the pulldowns: */

  for(int i = 0; i < max_menu_items; i++) {
    delete menu_items[i];
  }
  delete []menu_items;		/* Kill the pointer storage. */

  /* The managed widget destructor will take care of killing the menubar
  ** widget
  */
}

/*
** Method Description:
**   XMMenuBar::AddPulldown - This method adds a pulldown menu to a menu bar.
**                            If there is a free element in the widget list,
**                            a pulldown is dynamically created and added
**                            to the list. 
** Formal Parameters:
**   char *n:
**     Name of the pulldown menu -- will also wind up being the menu label.
**   int max_items:
**     Number of items in the menu.
**   ArgList l:
**     Possibly null set of resource overrides for the pulldown menu widget.
**     Note to override the cascade button widget resources, you must get
**     the cascade button widget and then SetAttributes on it.
**   Cardinal num_args:
**     Possibly zero argument list count.
** Returns:
**   A pointer to the pulldown menu object so that the user can stock it
**   with menu entries.
**/

XMPulldown *XMMenuBar::AddPulldown(char *n, int max_items,
				   ArgList l, Cardinal num_args)
{
  XMPulldown *pd;

  /*  Check to see that we have room for the new kid: */

  if(menu_count >= max_menu_items) {
    fprintf(stderr,
	    "XMMenuBar::AddPulldown - Menu bar size exceeded\n");
    exit(-1);
  }

  /* Create and add the new pulldown: */

  menu_items[menu_count] = pd = 
                              new XMPulldown(n, *this, max_items, l, num_args);
  if(pd == NULL) {
    fprintf(stderr,
	    "XMMenuBar::AddPulldown - Pulldown creation failed\n");
    exit(-1);
  }

  /* Update the count and return the widget: */

  menu_count++;
  return pd;
}

/**
** Method Description:
**   XMMenuBar::AddHelpPulldown - Adds a pulldown menu which will be
**                                registerd as the help menu for the bar.
** Formal Parameters:
**   char *n:
**     Name/label of the pulldown menu.
**  int max_items:
**     Maximum number of items in the pulldown.
**  ArgList l:
**     Possibly null argument list containing resource overrides.
**  Cardinal num_args:
**    Possibly zero count of arguments in l.
** Returns:
**   Pointer to the pulldown object created.
**  
*/
XMPulldown *XMMenuBar::AddHelpPulldown(char *n, int max_items,
				       ArgList l, Cardinal num_args)
{
  if(help_pulldown) {		/* Attempting second help menu */
    fprintf(stderr,
	    "XMMenuBar::AddHelpPulldown--attempted to add second pulldown \n");
    exit(-1);
  }
  help_pulldown = AddPulldown(n, max_items, l, num_args); /* It's a pulldown */
  if(help_pulldown == NULL) {
    fprintf(stderr,
	    "XMMenuBar::AddHelpPulldown - Add of help pulldown failed\n");
    exit(-1);
  }
  SetAttribute(XmNmenuHelpWidget, help_pulldown->GetCascadeButton()->getid());

  return help_pulldown;
}

/*
** Method Description:
**   XMMenuBar::GetPulldown   - Returns a pointer to a named pulldown object
**                               or NULL if there isn't a match
** Formal Parameters:
**   char *n:
**     The name of the pulldown sought.
** Returns:
**    Pointer to the pulldown or NULL if there is no match.
*/
XMPulldown *XMMenuBar::GetPulldown(char *n)
{
  for(int i = 0; i < menu_count; i++) {
    if(strcmp(menu_items[i]->getname(), n) == 0) 
      return menu_items[i];
  }
  return NULL;
}

/*
** Method Description:
**   XMMenuBar::GetMenuItem  - Searches the menu hiearchy attached to this
**                             menu bar for a match with a widget name.
** Formal Parameters:
**   char *n:
**     Name of the widget to hunt for.
** Returns:
**    Pointer to the XMMenuItem which describes the menu item or NULL if
**    there is no match in the menu hierarchy.
**/

XMMenuItem *XMMenuBar::GetMenuItem(char *n)
{
  XMMenuItem *theone;

  for(int i = 0; i < menu_count; i++) {
    theone = menu_items[i]->FindMenuItem(n);
    if(theone) 
      return theone;
  }
  return NULL;

}

/*
** Method Description:
**   XMMenuBar::GetNextPulldown  - This method iterates through the pulldown
**                                 menu list associated with the menu bar
**                                 and returns the next pulldown
** Returns:
**   XMPulldown * pointer to the next pulldown or NULL If there aren't any more
*/
XMPulldown *XMMenuBar::GetNextPulldown()
{
  XMPulldown *here;

  if(menu_cursor < menu_count) {
    here = menu_items[menu_cursor];
    menu_cursor++;
  }
  else
    here = NULL;
  return here;
}
