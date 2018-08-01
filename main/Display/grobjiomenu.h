/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


/*
** Facility:
**   Xamine - NSCL Histogram display program.
** Abstract:
**   grobjiomenu.h  - This file contains definitions to be used by clients
**                  of the Graphical object file I/O software:
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** Revision:
**   @(#)grobjiomenu.h	8.1 6/23/95 
*/

#ifndef GROBJIOMENU_H
#define GROBJIOMENU_H

/*
** Define the environment variable that can hold a default grobj directory
*/
#define XAMINE_GROBJ_ENV "XAMINE_OBJECT_DIR"

struct Xamine_grobjopen_client_data {
                                   void (*callback)(XMWidget *,
						    XtPointer,
						    XtPointer);
				   XtPointer      client_data;
				   Boolean        mustexist;
				 };

extern Xamine_grobjopen_client_data Xamine_Open_grobj_read;
extern Xamine_grobjopen_client_data Xamine_Open_grobj_write;
/*
**  Display the file open dialog
*/
void Xamine_Open_grobj_file(XMWidget *w, 
			     XtPointer client_data,
			     XtPointer callback_data);
/*
**  Read file callback
*/
void Xamine_Read_grobj_file(XMWidget *w,
			     XtPointer client_data,
			     XtPointer callback_data);
/*
** Write file callback
*/
void Xamine_Write_grobj_file(XMWidget *w,
			      XtPointer client_data,
			      XtPointer callback_data);


#endif
