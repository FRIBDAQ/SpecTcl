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

typedef enum {
               postscript,
	       color_postscript,
	       color_sixel,
	       ascii
	       } PrinterType;
	       


void Xamine_SetupPrinter(XMWidget *w, XtPointer user, XtPointer call);
char *Xamine_GetPrinterTempFilename();
char *Xamine_GetPrintCommand();
char *Xamine_GetExpandePrintCommand(char *buf);
PrinterType Xamine_GetPrinterType();
#endif
