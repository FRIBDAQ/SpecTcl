/*
** Facility:
**   Xamine  - NSCL display program.
** Abstract:
**   speccheck.cc
**      This file implements some spectrum sanity checking software.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/


/*
** Include files:
*/

#include "spccheck.h"
#include "panemgr.h"
#include "dispwind.h"
#include "refreshctl.h"
#include "dispshare.h"


/*
** Functional Description:
**   CheckSpectra:
**      This function checks all panes for consistency with the
**      spectrum database.  If an inconsistency is found then the pane
**      is forced to refresh.  The refresh will re-synch the pane
**      with the actual spectrum.
*/
void CheckSpectra()
{
  int rows = Xamine_Panerows();
  int cols = Xamine_Panecols();
  win_attributed *attrib;

  for(int r = 0; r < rows; r++) {
    for(int c = 0; c < cols; c++) {
      attrib = Xamine_GetDisplayAttributes(r, c);
      if(attrib != NULL) {	/* Pane contains a spectrum. */
	int sp = attrib->spectrum();
	spec_type tp = Xamine_SpectrumType(sp);
	if(tp == undefined) {
	  Xamine_RedrawPane(c,r); /* This will clear pane. */
	}
	else if(attrib->is1d() && 
		((tp == twodword) || (tp == twodbyte)))	/* Wrong spec type */
	  Xamine_RedrawPane(c,r);
      }
    }
  }
}

