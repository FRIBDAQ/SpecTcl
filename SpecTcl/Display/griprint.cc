/*
** Facility:
**   Xamine - NSCL Display program.
** Abstract:
** griprint.cc:
**   This file contains code to manage the printer setup and options for
**   Xamine. This includes generating the proper command file for the
**   scientific graphing package Gri. The spectrum's own attributes are 
**   used in this determination, and Gri produces a postscript file that
**   closely resembles the Xamine rendition of the same plot.
** Jason Venema
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/

static char *sccsinfo="@(#)griprint.cc	2.1 12/22/93 ";


/*
** Include files required:
*/
#include <string>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <fstream.h>
#include <math.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <vector>
#ifdef unix
#include <sys/time.h>
#include <sys/types.h>
#endif
#include "XMDialogs.h"
#include "XMManagers.h"
#include "XMPushbutton.h"
#include "XMText.h"
#include "XMLabel.h"
#include "XMTab.h"
#include "XMList.h"
#include "panemgr.h"
#include "helpmenu.h"
#include "printer.h"
#include "dispshare.h"
#include "axes.h"
#include "dispgrob.h"
#include "grobjmgr.h"
#include "griprint.h"
#include "colormgr.h"

extern "C" {
#ifdef ultrix
  time_t time(time_t *tloc);
#endif
}

/*
** Externally referenced global:
*/
#ifdef __NEED_OWN_ERRNO
extern int errno;
#endif

extern volatile spec_shared *xamine_shared;
extern win_db *database;

extern grobj_database Xamine_DefaultObjectDatabase;
extern grobj_database Xamine_DefaultGateDatabase;


/*
** Defines.... these define the default values for print commands
**             and print temp files:
*/

#define INITIAL_PRINTER_TYPE  postscript

#define INCH 2.54

#ifdef unix
#define DEFAULT_TEMPFILE "./Xamine_tempprint.out"
#define DEFAULT_PRINTCMD "lpr -Pu1_color_print";
#endif

#ifdef VMS
#define DEFAULT_TEMPFILE "SYS$SCRATCH:XAMINE_TEMPPRINT.OUT"
#define DEFAULT_PRINTCMD "PRINT/DELETE/QUEUE=EXP_HP %f"
#endif


/*
  FindConvert
    Locates the convert command in the file system. This is for converting
    the postscript that Gri produces into jpeg and png formats.
    Search in :
      /usr/bin/X11
      /usr/local/bin
      /optd/bin
      /usr/bin
      /opt/bin
*/
const char* FindConvert()
{
  if(!access("/usr/bin/X11/convert", X_OK))
    return "/usr/bin/X11/convert";

  if(!(access("/usr/local/bin/convert", X_OK)))
    return "/usr/local/bin/convert";
  
  if(!(access("/optd/bin/convert", X_OK)))
    return "/optd/bin/convert";

  if(!(access("/usr/bin/convert", X_OK))) 
    return "/usr/bin/convert";

  if(!(access("/opt/bin/convert", X_OK)))
    return "/opt/bin/convert";
      
  return NULL;
}

/*
  void Xamine_Print(XMWidget* w, XtPointer user, XtPointer call)

    The purpose of this function is to set up the page to print either
    single or multiple spectra, and invoke the Xamine_PrintSpectrum function
    the proper number of times.
*/
void Xamine_Print(XMWidget* w, XtPointer user, XtPointer call)
{
  PrintSpectrumDialog* dlg = (PrintSpectrumDialog*)user;

  // Get the number of spectra per page from the user
  int nRows = atoi(dlg->getrows());
  if(strlen(dlg->getrows()) == 0) nRows = Xamine_Panerows();
  int nCols = atoi(dlg->getcols());
  if(strlen(dlg->getcols()) == 0) nCols = Xamine_Panecols();

  // Now we either call Xamine_PrintSpectrum once (for print selected), or
  // multiple times.
  switch(dlg->getnum()) {
  case printsel:
    printf("Printing selected spectrum...\n");
    Xamine_PrintSpectrum(w, user, call, 
			 Xamine_GetSelectedDisplayAttributes(), "");
    break;
  case printall:
    int nCount = 0;   // the number of spectra displayed
    int nr, nc;       // the number of rows and columns on the display
    nr = Xamine_Panerows();
    nc = Xamine_Panecols();
    for(int r = 0; r < nr; r++) {
      for(int c = 0; c < nc; c++) {
	win_attributed *at = Xamine_GetDisplayAttributes(r,c);
	if(at != NULL) {
	  nCount++;
	}
      }
    }

    // Inform the user of how many pages will be printed...
    float nPages = (float)((float)nCount / (nRows*nCols));
    if(nPages != (int)nPages) nPages++;
    printf("Printing %d spectra on %d page(s)...\n", nCount, (int)nPages);
    float nPerPage = (nCount / nPages);
    if(nPerPage != (int)nPerPage) nPerPage++;

    int nPageNum  = 0;    // the page number we are currently printing
    int nCurrSpec = 0;    // the current spectrum number (beginning with 0)
    for(int r = 0; r < nr; r++) {
      for(int c = 0; c < nc; c++) {
	nCurrSpec++;
	if(nCurrSpec % (int)nPerPage == 0) nPageNum++;
	win_attributed *at = Xamine_GetDisplayAttributes(r, c);
	if(at != NULL) {
	  Xamine_PrintSpectrum(w, user, call,
			       at, Xamine_GetSpectrumTitle(r, c), nRows, nCols,
			       nCount, nCurrSpec, nPageNum);
	}
      }
    }
    break;
  }
}

/* 
  void Xamine_PrintSpectrum(XMWidget* w, XtPointer User, XtPointer Call, 
                            win_attributed* pAttributes, string sTitle,
			    int nRows, int nCols, int nSpectrumCount,
			    int nCurrSpec)
     This function is the real guts of the printing procedure. Xamine uses
     a scientific graphing package called Gri (Dan Kelly) to create 
     postscript files and uses the ImageMagick software suite to convert
     these to other formats. The basic idea is this:
        1) Retrieve the spectrum points using successive calls to getchannel
	2) Create a command file for Gri that includes these points,
	   draws the appropriate curve (1d) or image (2d) and includes
	   the appropriate title, axis labels, etc... based on the user
	   preferences in Xamine.
	3) Invoke Gri in a new child process to interpret the command file and 
	   generate postscript.
	4) Either send the postscript file to the user specified printer,
	   convert the postscript to some other file format, or just leave
	   it as postscript -- as the user specifies.
*/

