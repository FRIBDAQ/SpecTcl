/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

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
**   Motif C++ class support.
** Abstract:
**   XMDialogs.h  - This file contains support for C++ access to Motif
**                  dialogs.
** Author:
**  Ron Fox
**  NSCL
**  Michigan State University
**  East Lansing, MI 48824-1321
** Versioning
**   @(#)XMDialogs.h	8.2 7/10/95 
*/

#ifndef XMDIALOGS_H
#define XMDIALOGS_H

#include <stdio.h>

#include <X11/StringDefs.h>
#include <Xm/MessageB.h>
#include <Xm/SelectioB.h>
#include <Xm/FileSB.h>

#ifndef XMWIDGET_H
#include "XMWidget.h"
#endif

#ifndef XMPUSHBUTTON_H
#include "XMPushbutton.h"
#endif

#ifndef XMMANAGERS_H
#include "XMManagers.h"
#endif

#ifndef XMCALLBACK_H
#include "XMCallback.h"
#endif

/*
** #Defines:
**
*/
#define XMFILE_DEFAULT_DIR     "./"
#define XMFILE_DEFAULT_DIRMASK "./*"


/* The message box is the parent widget type of all message like dialogs: */

class XMMessageBox : public XMManagedWidget
{
 protected:
  XMPushButton *okbutton;
  XMPushButton *cancelbutton;
  XMPushButton *helpbutton;
  void GetButtons();

 public:
  /* Constructors and destructors */
  
  virtual ~XMMessageBox();
  
  XMMessageBox(char *n);
  
  XMMessageBox(Widget w);
  
  /* Get Button Widgets: */
  
  XMPushButton *GetOkButton();  
  XMPushButton *GetCancelButton();
  XMPushButton *GetHelpButton();
  
  /* Modify the dialog text: */
  
  virtual void SetText(char *txt);
  
  /* Add Ok/Cancel callbacks: */
  
  Callback_data *AddOkCallback(void (*cb)(XMWidget *w,
					  XtPointer, 
					  XtPointer),
			       XtPointer cd = NULL);
  
  Callback_data *AddCancelCallback(void (*cb)(XMWidget *w,
					      XtPointer,
					      XtPointer),
				   XtPointer cd = NULL);
  
  /* Label cancel/Help/OK buttons */
  
  void LabelCancelButton(const char *txt);
  void LabelOkButton(const char *txt);
  void LabelHelpButton(const char *txt);
  
  /* Set default button types: */
  
  void DefaultToOk();
  void DefaultToCancel();
  void DefaultToHelp();
  
  void Show();
  void Hide();
  void SetModal(unsigned char modality);
};



/* Error dialogs specialize the Message box widget for error messages */

class XMErrorDialog : public XMMessageBox
{
 public:
  /*  Constructors: */
  XMErrorDialog(const char *n, Widget parent, char *msg,
		void (*cb)(XMWidget *,
			   XtPointer, XtPointer) = NULL,
		XtPointer cbd = NULL,
		ArgList list = NULL, Cardinal argcount = 0);

  XMErrorDialog(const char *n, XMWidget &parent, char *msg,
		void (*cb)(XMWidget *,
			   XtPointer, XtPointer) = NULL,
		XtPointer cbd = NULL,
		ArgList list = NULL, Cardinal argcount = 0);

  XMErrorDialog(Widget w);
};

/* Information dialogs specialize MessageBox for informative messages: */


class XMInformationDialog : public XMMessageBox
{
 public:
  /*  Constructors: */
  XMInformationDialog(char *n,Widget parent, char *msg,
		      void (*cb)(XMWidget *,
				 XtPointer, XtPointer) = NULL,
		      XtPointer cbd = NULL,
		      ArgList list = NULL, Cardinal argcount=0);

  XMInformationDialog(char *n, XMWidget &parent, char *msg,
		      void (*cb)(XMWidget *,
				 XtPointer, XtPointer) = NULL,
		      XtPointer cbd = NULL,
		      ArgList list = NULL, Cardinal argcount=0);

  XMInformationDialog(Widget w);
};

/*  Message Dialogs: Put up interaction messages: */


class XMMessageDialog : public XMMessageBox
{
 public:
  /*  Constructors: */
  XMMessageDialog(char *n,Widget parent, char *msg,
		  void (*cb)(XMWidget *,
			     XtPointer, XtPointer) = NULL,
		  XtPointer cbd = NULL,
		  ArgList list = NULL, Cardinal argcount=0);
  XMMessageDialog(char *n, XMWidget &parent, char *msg,
		  void (*cb)(XMWidget *,
			     XtPointer, XtPointer) = NULL,
		  XtPointer cbd = NULL,
		  ArgList list = NULL, Cardinal argcount=0);
  XMMessageDialog(Widget w);
};

/*  Question dialogs ask users questions about things: */

class XMQuestionDialog : public XMMessageBox
{
 public:
  /*  Constructors: */
  XMQuestionDialog(char *n,Widget parent, char *msg,
		   void (*cb)(XMWidget *,
			      XtPointer, XtPointer) = NULL,
		   XtPointer cbd = NULL,
		   ArgList list = NULL, Cardinal argcount=0);
  XMQuestionDialog(char *n, XMWidget &parent, char *msg,
		   void (*cb)(XMWidget *,
			      XtPointer, XtPointer) = NULL,
		   XtPointer cbd = NULL,
		   ArgList list = NULL, Cardinal argcount=0);
  XMQuestionDialog(Widget w);
};
/*
**  XMQuestioner is a self contained version of the XMQuestionDialog
**  It makes available virtual functions for the Ok and Cancel buttons
**  which both just unmanage the dialog.  The user is supposed to derive
**  a class specific to their own application which replaces one or more
**  of the callback methods.
*/

class XMQuestioner : public XMQuestionDialog {
 public:
  XMQuestioner(char *n, Widget parent, char *msg,
	       XtPointer cbd,
	       ArgList list = NULL, Cardinal argcount = 0);
  XMQuestioner(char *n, XMWidget &parent, char *msg,
	       XtPointer cbd,
	       ArgList list = NULL, Cardinal argcount = 0);
  ~XMQuestioner();
 protected:
  XMCallback<XMQuestioner> yescallback;
  XMCallback<XMQuestioner> nocallback;
  virtual void Yescb(XMWidget *wid, XtPointer ud, XtPointer cd);
  virtual void Nocb(XMWidget *wid, XtPointer ud, XtPointer cd);
 private:
};
/* Warning dialogs warn the user of some impending disaster */


class XMWarningDialog : public XMMessageBox
{
 public:
  /*  Constructors: */
  XMWarningDialog(char *n,Widget parent, char *msg,
		  void (*cb)(XMWidget *,
			     XtPointer, XtPointer) = NULL,
		  XtPointer cbd = NULL,
		  ArgList list = NULL, Cardinal argcount=0);
  XMWarningDialog(char *n, XMWidget &parent, char *msg,
		  void (*cb)(XMWidget *,
			     XtPointer, XtPointer) = NULL,
		  XtPointer cbd = NULL,
		  ArgList list = NULL, Cardinal argcount=0);
  XMWarningDialog(Widget w);
};

/*  Working dialogs let the user know something is happening */


class XMWorkingDialog : public XMMessageBox
{
 public:
  /*  Constructors: */
  XMWorkingDialog(char *n,Widget parent, char *msg,
		  void (*cb)(XMWidget *,
			     XtPointer, XtPointer) = NULL,
		  XtPointer cbd = NULL,
		  ArgList list = NULL, Cardinal argcount=0);
  XMWorkingDialog(char *n, XMWidget &parent, char *msg,
		  void (*cb)(XMWidget *,
			     XtPointer, XtPointer) = NULL,
		  XtPointer cbd = NULL,
		  ArgList list = NULL, Cardinal argcount=0);
  XMWorkingDialog(Widget w);
};

/*  In order to prevent a circular include file dependency, helpmenu.h has been
**  textually included here.  At some point another pass through the XM Object system
**  def must be done to eliminate some of these circular dependencies.
*/

/*
 ** Structure definitions:
 */

/* Xamine_help_client_data is a client data structure which is build to
 ** drive the Xamine_display_help callback function.
 */

struct Xamine_help_client_data { 
  char                *name; /* Widget name */
  XMInformationDialog *dialog; /* Widget object */
  char                **text; /* Help text. */
};

/*
 ** The following public items define the help menus which are described in
 ** helpmenu.cc:
 */

extern Xamine_help_client_data Xamine_About;
extern Xamine_help_client_data Xamine_Help_Overview;
/*
** The following are public entry points to the module:
*/

XMInformationDialog *Xamine_help(char *name, 
				 XMWidget *parent, char **help_text);
void                 Xamine_display_help(XMWidget *caller,
					 XtPointer client_data,
					 XtPointer reason = NULL);



/*
**  SelectionBox is the base class for all the selection box dialogs.
**  These include:
**    List selection boxes,
**    File selection dialogs
** and Prompt dialogs.
**   While command widgets are similar in nature they are different since they
**   don't live in dialog shells.
**/
class XMSelection : public XMMessageBox
       {
       protected:
	 XMPushButton *applybutton;
	 void GetButtons();

       public:
	 /*
         ** Constructors and destructors:
	 */
	 XMSelection(char *name);
	 XMSelection(Widget w);
	 virtual ~XMSelection();

	 /*  Set the width of the text field: */

	 void SetTextWidth(Cardinal n);

	 /* Functions to add callbacks to functions:  */

	 Callback_data *AddDoCallback(void (*cb)(XMWidget *, 
						 XtPointer, 
						 XtPointer),
				      XtPointer client_data = NULL,
				      Callback_data **apply = NULL);
	 Callback_data *AddApplyCallback(void (*cb)(XMWidget *,
					  XtPointer,
					  XtPointer),
			       XtPointer client_data  = NULL);

	 /* Get the button widgets: */

	 XMPushButton *GetApplyButton();
	
	 /* Labelling functions */

	 virtual void SetText(char *txt);
	 virtual void SetLabelString(char *txt);

	 /* Label the apply buton: */

	 void LabelApplyButton(char *txt);

	 /* Set default button: */

	 void DefaultToOk();
	 void DefaultToCancel();
         void DefaultToHelp();
	 void DefaultToApply();
       };
/*
** Prompt dialog - This dialog consists of a single line text widget and
**                 associated label string.  By default the Apply button
**                 is not managed.
**/

class XMPromptDialog : public XMSelection
       {
       protected:
	 void GetButtons();

       public:
	 XMPromptDialog(char *n, Widget parent, char *prompt = NULL,
			void (*cb)(XMWidget *,
				   XtPointer, XtPointer) = NULL,
			XtPointer cbd = NULL,
			ArgList list = NULL, Cardinal argcount = 0);
	 XMPromptDialog(char *n, XMWidget &parent, char *prompt = NULL,
			 void (*cb)(XMWidget *,
				    XtPointer, XtPointer) = NULL,
			 XtPointer cbd = NULL,
			 ArgList list = NULL, Cardinal argcount = 0);
	 XMPromptDialog(Widget w);
       };
/*
** XMPrompter specializes XMPromptDialog to encapsulate the behavior of
** the Action area of the dialog as follows:
**    Ok    - Perform virtual function is called and if it returns
**            True then the box is unmanaged.
**    Apply - Perform virtual function is called.
**    Cancel- Box is unmanaged.
**    Help  - Help text is displayed... Either the text last 
**            set via Set Help Text or a default help text which describes
**            in general terms how to use a prompter dialog.
*/
class XMPrompter : public XMPromptDialog {
 public:
  XMPrompter(char *name, Widget parent, char *prompt = NULL,
	     XtPointer calldata = NULL);
  XMPrompter(char *name, XMWidget &parent, char *prompt = NULL,
	     XtPointer calldata = NULL);
  ~XMPrompter();


  void SetHelpText(char **new_help);
  void RevertHelpText();
 protected:
  XMCallback<XMPrompter> ok;
  XMCallback<XMPrompter> apply;
  XMCallback<XMPrompter> cancel;
  XMCallback<XMPrompter> help;
 private:
  virtual void OkCallback(XMWidget *wid, XtPointer userd, XtPointer calld);
  virtual void ApplyCallback(XMWidget *wid, XtPointer userd, XtPointer calld);
  virtual void CancelCallback(XMWidget *wid, XtPointer userd, XtPointer calld);
  virtual void HelpCallback(XMWidget *wid, XtPointer userd, XtPointer calld);
  virtual Boolean Perform(XMWidget *wid, XtPointer userd, XtPointer calld);

  struct Xamine_help_client_data help_info;
};
/*
** A selection dialog has a list of alternatives to choose from.
** The list can be scrolled around and either picked from with the mouse
** or typed in to a text widget.
*/
class XMSelectionDialog : public XMSelection
      {
      public:
	 XMSelectionDialog(char *n, Widget parent, char *prompt = NULL,
			void (*cb)(XMWidget *,
				   XtPointer, XtPointer) = NULL,
			XtPointer cbd = NULL,
			ArgList list = NULL, Cardinal argcount = 0);
	 XMSelectionDialog(char *n, XMWidget &parent, char *prompt = NULL,
			 void (*cb)(XMWidget *,
				    XtPointer, XtPointer) = NULL,
			 XtPointer cbd = NULL,
			 ArgList list = NULL, Cardinal argcount = 0);
	 XMSelectionDialog(Widget w);

	 /*
         ** Selection boxes also need a method to set the list... it can be
         ** set in the argument list, but the following is more convenient
	 */

	 void SetSelectionList(Cardinal list_count, char **selections);
	 void SetVisibleItemCount(Cardinal num_visible);

	 /*
         ** Selection boxes are allowed to restrict choices to items on the
	 ** list.  This gives them an additional callback (XmNnoMatchCallback).
	 */

	 void RestrictChoices();
	 void NoRestrictChoices();
	 Callback_data *AddNoMatchCallback(void (*cb)(XMWidget *,
					    XtPointer, XtPointer),
				 XtPointer client_data = NULL);
       };

/*
** XMSelector:  This class is a self contained version of XMSelectionDialog
**              Self contained means that the class contains methods to
**              respond to the callbacks as well as some additional
**              virtual functions intended to be superceded when the
**              user actually makes use of this class.  The class is not
**              intended to be typically used, Usually, the client will
**              specialize the class, supplying overrides for one or more
**              of the methods:
**                   SetUpList     - Defines the set of items in the list.
**                   Perform       - Application sensitive processing of
**                                   the Ok or Apply buttons.
**                   Constructors/Destructors.
*/
class XMSelector : public XMSelectionDialog
{
  public:
     XMSelector(char *n, Widget parent, char *prompt = NULL,
		XtPointer cbd = NULL,
		ArgList list = NULL, Cardinal argcount = 0);
     XMSelector(char *n, XMWidget &parent, char *prompt = NULL,
		XtPointer cbd = NULL,
		ArgList list = NULL, Cardinal argcount = 0);
     ~XMSelector();

     virtual void SetupList();		/* Set up the selectionlist. */
     void         SetHelpText(char **text);
     void         RevertHelpText();
   protected:
     virtual Boolean Perform(XMWidget *wid, XtPointer cd,
			  int reason, XmString value, int size);
     virtual void OkCb(XMWidget *wid, XtPointer cd, XtPointer ud);
     virtual void NoMatchCb(XMWidget *wid, XtPointer cd, XtPointer ud);
     virtual void ApplyCb(XMWidget *wid, XtPointer cd, XtPointer ud);
     virtual void CancelCb(XMWidget *wid, XtPointer cd, XtPointer ud);
     virtual void HelpCb(XMWidget *wid, XtPointer cd, XtPointer ud);
  private:
     XMCallback<XMSelector> okbuttonCB;
     XMCallback<XMSelector> applybuttonCB;
     XMCallback<XMSelector> cancelbuttonCB;
     XMCallback<XMSelector> helpbuttonCB;
     XMCallback<XMSelector> nomatchCB;

     struct Xamine_help_client_data helpinfo;
 
};
/*
** A file selection dialog is a pair of list boxes.  One selects/displays
** the directories.  Another displays the list of files present in that 
** directory.  There are two text entry areas.  The top one allows the user
** to set a file directory/search mask.  The bottom one allows the user to
** enter filenames directly.  The following buttons are available by default:
**  Ok, Apply, Cancel.  Help is present but default ghosted.
*/
class XMFileListDialog : public XMSelection
       {
       protected:
	 void GetButtons();
								    
       public:
	 XMFileListDialog(char *n, Widget parent, char 
			  *directory = const_cast<char*>(XMFILE_DEFAULT_DIRMASK),
			void (*cb)(XMWidget *,
				   XtPointer, XtPointer) = NULL,
			XtPointer cbd = NULL,
			ArgList list = NULL, Cardinal argcount = 0);
	 XMFileListDialog(char *n, XMWidget &parent, 
			  char *directory =const_cast<char*>( XMFILE_DEFAULT_DIRMASK),
			 void (*cb)(XMWidget *,
				    XtPointer, XtPointer) = NULL,
			 XtPointer cbd = NULL,
			 ArgList list = NULL, Cardinal argcount = 0);
	 XMFileListDialog(Widget w);

	 /* Methods which manipulate the search strings: */
	 void DoSearch(XmString dir);
	 void DoSearch(const char *dir);
	 void DoSearch();
	 char *GetDirectory();
	 char *GetFileMask();
	 char *GetFullSearchString();

	 /* Methods to manipulate the labels: */

	 virtual void SetLabelString(const char *txt);
	 virtual void SetFilterString(const char *txt);

	 /* Set restrictions on the file types searched: */

	 void SetFileTypes(unsigned char fileset);
	 void RestrictChoices();
	 void NoRestrictChoices();

	 /* Add callbacks if no match on filename selected */

	 Callback_data *AddNoMatchCallback(void (*cb)(XMWidget *,
					    XtPointer, XtPointer),
				 XtPointer client_data = NULL);
       };
/*
**  The XMFileSelector Dialog is a self contained version of the 
**  XMFileListDialog.  Self contained, in this case means that the
**  callback functions are redirected to method functions and that
**  the method functions are virtualized to allow descendent application
**  specific instances of these classes to be constructed.
**  The strategy used for virtualizing is similar to that taken by
**  the XMSelector class.  Individual callbacks can be overridden by the
**  user to supply ok, filter, cancel and help callbacks, however,
**  the Ok callback calls the Perform function with the filename string
**  as an argument and in most cases that's all that the client will need
**  to override (as well as supplying constructors and desctructors.
*/
class XMFileSelector : public XMFileListDialog
{
  public:
    XMFileSelector(char *n, Widget parent, 
		   XtPointer calld = NULL,
		   char *directory = const_cast<char*>(XMFILE_DEFAULT_DIRMASK));

    XMFileSelector(char *n, XMWidget &parent, 
		   XtPointer ud = NULL,
		   char *directory = const_cast<char*>(XMFILE_DEFAULT_DIRMASK));


    ~XMFileSelector();
		   
    void SetHelpText(char **text);
    void RevertHelpText();
  protected:
    virtual Boolean Perform(XMWidget *wid, XtPointer ud,
			    char *filename,
			    int reason);
    virtual void OkCb(XMWidget *wid, XtPointer ud, XtPointer cd);
    virtual void NomatchCb(XMWidget *wid, XtPointer ud, XtPointer cd);
    virtual void FilterCb(XMWidget *wid, XtPointer ud, XtPointer cd);
    virtual void CancelCb(XMWidget *wid, XtPointer ud, XtPointer cd);
    virtual void HelpCb(XMWidget *wid, XtPointer ud, XtPointer cd);
			 

  private:
    XMCallback<XMFileSelector> okcb;
    XMCallback<XMFileSelector> nomatchcb;
    XMCallback<XMFileSelector> filtercb;
    XMCallback<XMFileSelector> cancelcb;
    XMCallback<XMFileSelector> helpcb;

    struct Xamine_help_client_data helpinfo;
};

/*
** The  XMCustomDialog class is a useful subclass for building basic
** Customized dialogs.  It is implemented as follows:
**    A dialog shell is created which contains a paned window as a manger
**    child.  The paned window manages two widgets.  A form called
**    work_area which is where the user should lay out the work area of
**    the form and a row_column widget called action_area which contains
**    the action area buttons.
** The idea is that clients will specialize constructors and other methods
** to build up, maintain and process the work_area subwidgets.
** Action area buttons are:
**     OK  - when pressed, the Do call back is initiated.
** Accept  -  When pressed, the Do callback is initiated.
** Cancel  -  When pressed, the Cancel callback is initiated.
** Help    - When pressed, the Help callback is initiated.
*/

class XMCustomDialog : public XMWidget /* Create unmanaged for greater layout ctl. */
{
 protected:
  XMForm        *shell_child;   /* Form present to allow modalization */
  XMPanedWindow *top_manager;	/* Top level manager widget. */
  XMRowColumn   *action_area;	/* The action area widget. */
  XMForm        *work_area;	/* The form which manages the work area */
  XMPushButton  *Ok;		/* Buttons in the action area. */
  XMPushButton  *Apply;
  XMPushButton  *Cancel;
  XMPushButton  *Help;
  void CreateDialog(const char *name, Widget parent, char *title,
		    ArgList l, Cardinal num_args);
 public:
  /* Constructors: */

  XMCustomDialog(const char *name, XMWidget &parent, char *title,
		 ArgList l = NULL, Cardinal num_args = 0); 
  XMCustomDialog(const char *name, Widget parent, char *title,
		 ArgList l = NULL, Cardinal num_args = 0);

  /* Destructors:  */

  ~XMCustomDialog();

  /* Operation on the skeleton dialog */

  void Manage();
  void UnManage();
  void popDown();
  void popUp();

  XMPanedWindow *TopManager();
  XMRowColumn   *ActionArea();
  XMForm        *WorkArea();   
  XMPushButton  *ok();
  XMPushButton  *apply();
  XMPushButton  *cancel();
  XMPushButton  *help();

  Callback_data *AddDoCallback(void (*callback)(XMWidget *, XtPointer, 
						XtPointer),
			       XtPointer user_data = NULL,
			       Callback_data **okcb = NULL); 
  Callback_data *AddOkCallback(void (*callback)(XMWidget *, XtPointer, 
						XtPointer),
			       XtPointer user_data = NULL);
  Callback_data *AddApplyCallback(void (*callback)(XMWidget *, XtPointer, 
						   XtPointer),
				  XtPointer user_data = NULL);
  Callback_data *AddCancelCallback(void (*callback)(XMWidget *, XtPointer, 
						    XtPointer),
				   XtPointer user_data = NULL);
  Callback_data *AddHelpCallback(void (*callback)(XMWidget *, XtPointer, 
						  XtPointer),
				 XtPointer user_data = NULL);

  void SetModal(unsigned char modality);
};
/*
** Base class for a custom dialog box.
*/
class XMCustomDialogBox : public XMCustomDialog
{
 public:
  XMCustomDialogBox(char *name, XMWidget &parent, char *title,
		    ArgList l= NULL, Cardinal num_args=0);
  XMCustomDialogBox(char *name, Widget parent, char *title,
		    ArgList l=NULL, Cardinal num_args=0);
  ~XMCustomDialogBox();


  void SetHelpText(char **help_text);
  void RevertHelpText();

 protected:
  XMCallback<XMCustomDialogBox>   OkCb;
  XMCallback<XMCustomDialogBox>   ApplyCb;
  XMCallback<XMCustomDialogBox>   CancelCb;
  XMCallback<XMCustomDialogBox>   HelpCb;

  virtual Boolean Perform(XMWidget *wid, XtPointer cli, XtPointer call);
  virtual void OkPressed(XMWidget *wid, XtPointer cli, XtPointer call);
  virtual void ApplyPressed(XMWidget *wid, XtPointer cli, XtPointer call);
  virtual void CancelPressed(XMWidget *wid, XtPointer cli, XtPointer call);
  virtual void HelpPressed(XMWidget *wid, XtPointer cli, XtPointer call);
  
 private:
  void InitializeHelp();
  void SetCallbacks();		/* Used by all constructors. */
  Xamine_help_client_data help_info;

};
#endif
