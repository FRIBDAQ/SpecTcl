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


#include "wysiwygPrint.h"

#include "errormsg.h"
#include "panemgr.h"
#include "printer.h"

#include "XMWidget.h"
#include "XMManagers.h"
#include "XMDialogs.h"
#include "XMLabel.h"
#include "XMPushbutton.h"
#include "XMText.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

#include <gd.h>

#include <string>
#include <vector>


// This file implements 'printing' via a bitmap capture of the Xamine
// spectrum area.

//////////////////////////////////////////////////////////////////////////
//
// Forward function prototypes:

static void printCapture(XtPointer userd, XtIntervalId* id);

////////////////////////////////////////////////////////////////////////
//
//    Data structures:
//
// This struct holds the rgb version of a single pane image:

typedef struct paneImage {
  Dimension       w,h;		// Dimensions of the image.
  unsigned long* reds;		// The colors.
  unsigned long* greens;
  unsigned long* blues;
  
  paneImage(Dimension Width, Dimension Height) :
    w(Width),
    h(Height),
    reds(new unsigned long[w*h]),
    greens(new unsigned long[w*h]),
    blues(new unsigned long[w*h])
  {
  }
  ~paneImage() {
    delete []reds;
    delete []blues;
    delete []greens;
  }
  unsigned long Index(Dimension x, Dimension y) {
    return w*y + x;
  }
} *pPaneImage;


// These typedefs organize the images for the entire set of panes.
//

typedef vector<pPaneImage>    rowOfPanes;
typedef vector<rowOfPanes>  allPanes;

///////////////////////////////////////////////////////////////////////
//
// Custom dialog to prompt for printing parameters.  This dialog will
// have the following appearance:
//     +---------------------------------------+
//     | Print Command  [lpr -Pu6_color %s ]   |
//     | Width (inches) [                  ]   |
//     | [ ] Landscape                         |
//     +---------------------------------------+
//     | [ Ok ]    [ Cancel ]                  |
//     +---------------------------------------+
//
class wysiwygPrintDialog : public XMCustomDialogBox
{
private:
  XMRowColumn*    m_layout;
  XMLabel*        m_printCommandLabel;
  XMTextField*    m_printCommand;
  XMLabel*        m_widthLabel;
  XMTextField*    m_width;
  XMToggleButton* m_orientation;
public:
  wysiwygPrintDialog(char* name, XMWidget& parent, char*title,
		     ArgList l= NULL, Cardinal num_args=0);
  virtual ~wysiwygPrintDialog();

  virtual void Manage();
  virtual void OkPressed(XMWidget* wid, XtPointer cli, XtPointer call);
  virtual void CancelPressed(XMWidget* wid, XtPointer cli, XtPointer call);

  // Fetch stuff from the dialog controls.

  string getPrintCommand();
  float  getWidth();
  Boolean   isLandscape();

};
char* dialogHelpText[] = {
  "1. Fill in the top text entry with the command that will print the \n",
  "   graphics file.  Put a %s where you want the name of that file to appear\n",
  "2. Fill in the \"width\" text entry with the desired width of the graphic \n",
  "   in inches.\n",
  "3. Check the landscape checkbox for landscape orientation, uncheck it for\n",
  "   portraoit orientation\n",
  "4. Click the \"Ok\" button to print or \"Cancel\" to cancel the print\n",
  NULL
};

//  Construct the dialog:

