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
	       } PrinterType;
	       


void Xamine_SetupPrinter(XMWidget *w, XtPointer user, XtPointer call);
char *Xamine_GetPrintCommand();
PrinterType Xamine_GetPrinterType();

void Xamine_printSelected(XMWidget* w, XtPointer user, XtPointer call);
void Xamine_printAll(XMWidget* w, XtPointer user, XtPointer call);
#endif
