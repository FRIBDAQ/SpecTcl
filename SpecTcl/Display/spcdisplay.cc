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

static const char* Copyright = "(C) Copyright Michigan State University 1994, All rights reserved";
/*
** Facility:
**   Xamine   - NSCL display program.
** Abstract:
**   spcidsplay.cc - This module contains code to coordinate the display
**                   of a spectrum in a window.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/


/*
** Include files required:
*/
#include <config.h>
#include <stdio.h>
#include "errormsg.h"
#include "dispshare.h"
#include "spcdisplay.h"
#include "dispwind.h"
#include "panemgr.h"
#include "refreshctl.h"
#include "dfltmgr.h"
#include "exit.h"
#include "pkgmgr.h"
#include "superpos.h"
#include "buttonsetup.h"
#include <Iostream.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


extern volatile spec_shared *xamine_shared;

/*
** Global variables:
*/

int Xamine_WindowAdvance = 1;


/*
** Functional Description:
**   Xamine_SetAdvance:
**      This function sets the advance count:
** Formal Parameters:
**   int advance:
**      The new advance value.
*/
void Xamine_SetAdvance(int advance)
{
  Xamine_WindowAdvance = advance;
}
/*
** Functional Description:
**   Xamine_GetAdvance:
**      Return the advance value:
** Returns:
**   The current value of Xamine_WindowAdvance.
*/
int Xamine_GetAdvance()
{
  return Xamine_WindowAdvance;
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      Xamine_Display_Pane:
**       Display a spectrum in an arbitrary pane.
**
**  FORMAL PARAMETERS:
**
**      int row:
**          row of pane
**      int col:
**          column of pane.
**      int specid:
**          Id of spectrum
**
*/
void Xamine_DisplayPane (int row, int col, int specid)
{

  /* Set the spectrum in the pane. */

  Xamine_SetDisplay(row, col, specid); /* Set the spectrum. */

  /* Construct the proper display attributes */

  win_attributed *attribs;
  win_1d         attr1;
  win_2d         attr2;

  switch(Xamine_SpectrumType(specid)) {
  case onedlong:
  case onedword:
    Xamine_Construct1dDefaultProperties(&attr1);
    attribs = (win_attributed *)&attr1;
    break;
  case twodlong:
  case twodword:
  case twodbyte:
    Xamine_Construct2dDefaultProperties(&attr2);
    attribs = (win_attributed *)&attr2;
    break;
  case undefined:
    /* Should we raise an error? */
    break;
  }

#define TF(log) ((log) ? "True" : "False")


  // Figure out state of mapping button:\

  float xmax = xamine_shared->getxmax_map(specid);
  float xmin = xamine_shared->getxmin_map(specid);

  if((xmax == 0) && (xmin == 0)) { // Unmappable..
    Xamine_SetApplyMapSensitivity(False);
  }
  else {			// mappable
    Xamine_SetApplyMapSensitivity(True);
    Xamine_SetButtonBarApplyMap(attribs->ismapped());
  }

  

  // Stetup the update:

  Xamine_SetDisplayAttributes(row, col, attribs);
  Xamine_RedrawPane(col, row);
  Xamine_ChangedWindows();	/* Mark windows changed. */
  

  
}


static void
dump(int spno)
{
  spno--;
  long offset = xamine_shared->dsp_offsets[spno];
  printf("--------------------\n");
  printf("  Spectrum: %d\n", spno);
  printf("   xdim   = %d\n", xamine_shared->dsp_xy[spno].xchans);
  printf("   ydim   = %d\n", xamine_shared->dsp_xy[spno].ychans);
  printf("   offset = %d\n", offset);




}
/*
** Functional Description:
**    Xamine_Display:
**       Display the requested spectrum in the currently selected window.
** Formal Parameters:
**   int specid:
**      The id of the spectrum to display
*/
void Xamine_Display(int specid)
{
  int row, col;


  row = Xamine_PaneSelectedrow();
  col = Xamine_PaneSelectedcol();
  Xamine_DisplayPane(row,col, specid);
}

/*
** Functional Description:
**    Xamine_DisplayAdvance:
**      This function displays a requested spectrum then advances the window
**      selection by the requested amount.
** Formal Parameters:
**    int specid:
**      The Id of the spectrum to display
*/
void Xamine_DisplayAdvance(int specid)
{
  int row, column;
  int nrow, ncol;
  int pane_index;
  pane_db   *pdb = Xamine_GetPaneDb();

  int advance;

  /* First just display the spectrum. */

  Xamine_Display(specid);
  if(pdb->iszoomed()) return;	/* Don't advance *EVER* if zoomed. */


  /* Now get the advance value and advance the selection. */

  advance = Xamine_GetAdvance();
  row     = Xamine_PaneSelectedrow();
  column     = Xamine_PaneSelectedcol();

  nrow    = Xamine_Panerows();
  ncol    = Xamine_Panecols();

  /* Figure out where we are and compute where we should be as an index: */

  pane_index = column + ncol*row;
  pane_index += advance;

  pane_index = pane_index % (nrow*ncol); /* Wrap to zero if necessary */
  while(pane_index < 0) pane_index += ncol*nrow; /* Wrap negative through end */


  row        = pane_index / ncol;
  column        = pane_index % ncol;

  Xamine_SelectPane(row, column);

}

/*
** Functional Description:
**    Xamine_Superimpose:
**      This function superimposes a spectrum on the current window.
** Formal Parameters:
**    int specid:
**       Id of spectrum to add to current display.
*/
void Xamine_Superimpose(int specid)
{

  /* Add the spectrum to the superposition list of the current spectrum */

  win_attributed *att = Xamine_GetSelectedDisplayAttributes();
  if(att == NULL) return;	/* No action if attributes are null. */
  if(!att->is1d()) return;
  win_1d *a = (win_1d *)att;
  SuperpositionList &s = a->GetSuperpositions();
  if(s.Count() == MAX_SUPERPOSITIONS) {
    Xamine_error_msg(Xamine_Getpanemgr(),
		     "Too many superpositions on this spectrum");
    return;
  }
  s.Add(specid);

  /* Indicate display changes: */

  Xamine_RedrawSelectedPane();	/* Redraw the pane with additional spec. */
  Xamine_ChangedWindows();	/* Mark windows changed. */
}

/*
** Functional Description:
**   Xamine_SetSelectedRefreshRate:
**     Sets the refresh rate of the selected spectrum.
** Formal Parameters:
**    int sec:
**       Number of seconds between automatic refreshes.
*/
void Xamine_SetSelectedRefreshRate(int sec)
{
  if(sec > 0) {
    Xamine_ScheduleTimedUpdate(Xamine_GetSelectedDrawingArea(),
			       Xamine_PaneSelectedcol(),
			       Xamine_PaneSelectedrow(),
			       sec);
    
  }
  else {
    Xamine_CancelTimedUpdate(Xamine_PaneSelectedcol(),
			     Xamine_PaneSelectedrow());
  }
}

/*
** Functional Description:
**   Xamine_SetSelectedAxisVisibility:
**     This function turns on or off axis visibility for the selected pane.
** Formal Parameters:
**   int onoff:
**    nonzero means visible, zero means not visible.
*/
void Xamine_SetSelectedAxisVisibility(int onoff)
{
  fprintf(stderr, "Setting axis visibility %s\n", onoff ? "ON" : "OFF");
}

/*
** Functional Descrption:
**   Xamine_SetSelectedAxisTicks:
**      This function sets the visibility of Axis tick marks for the selected
**      pane.
** Formal Parameters:
**   int onoff:
**    nonzero means visible, zero means not visible.
*/
void Xamine_SetSelectedAxisTicks(int onoff)
{
  fprintf(stderr, "Setting tick visibility %s\n", onoff ? "ON" : "OFF");
}

/*
** Functional Description:
**   Xamine_SetSelectedAxisLabels
**     This function sets the visibility of axis labels for the selected pane.
** Formal Parameters:
**   int onoff:
**    nonzero means visible, zero means not visible.
*/
void Xamine_SetSelectedAxisLabels(int onoff)
{
  fprintf(stderr, "Setting axis label visibility %s\n", onoff ? "ON" : "OFF");
}

/*
** Functional Description:
**   Xamine_SetSelectedNameVisible
**     Sets the visibility of the spectrum name on the title.
** Formal Parameters:
**   int onoff:
**     If true then it becomes visible else it becomes invisible.
*/
void Xamine_SetSelectedNameVisible(int onoff)
{
  fprintf(stderr,"Setting spectrum nameto %s\n", onoff ? "Visible" : "Invisible");
}

/*
** Functional Description:
**   Xamine_SetSelectedIdVisible:
**     Sets the visibility of the spectrum id on the title.
** Formal Parameters:
**   int onoff:
**     If true then it becomes visible else it becomes invisible.
*/
void Xamine_SetSelectedIdVisible(int onoff)
{
  fprintf(stderr,"Setting Spectrum id to %s\n", onoff ? "Visible" : "Invisible");
}

/*
** Functional Description:
**   Xamine_SetSelectedDescriptionVisible:
**      Sets the spectrum description visibility.
** Formal Parameters:
**   int onoff:
**     If true then it becomes visible else it becomes invisible.
*/
void Xamine_SetSelectedDescriptionVisible(int onoff)
{
  fprintf(stderr,"Setting Spectrum description to %s\n", onoff ? "Visible" : "Invisible");
}

/*
** Functional Description:
**   Xamine_SetSelectedMaxVisible:
**     Displays the position and peak height of the highest peak.
** Formal Parameters:
**   int onoff:
**     If true then it becomes visible else it becomes invisible.
*/
void Xamine_SetSelectedMaxVisible(int onoff)
{
  fprintf(stderr,"Setting Biggest peak stats to %s\n", onoff ? "Visible" : "Invisible");
}

/*
** Functional Description:
**   Xamine_SetSelectedUpdateVisible:
**     Enables or disables display of the update rate.
** Formal Parameters:
**   int onoff:
**     If true then it becomes visible else it becomes invisible.
*/
void Xamine_SetSelectedUpdateVisible(int onoff)
{
  fprintf(stderr,"Setting Update rate to %s\n", onoff ? "Visible" : "Invisible");
}

/*
** Functional Description:
**   Xamine_SetSelectedObjectsVisible:
**     Sets visibility of object names.
** Formal Parameters:
**   int onoff:
**     If true then it becomes visible else it becomes invisible.
*/
void Xamine_SetSelectedObjectsVisible(int onoff)
{
  fprintf(stderr,"Setting Object names to %s\n", onoff ? "Visible" : "Invisible");
}


/*
** Functional Description:
**   Xamine_SetSelectedFlip:
**     This function sets the axis flip state of the selected spectrum.
** Formal Parameters:
**    int onoff:
**      true to flip false otherwise.
*/
void Xamine_SetSelectedFlip(int onoff)
{
  fprintf(stderr, "Setting selected spectrum orientation to %s\n",
	  onoff ? "Flipped" : "Normal");
}

/*
** Xamine_SetSelectedReduction:
**   This function sets the reduction mode of the selected spectrum pane.
** Formal Parameters:
**   reduction_mode mode:
**     New reduction mode.
*/
void Xamine_SetSelectedReduction(reduction_mode mode)
{
  char *new_mode;

  switch(mode) {
  case sampled:
    new_mode = "Sampled";
    break;
  case summed:
    new_mode = "Summed";
    break;
  case averaged:
    new_mode = "Averaged";
    break;
  default:
    new_mode = "Ill Defined[216z";
    break;
  }
  fprintf(stderr, "Setting selected reduction mode to %s\n", new_mode);
}

/*
** Functional Description:
**   Xamine_SetSelectedScaleMode:
**     This function sets the scaling mode of the selected spectrum.
** Formal Parameters:
**   int newmode:
**     True for auto scale, false for manual scale.
*/
void Xamine_SetSelectedScaleMode(int newmode)
{
  fprintf(stderr, "Setting scale mode to %s\n",
	  newmode ? "Auto" : "Manual");
}

/*
** Functional Description:
**   Xamine_MultiplySelectedScale:
**     Multiply the current full scale of the selected spectrum by a constant.
** Formal Parameters:
**   float multiplier:
**     The multiplication factor.
*/
void Xamine_MultiplySelectedScale(float multiplier)
{
  win_attributed *at = Xamine_GetSelectedDisplayAttributes();

  if(at == NULL) return;	/* Spectrum became undefined. */

  /*  If autoscale is on then return: */


  unsigned int scale = at->getfsval();
  scale = (unsigned int)((float)scale*multiplier);
  if(scale == 0) scale = 1;	/* Don't allow scale to go to zero. */
  if( (scale < at->getfsval()) && (multiplier > 1.0))
    scale = (unsigned int)0xfff00000;      /* Don't let scale wrap. */
  at->setfs(scale);
  Xamine_RedrawSelectedPane(); /* Update the display */
  Xamine_ChangedWindows();	/* Indicate windows are changed too. */


}

/*
** Functional Description:
**  Xamine_MultiplySelectedScaleCb:
**    This function is a jacket for Xamine_MultiplySelectedScale.
**    It is a callback which just relays a call into Xamine_MultiplySelected
**    Scale.
** Formal Parameters:
**    XMWidget *w:
**      Widget which invoked us.
**    XtPointer user_d:
**      User call data, in this case a pointer to a floating point value 
**      which is the scale multiplier.  Note that we use a pointer since we
**      don't know if the size of a pointer is big enough to hold a float
**      on all architectures.
**    XtPointer call_d:
**      MOtif callback data ignored.
*/
void Xamine_MultiplySelectedScaleCb(XMWidget *w, 
				    XtPointer user_d, XtPointer call_d)
{
  float *multiplier = (float *)user_d;

  Xamine_MultiplySelectedScale(*multiplier);
}

/*
** Functional Description:
**    Xamine_ClearSelectedPane:
**      This function removes a spectrum from a pane making it undefined.
** Formal Parameters:
**    Typical callback parameters all of which are ignored.
*/
void Xamine_ClearSelectedPane(XMWidget *w, XtPointer ud, XtPointer cd)
{
  int row = Xamine_PaneSelectedrow();
  int col = Xamine_PaneSelectedcol();

  Xamine_ClearPane(row, col);
}

/*
** Functional description:
**   Xamine_ClearPane:
**      This function clears a pane given it's row and column.
** Formal Parameters:
**   int row:
**      The row to clear.
**   int col:
**      The column to clear.
*/
void Xamine_ClearPane(int row, int col)
{
 pane_db *db = Xamine_GetPaneDb();

  Xamine_CancelPendingUpdate(col, row);        /* Don't need to update. */
  db->undefine(col, row);	        /* Remove the definition. */
  Xamine_RedrawPane(col, row);	        /* Get the pane cleared.  */

  /* Adjust the menus/buttons   */

  if( (row == Xamine_PaneSelectedrow()) &&
      (col == Xamine_PaneSelectedcol())) {
      Xamine_DisableSpectrumSelectedPackage();
      Xamine_Disable1dSelectedPackage();
      Xamine_Disable2dSelectedPackage();
  }
  int nx = db->nx();
  int ny = db->ny();
  int num = 0;
  for(int j = 0; j < ny; j++)
    for(int i = 0; i < nx; i++)
      if(db->defined(i,j))num++;
  if(num == 0) Xamine_DisableSpectrumPresentPackage();

}