wysiwygPrintDialog::wysiwygPrintDialog(char* name, XMWidget& parent, char* title,
				       ArgList l, Cardinal num_args) : 
  XMCustomDialogBox(name, parent, title, l, num_args),
  m_printCommandLabel(0),
  m_printCommand(0),
  m_widthLabel(0),
  m_width(0),
  m_orientation(0)
{
  SetHelpText(dialogHelpText);

  XMForm* manager = WorkArea();
  m_layout = new XMRowColumn("wysiwygprint.layout", *manager);
  manager->SetLeftAttachment(*m_layout, XmATTACH_FORM);
  manager->SetTopAttachment(*m_layout, XmATTACH_FORM);
  manager->SetRightAttachment(*m_layout, XmATTACH_FORM);
  manager->SetBottomAttachment(*m_layout, XmATTACH_FORM);

  m_layout->SetOrientation(XmVERTICAL);
  m_layout->SetPacking(XmPACK_COLUMN);
  m_layout->SetRowColumns(2);

  // Define the work area widgets..

  m_printCommandLabel = new XMLabel("wysiwygprint.commandlabel",
				    *m_layout, "Print command: ");
  m_widthLabel        = new XMLabel("wysiwygprint.widlabel",
				    *m_layout, "Width (inches):");
  m_orientation       = new XMToggleButton("wysiwygprint.orientation",
					   *m_layout);
  m_orientation->Label("Landscape orientation");


  m_printCommand      = new XMTextField("wysiwygprint.command",
				        *m_layout);
  m_printCommand->SetText(Xamine_GetPrintCommand());

 
  m_width             = new XMTextField("wysiwygprint.width",
					*m_layout, 5);
  m_width->SetText("7.5");	// Default width.
  
 


}
//  Destructor must unmanage and delete the dynamically created widgets:

wysiwygPrintDialog::~wysiwygPrintDialog()
{
  UnManage();
  delete m_layout;
  delete m_printCommandLabel;
  delete m_printCommand;
  delete m_widthLabel;
  delete m_width;
  delete m_orientation;

}

// Manage ... does the base class manage, but unmanages the apply button.
// 

void
wysiwygPrintDialog::Manage()
{
  m_layout->Manage();
  XMCustomDialogBox::Manage();
  apply()->UnManage();
}

// Ok was clicked.. schedule the call to printCapture
// but pass this as the client data.


void 
wysiwygPrintDialog::OkPressed(XMWidget* wid, XtPointer cli, XtPointer call)
{
  UnManage();			// Schedule ourselves to vanish.
  XtAppAddTimeOut(XtWidgetToApplicationContext(WorkArea()->getid()),
					       100, printCapture, (XtPointer)this);
}
//
// Cancel means make invisible with no action:
//
void 
wysiwygPrintDialog::CancelPressed(XMWidget* wid, XtPointer cli, XtPointer call)
{
  UnManage();
}

// Return the print command template:
//
string
wysiwygPrintDialog::getPrintCommand()
{
  return string(m_printCommand->GetText());
}
// Return the width.
//
float
wysiwygPrintDialog::getWidth()
{
  return atof(m_width->GetText());
}
// Return the landscape switch:
//
Boolean
wysiwygPrintDialog::isLandscape()
{
  return m_orientation->GetState();
}

// The entire dialog is sort of a singleton class.
// it's cached both to save state and to make popping it up efficient
// once it's initially created.

static wysiwygPrintDialog* pDialog(0);

///////////////////////////////////////////////////////////////////////
//
// Color extractor helper class:
// This is a function object that is instantiated on a color
// mask, the constructor generates a right shift count
// and mask that allow the extraction of that color.
// The function call operator extracts that color:

class colorExtractor 
{
  unsigned long m_rightShift;
  unsigned long m_positionedMask;
public:
  colorExtractor(unsigned long mask);
  unsigned long operator()(unsigned long pixel);
};
colorExtractor::colorExtractor(unsigned long mask) :
  m_rightShift(0),
  m_positionedMask(0)
{
  while ( (mask & 1) == 0) {
    m_rightShift++;
    mask = mask >> 1;
  }
  m_positionedMask = mask;
}
// Returns the color on the scale 0-255.
unsigned long 
colorExtractor::operator()(unsigned long pixel)
{
  unsigned long rawColor = (pixel >> m_rightShift) & m_positionedMask;
  return rawColor*256/(m_positionedMask+1);
}