void
Xamine_PrintSpectrum(XMWidget* w, XtPointer User, 
		     XtPointer Call, win_attributed* pAttributes, 
		     string sTitle, int nRows=1, int nCols=1, 
		     int nSpectrumCount=1, int nCurrSpec=1, int nPageNum=1)
{
  // First we get the spectrum number that we are dealing with...
  int nSpectrum = pAttributes->spectrum();

  // This is where we get the values of the options from
  // the tabs print option dialog box.
  PrintSpectrumDialog* Options = (PrintSpectrumDialog*)User;
  Dimension nx, ny;
  Dimension xbase, ybase;

  // Next, we get the width and height of the window displaying us...
  XMWidget* pane = Xamine_GetSelectedDrawingArea();
  pane->GetAttribute(XmNwidth, &nx);
  pane->GetAttribute(XmNheight, &ny);
  xbase = (int)((float)nx * (float)XAMINE_MARGINSIZE);
  ybase = ny - (int)((float)ny * (float)XAMINE_MARGINSIZE);

  ofstream fStr;   // the output file stream to the Gri command file...

  // Get the title from the user...
  string Title;
  if(nSpectrumCount == 1)
    Title = string(Options->gettitle());
  else
    Title = sTitle;

  int nCounts;                             // # of counts in a channel
  int nFullScale;                          // fullscale value
  int nFloor = ((pAttributes->hasfloor()) ? pAttributes->getfloor() : 0);
  int nCeiling = ((pAttributes->hasceiling()) ? pAttributes->getceiling() : 0);
  int nIncr;                               // color scale increments
  int nFontSize = 12;                      // the font size to draw in
  int fCropped = 0;                        // true if image needs to be cropped
  int nXTickInterval,                      // interval between ticks on x-axis
    nYTickInterval;                        // interval between ticks on y-axis
  double XMajor, XMinor,                   // user specified tic intervals
    YMajor, YMinor;
  double xLegPos, yLegPos;                 // position for symbol/line legend
  int xdim = xamine_shared->getxdim(nSpectrum);  // channels in x direction
  int ydim = xamine_shared->getydim(nSpectrum);  // channels in y direction
  float xm, ym;                            // the x and y margins
  float XPageSize, 
    YPageSize;                             // page size in centimeters
  float XLength,
    YLength;                               // axis lengths for each spectra

  // Make sure floor and ceiling are okay
  if((nCeiling) && (nFloor) && (nFloor > nCeiling)) {
    int temp = nFloor;
    nFloor = nCeiling;
    nCeiling = temp;
  }

  // And set the floor and ceiling to powers of 10, if scale is log...
  if(pAttributes->islog()) {
    int nPower;
    if(nFloor) {
      nPower     = log10(nFloor);
      nFloor     = (int)pow(10, (nPower));
    }
    if(nCeiling) {
      nPower     = log10(nCeiling);
      nCeiling   = (int)pow(10, (nPower+1));
    }
  }

  // First set the tick intervals. These can be specified...
  XMajor = Options->getxmajor();
  XMinor = Options->getxminor();
  YMajor = Options->getymajor();
  YMinor = Options->getyminor();

  // Determine the margins of the page and the length of the spectra. This
  // needs to be in centimeters for Gri. Thus, 8.5" x 11" = 21cm x 27cm approx.
  // for letter sized page.
  XPageSize = ((Options->getlayout() == portrait) ? 8.5*INCH : 11.0*INCH);
  YPageSize = ((Options->getlayout() == portrait) ? 11.0*INCH : 8.5*INCH);
  XLength = Options->getxlen();
  YLength = Options->getylen();
  
  // If there is only one spectrum, center it on the page...
  if(nSpectrumCount == 1 || (nRows*nCols) == 1) {
    xm = (float)((XPageSize - XLength) / 2.0);
    ym = (float)((YPageSize - YLength) / 2.0);
  }

  // Otherwise, tile the spectra appropriately...
  else {

    // Figure out the current column and row number..
    float nPages = (float)((float)nSpectrumCount / (nRows*nCols));
    if(nPages != (int)nPages) nPages++;
    int nPerPage  = nRows * nCols;    // spectra per page...
    int nCS = nCurrSpec % nPerPage;
    if(nCS == 0) nCS = nPerPage;

    int nCurrCol = nCS % nCols;
    if(nCurrCol == 0) nCurrCol = nCols;
    int nCurrRow = 1;
    int ncc = nCurrCol;
    while(ncc / nCS == 0) {
      nCurrRow++;
      ncc += nCols; 
    }

    // Here, we make sure that the lengths of the axes are as big as
    // possible, while still maintaining an aspect ratio of 2:3. Otherwise,
    // you can easily end up with squished spectrum!
    XLength = (float)(XPageSize - ((nCols+2)*INCH)) / (float)nCols;
    YLength = (float)(YPageSize - ((nRows+2)*INCH)) / (float)nRows;

    // If the length is less than an inch, then border boxes will not
    // be drawn. This is done by setting the flag fCropped to TRUE.
    if((XLength < INCH) || (YLength < INCH)) fCropped = 1;

    // If the maximum size of a spectrum is more than an inch, then we
    // display border boxes around individual spectra...
    if(!fCropped) {
      if(XLength > YLength) {
	float ratio = XLength / YLength;
	if(ratio > 1.5) XLength = YLength * 1.5;
      }
      else {
	float ratio = YLength / XLength;
	if(ratio > 1.5) YLength = XLength * 1.5;
      }
      float xgrid_size = (INCH*(nCols+1) + XLength*nCols);
      float ygrid_size = (INCH*(nRows+1) + YLength*nRows);
      xm  = (XPageSize - xgrid_size) / 2.0;
      xm += (INCH*(nCurrCol) + (XLength*(nCurrCol-1)));
      ym  = (YPageSize - ygrid_size) / 2.0;
      ym += ((INCH*(nRows-nCurrRow+1)) + (nRows-nCurrRow)*YLength);
    }

    // If the maximum size of a spectrum is less than an inch, then we
    // don't display border boxes around individual spectra.
    else if(fCropped) {
      // Reduce the space between spectra from INCHcm (1") to 1cm...
      XLength = (float)((XPageSize - (nCols+2)) / (float)nCols);
      YLength = (float)((YPageSize - (nRows+2)) / (float)nRows);
      if(XLength > YLength) {
	float ratio = XLength / YLength;
	if(ratio > 1.5) XLength = YLength * 1.5;
      }
      else {
	float ratio = YLength / XLength;
	if(ratio > 1.5) YLength = XLength * 1.5;
      }
      float xgrid_size = (nCols+1) + XLength*nCols;
      float ygrid_size = (nRows+1) + YLength*nRows;
      xm  = (XPageSize - xgrid_size) / 2.0;
      xm += nCurrCol + (XLength*(nCurrCol-1));
      ym  = (YPageSize - ygrid_size) / 2.0;
      ym += (nRows-nCurrRow+1) + (nRows-nCurrRow)*YLength;
    }
  }

  // Now we need to do different things for 1d and 2d spectra...
  switch(xamine_shared->gettype(nSpectrum)) {    // determine spectrum type
  case twodbyte:
  case twodword: {
    win_2d* pAttrib = (win_2d*)pAttributes;   // the spectrum attributes
    
    // Get the x- and y-axis high and low limits, and the fullscale value...
    int isFlipped  = pAttrib->isflipped();
    int nXLowLimit  = 0;
    int nXHighLimit = (isFlipped ? ydim : xdim);
    int nYLowLimit  = 0;
    int nYHighLimit = (isFlipped ? xdim : ydim);
    nFullScale = pAttrib->getfsval();
    float red_max, green_max, blue_max;  // the color to draw sing. chan. peaks

    // If spectrum is expanded, we need to adjust the high and low limits
    if(pAttrib->isexpanded()) {
      nXLowLimit  = (isFlipped ? pAttrib->ylowlim() : pAttrib->xlowlim());
      nXHighLimit = (isFlipped ? pAttrib->yhilim()  : pAttrib->xhilim());
      nYLowLimit  = (isFlipped ? pAttrib->xlowlim() : pAttrib->ylowlim());
      nYHighLimit = (isFlipped ? pAttrib->xhilim()  : pAttrib->yhilim());
    }

    // Get the pixel limits so we can get the tick mark intervals...
    int nXRange = (nXHighLimit - nXLowLimit + 1);
    int nYRange = (nYHighLimit - nYLowLimit + 1);
    nXTickInterval = Xamine_getTickInterval(nXRange, (nx-xbase+1));
    nYTickInterval = Xamine_getTickInterval(nYRange, (ybase+1));
    if(XMajor == 0) XMajor = nXTickInterval;
    if(YMajor == 0) YMajor = nYTickInterval;

    // Now put all the data points into a temporary file, leaving
    // out the ones that are not in the expanded rendition...
    int nMaxCounts = 0;
    int nXMaxChan  = 0,
      nYMaxChan    = 0;

    // If the spectrum is not expanded, then its low limit is zero and we
    // must therefore subtract one from the high limits to avoid reading
    // a channel that is too high.
    nXRange = (nXHighLimit-nXLowLimit);
    nYRange = (nYHighLimit-nYLowLimit);
    if(pAttrib->isexpanded()) {
      nXRange++;
      nYRange++;
    }

    // Start writing the Gri command file...
    fStr.open("temp.gri", fstream::out | fstream::app);
    fStr << "# Commands for spectrum " << nCurrSpec << endl;
    fStr << "set page size letter\n";
    fStr << "set tics in\n";

    // Make sure that the scales are linear and the grid size is reset...
    fStr << "delete x scale\n";
    fStr << "set x type linear\n";
    fStr << "delete y scale\n";
    fStr << "set y type linear\n";
    fStr << "delete grid\n";

    // Read the grid data...
    int reso;
    switch(Options->getres()) {
    case one:
      reso = 1;
      break;
    case two:
      reso = 2;
      break;
    case four:
      reso = 4;
      break;
    case eight:
      reso = 8;
    }

    // Adjust the range to fit the data if reduction is to be done
    int nXRemainder = (nXRange % reso);
    int nYRemainder = (nYRange % reso);
    if((reso > 1) && (nXRange % reso != 0 || nYRange % reso != 0)) {
      nXRange -= (nXRange % reso);
      nYRange -= (nYRange % reso);
    }

    // If the spectrum is expanded, then the limits are inclusive...
    fStr << "read grid data " << nYRange/reso << " " << nXRange/reso << endl;

    int x_index = nXLowLimit;      // The x-value to start at (low)
    int y_index = nYHighLimit-1;   // The y-value to start at (high)
    if(pAttrib->isexpanded()) y_index++;
    int hival = 0;
    int sum   = 0;
    reduction_mode sr = pAttrib->getreduction();

    // Here is where we get the points and store them. The format is that of
    // an n x m matrix, where each column is separated by a space and each row
    // is separated by a newline. Note that the first line will be the top
    // row of the spectrum, i.e. the highest y-value and lowest x-value.
    // 
    // The reduction is performed by us to avoid losing any single channel
    // peaks that may be of importance.
    //
    for(int i = 0; i < nYRange/reso; i++) {
      for(int j = 0; j < nXRange*reso; j++) {
	if(!(pAttrib->isexpanded()) &&
	   (x_index >= nXHighLimit || y_index >= nYHighLimit)) 
	  break;

	// Perform the spectrum reduction by hand. If the resolution is 1:1
	// then no further reduction has to be done. Otherwise, the method
	// of reduction used is the same as that use by Xamine.
	switch(reso) {
	case 1: {
	  if(isFlipped) 
	    nCounts = xamine_shared->getchannel(nSpectrum, y_index, x_index);
	  else 
	    nCounts = xamine_shared->getchannel(nSpectrum, x_index, y_index);
	  
	  // If the axis is a log scale, we can't have any zeros...
	  if(nCounts == 0 && pAttrib->islog()) nCounts = 1;
	  if(nCounts > nMaxCounts) {
	    nMaxCounts = nCounts;  // the highest count
	    nXMaxChan  = j;        // the x channel of the highest count
	    nYMaxChan  = i;        // the y channel of the highest count
	  }
	  if(nFloor)
	    if(nCounts < nFloor) nCounts = 0;
	  if(nCeiling)
	    if(nCounts > nCeiling) nCounts = 0;
	  fStr << nCounts << " ";
	  x_index++;
	  break;
	}

	  // For reduced resolution spectrum, we need to employ some sort
	  // of spectrum reduction to make things fit in a smaller number of
	  // pixels. Currently, this consists of sampling, summing and 
	  // averaging of the data values.
	case 2:
	case 4:
	case 8: {
	  int cnt = isFlipped ? 
	    xamine_shared->getchannel(nSpectrum, y_index, x_index) :
	    xamine_shared->getchannel(nSpectrum, x_index, y_index);

	  // If the axis is a log scale, we can't have any zeros...
	  if(cnt == 0 && pAttrib->islog()) cnt = 1;
	  if(cnt > nMaxCounts) {
	    nMaxCounts = cnt;      // the highest count
	    nXMaxChan  = x_index;  // the x channel of the highest count
	    nYMaxChan  = y_index;  // the y channel of the highest count
	  }
	  if(nFloor)
	    if(cnt < nFloor) cnt = 0;
	  if(nCeiling)
	    if(cnt > nCeiling) cnt = 0;
	  sum += cnt;
	  
	  // Maintain the hi value for sampled reduction method
	  if(cnt > hival) hival = cnt;
	  x_index++;
	  if((j+1) % (reso*reso) == 0) {
	    switch(sr) {
	      // Summing means adding all values within this box
	    case summed:
	      fStr << sum << " ";
	      break;
	      // Averaging is the same as summing, but we divide by the
	      // number of pixels in this box
	    case averaged:
	      fStr << (sum/(reso*reso)) << " ";
	      break;
	      // Sampling means taking the maximum value in this box
	    case sampled:
	      fStr << hival << " ";
	    }
	    sum = 0;
	    hival = 0;
	    y_index += (reso-1);
	  }
	  else if((j+1) % (reso) == 0) {
	    y_index--;
	    x_index -= reso;
	  }
	}
	}
      }
      x_index  = nXLowLimit;
      y_index -= reso;
      fStr << endl;
    }
    fStr << endl;
    
    // Set the font size based on the number of spectra we're displaying,
    // and the size of the title and fullscale values...
    if((nSpectrumCount > 1) && (nRows*nCols > 1))
      nFontSize = 8;
    if(fCropped) {
      nFontSize = 5;
      string temp_title;
      int max_title_length = 32 - nCols;
      while(Title.size() > max_title_length) {
	temp_title = Title.substr(0, (Title.size()-5));
	temp_title.append("...");
	Title = temp_title;
      }
    }
    fStr << "set font size " << nFontSize << endl;

    // If the log scale is set, then take the log of the data. The operator
    // _= is the log10 operator in Gri.
    // BUGBUGBUG -- There's no way currently in Gri to give the image palette
    // a scale which is other than linear. Thus, if a 2d spectrum has its
    // scale set to log, the palette will be unlabelled.
    if(pAttrib->islog())
      fStr << "grid data _= 10\n";

    // Show ticks. If the axes are flipped, we need to set the low and
    // high limits on the y-axis. Otherwise, set them on the x-axis...
    if(pAttrib->showticks()) {
      //int nXAxisMax = (reso > 1 ? 
      //	       nXLowLimit+nXRange-reso+(reso/2) : nXHighLimit-1);
      //int nYAxisMax = (reso > 1 ?
      //	       nYLowLimit+nYRange-reso+(reso/2) : nYHighLimit-1);

      // BUGBUGBUG -- Printing at reduced resolution yields empty channels.
      // This is because, when the resolution is reduced, the range of values
      // read in must be a multiple of the new resolution (2, 4 or 8) to work
      // cleanly.
      int nXAxisMax = nXHighLimit-1;
      int nYAxisMax = nYHighLimit-1;

      fStr << "set x grid " << nXLowLimit << " " 
	   << nXLowLimit+nXRange-reso << " " << reso << endl;

      // The tic marks are user-specifiable, so we need to check if they
      // have been specified. If not, we use the default Xamine tic marks.
      switch(Options->gettics()) {
      case deflt:
	if(nXTickInterval >= (nXHighLimit - nXLowLimit-1)) nXTickInterval = 0;
	fStr << "set x axis " << nXLowLimit << " " << nXAxisMax << " ";
	if(nXTickInterval) fStr << nXTickInterval << endl;
	else fStr << endl;
	break;
      case user:
	if(XMajor >= (nXHighLimit - nXLowLimit-1)) XMajor = 0;
	fStr << "set x axis " << nXLowLimit << " " << nXAxisMax << " ";
	if(XMajor) {
	  fStr << XMajor << " ";
	  if(XMinor) fStr << XMinor << endl;
	  else fStr << endl;
	}
	else fStr << endl;
	break;
      }

      // Do this for both dimensions...
      fStr << "set y grid " << nYLowLimit << " " 
	   << nYLowLimit+nYRange-reso << " " << reso << endl;
      
      switch(Options->gettics()) {
      case deflt:
	if(nYTickInterval >= (nYHighLimit - nYLowLimit-1)) nYTickInterval = 0;
	fStr << "set y axis " << nYLowLimit << " " << nYAxisMax << " ";
	if(nYTickInterval) fStr << nYTickInterval << endl;
	else fStr << endl;
	break;
      case user:
	if(YMajor >= (nYHighLimit - nYLowLimit-1)) YMajor = 0;
	fStr << "set y axis " << nYLowLimit << " " << nYAxisMax << " ";
	if(YMajor) {
	  fStr << YMajor << " ";
	  if(YMinor) fStr << YMinor << endl;
	  else fStr << endl;
	}
	else fStr << endl;
	break;
      }
    }
    
    // Set the axis labels (values) to 2 decimal places...
    if(nYHighLimit >= 100000) {
      fStr << "set y format %.1e\n";
    }
    else {
      fStr << "set y format default\n";
    }

    // Set the axes names...
    fStr << "set x name \"" << Options->getxname() << "\"\n";
    fStr << "set y name \"" << Options->getyname() << "\"\n";

    // Set the page margins and drawing size...
    fStr << "set x margin " << xm << endl;
    fStr << "set y margin " << ym << endl;
    fStr << "set x size " << XLength << "\n";
    fStr << "set y size " << YLength << "\n";

    // Set the layout, too...
    switch(Options->getlayout()) {
    case portrait:
      fStr << "set page portrait\n";
      break;
    case landscape:
      fStr << "set page landscape\n";
      break;
    }

     // Draw the title...
    if(!fCropped) {
      fStr << "draw title \"" << Title << "\"\n";
    }
    else {
      fStr << "draw label \"" << Title << "\" centered at " 
	   << ((XLength+(2*xm))/2.0) << " " << (YLength+ym+.2) << " cm\n";
    }

    // Set rendition type...
    switch(pAttrib->getrend()) {
    case scatter:
    case boxes:
    case color:
      // Draw a box around the image if there is more than one
      if(nSpectrumCount > 1) {
	if(!fCropped && ((nRows*nCols) != 1))
	  fStr << "draw border box " << (xm-(INCH/2)) << " " << (ym-1.07) 
	       << " " << (xm+XLength+(INCH/2)) << " " << (ym+YLength+1.47) 
	       << " 0.01 0\n";
      }

      // Determine the range of tick marks for the color palette...
      int nRange = nFullScale;
      if(nFloor && nCeiling)
	nRange = nCeiling - nFloor;
      else if(nFloor)
	nRange = nFullScale - nFloor;
      else if(nCeiling) 
	nRange = nCeiling;

      // Find the increment tick increments for the color palette...
      int nPower = (int)(log10(nRange));
      nIncr  = (int)(nRange / (pow(10, nPower))) * pow(10, (nPower-1));
      if(nIncr == 0) nIncr++;
      while(nIncr * 10 < nRange)
	nIncr *=2;

      // Next we get the resolution specified by the user. If the resolution
      // is 1:1 (enumeration value 'one'), we don't need to do anything.
      int xres = (nXHighLimit - nXLowLimit);
      int yres = (nYHighLimit - nYLowLimit);
      switch(Options->getres()) {
      case two:
	xres /= 2;
	yres /= 2;
	break;
      case four: {
	xres /= 4;
	yres /= 4;
	break;
      }
      case eight: {
	xres /= 8;
	yres /= 8;
      }
      }

      // The fullscale value for a log plot. This is done by taking the log
      // of the data read in and altering the image range to fit the new data.
      // It's kind of pulling the wool over the user's eyes, but they'll never
      // know. Unfortunately, it makes drawing an appropriate image palette
      // even more difficult.
      double nLogFS = (double)log10(nFullScale);
      int    newFS  = (int)pow(10, ((int)nLogFS)+1);
      double nLogMax = log10(nMaxCounts);
      int hi_range = nFullScale, 
	lo_range = 0;
      if(pAttrib->islog()) hi_range = nLogFS;
      if(nFloor) lo_range = nFloor;
      if(nCeiling) hi_range = nCeiling;

      // Set the image range so Gri knows what colors to use...
      fStr << "set image range " << lo_range << " " << hi_range << endl;

      // Now read the colorscale from the Xamine colortable file. The file
      // is the same as the one used for displaying, so the printed output
      // should look exactly the same as the input.
      fStr << "read image colorscale rgb\n";
      int red, green, blue;
      unsigned long pixels[100];

      // Store all the pixel values in an array...
      for(int i = 0; i < 100; i++) {
	pixels[i] = Xamine_PctToPixel(i);
      }

      // Determine the rgb value for each of 256 color channels that Gri
      // can assign. Gri must have these values in a range between 0 and 1.
      unsigned long curr_pix;
      for(int i = 0; i < 256; i++) {
	curr_pix = pixels[((i*100)/256)];
	if((curr_pix == 0) && (i == 0)) curr_pix = 0xffffff;  // white backgrnd
	else if((curr_pix == 0) && (i > 0)) curr_pix = 0x900000;
	red   = (curr_pix & 0xff);
	green = ((curr_pix & 0xff00) >> 8);
	blue  = ((curr_pix & 0xff0000) >> 16);
	fStr << ((double)red / 256) << " " << ((double)green / 256)
	     << " " << ((double)blue / 256) << endl;
      }
      red_max   = (double)red / 256;
      green_max = (double)green / 256;
      blue_max  = (double)blue / 256;

      // Convert the grid to an image with the appropriate size, based on
      // the user specified resolution
      int xr = xres;
      int yr = yres;
      if(xres < 2) xr = 2;
      if(yres < 2) yr = 2;
      fStr << "convert grid to image size " << xr << " " << yr << endl;

      // If the plot is expanded, we need to clip anything outside of the
      // plotting area...
      if(!pAttrib->isexpanded()) {
	fStr << "set clip postscript on 0 " << nXHighLimit-1 << " 0 " 
	     << nYHighLimit-1 << endl;
      }
      else {
	fStr << "set clip postscript on " << nXLowLimit << " " << nXHighLimit-1
	     << " " << nYLowLimit << " " << nYHighLimit-1 << endl;
      }

      // Now draw the image...
      fStr << "draw image\n";
      fStr << "set clip postscript off\n";

      // Draw the image palette if necessary. Note that we only draw it
      // beneath the spectrum if this is the only spectrum on the page.
      // If there are multiple spectra, then we draw one image palette
      // at the top of the page.
      if((Options->getdraw_palette())) {
	int PalHi = (nCeiling ? nCeiling : nFullScale);
	double PalScale = 
	  ((pAttrib->islog()) ? log10(nFullScale) : PalHi);
	if(nSpectrumCount == 1) {
	  fStr << "draw image palette axisright left " << nFloor << " right " 
	       << PalScale << " increment " << nIncr << " box " 
	       << (xm+XLength+.4) << " " << ym << " " << (xm+XLength+.6) 
	       << " " << (ym+YLength) << endl;
	}
	else {
	  // Only draw the image palette if it will fit...
	  if(!fCropped) {
	    if(PalScale >= 1000) fStr << "set font size 5\n";
	    fStr << "set tics out\n";
	    fStr << "draw image palette axisright left " << nFloor << " right "
 		 << PalScale << " increment " << nIncr << " box " 
		 << (xm+XLength+.2) << " " << ym << " " << (xm+XLength+.4) 
		 << " " << (ym+YLength) << endl;
	    fStr << "set tics in\n";
	    fStr << "set font size " << nFontSize << endl;
	  }
	}
      }

      // Draw contours if necessary...
      if((Options->getdraw_contours()) && (nSpectrumCount == 1)) {
	float nContInc = atof(Options->getcontour_inc());
	if(strlen(Options->getcontour_inc()) == 0)
	  nContInc = (double)nFullScale / 4;

	// Note that contour increments must be within 0.1% of a multiple
	// of the fullscale value.
	int nCurrContour = 1;
	int nContours = (int)(nFullScale / nContInc) + 1;
	fStr << "set graylevel 0.7\n";
	while((nCurrContour*nContInc) < nFullScale) {
	  fStr << "draw contour " << (nContInc*nCurrContour) 
	       << " unlabelled\n";
	  nCurrContour++;
	}
	fStr << "set graylevel 0.0\n";
      }
      break;
    }
    
    // Draw ticks, axes, and tick labels...
    if(!(pAttrib->showaxes())) {
      fStr << "draw axes none\n";
    }
    else if(!(pAttrib->showticks())) {
      fStr << "draw axes frame\n";
    }
    else 
      fStr << "draw axes 0\n";
    
    // Draw the graphical objects. For 2d spectra, these can be any
    // of the following: Summing Region, Marker, Contour, or Band.
    Xamine_SetObjectDatabase(&Xamine_DefaultObjectDatabase);
    int nObjects = Xamine_GetSpectrumObjectCount(nSpectrum);
    if(nObjects) {
      grobj_generic **pObjects = new grobj_generic*[nObjects];
      if(!pObjects) {
	fprintf(stderr, "'new' failed in Xamine_PrintSpectrum **");
      } 
      Xamine_GetSpectrumObjects(nSpectrum, pObjects, nObjects, 1);
      string sDrawString;
      for(int i = 0; i < nObjects; i++) {
	sDrawString.erase();
	sDrawString = 
	  Xamine_DrawGraphicalObj2d(nXLowLimit, nXHighLimit, 
				    nYLowLimit, nYHighLimit, 
				    pAttrib, pObjects[i]);
	fStr << sDrawString;
      }
      delete [] pObjects;
    }

    // Draw the gates...
    int nGates = Xamine_GetSpectrumGateCount(nSpectrum);
    if(nGates) {
      grobj_generic **pObjects = new grobj_generic*[nGates];
      if(!pObjects) {
	fprintf(stderr, "'new' failed in Xamine_PrintSpectrum **");
      }
      Xamine_GetSpectrumGates(nSpectrum, pObjects, nGates, 1);
      string sDrawString;
      for(int i = 0; i < nGates; i++) {
	sDrawString.erase();
	sDrawString = 
	  Xamine_DrawGraphicalObj2d(nXLowLimit, nXHighLimit, 
				    nYLowLimit, nYHighLimit, 
				    pAttrib, pObjects[i]);
	fStr << sDrawString;
      }
      delete [] pObjects;
    }

    break;
  }
  case onedlong:
  case onedword: {
    win_1d* pAttrib = (win_1d*)pAttributes;  // the 1d spectrum attributes

    // Get the x-axis high and low limits...
    int nLowLimit = 0;
    int nHighLimit = xdim;
    nFullScale = pAttrib->getfsval();

    // Set the low and high limits based on the expansion characteristics...
    if(pAttrib->isexpanded()) {
      nLowLimit  = pAttrib->lowlimit();
      nHighLimit = pAttrib->highlimit();
    }

    // Get the pixel limits so we can get the tick mark intervals...
    int    nXRange = (nHighLimit-nLowLimit+1);
    int    nYRange = (nFullScale+1);
    if(nFloor && nCeiling) nYRange = (nCeiling - nFloor);
    else if(nFloor) nYRange = nFullScale - nFloor;
    else if(nCeiling) nYRange = nCeiling;
    nXTickInterval = Xamine_getTickInterval(nXRange, (nx-xbase+1));
    nYTickInterval = Xamine_getTickInterval(nYRange, (ybase+1));
    if(XMajor == 0) XMajor = nXTickInterval;
    if(YMajor == 0) YMajor = nYTickInterval;

    // Now put all the data points into a temporary file, leaving
    // out the ones that are not in the expanded rendition...
    int nMaxCounts = 0;
    int nMaxChan   = 0;
    int isFlipped  = pAttrib->isflipped();
    int nr = Xamine_Panerows();
    int nc = Xamine_Panecols();

    // Use nHighCnt to determine the highest point we can display. If a 
    // point is greater than this value, we set that point equal to this
    // value and avoid graphing outside our range.
    int nHighCnt = nFullScale;
    if(pAttrib->islog()) {
      int nPower   = (int)(log10(nFullScale));
      nHighCnt = pow(10, (nPower+1));
    }

    // If expanded, decrement the high limit to avoid trying to get a channel
    // that isn't there (since we're starting at channel zero).
    if(!(pAttrib->isexpanded())) {
      nHighLimit--;
    }

    // Now get the superpositions...
    SuperpositionList SuperList = pAttrib->GetSuperpositions();
    int nSuperposCount = SuperList.Count();
    int aSpecIds[nSuperposCount+1];
    aSpecIds[0] = nSpectrum;
    SuperpositionListIterator SuperIt(SuperList);
    for(int p = 0; p < nSuperposCount; p++) {
      aSpecIds[p+1] = (SuperIt.Next()).Spectrum();
    }
    int specID = nSpectrum;
    int p = 0;

    // We figure out the length of the longest superpos. title so we can
    // position the legend in an intelligent place.
    int nLongestTitle = 0;
    if(nSuperposCount) {
      for(int n = 0; n <= nSuperposCount; n++) {
	char name[40];
	string Title = 
	  string((const char*)xamine_shared->getname(name, aSpecIds[n]));
	if(Title.length() > nLongestTitle) nLongestTitle = Title.length();
      }
    }

    // Open a temporary gri file...
    fStr.open("temp.gri", fstream::out | fstream::app);
    fStr << "\n# Commands for spectrum " << nCurrSpec << endl;
    fStr << "set page size letter\n";
    fStr << "set tics in\n";

    // Set the font size based on the number of spectra we're displaying,
    // and the size of the title and fullscale values...
    if((nSpectrumCount > 1)  && (nRows*nCols > 1))
      nFontSize = 8;
    if(fCropped) {
      nFontSize = 5;
      string temp_title;
      int max_title_length = 35 - nCols;
      while(Title.size() > max_title_length) {
	temp_title = Title.substr(0, (Title.size()-5));
	temp_title.append("...");
	Title = temp_title;
      }
    }
    fStr << "set font size " << nFontSize << endl;

    // We set the axis type to log first, so that Gri
    // can autoscale the axes appropriately.
    if(pAttrib->islog()) {
      if(isFlipped) {
	fStr << "delete x scale\n";
	fStr << "set x type log\n";
      }
      else {
	fStr << "delete y scale\n";
	fStr << "set y type log\n";
      }
    }
    else {
      fStr << "delete x scale\n";
      fStr << "set x type linear\n";
      fStr << "delete y scale\n";
      fStr << "set y type linear\n";
    }


    // Determine if the axes are flipped, and if there are superpositions...
    int nLegendPos = (int)((float)nFullScale * 0.8);
    int nLegendSpacing = (int)((float)nFullScale / 25);

    // If there are no superpositions, it's very easy...
    if(!nSuperposCount) {
      if(isFlipped) {
	fStr << "read columns y x\n";
      } 
      else {
	fStr << "read columns x y\n";
      }

      // Get the points and store them in the command file
      specID = nSpectrum;
      for(int i = nLowLimit; i <= nHighLimit; i++) {
	fStr << i << " ";
	nCounts = xamine_shared->getchannel(specID, i);
	if((nCounts > nMaxCounts) && (specID == nSpectrum)) {
	  nMaxCounts = nCounts;
	  nMaxChan   = i;
	}
	if(nCounts > nHighCnt) nCounts = nHighCnt;
	if(pAttrib->islog() && nCounts == 0) nCounts = 1;
	if(nFloor)
	  if(nCounts < nFloor) nCounts = nFloor;
	if(nCeiling)
	  if(nCounts > nCeiling) nCounts = nCeiling;
	fStr << nCounts << endl;
      }
      fStr << endl;
    }
    
    // Otherwise, it becomes much more complicated! We begin by defining
    // within the command file, a procedure called 'draw curves' that
    // will draw multiple curves on the same plot.
    else {
      fStr << "`draw curves" << nCurrSpec << " \\xname \\y1name ...'\n";
      fStr << "{\n";
      fStr << ".num_of_y_columns. = {rpn wordc 3 -}\n";
      fStr << ".col. = 0\n";   // the column number
      fStr << ".r. = 0.0\n";   // the red color value
      fStr << ".g. = 0.0\n";   // the green color value
      fStr << ".b. = 0.0\n";   // the blue color value
      fStr << "while {rpn .num_of_y_columns. .col. <}\n";  // for each column..
      fStr << "  .ycol. = {rpn .col. 2 +}\n";
      if(isFlipped) {
	fStr << "  read columns y=1 x=.ycol.\n";  // read the points...
      }
      else {
	fStr << "  read columns x=1 y=.ycol.\n";
      }

      // For each superposed spectrum, get the points and store them in the
      // command file, each in it's own column.
      for(int i = nLowLimit; i <= nHighLimit; i++) {
	fStr << i << " ";
	for(int n = 0; n <= nSuperposCount; n++) {
	  nCounts = xamine_shared->getchannel(specID, i);
	  if((nCounts > nMaxCounts) && (specID == nSpectrum)) {
	    nMaxCounts = nCounts;
	    nMaxChan   = i;
	  }
	  if(nCounts > nHighCnt) nCounts = nHighCnt;
	  if(pAttrib->islog() && nCounts == 0) nCounts = 1;
	  if(nFloor)
	    if(nCounts < nFloor) nCounts = nFloor;
	  if(nCeiling)
	    if(nCounts > nCeiling) nCounts = nCeiling;
	  fStr << nCounts << " ";
	  if(p > nSuperposCount)
	    p = 0;
	  specID = aSpecIds[++p];
	}
	fStr << endl;
	specID = nSpectrum;
      }
      fStr << endl;

      fStr << "  set color rgb .r. .g. .b.\n";    // set the curve color...
      fStr << "  set font size 6\n";              // set the legend font size..
      fStr << "  set clip postscript on\n";
      if(Options->getuse_symbols())
	fStr << "  draw symbol .col.\n";   // draw the curve using a symbol...
      else
	fStr << "  draw curve\n";          // draw the curve using lines...
      fStr << "  set clip off\n";
      fStr << "  set font size " << nFontSize << endl;

      // Position the symbol legend (i.e. spectrum name and symbol) in an
      // appropriate position.
      xLegPos = (XLength + xm + 0.85);
      yLegPos = (YLength + ym)*0.95;
      fStr << "  .leg. = {rpn .col. 0.3 *}\n";
      if(nSuperposCount)
	fStr << "  set font size 6\n";

      // Make the legend either a symbol legend or a line legend. Also,
      // only draw a legend if a single spectrum is being printed. Otherwise,
      // it will not fit.
      if(nSpectrumCount == 1) {
	if(Options->getuse_symbols())
	  fStr << "  draw symbol legend .col. {rpn .col. 3 + wordv} at " 
	       << xLegPos << " {rpn " << yLegPos << " .leg. -} cm\n";
	else
	  fStr << "  draw line legend {rpn .col. 3 + wordv} at "
	       << xLegPos << " {rpn " << yLegPos << " .leg. -} cm .25\n";
      }

      if(nSuperposCount)
	fStr << "  set font size " << nFontSize << endl;

      fStr << "  .col. += 1\n";               // increment the column number...
      fStr << "  .r. += 0.5\n";               // choose a different color...
      fStr << "  if {rpn .r. 1.0 <}\n";
      fStr << "    .r. = 0.0\n";
      fStr << "    .g. += 0.5\n";
      fStr << "  else if {rpn .g. 1.0 <}\n";
      fStr << "    .g. = 0.0\n";
      fStr << "    .b. += 0.5\n";
      fStr << "  else if {rpn .b. 1.0 <}\n";
      fStr << "    .b. = 0.0\n";
      fStr << "    .g. = 0.0\n";
      fStr << "    .r. = 0.25\n";
      fStr << "  end if\n";
      fStr << "end while\n";
      fStr << "}\n";
    }

    // Show ticks. If the axes are flipped, we need to set the low and
    // high limits on the y-axis. Otherwise, set them on the x-axis...
    if(pAttrib->showticks()) {
      char first, second;
      first  = (isFlipped) ? 'y' : 'x';
      second = (isFlipped) ? 'x' : 'y';

      // The tics can be user specified or the Xamine tics will be used...
      switch(Options->gettics()) {
      case deflt:
	if(nXTickInterval >= (nHighLimit - nLowLimit)) nXTickInterval = 0;
	fStr << "set " << first << " axis " << nLowLimit << " " << nHighLimit;
	if(nXTickInterval) fStr << " " << nXTickInterval << endl;
	else fStr << endl;
	break;
      case user:
	if(XMajor >= (nHighLimit - nLowLimit)) XMajor = 0;
	fStr << "set " << first << " axis " << nLowLimit << " " << nHighLimit;
	if(XMajor) {
	  fStr << " " << XMajor << " ";
	  if(XMinor) fStr << XMinor << endl;
	  else fStr << endl;
	}
	else fStr << endl;
	break;
      }

      // If the axes are log scaled, then we don't allow the user to adjust
      // the tic intervals. We just use the Xamine default tics...
      int low = ((nFloor) ? nFloor : 0);
      int hi  = ((nCeiling && (nCeiling<nFullScale)) ? nCeiling : nFullScale);
      if(!pAttrib->islog()) {
	switch(Options->gettics()) {
	case deflt:
	  if(nYTickInterval >= (hi-low)) nYTickInterval = 0;
	  fStr << "set " << second << " axis " << low << " " << hi << " "; 
	  if(nYTickInterval) 
	    fStr << nYTickInterval << endl;
	  else fStr << endl;
	  break;
	case user:
	  if(YMajor >= (hi-low)) YMajor = 0;
	  fStr << "set " << second << " axis " << low << " " << hi << " ";
	  if(YMajor) {
	    fStr << YMajor << " ";
	    if(YMinor) fStr << YMinor << endl;
	    else fStr << endl;
	  }
	  else fStr << endl;
	}
      }
      else {
	int nPower = (int)log10(nFullScale);
	int nLogMax, nLogMin;
	nLogMax = pow(10.0, (nPower+1));
	nLogMin = 1;
	if(nCeiling) {
	  nPower  = (int)log10(nCeiling);
	  nLogMax = (int)pow(10, (nPower));
	}
        if(nFloor) {
	  nPower = (int)log10(nFloor);
	  nLogMin = (int)pow(10, nPower);
	}
	fStr << "set " << second << " axis " << nLogMin 
	     << " " << nLogMax << " ";
	if(nLogMax - nLogMin > nYTickInterval) fStr << nYTickInterval << endl;
	else fStr << endl;
      }
    }

    // Set the axis labels (values) to 1 decimal place...
    if(nCeiling >= 100000 || nFullScale >= 100000) {
      fStr << "set y format %.1e\n";
    }
    else {
      fStr << "set y format default\n";
    }

    // Set the tics to be inside the box...
    fStr << "set tics in\n";

    // Set the x and y-axes names...
    fStr << "set x name \"\"\n";
    fStr << "set y name \"\"\n";

    // Set the page margins and drawing size...
    fStr << "set x margin " << xm << endl;
    fStr << "set y margin " << ym << endl;
    fStr << "set x size " << XLength << "\n";
    fStr << "set y size " << YLength << "\n";

    // Set the page layout...
    switch(Options->getlayout()) {
    case portrait:
      fStr << "set page portrait\n";
      break;
    case landscape:
      fStr << "set page landscape\n";
      break;
    }

    // Draw the graphical objects. For 1d spectra, these can be any
    // of the following: Cut, Summing Region, or Marker.
    Xamine_GetSpectrumObjectCount(nSpectrum);
    int nObjects = Xamine_GetSpectrumObjectCount(nSpectrum);
    if(nObjects) {
      grobj_generic **pObjects = new grobj_generic*[nObjects];
      if(!pObjects) {
	fprintf(stderr, "'new' failed in Xamine_PrintSpectrum **"); 
      }
      Xamine_GetSpectrumObjects(nSpectrum, pObjects, nObjects, 1);
      string sDrawString;
      for(int i = 0; i < nObjects; i++) {
	sDrawString.erase();
	sDrawString = 
	  Xamine_DrawGraphicalObj1d(nLowLimit, nHighLimit, nFloor, nCeiling, 
				    pAttrib, pObjects[i]);
	fStr << sDrawString;
      }
      delete [] pObjects;
    }

    // Draw the gates...
    int nGates = Xamine_GetSpectrumGateCount(nSpectrum);
    if(nGates) {
      grobj_generic **pObjects = new grobj_generic*[nGates];
      if(!pObjects) {
	fprintf(stderr, "'new' failed in Xamine_PrintSpectrum **");
      }
      Xamine_GetSpectrumGates(nSpectrum, pObjects, nGates, 1);
      string sDrawString;
      for(int i = 0; i < nGates; i++) {
	sDrawString.erase();
	sDrawString = 
	  Xamine_DrawGraphicalObj1d(nLowLimit, nHighLimit, nFloor, nCeiling,
				    pAttrib, pObjects[i]);
	fStr << sDrawString;
      }
      delete [] pObjects;
    }
    
    // Set rendition type...
    switch(pAttrib->getrend()) {
    case smoothed:
    case histogram:
    case lines:
      if(nSpectrumCount > 1) {
	if(!fCropped && ((nRows*nCols) != 1))
	  fStr << "draw border box " << (xm-(INCH/2)) << " " << (ym-1.07) << " " 
	       << (xm+XLength+(INCH/2)) << " " << (ym+YLength+1.47) << " 0.01 0\n";
      }

      // If there are superpositions, we instruct Gri to invoke the
      // 'draw curves' procedure defined earlier for each spectrum...
      if(nSuperposCount) {
	fStr << "draw curves" << nCurrSpec << " channel ";
	for(int n = 0; n <= nSuperposCount; n++) {
	  char name[40];
	  string Title = 
	    string((const char*)xamine_shared->getname(name, aSpecIds[n]));
	  fStr << "\"" << Title << "\" ";
	  if(Title.length() > nLongestTitle) nLongestTitle = Title.length();
	}
	fStr << endl;
	fStr << "set color rgb 0.0 0.0 0.0\n";
	if(nSpectrumCount == 1) {
	  fStr << "draw border box " << (xLegPos*.98) << " " << (yLegPos+.20)
	       << " " << (xLegPos + 0.6 + (0.15*nLongestTitle)) << " " 
	       << (yLegPos-(nSuperposCount*0.4)) << " 0.04 0\n";
	}
      }
      // Otherwise, we just draw the curve.
      else
	fStr << "draw curve\n";
      break;
    case points: {
      // If the rendition is points, then use small bullets instead of
      // drawing a smooth curve.
      if(!nSuperposCount) {
	fStr << "set symbol size 0.02\n";
	fStr << "set clip on\n";
	fStr << "draw symbol bullet\n";
	fStr << "set clip off\n";
      }
      else {
	int nLongestTitle = 0;
	fStr << "draw curves" << nCurrSpec << " channel ";
	for(int n = 0; n <= nSuperposCount; n++) {
	  char name[40];
	  string Title = 
	    string((const char*)xamine_shared->getname(name, aSpecIds[n]));
	  fStr << "\"" << Title << "\" ";
	}
	fStr << endl;
	fStr << "set color rgb 0.0 0.0 0.0\n";
	if(nSpectrumCount == 1) {
	  fStr << "draw border box " << (xLegPos*.98) << " " << (yLegPos+.20)
	       << " " << (xLegPos + 0.6 + (0.15*nLongestTitle)) << " " 
	       << (yLegPos-(nSuperposCount*0.4)) << " 0.04 0\n";
	}
      }
      break;
    }
    }
    
    // Draw the title...
    if(!fCropped) {
      fStr << "draw title \"" << Title << "\"\n";
    }
    else {
      fStr << "draw label \"" << Title << "\" centered at " 
	   << ((XLength+(2*xm))/2.0) << " " << (YLength+ym+.2) << " cm\n";
    }

    // Set the x and y-axes names...
    fStr << "set x name \"" << Options->getxname() << "\"\n";
    fStr << "set y name \"" << Options->getyname() << "\"\n";

    // Draw ticks, axes, and tick labels...
    if(!(pAttrib->showaxes())) {
      fStr << "set axes style none\n";
    }
    else if(!(pAttrib->showticks())) {
      fStr << "set axes style frame\n";
    }
    // We need to do this so that the tick labels get drawn on log axes
    else if(pAttrib->islog()) {
      char axis;
      axis = ((pAttrib->isflipped()) ? 'x' : 'y');
      fStr << "delete " << axis << " scale\n";
      fStr << "set " << axis << " type log\n";
      fStr << "set axes style 0\n";
    }
    else
      fStr << "set axes style 0\n";

    if(!(nSuperposCount) && !(nGates) && !(nObjects))
      fStr << "draw axes if needed\n";
    else
      fStr << "draw axes 0\n";
  }
  }

  /*-------------------------------------------------------
    This is where we print the actual thing
    -------------------------------------------------------*/

  // If there are more spectra to add to the Gri command file, don't
  // do any printing yet...
  if(nCurrSpec == nSpectrumCount) {
    // Draw timestamp if necessary...
    if(Options->getdraw_time()) {
      time_t tim = time(NULL);
      if(tim != (time_t)-1) {
	struct tm* t = localtime(&tim);
	char time_string[30];
	sprintf(time_string, "SpecTcl: %04d/%02d/%02d %02d:%02d:%02d", 
		(t->tm_year+1900), (t->tm_mon+1), t->tm_mday, 
		t->tm_hour, t->tm_min, t->tm_sec);
	if(t->tm_isdst)
	  strcat(time_string, " DST");
	else
	  strcat(time_string, " EST");
	fStr << "set font size 6\n";
	if(nSpectrumCount == 1)
	  fStr << "draw label \"" << time_string << "\" at " 
	       << (xm) << " " << (ym+YLength+1.8) << " cm\n";
	else
	  fStr << "draw label \"" << time_string << "\" at 1.27 "
	       << ym - 1.5 << " cm\n";
      }
    }

    // Draw the page number now...
    if(nSpectrumCount > 1)
      fStr << "draw label \"page " << nPageNum << "\" at "
	   << (XPageSize-2.0) << " " << (ym-1.5) << " cm\n";

    // Now figure out what to name the file if printing to a file is 
    // requested. If no name is supplied by the user, use a default.
    string sFilename = string(Options->getfile());
    string sDest;
    
    // Now determine what to do with the command file and the resulting
    // postscript. The idea is to create a system command and let the shell
    // interpret it for us.
    char GriCmd[200];
    char printcmd[1000];
    char buf1[200];
    char buf[200];        // temporary buffer for concatenating to GriCmd
    strcpy(printcmd, Xamine_GetPrintCommand());
    char instdir[75];
    sprintf(instdir, "%s", HOME);
    char bindir[80];
    char etcdir[80];
    sprintf(bindir, "%s/Bin/gri", instdir);
    sprintf(etcdir, "%s/Etc", instdir);
    
    // Send to either the printer, or to file...
    switch(Options->getdest()) {
    case toprinter: {
      sprintf(GriCmd, "%s -directory %s -c 0 -no_cmd_in_ps temp; ", 
	      bindir, etcdir);
      sprintf(buf1, printcmd, "temp.ps");
      string s(buf1);
      sprintf(buf, "%s; rm -f temp.ps; rm -f temp.gri", s.c_str());
      strcat(GriCmd, buf);
      break;
    }
    case tofile: {
      string sType = string(Options->gettype());
      
      // Append the appropriate postfix to the filename..
      if(sType == "JPEG, 72 dpi (*.jpg)") {
	sFilename.append(".jpg");
	sDest += "temp.ps";
      }
      else if(sType == "JPEG, 300 dpi (*.jpg)") {
	sFilename.append(".jpg");
	sDest += "-density 300x300 temp.ps";
      }
      else if(sType == "Portable Network Graphics, 72 dpi (*.png)") {
	sFilename.append(".png");
	sDest += "temp.ps";
      }
      else if(sType == "Portable Network Graphics, 300 dpi (*.png)") {
	sFilename.append(".png");
	sDest += "-density 300x300 temp.ps";
      }
      else
	sFilename.append(".ps");
      
      sprintf(GriCmd, "%s -directory %s -c 0 -no_cmd_in_ps temp.gri; ", 
	      bindir, etcdir);
      
      // If postscript requested, don't add the following:
      char buf[100];
      if(sType != "Postscript (*.ps)") {
	char convert_path[50];
	strcpy(convert_path, FindConvert());
	sprintf(buf, "%s -page letter %s %s; rm -f temp.ps; ", convert_path,
		sDest.c_str(), sFilename.c_str());
      }
      else {
	sprintf(buf, "mv temp.ps %s; ", sFilename.c_str());
      }
      strcat(GriCmd, buf);
      strcat(GriCmd, "rm -f temp.gri");
    }
    }
    
    // Here's where we fork a new process to handle the printing. As long
    // as the spectrum is less than 1k by 1k, printing doesn't take long.
    // As the spectrum gets bigger, it starts to take a very long time to
    // print, so forking makes sense.
    pid_t child_pid;
    if((child_pid = fork()) < 0) {
      fprintf(stderr, "Unable to fork new process in Xamine_PrintSpectrum.\n");
      fprintf(stderr, "Cancelled print!\n");
    }
    else if(child_pid > 0) {
      int x;
      waitpid(-1, &x, WNOHANG);
    }
    else {
      system(GriCmd);
      printf("Finished\n");     
      exit(0);
    }
  }
  else if(nCurrSpec % (nRows*nCols) == 0) {
    // Draw timestamp if necessary...
    if(Options->getdraw_time()) {
      time_t tim = time(NULL);
      if(tim != (time_t)-1) {
	struct tm* t = localtime(&tim);
	char time_string[30];
	sprintf(time_string, "SpecTcl: %04d/%02d/%02d %02d:%02d:%02d", 
		(t->tm_year+1900), (t->tm_mon+1), t->tm_mday, 
		t->tm_hour, t->tm_min, t->tm_sec);
	if(t->tm_isdst)
	  strcat(time_string, " DST");
	else
	  strcat(time_string, " EST");
	fStr << "set font size 6\n";
	if(nSpectrumCount == 1)
	  fStr << "draw label \"" << time_string << "\" at " 
	       << (xm) << " " << (ym+YLength+1.8) << " cm\n";
	else
	  fStr << "draw label \"" << time_string << "\" at 1.27 "
	       << ym - 1.5 << " cm\n";
      }
    }
    // Draw the page number now...
    if(nSpectrumCount > 1)
      fStr << "draw label \"page " << nPageNum << "\" at "
	   << (XPageSize-2.0) << " " << (ym-1.5) << " cm\n";
    fStr << "new page\n";
  }

  fStr.close();
}

