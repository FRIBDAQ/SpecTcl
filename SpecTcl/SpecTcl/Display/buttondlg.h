/*
** Facility:
**   Xamine -- NSCL display program.
** Abstract:
**   buttondlg.h:
**     This file defines the classes which are used to implement the
**     client's button box.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
**   Nov. 15, 1995
**
**   @(#)buttondlg.h	8.1 6/23/95 
*/
#ifndef BUTTONDLG_H
#define BUTTONDLG_H

#include "XMManagers.h"
#include "clientops.h"


   /* The button class contains the attributes and behavior common
   ** for all client buttons.
   */
class Button {
  public:
     Button(XMWidget *parent,  XtPointer userd);
     virtual ~Button();

     void SetLabel(ButtonTitle label);
     void GetLabel(ButtonTitle label);

     void Enable();
     void Disable();
     void Manage();
     void UnManage();

     Logical IsEnabled() { return enabled; }

     XMButton *GetButton() { return button; }
     virtual Logical  IsToggle()   { return F; }
  protected:
     XMButton  *button;		// Push button that activates us.
     Logical   enabled;		// True if button is enabled.

     virtual void ButtonPresscb(XMWidget *wid, XtPointer userd,
			      XtPointer buttond) = 0;
     XMWidget  *parent;		// Widget's parent.

};


    /*
    ** This class implements client push buttons which don't prompt.
    */

class PushButton : public Button {
 public:
     PushButton(XMWidget *parent, XtPointer userarg);
     virtual ~PushButton();
 private:
     XMCallback<PushButton> pushed;
 protected:
     virtual void ButtonPresscb(XMWidget *wid, XtPointer userd, 
			      XtPointer buttond);
};


    /* 
    ** This class implements client toggle buttons which are not allowed
    ** to prompt for anything.
    */

class ToggleButton : public Button {
   public:
      ToggleButton(XMWidget *parent, XtPointer calldata);
      virtual ~ToggleButton();

      void SetState(Logical new_state);
      Logical GetState();
      virtual Logical IsToggle() { return T; }

    protected:
      Logical state;
      virtual void ButtonPresscb(XMWidget *wid, XtPointer userd,
			       XtPointer buttond);
    private:
     XMCallback<ToggleButton> pushed;
};

/*
** The button box is an array of buttons, however the size is determined
** by the constructor.  It's widget representation is that of a Top level
** shell containing a form widget which in turn contains the buttons.
*/
typedef Button *pButton, **ppButton;

class ButtonBox {
  public:
    ButtonBox(XMWidget *parent, String name, Cardinal nrows, Cardinal ncols);
    ~ButtonBox();

    void     SetButton(Button *button, Cardinal row, Cardinal col);
    Button * ClrButton(Cardinal row, Cardinal col);

    Button * GetButton(Cardinal row, Cardinal col);

    void EnableButton(Cardinal row, Cardinal col);
    void DisableButton(Cardinal row, Cardinal col);

    XMWidget * GetButtonParent() { return ButtonLayout; }
 
    void Manage()                { Box->Manage(); }
    void UnManage()              { Box->UnManage(); }
    Cardinal GetColumns()        { return columns; }
    Cardinal GetRows()           { return rows; }
    Boolean  ValidLocation(Cardinal row, Cardinal col)
      { return ((row >= 0) && (row < rows) &&
		(col >= 0) && (col < columns)); }
  protected:
    Cardinal Index(Cardinal row, Cardinal col) { return (col + row*columns); }
  private:
    TopLevelShell  *Box;
    XMForm         *ButtonLayout;
    ppButton         Buttons;

    Cardinal        columns;
    Cardinal        rows;    
};

#endif