////////////////////////////////////////////////////////////////////////////
//
// Capture an image of one pane.
//    pWidget  - The widget whose bitmap will be captured.
//
static pPaneImage
capturePane(XMWidget* pWidget)
{
  // To do the getimage we need to pull out the:
  //   - display we're showing on.
  //   - window  The window we're going to grab the image fromm.
  //   - w,h     The geometry of the window we're going to grab from.

  Widget   widget  = pWidget->getid();

  Window   window  = XtWindow(widget);
  Display* display = XtDisplay(widget);

  Dimension w;
  Dimension h;
  Dimension bw;

  pWidget->GetAttribute(XmNwidth,  &w);
  pWidget->GetAttribute(XmNheight, &h);
  pWidget->GetAttribute(XmNborderWidth, &bw);

  // The actual size we want must be decreased by the border:

  w  -= 2*bw;
  h  -= 2*bw;

  // Now dump the whole set of panes to a single image.
  

  XImage* image = XGetImage(display, window, bw, bw, w, h, 
			    ~0UL, ZPixmap);

  // Make color converter objects for the three primary colors:

  colorExtractor red(image->red_mask);
  colorExtractor blue(image->blue_mask);
  colorExtractor green(image->green_mask);

  // Create the pPaneImage and fill in the RGB pieces:

  pPaneImage p = new paneImage(w,h);

  for (Dimension r = 0; r < h; r++) {
    for(Dimension c = 0; c < w; c++) {
      unsigned long pixel = XGetPixel(image, c, r);
      p->reds[p->Index(c,r)]   = red(pixel);
      p->greens[p->Index(c,r)] = green(pixel);
      p->blues[p->Index(c,r)]  = blue(pixel);
    }
  }
  XDestroyImage(image);
  return p;
}

// Release storage associated with all the rgb images.
//
static void releaseImages(allPanes& theImages)
{
  for(int i =0; i < theImages.size(); i++) {
    for(int j =0; j < theImages[i].size(); j++) {
      delete theImages[i][j];
    }
  }
}

////////////////////////////////////////////////////////////////////////
//
//  Capture all the images in the paned display and produce
//  an output file of them.
//  In order to ensure that there's no 'stuff' on top of the
//  windows, this is scheduled as a timeout.
//     userd   - Client data for the capture (pointer to wysiwygPrintDialog).
//     id      - Timeout id from XtAppAddTimeOut.

