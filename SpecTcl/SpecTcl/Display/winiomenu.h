/*
** Facility:
**   Xamine - NSCL Histogram display program.
** Abstract:
**   winiomenu.h  - This file contains definitions to be used by clients
**                  of the Window file I/O software:
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** Revision:
**   @(#)winiomenu.h	8.1 6/23/95 
*/

#ifndef WINIOMENU_H
#define WINIOMENU_H

/*
**  The environment variable defined below is checked.  If it is defined
**  and if it points to a directory, then it is assumed to 
**  contain the window files the user is interested in using for this
**  run of Xamine.
*/

#define XAMINE_WINDOW_ENV "XAMINE_WINDOW_DIR" 

struct Xamine_winopen_client_data {
                                   void (*callback)(XMWidget *,
						    XtPointer,
						    XtPointer);
				   XtPointer      client_data;
				   Boolean        mustexist;
				 };

extern Xamine_winopen_client_data Xamine_Open_win_read;
extern Xamine_winopen_client_data Xamine_Open_win_write;
/*
**  Display the file open dialog
*/
void Xamine_Open_window_file(XMWidget *w, 
			     XtPointer client_data,
			     XtPointer callback_data);
/*
**  Read file callback
*/
void Xamine_Read_window_file(XMWidget *w,
			     XtPointer client_data,
			     XtPointer callback_data);
/*
** Write file callback
*/
void Xamine_Write_window_file(XMWidget *w,
			      XtPointer client_data,
			      XtPointer callback_data);

char *Xamine_GetLastWindowFile();
char *Xamine_GetSearchMask(char *envstr, char *fallbackdir, 
			   char *mask);
#endif
