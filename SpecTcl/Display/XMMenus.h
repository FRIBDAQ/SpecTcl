/*
** Facility:
**   Xamine - Motif support for C++
** Abstract:
**   XMMenus.h  - Defines some classes which support menus and menu-bars.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** Version:
**   @(#)XMMenus.h	8.1 6/23/95 
*/

#ifndef XMMenus_H
#define XMMenus_H

#include "XMWidget.h"
#include "XMPushbutton.h"
#include <Xm/RowColumn.h>
#include <stdlib.h>
/*
**  This is a pulldown menu... it contains a puldown menu widget
**  (The inherited widget), a cascade button which activates the pulldown,
**  and a list of menu items which can be either buttons, XMPulldowns or
**  separators.
*/

enum   XMMenuEntryType {
                         Unused,
			 Separator,
			 Button,
			 ToggleButton,
			 Submenu
			 };
struct XMMenuItem {
                    XMWidget *item;
		    XMMenuEntryType type;
		  };
class XMPulldown : public XMManagedWidget
                {
		protected:
		  XMCascadeButton *pd_button;
		  XMMenuItem      *menu_items;
		  Cardinal         menu_count;
		  Cardinal         max_menu_items;
		  Cardinal         menu_cursor;
		  void BuildMenu(Cardinal max_items, Widget parent,
				 ArgList l, Cardinal num_args);
		public:
		  /* Constructors and destructor: */

		  XMPulldown(char *n, Widget &parent, Cardinal max_items,
			     ArgList l=NULL, Cardinal num_args=0) :
			       XMManagedWidget(n)
		    {
		      BuildMenu(max_items, parent, l, num_args);
		    }
		  XMPulldown(char *n, XMWidget &parent, Cardinal max_items,
			     ArgList l = NULL, Cardinal num_args = 0) :
			       XMManagedWidget(n)
			       {
				 BuildMenu(max_items, parent.getid(), 
					   l, num_args);
			       }
		  ~XMPulldown();
		  /* Label the pulldown button */

		  void Label(char *label) { pd_button->Label(label); }

		  /* Set the toggle button policy */

		  void RadioMenu() /* Request toggles be radio buttons */
		    { SetAttribute(XmNradioBehavior, (XtArgVal)True); }
		  void RadioForceOne() /* Request one toggle always be set */
		    { SetAttribute(XmNradioAlwaysOne,
				   (XtArgVal)True);
		    }
		  void NoRadioMenu()
		    { SetAttribute(XmNradioBehavior, (XtArgVal)False); }
		  void RadioNoForceOne()
		    { SetAttribute(XmNradioAlwaysOne,
				   (XtArgVal)False);
		    }	      
		  /* Add entries to the menu: */
		  
		  XMPushButton *AddMenuButton(char *n, 
					      void (*callback)(XMWidget *,
							       XtPointer,
							       XtPointer) = NULL,
					      XtPointer client_data = NULL,
					      ArgList l = NULL,
					      Cardinal num_args = 0
					      );
		  XMToggleButton *AddMenuToggleButton
		                             (char *n,
					      void (*callback)(XMWidget *,
							       XtPointer,
							       XtPointer) 
					                       = NULL,
					      XtPointer client_data = NULL,
					      ArgList l = NULL,
					      Cardinal num_args = 0
					      );
		  XMWidget   *AddSeparator();
		  XMPulldown *AddSubmenu(char *n, int max_items,
					 ArgList l = NULL, 
					 Cardinal num_args=0);
		  
		  /* Information functions: */
		  
		  int MenuSize() { return menu_count; }
		  int MaxMenuSize() { return max_menu_items; }
		  XMMenuItem *GetMenuItem(Cardinal index) 
		    { return ( (index < menu_count) ?
			      &(menu_items[index]) : (XMMenuItem *)NULL); }
		  XMMenuItem *FindMenuItem(char *n);
		  XMWidget *GetCascadeButton() { return pd_button; }
		  /*
		  ** The methods below allow one to traverse the menu list.
                  */

		  XMMenuItem *GetNextMenuItem();	   
		  XMMenuItem *GetFirstMenuItem()
		    { menu_cursor = 0;
		      return GetNextMenuItem();
		    }
		};

/*
**   Menu bar widget class.
*/
class XMMenuBar : public XMManagedWidget
                {
		protected:
                  XMPulldown *help_pulldown;
                  XMPulldown **menu_items;
		  Cardinal   menu_cursor;
                  Cardinal   menu_count;
                  Cardinal   max_menu_items;
		  void mbCreate(Widget parent,Cardinal num_menus,
				ArgList l, Cardinal num_args);
		public:

                  /* Constructors and destructors: */

		  XMMenuBar(char *n, Widget parent, Cardinal num_menus,
			    ArgList l = NULL, Cardinal num_args = 0) :
                   XMManagedWidget(n)
		    {
		      mbCreate(parent, num_menus, l, num_args);
		    }
		  XMMenuBar(char *n, XMWidget &parent, Cardinal num_menus,
			    ArgList l = NULL, Cardinal num_args = 0) :
                     XMManagedWidget(n)
		    {
		      mbCreate(parent.getid(), num_menus, l, num_args);
		    }
                  ~XMMenuBar();

                  /* Methods to add pulldowns: */

                  XMPulldown *AddPulldown(char *n, int max_items,
					  ArgList l = NULL, 
					  Cardinal num_args=0);
                  XMPulldown *AddHelpPulldown(char *n, int max_items,
					      ArgList l = NULL,
					      Cardinal num_args = 0);

                  /* Information methods: */

                  int NumMenus() { return menu_count; }
                  XMPulldown *GetPulldown(char *n);
                  XMPulldown *GetPulldown(Cardinal index)
		    { return ( (index < menu_count) ?
			        menu_items[index] : (XMPulldown *)NULL); }
                  XMPulldown *GetHelpPulldown()
		    { return help_pulldown; }
                  XMMenuItem *GetMenuItem(char *n);
                  /*
                  ** Methods to allow an iterative walk of the menubars:
		  */
		  XMPulldown *GetNextPulldown();
		  XMPulldown *GetFirstPulldown()
		    {  menu_cursor = 0; 
		       return GetNextPulldown(); 
		     }
		};


#endif
