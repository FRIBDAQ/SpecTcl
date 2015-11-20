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

#ifndef XMMENUS_H
#define XMMENUS_H

#ifndef XMWIDGET_H
#include "XMWidget.h"
#endif

#ifndef XMPUSHBUTTON_H
#include "XMPushbutton.h"
#endif

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

		  XMPulldown(const char *n, Widget &parent, Cardinal max_items,
			     ArgList l=NULL, Cardinal num_args=0);
		  XMPulldown(const char *n, XMWidget &parent, Cardinal max_items,
			     ArgList l = NULL, Cardinal num_args = 0);
		  ~XMPulldown();
		  /* Label the pulldown button */

		  void Label(const char *label);

		  /* Set the toggle button policy */

		  void RadioMenu(); /* Request toggles be radio buttons */
		  void RadioForceOne(); /* Request one toggle always be set */
		  void NoRadioMenu();
		  void RadioNoForceOne();
		  /* Add entries to the menu: */
		  
		  XMPushButton *AddMenuButton(const char *n, 
					      void (*callback)(XMWidget *,
							       XtPointer,
							       XtPointer) = NULL,
					      XtPointer client_data = NULL,
					      ArgList l = NULL,
					      Cardinal num_args = 0
					      );
		  XMToggleButton *AddMenuToggleButton
		                             (const char *n,
					      void (*callback)(XMWidget *,
							       XtPointer,
							       XtPointer) 
					                       = NULL,
					      XtPointer client_data = NULL,
					      ArgList l = NULL,
					      Cardinal num_args = 0
					      );
		  XMWidget   *AddSeparator();
		  XMPulldown *AddSubmenu(const char *n, int max_items,
					 ArgList l = NULL, 
					 Cardinal num_args=0);
		  
		  /* Information functions: */
		  
		  int MenuSize();
		  int MaxMenuSize();
		  XMMenuItem *GetMenuItem(Cardinal index); 
		  XMMenuItem *FindMenuItem(const char *n);
		  XMWidget *GetCascadeButton();
		  /*
		  ** The methods below allow one to traverse the menu list.
                  */

		  XMMenuItem *GetNextMenuItem();	   
		  XMMenuItem *GetFirstMenuItem();
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

		  XMMenuBar(const char *n, Widget parent, Cardinal num_menus,
			    ArgList l = NULL, Cardinal num_args = 0);
		  XMMenuBar(const char *n, XMWidget &parent, Cardinal num_menus,
			    ArgList l = NULL, Cardinal num_args = 0);
                  ~XMMenuBar();

                  /* Methods to add pulldowns: */

                  XMPulldown *AddPulldown(const char *n, int max_items,
					  ArgList l = NULL, 
					  Cardinal num_args=0);
                  XMPulldown *AddHelpPulldown(const char *n, int max_items,
					      ArgList l = NULL,
					      Cardinal num_args = 0);

                  /* Information methods: */

                  int NumMenus();
                  XMPulldown *GetPulldown(const char *n);
                  XMPulldown *GetPulldown(Cardinal index);
                  XMPulldown *GetHelpPulldown();
                  XMMenuItem *GetMenuItem(const char *n);
                  /*
                  ** Methods to allow an iterative walk of the menubars:
		  */
		  XMPulldown *GetNextPulldown();
		  XMPulldown *GetFirstPulldown();
		};


#endif