static void
printCapture(XtPointer userd, XtIntervalId* id)
{
  // Get the print dialog so we can fetch the parameters:

  wysiwygPrintDialog* pDialog = static_cast<wysiwygPrintDialog*>(userd);

  // Get the user stuff:

  string  printCommand = pDialog->getPrintCommand();
  float   width        = pDialog->getWidth();
  Boolean flip         = pDialog->isLandscape();


  // Capture the images.

  int paneRows = Xamine_Panerows();
  int paneCols = Xamine_Panecols();

  allPanes       theImages;

  pane_db*  db = Xamine_GetPaneDb();
  if (db->iszoomed()) {
    rowOfPanes images;
    XMWidget* w = Xamine_GetFrame(Xamine_PaneSelectedrow(),
				  Xamine_PaneSelectedcol());
    pPaneImage i = capturePane(w);
    images.push_back(i);
    theImages.push_back(images);
    paneRows = 1;
    paneCols = 1;
  }
  else {
    for (int pr = 0; pr < paneRows; pr++) {
      rowOfPanes   rowOfImages;
      for (int pc = 0; pc < paneCols; pc++) {
	XMWidget*  w = Xamine_GetFrame(pr, pc);
	pPaneImage i = capturePane(w);
	rowOfImages.push_back(i);
      }
      theImages.push_back(rowOfImages);
    }
  }

  // Figure out the size of the stitched together image.
  // We assume that while columns may vary in width, 
  // all the panes in one column have the same width.
  // Similarly, while rows may vary in height, all the panes
  // in one row have the same height.
  // This allows us to get :
  //   Total width by summing widths across the top row.
  //   Total heigh by summing heights down the left row.
  //
  
  unsigned long imageWidth  = 0;
  unsigned long imageHeight = 0;
  for  (int c = 0; c < paneCols; c++) {
    imageWidth += theImages[0][c]->w;
  }
  for (int r = 0; r < paneRows; r++) {
    imageHeight += theImages[r][0]->h;
  }
  
  
  // Make the giant image that's stitched together out of
  // all the subimages.
  //   In stiching together:
  //     x,y    - are upper left coordinates of the box in the
  //              big image in which we are putting the current subimage.
  //     r,c    - are the row/column indices of the subimage we are
  //              extracting into the image.
  //    rx,ry   - Are the pixel coordinates of the image we are extracting
  //              from a subimage (inner most loop if you will).
  //  Color handling will be on demand.. For each pixel, we'll
  //  attempt to find an exact match to an existing color in the image.
  //  if this fails, we'll just add the color to the pallette.
  //  Since Xamine color images only use 64 colors, and the 1-d's only 
  //  use bg/fg colors, we'll easily fit in the limit of 256 colors.
  //  In this version we don't put any markings between subimages.
  //  TODO: resize the dest image a bit and box the subimages.
  //

  gdImagePtr im = gdImageCreate(imageWidth, imageHeight);

  unsigned long x = 0;
  unsigned long y = 0;
  for(int r = 0; r < paneRows; r++) {
    for(int c = 0; c < paneCols; c++) {
      pPaneImage pI = theImages[r][c];
      for (unsigned long ry = 0; ry < pI->h; ry++) {
	for (unsigned long rx =0; rx < pI->w; rx++) {
	  unsigned long i = pI->Index(rx, ry);
	  int value = gdImageColorExact(im, pI->reds[i], pI->greens[i], pI->blues[i]);
	  if (value == -1) {
	    value = gdImageColorAllocate(im, pI->reds[i], pI->greens[i], pI->blues[i]);
	  }
	  gdImageSetPixel(im, rx+x, ry+y, value);
	} 
      }
      x += pI->w;		// Next column offset.
    }
    x  = 0; 			// Next row is at col offset 0 and...
    y += theImages[r][0]->h;	// down the height of the prior row.
  }
  

  // Write the image to file, print it and release the storage.
  //

  char filebase[PATH_MAX+1];
  string pngFile(tmpnam(filebase));
  string psFile = pngFile;
  pngFile += ".png";
  psFile  += ".ps";
  FILE* fp = fopen(pngFile.c_str(), "w");
  if (!fp) {
    Xamine_error_msg(pDialog, " Unable to open output .png file");
    gdImageDestroy(im);
    releaseImages(theImages);
    return;
  }
  gdImagePng(im, fp);
  gdImageDestroy(im);
  fclose(fp);

  // Create the pipeline to make the .ps file:

  char cstrwidth[100];
  sprintf(cstrwidth, "%f", width);
  string convertPipeline("pngtopnm ");
  convertPipeline += pngFile;
  convertPipeline += flip ? "| pnmtops -imageheight " : " | pnmtops -imagewidth ";
  convertPipeline += cstrwidth;
  convertPipeline += flip ? " -turn >" : " -noturn >";
  convertPipeline += psFile;
  if(system(convertPipeline.c_str()) == -1) {
    Xamine_error_msg(pDialog, ".png -> .ps conversion failed.");
    releaseImages(theImages);
    return;
  }


  // Now create and execute the print command:

  char cstrPrintCommand[PATH_MAX*2];
  sprintf(cstrPrintCommand, printCommand.c_str(), psFile.c_str());
  if (system(cstrPrintCommand) == -1) {
    Xamine_error_msg(pDialog, "Print failed!");
    releaseImages(theImages);
    unlink(pngFile.c_str());
    return;
  }
  

  // Remove the temp files.

  unlink(pngFile.c_str());
  unlink(psFile.c_str());

  // Release all dynamic tied up in paneImages...The 
  // vectors will take care of deleting themselves..


  releaseImages(theImages);


}
/*!
   This function is invoked when the Print WYSISYG menu button is clicked.
   We will create and manage a dialog that will prompt for 
   the following printing parameters:
     - Print command in the form lpr -Pqueue %s
       where %s will be substituted with the name of the file that will
       be created/printed.
     - Width - in inches of the printout.
     - Orientation - portrait (default) or landscape

   \param who  : XMWidget*
       The menubutton widget
   \param ud   : XtPointer 
       Data associated with the XEvent.
   \param cd   : XtPointer
       Client data associated with the event when it was established.
*/ 

void
Xamine_printCapture(XMWidget* who,
		    XtPointer ud,
		    XtPointer cd)
{
  if (!pDialog) {
    pDialog = new wysiwygPrintDialog("wysiwygprint", *who, "Print options");
    pDialog->AddCallback(XtNdestroyCallback, NullPointer, (XtPointer)&pDialog);
  }
  pDialog->Manage();

}
