/*
** Facility:
**   Xamine  - NSCL Display program.
**  Abstract:
**    location.cc:
**      This file implements the methods of the Xamine_Location class.
**      That class is responsible for building a widget like object which
**      displays the current location of the cursor.
**      Clients of this object should include the header file locator.h
** Author:
**    Ron Fox
**    NSCL
**    Michigan State University
**    East Lansing, MI 48824-1321
*/
static char *sccsinfo = "@(#)location.cc	2.3 3/22/94 ";

/*
** Include files:
*/
#include "XMManagers.h"
#include "XMLabel.h"
#include "location.h"


/*
** Functional Description:
**   Xamine_Location::CreateLocation:
**     This function is intended to be called by the constructor methods.
**     It provides call a type independent method for setting up the widget
**     hierarchy.  At this time, the Form manager widget has already been
**     created and it's widget id is set as the id field. Since we inherit
**     from form, we can just call XMForm methods to layout the child widgets.
**     We're going to create a bunch of labels as follows:
** 
**      "Spectrum: " "????"  "X  " "????"  "Y "  "????"  "Counts " "????"
**
**         The ???? fields can be altered via methods.
*/
void Xamine_Location::CreateLocation()
{

  SetAttribute(XmNresizePolicy, (XtArgVal)XmRESIZE_NONE);

  /* Build and place the spectrum name/number pair: */
  speclabel = new XMLabel("Speclabel", id, "Spectrum");
  specnumber= new XMLabel("Specnumbr", id, "   ");

  SetTopAttachment(*speclabel,   XmATTACH_FORM);
  SetLeftAttachment(*speclabel,  XmATTACH_FORM);
  SetBottomAttachment(*speclabel, XmATTACH_FORM);

  SetTopAttachment(*specnumber,  XmATTACH_FORM);
  SetLeftAttachment(*specnumber, XmATTACH_WIDGET);
  SetLeftWidget(*specnumber, *speclabel);
  SetBottomAttachment(*specnumber, XmATTACH_FORM);

  /* Build and place the X position/value pair: */

  xposlabel = new XMLabel("Xposlabel", id, "X");
  xposvalue = new XMLabel("Xposval",   id, "      ");

  SetTopAttachment(*xposlabel,   XmATTACH_FORM);
  SetLeftAttachment(*xposlabel,  XmATTACH_POSITION);
  SetLeftPosition(*xposlabel, 25);
  SetBottomAttachment(*xposlabel, XmATTACH_FORM);

  SetTopAttachment(*xposvalue, XmATTACH_FORM);
  SetLeftAttachment(*xposvalue, XmATTACH_WIDGET);
  SetLeftWidget(*xposvalue, *xposlabel);
  SetBottomAttachment(*xposvalue, XmATTACH_FORM);

  /* Build and place the Y position/value pair */

  yposlabel  = new XMLabel("Yposlabel", id, "Y");
  yposvalue  = new XMLabel("Yposvalue", id, "       ");

  SetTopAttachment(*yposlabel, XmATTACH_FORM);
  SetLeftAttachment(*yposlabel,   XmATTACH_POSITION);
  SetLeftPosition(*yposlabel, 50);
  SetBottomAttachment(*yposlabel, XmATTACH_FORM);

  SetTopAttachment(*yposvalue, XmATTACH_FORM);
  SetLeftAttachment(*yposvalue,   XmATTACH_WIDGET);
  SetLeftWidget(*yposvalue, *yposlabel);
  SetBottomAttachment(*yposvalue, XmATTACH_FORM);

  /* Create and position the counts label/value */

  countslabel = new XMLabel("Countslabel", id, "Counts");
  countsvalue = new XMLabel("Countsvalue", id, "       ");

  SetTopAttachment(*countslabel,    XmATTACH_FORM);
  SetLeftAttachment(*countslabel,   XmATTACH_POSITION);
  SetLeftPosition(*countslabel,       75);
  SetBottomAttachment(*countslabel, XmATTACH_FORM);

  SetTopAttachment(*countsvalue,   XmATTACH_FORM);
  SetLeftAttachment(*countsvalue,  XmATTACH_WIDGET);
  SetLeftWidget(*countsvalue,      *countslabel);
  SetBottomAttachment(*countsvalue, XmATTACH_FORM);

  placeholder = new XMLabel("Placeholder", id, "     ");
  SetTopAttachment(*placeholder,   XmATTACH_FORM);
  SetLeftAttachment(*placeholder,  XmATTACH_WIDGET);
  SetLeftWidget(*placeholder,      *countsvalue);
  SetBottomAttachment(*placeholder, XmATTACH_FORM);
  SetRightAttachment(*placeholder, XmATTACH_FORM);

}

/*
** Functional Description:
**   Xamine_Location::~Xamine_Location:
**    This function is the destructor for the Xamine_Location object class.
**    We must leave everything free and clear for the XMForm destructor to 
**    finish us off.
*/
Xamine_Location::~Xamine_Location()
{
  delete speclabel;
  delete specnumber;
  delete xposlabel;
  delete xposvalue;
  delete yposlabel;
  delete yposvalue;
  delete countslabel;
  delete countsvalue;
  delete placeholder;
}

/*
** Functional Description:
**   Xamine_Location::Manage()
**     Places the children under the geometry management of the form
**     and manages the form itself.
*/
void Xamine_Location::Manage()
{
  speclabel->Manage();
  specnumber->Manage();
  xposlabel->Manage();
  xposvalue->Manage();		/* Manage the children. */
  yposlabel->Manage();
  yposvalue->Manage();
  countslabel->Manage();
  countsvalue->Manage();
  placeholder->Manage();

  XMForm::Manage();			/* Manage the manager. */
}

/*
** Functional Description:
**    Xamine_Location::UnManage:
**       Make the subhierarchy invisible:
*/
void Xamine_Location::UnManage()
{
  speclabel->UnManage();
  specnumber->UnManage();
  xposlabel->UnManage();
  xposvalue->UnManage();
  yposlabel->UnManage();	/* Unmanage the children. */
  yposvalue->UnManage();
  countslabel->UnManage();
  countsvalue->UnManage();
  placeholder->UnManage();
  XMForm::UnManage();			/* Unmanage the manager. */
}
