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

#include <fstream.h>
#include <dispgrob.h>

#include <string>
#include "XMList.h"

typedef enum {
  postscript,
  color_postscript,
} PrinterType;

typedef enum { 
  portrait,
  landscape           
} PrintLayout;

typedef enum {
  printall,
  printsel,
} PrintNum;

typedef enum {
  tofile,
  toprinter,
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

void Xamine_SetupPrinter(XMWidget *w, XtPointer user, XtPointer call);
void Xamine_PrintSpectrumDialog(XMWidget *w, XtPointer user, XtPointer call);
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

void Xamine_PrintSpectrum(XMWidget* w, XtPointer user, 
			  XtPointer call, win_attributed* pAttributes,
			  string sTitle, int nRows=1, int nCols=1, 
			  int nSpectrumCount=1, int nCurrSpec=1, 
			  int nPageNum=1);
void Xamine_Print(XMWidget* w, XtPointer user, XtPointer call);
int  Xamine_getTickInterval(int nRange, int nPixels);
string Xamine_DrawGraphicalObj1d(int nLowLimit, int nHighLimit, int floor,
				 int ceiling, win_1d* pAttrib, 
				 grobj_generic* pObj);
string Xamine_DrawGraphicalObj2d(int nXLowLimit, int nXHighLimit,
				 int nYLowLimit, int nYHighLimit, 
				 win_2d* pAttrib, grobj_generic* pObj);
string Xamine_GetSpectrumTitle(int r = -1, int c = -1);
string Xamine_GetOutputFilename();

#endif