/*
  int Xamine_getTickInterval(int nRange, int nPixels)

    This function calculates the interval between ticks in the same
    way that Xamine does, so that the tick interval on the Gri postscript
    matches that of the Xamine display.
*/
int
Xamine_getTickInterval(int nRange, int nPixels)
{
  double TickMin  = nRange / XAMINE_TICK_MAXCOUNT;
  int    nTickPix = nPixels / XAMINE_TICK_MAXCOUNT;     // pixels per tick
  int    nTick    = XAMINE_TICK_MAXCOUNT;               // Number of ticks
  
  while((nTickPix < XAMINE_TICK_MINSPACE) && (nTick > 1)) {
    nTick--;
    TickMin  = nRange / nTick;
    nTickPix = nPixels / nTick;
  }
  
  if(TickMin <= 0.0) TickMin = 1.0;
  double TickDec  = log10(TickMin);   // # of decades between ticks
  int    iTickDec = (int)TickDec;
  double TickMult = pow(10.0, (double)iTickDec);  // # of 0's after each tick
  TickDec         = TickDec - (double)iTickDec;   // Fractional part of tight
  double TickMant = pow(10.0, TickDec);           // Mantissa for tight pack

  if(TickMant < 1.0) 
    return (int)TickMult;
  if(TickMant < 2.0)
    return (int)(TickMult * 2.0);
  if(TickMant < 5.0) 
    return (int)(TickMult * 5.0);

  return (int)(10.0 * TickMult);
}

