/*
** Facility:
**   Xamine - NSCL Display program.
** Abstract:
**   ptlist.h:
**      This file defines an object which prompts for a list of spectrum
**      points from the user for the client process.  The point list is
**      accepted in a dialog much like that used for user gate grahpical
**      objects.  The class defined here need only usually have the 
**      DoAccept function overridden.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
**   @(#)ptlist.h	8.1 6/23/95 
*/
#ifndef _PTLIST_H
#define _PTLIST_H
#include "XMWidget.h"
#include "dispgrob.h"
#include "sumregion.h"

class AcceptPointList : public AcceptSummingRegion
{
public:
  AcceptPointList(char *name, XMWidget *parent, 
		  int minpts = 1,
		  int maxpts = GROBJ_MAXPTS,
		  char **help_text = NULL);
  virtual ~AcceptPointList() {}	     // No actions required from this level.
  virtual  void AddPoint(point &pt); // Override to range check point count.
  virtual  Boolean DoAccept();	     // Range check but user should override.
  virtual void ChooseDefaultName();  // Set an initial name.
  virtual void SelectChanged(int oldc, int oldr, int newc, int newr);
  virtual void RebuildObject();		// Build the graphical object for pane.
protected:
  int min_pts;
  int max_pts;
private:

};

#endif
