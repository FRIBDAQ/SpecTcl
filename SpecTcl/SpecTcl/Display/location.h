/*
** Facility:
**    Xamine  - NSCL display program.
** Abstract:
**    location.h:
**      This include file is intended to provide interface definitions
**      for clients of the Xamine_Location class.  The Xamine_Location
**      class is a widget-like object that displays locations on the screen.
** Author:
**    Ron Fox
**    NSCL
**    Michigan State University
**    East Lansing, MI 48824-1321
** SCCS History:
**    @(#)location.h	8.1 6/23/95 
*/

#ifndef _LOCATION_H_INSTALLED
#define _LOCATION_H_INSTALLED
#include <stdio.h>

#include "XMManagers.h"
#include "XMLabel.h"

class Xamine_Location : public XMForm {
 protected:
   int specid;			/* Last value displayed in specid label. */
   int xpos;			/* Last value displayed in X label. */
   int ypos;			/* Last value displayed in Y label  */
   int counts;			/* Last value displayed in Counts label. */

   XMLabel *speclabel;		/* Fixed label and */
   XMLabel *specnumber;		/* value of spectrum number. */

   XMLabel *xposlabel;		/* Fixed label and */
   XMLabel *xposvalue;		/* Value of x position. */

   XMLabel *yposlabel;		/* Fixed label and */
   XMLabel *yposvalue;		/* Value of Y position */
   
   XMLabel *countslabel;	/* Fixed label and */
   XMLabel *countsvalue;	/* Value of counts field. */
   XMLabel *placeholder;	/* Widget to stretch the right side. */
   void CreateLocation();

 public:
   /* Constructors: */

   Xamine_Location(char *name, Widget parent) : XMForm(name, parent) {
     CreateLocation();
   }
   Xamine_Location(char *name, XMWidget &parent) : XMForm(name, parent) {
     CreateLocation();
   }
   ~Xamine_Location();		/* Destructor. */

   /* Control geometry management and also visibility. */

   void Manage();		/* Make everything visible. */
   void UnManage();		/* Make everything invisible. */

   /* Control field values: */

   void Spectrum(int id) {
     if(id != specid) {
       specid = id;
       if(id < 0) {
	 specnumber->SetLabel(" ");
       }
       else {
	 char string[10];
	 sprintf(string, "%3d", id);
	 specnumber->SetLabel(string);
       }
     }
   }
   void Xpos(int x) {
     if(x != xpos) {
       xpos = x;
       if(x < 0) {
	 xposvalue->SetLabel(" ");
       }
       else {
	 char string[10];
	 sprintf(string, "%5d", x);
	 xposvalue->SetLabel(string);
       }
     }
   }
   void Ypos(int y) {
     if(y != ypos) {
       ypos = y;
       if(y < 0) {
	 yposvalue->SetLabel(" ");
       }
       else {
	 char string[10];
	 sprintf(string, "%6d", y);
	 yposvalue->SetLabel(string);
       }
     }
   }
   void Counts(unsigned int c) {
     if(c != counts) {
       counts = c;
       char string[10];
       sprintf(string, "%6d", c);
       countsvalue->SetLabel(string);
     }
   }

};

#endif