/*
  string Xmaine_DrawGraphicalObj1d(int nLowLimit, int nHighLimit,
                                   int nFullScale, win_1d* pAttrib,
				   grobj_generic* pObj)
    This function will generate the instructions that tell Gri to draw
    graphical objects (gates, markers, etc..) in the right places. The
    return value is a string of the instructions for Gri.
*/
string Xamine_DrawGraphicalObj1d(int nLowLimit, int nHighLimit, int nFloor,
				 int nCeiling, win_1d* pAttrib, 
				 grobj_generic* pObj)
{
  // Get the type of object we're dealing with...
  grobj_type Type = pObj->type();
  string result = "";
  int fSumming = (Type == summing_region_1d);
  int fFlipped = pAttrib->isflipped();
  int fLog     = pAttrib->islog();

  int x, y;
  int nFullScale = pAttrib->getfsval();
  int lo_line = ((nFloor) ? nFloor : fLog);
  int hi_line = ((nCeiling && (nCeiling<nFullScale)) ? nCeiling : nFullScale);

  // If the plot is a log plot, set the full scale value to be the next highest
  // power of ten.
  if(fLog) {
    hi_line = (int)log10(hi_line);
    hi_line = (int)pow(10, hi_line+1);
  }

  switch(Type) {
  case summing_region_1d:
  case cut_1d: {
    grobj_point *FirstPt  = pObj->firstpt();
    grobj_point *SecondPt = pObj->nextpt();
    x = FirstPt->getx();
    if((x >= nLowLimit) && (x <= nHighLimit)) {
      result += "set line width 0.3\n";
      if(fSumming) result += "set dash 0.05 0.05\n";
      char szBuf[100];
      if(fFlipped)
	sprintf(szBuf, "draw line from %d %d to %d %d\n", 
		lo_line, x, hi_line, x);
      else
	sprintf(szBuf, "draw line from %d %d to %d %d\n", 
		x, lo_line, x, hi_line);
      result += szBuf;
    }
    x = SecondPt->getx();
    if((x >= nLowLimit) && (x <= nHighLimit)) {
      char szBuf[100];
      if(fFlipped)
	sprintf(szBuf, "draw line from %d %d to %d %d\n", 
		lo_line, x, hi_line, x);
      else
	sprintf(szBuf, "draw line from %d %d to %d %d\n", 
		x, lo_line, x, hi_line);
      result += szBuf;
    }
    if(fSumming)
      result += "set dash off\n";
    break;
  }
  case marker_1d: {
    grobj_point* Point = pObj->firstpt();
    x = Point->getx();
    y = Point->gety();
    if((x >= nLowLimit) && (x <= nHighLimit) && (y <= hi_line)) {
      x = ((fFlipped) ? Point->gety() : Point->getx());
      y = ((fFlipped) ? Point->getx() : Point->gety());
      result += "set symbol size .1\n";
      char szBuf[100];
      sprintf(szBuf, "draw symbol filleddiamond at %d %d\n", x, y);
      result += szBuf;
    }
    break;
  }
  default: {
    fprintf(stderr, "Tried to print unknown graphical object type!\n");
  }
  }
  return result;
}

