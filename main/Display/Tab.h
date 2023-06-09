
/************************************************************************* 
 * Version 1.1  on  July-30-1997
 * (c) 1997 Pralay Dakua (pkanti@hotmail.com)
 *     
 * This is a free software and permission to use, modify, distribute,
 * selling and using for commercial purpose is hereby granted provided
 * that THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE
 * INCLUDED IN ALL COPIES AND THEIR SUPPORTING DOCUMENTATIONS.
 *
 * There is no warranty for this software. In no event Pralay Dakua
 * will be liable for merchantability and fitness of the software and 
 * damages due to this software.
 *
 * Author:
 * Pralay Dakua (pkanti@hotmail.com)
 *
 **************************************************************************/

#ifndef TAB_H
#define TAB_H

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

extern WidgetClass xmTabWidgetClass;

typedef struct _XmTabClassRec *XmTabWidgetClass;
typedef struct _XmTabRec *XmTabWidget;

#define XmNtabFontList "tabFontList"
#define XmCTabFontList "TabFontList"

#define XmNresizeChildren "resizeChildren"
#define XmCResizeChildren "ResizeChildren"

/***** constraint resource name//representations ****/

#define XmNtabLabel "tabLabel"
#define XmCTabLabel "TabLabel"

typedef struct {
	int reason;
	XEvent *event;
	Widget tab_child;
	XmString tab_label;
}XmTabCallbackStruct;

extern void XmTabSetTabWidget(Widget, Widget, Boolean);
extern Widget XmTabGetTabWidget(Widget);

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /**  __TAB_H__  **/

