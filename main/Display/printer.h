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
**   Xamine:  NSCL Display program.
** Abstract:
**    printer.h
**     This file contains definitions for functions that get printer default
**     information and prompt for it.
*/
#ifndef _PRINTER_H
#define _PRINTER_H

#include <fstream>
#include <dispgrob.h>

#include <string>
#include "XMList.h"
#include "XMText.h"
#include "XMPushbutton.h"


#define XAMINE_PRINT_DEFAULTS_FILE "Xamine.PrintDefaults"

typedef enum {
  postscript,
  color_postscript
} PrinterType;

typedef enum { 
  portrait,
  landscape           
} PrintLayout;

typedef enum {
  printall,
  printsel
} PrintNum;

typedef enum {
  tofile,
  toprinter
} PrintDest;

typedef enum {
  one,
  two,
  four,
  eight
} Resolution;

typedef enum {
  deflt,
  user
} Tics;

struct PrintAllData {
  XMText* t_rows;
  XMText* t_cols;
  XMText* t_title;
  XMText* t_name_x;
  XMText* t_name_y;
  XMText* t_length_x;
  XMText* t_length_y;
  XMText* t_xmajor;
  XMText* t_xminor;
  XMText* t_ymajor;
  XMText* t_yminor;
  XMToggleButton* ten_by_ten;
  XMToggleButton* ten_by_fifteen;
  XMToggleButton* user_specified;
  XMToggleButton* draw_palette;
  XMToggleButton* draw_contours;
};

struct PrinterOptionsData {
  XMText* t_file;
  XMText* t_cmd;
  XMScrolledList* list_type;
};

struct ContoursData {
  XMText* t_contour_inc;
  XMToggleButton* tb;
};

struct DefaultPrintOptions {
  PrintLayout layout;
  PrintNum    num;
  PrintDest   dest;
  Resolution  res;
  int         file_type;
  float       xlen, ylen;
  bool        time_stamp;
  bool        color_pal;
  bool        contours;
  bool        symbols;
  char        print_cmd[200];
  char        rows[10];
  char        cols[10];
  DefaultPrintOptions();

};

void Xamine_SetupPrinter(XMWidget *w, XtPointer user, XtPointer call);
void Xamine_PrintSpectrumDialog(XMWidget *w, XtPointer user, XtPointer call);
void Xamine_NoGriDialog(XMWidget *w, XtPointer user, XtPointer call);
char *Xamine_GetPrintCommand();
PrinterType Xamine_GetPrinterType();
void PrintAllCallback(XMWidget* w, XtPointer user, XtPointer call);
void PrintSelCallback(XMWidget* w, XtPointer user, XtPointer call);
void ToPrinterCallback(XMWidget* w, XtPointer user, XtPointer call);
void ToFileCallback(XMWidget* w, XtPointer user, XtPointer call);
void DrawContoursCallback(XMWidget* w, XtPointer user, XtPointer call);
void PresetSizeCallback(XMWidget* w, XtPointer user, XtPointer call);
void UseDefaultTicsCallback(XMWidget* w, XtPointer user, XtPointer call);
void SpecifyTicsCallback(XMWidget* w, XtPointer user, XtPointer call);
int GrabPoints2d(int nXLowLimit, int nXHighLimit, 
		 int nYLowLimit, int nYHighLimit,
		 int nFloor, int nCeiling,
		 int* nXMaxChan, int* nYMaxChan,
		 int nXRange, int nYRange, int reso,
		 STD(ofstream)& fStr, win_2d* pAttrib);
int GrabPoints1d(int nLowLimit, int nHighLimit, int* nMaxChan,
		 int nHighCnt, int nFloor, int nCeiling, 
		 win_1d* pAttrib, STD(ofstream)& fStr);
static char *ConstructPrintFilename();

void Xamine_PrintSpectrum(XMWidget* w, XtPointer user, 
			  XtPointer call, win_attributed* pAttributes,
			  STD(string) sTitle, int nRows=1, int nCols=1, 
			  int nSpectrumCount=1, int nCurrSpec=1, 
			  int nPageNum=1, char* cmd_file = (char*) NULL);
void   Xamine_Print(XMWidget* w, XtPointer user, XtPointer call);
int    Xamine_getTickInterval(int nRange, int nPixels);
float  Xamine_getMappedTickInterval(float paramrange, int pixels);
STD(string) Xamine_DrawGraphicalObj1d(int nLowLimit, int nHighLimit, int floor,
				 int ceiling, win_1d* pAttrib, 
				 grobj_generic* pObj);
STD(string) Xamine_DrawGraphicalObj2d(int nXLowLimit, int nXHighLimit,
				 int nYLowLimit, int nYHighLimit, int reso,
				 win_2d* pAttrib, grobj_generic* pObj);
STD(string) Xamine_GetSpectrumTitle(int r = -1, int c = -1);
STD(string) Xamine_GetOutputFilename();
int    Xamine_SetDfltPrintOpts(XMWidget* w, XtPointer user, XtPointer call);
int    Xamine_ReadPrintDefaults();
int    Xamine_WritePrintDefaults(struct DefaultPrintOptions dflts);
struct DefaultPrintOptions* Xamine_GetDefaultPrintOptions();
void   Xamine_SetDefaultPrintOptions(struct DefaultPrintOptions* p);

#endif