/*
  string Xmaine_DrawGraphicalObj2d(int nXLowLimit, int nXHighLimit,
                                   int nYLowLimit, int nYHighLimit,
				   win_2d* pAttrib, grobj_generic* pObj)
    This function generates the instructions for Gri to draw graphical objects
    in the appropriate places on 2d spectra. The return value is a string of
    the instructions for Gri.
*/
string Xamine_DrawGraphicalObj2d(int nXLowLimit, int nXHighLimit,
				 int nYLowLimit, int nYHighLimit, 
				 win_2d* pAttrib, grobj_generic* pObj)
{
  string result = "";
  int fSumming,
    nPointCount;
  int fExpanded = pAttrib->isexpanded();
  int fFlipped  = pAttrib->isflipped();
  nPointCount     = pObj->pointcount();
  fSumming        = (pObj->type() == summing_region_2d);
  grobj_point *Pt = pObj->firstpt();
  grobj_point *NextPt;
  result += "set line width 0.5\n";

  // If the spectrum is expanded, we need to set clipping on to avoid
  // drawing outside of the drawing area.
  if(fExpanded) {
    char szBuf[100];
    sprintf(szBuf, "set clip postscript on %d %d %d %d\n", 
	    nXLowLimit, nXHighLimit, nYLowLimit, nYHighLimit);
    result += szBuf;
  }
  if(fSumming)
    result += "set dash 0.05 0.05\n";

  // Now draw the lines for each point...
  int x1, y1, x2, y2;
  for(int j = 0; j < nPointCount; j++) {
    switch(pObj->type()) {
    case contour_2d:
    case summing_region_2d: {
      if(j < (nPointCount-1)) NextPt = pObj->nextpt();
      else NextPt = pObj->firstpt();
      break;
    }
    case band: {
      if(j < (nPointCount-1)) NextPt = pObj->nextpt();
      else {
	if(fExpanded)
	  result += "set clip postscript off\n";
	return result;
      }
      break;
    }
    case marker_2d: {
      x1 = ((fFlipped) ? Pt->gety() : Pt->getx());
      y1 = ((fFlipped) ? Pt->getx() : Pt->gety());
      if(((x1 >= nXLowLimit) && (x1 <= nXHighLimit)) &&
	 ((y1 >= nYLowLimit) && (y1 <= nYHighLimit))) {
	char szBuf[100];
	result += "set symbol size .1\n";
	sprintf(szBuf, "draw symbol filleddiamond at %d %d\n", x1, y1);
	result += szBuf;
	if(fExpanded)
	  result += "set clip postscript off\n";
	return result;
      }
      else
	return string("");
      break;
    }
    }
    x1 = ((fFlipped) ? Pt->gety() : Pt->getx());
    y1 = ((fFlipped) ? Pt->getx() : Pt->gety());
    x2 = ((fFlipped) ? NextPt->gety() : NextPt->getx());
    y2 = ((fFlipped) ? NextPt->getx() : NextPt->gety());
    char szBuf[100];
    sprintf(szBuf, "draw line from %d %d to %d %d\n", x1, y1, x2, y2);
    result += szBuf;
    Pt = NextPt;
  }
  if(fSumming)
    result += "set dash off\n";
  if(fExpanded)
    result += "set clip postscript off\n";
  return result;
}

/*
  string Xamine_GetSpectrumTitle()
    This function determines a default title to be used for Gri printouts
    which is based on the user's title settings in Xamine. The return value
    is the generated title.
*/
string
Xamine_GetSpectrumTitle(int r = -1, int c = -1)
{
  // First we get the spectrum number that we are dealing with...
  if(r == -1) {
    r = Xamine_PaneSelectedrow();
    c = Xamine_PaneSelectedcol();
  }
  win_attributed* pAttributed = Xamine_GetDisplayAttributes(r, c);
  int nSpectrum = pAttributed->spectrum();
  int xdim = xamine_shared->getxdim(nSpectrum);
  int ydim = xamine_shared->getydim(nSpectrum);

  string Title = "";

  // Add spectrum number...
  if(pAttributed->shownum()) {
    char szBuf[6];
    sprintf(szBuf, "[%d] ", nSpectrum);
    Title += szBuf;
  }
  // Add spectrum name...
  if(pAttributed->showname()) {
    char name[72];
    Title += string((const char*)xamine_shared->getname(name, nSpectrum));
  }

  switch(xamine_shared->gettype(nSpectrum)) {    // determine spectrum type
  case onedlong:
  case onedword: {
    win_1d* pAttrib = (win_1d*)pAttributed;

    // Get the x-axis high and low limits...
    int nMaxCounts = 0, 
      nMaxChan;
    int nCounts;
    int nLowLimit = 0;
    int nHighLimit = xdim;
    if(pAttrib->isexpanded()) {
      nLowLimit  = pAttrib->lowlimit();
      nHighLimit = pAttrib->highlimit();
    }

    if(!(pAttrib->isexpanded())) {
      nHighLimit--;
    }
    for(int i = nLowLimit; i <= nHighLimit; i++) {
      nCounts = xamine_shared->getchannel(nSpectrum, i);
      if(nCounts > nMaxCounts) {
	nMaxCounts = nCounts;
	nMaxChan   = i;
      }
    }

    // Add the bin count...
    if(pAttrib->showdescrip()) {
      char szBuf[20];
      sprintf(szBuf, "  %d bins", xdim);
      Title += szBuf;
    }
    // Add the peak information...
    if(pAttrib->showpeak()) {
      char szBuf[30];
      sprintf(szBuf, "  Max=%d @ %d  ", nMaxCounts, nMaxChan);
      Title += szBuf;
    }
    break;
  }
  case twodbyte:
  case twodword: {
    win_2d* pAttrib = (win_2d*)pAttributed;

    // Get the x- and y-axis high and low limits...
    int nXLowLimit  = 0;
    int nXHighLimit = xdim;
    int nYLowLimit  = 0;
    int nYHighLimit = ydim;
    int nMaxCounts = 0;
    int nXMaxChan, nYMaxChan;
    int nCounts;

    // If spectrum is expanded, we need to adjust the high and low limits
    if(pAttrib->isexpanded()) {
      nXLowLimit  = pAttrib->xlowlim();
      nXHighLimit = pAttrib->xhilim();
      nYLowLimit  = pAttrib->ylowlim();
      nYHighLimit = pAttrib->yhilim();
    }

    if(!(pAttrib->isexpanded())) {
      nXHighLimit--;
      nYHighLimit--;
    }
    for(int i = nYHighLimit; i >= nYLowLimit; i--) {
      for(int j = nXLowLimit; j <= nXHighLimit; j++) {
	nCounts = xamine_shared->getchannel(nSpectrum, j, i);
	if(nCounts > nMaxCounts) {
	  nMaxCounts = nCounts;
	  nXMaxChan  = j;
	  nYMaxChan  = i;
	}
      }
    }

    // Add the number of bins...
    if(pAttrib->showdescrip()) {
      char szBuf[20];
      sprintf(szBuf, "  %d x %d bins", xdim, ydim);
      Title += szBuf;
    }
    // Add the peak information...
    if(pAttrib->showpeak()) {
      char szBuf[30];
      sprintf(szBuf, "  Max=%d @ %d,%d  ", nMaxCounts, nXMaxChan, nYMaxChan);
      Title += szBuf;
    }
  }
  break;
  }

  // Add the previous update time...
  if(pAttributed->showupdt()) {
    time_t tm = pAttributed->lastupdated();
    char *timestr1 = ctime(&tm);
    Title += timestr1;
    Title.resize(Title.size() - 1);
  }
  return Title;
}
