/*
** Facility:
**   Xamine - NSCL display program.
** Abstract:
**   text.cc
**     This file contains code which draws text using Xlib calls.
**     We try to simplify the calls in order to make the calling code a bit
**     easier.  This is done by using a family of fonts and hiding its 
**     existence from the callers.
** Author:
**    Ron Fox
**    NSCL
**    Michigan State University
**    East Lansing, MI 48824-1321
*/


/*
** Include files:
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

    /* X include files */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>

    /* Local function prototypes. */

#include "text.h"

/*
** External function definitions:
*/
#ifndef Linux
extern "C" {  void exit(int); }
#endif

/*
**  Constant definitions:
*/
#define XAMINE_FONT_FAMILY "-adobe-courier-medium-r*"

/*
** Class definitions
*/

class FontDatabase {		/* Database of fonts. */
 protected:
  int nfonts;			/* Number of fonts in the database */
  int maxfonts;			/* Maximum number of fonts in database */
  char **font_names;		/* List of font names. */
  XFontStruct **font_properties; /* List of properties of fonts. */
 public:
  FontDatabase(int size = 20) {	/* Constructor */
    nfonts = 0;
    maxfonts = size;
    font_names = new char *[size];
    font_properties = new XFontStruct *[size];
  }
  ~FontDatabase() {		/* Destructor  */
     XFreeFontNames(font_names);
    delete font_properties;
  }
  void AddFont(char *name, XFontStruct *font_properties);
  XFontStruct *SelectFont(char *text, int xpixels, int ypixels);
  XFontStruct *SelectSmallest();
  int Count();
  XFontStruct *GetFont(int n);
  char *GetName(int n);
  int   GetSmallestIndex();
};
/*
** Module visible storage:
*/
static FontDatabase *fonts = NULL;

/*
** The pages below implement the methods in the FontDatabase class which
** for length reasons were not implemented in line.
** Note that while these methods will create global namespace entries,
** the class is module private since it is only declared in the implementation
** file and not in the header file.
*/

/*
** Method Description:
**   FontDatabase::Count:
**     Return the number of fonts in the database.
*/
int FontDatabase::Count()
{
  return nfonts;
}

/*
** Method Description:
**   FontDatabase::GetFont:
**     Returns the font struture associated with a given font index.
** Formal Parameters:
**   int n:
**     Font index.
** Returns:
**   Pointer to the font struct or NULL if there isn't an associated index.
*/
XFontStruct *FontDatabase::GetFont(int n)
{
  if(n < 0) return (XFontStruct *)NULL;
  if(n >= Count()) return (XFontStruct *)NULL;

  return font_properties[n];
}

/*
** Method Description:
**   FontDatabase::GetName:
**     Return a pointer to the name of a font given an index.
** Formal Parameters:
**    int n:
**     Number of font to get
** Returns:
**    Character string pointer to the name or NULL if no such font.
*/
char *FontDatabase::GetName(int n)
{
  if(n < 0) return (char *)NULL;
  if(n >= Count()) return (char *)NULL;

  return font_names[n];
}

/*
** Method Description:
**   FontDatabase::GetSmallestIndex:
**     Returns the index of the smallest font:
*/
int FontDatabase::GetSmallestIndex()
{
  return nfonts-1;
}
/*
** Method Description:
**   FontDatabase::AddFont:
**     This method adds a font to the font database.  Fonts are retained
**     sorted by descending maximum right bearing.
** Formal Parameters:
**   char *name:
**      Null terminated font name.
**   XFontStruct *properties:
**      Properties of the font e.g. returned from XQueryFont.
** NOTE:
**    The program will fail if the database overflows.
*/
void FontDatabase::AddFont(char *name, XFontStruct *properties)
{
  if( (name == (char *)NULL) || (properties == (XFontStruct *)NULL))
    return;			/* Some crazy PC configs can do this. */

  if(nfonts >= maxfonts) {
    fprintf(stderr, "FontDatabase::AddFont %s exceeds database size\n",
	    name);
    exit(-1);
  }

  int size = properties->max_bounds.rbearing; /* Size of inserted font. */

  /* First thing we do is locate the insertion point.  The font goes in front
  ** of the first font with a max_bounds.lbearing that's smaller than ours,
  ** or at the front if there are not any fonts in the database.
  */

  int font_idx = 0;
  while(font_idx < nfonts) {
     if(font_properties[font_idx]->max_bounds.rbearing < size) break;
     font_idx++;
  }
  /* At this point in time, font_idx is the index of the font that's */
  /* Just past the insertion point.  All fonts from the end to here must */
  /* be slid down and then our font goes in here.                   */

  for(int i = nfonts; i > font_idx; i--) {
    font_names[i] = font_names[i-1];
    font_properties[i] = font_properties[i-1];
  }
  nfonts++;			/* Indicate a new font is being added. */
  font_names[font_idx] = name;
  font_properties[font_idx] = properties;

}

/*
** Functional Description:
**    FontDatabase::SelectSmallest:
**      This function returns the smallest font in the list (the last one).
*/
XFontStruct *FontDatabase::SelectSmallest()
{
  return font_properties[nfonts-1];
}


/*
** Functional Description:
**   FontDatabase::SelectFont:
**     This function Selects the appropriate font given the amount of
**     space available for some text that has been supplied.  The most
**     appropriate font is considered to be the largest font which can be
**     bounded within the extents supplied.
** Formal Parameters:
**   char *text:
**     The null terminated text string to fill.
**   int xpixels,ypixels:
**     The size of the region that is supposed to hold the text string.
** Returns:
**   XFontStruct *:
**     A pointer to a font structure which describes the font that makes
**     everything work out.  If none of the fonts in the database will
**     work, then NULL Is returned.
*/
XFontStruct *FontDatabase::SelectFont(char *text, int xpixels, int ypixels)
{
  int direction, ascent, descent;
  XCharStruct overall;
  int tlen = strlen(text);


  for(int i = 0; i < nfonts; i ++) {
    XTextExtents(font_properties[i],
		 text, tlen,
		 &direction, &ascent, &descent,
		 &overall);
    if( ( overall.rbearing <= xpixels) && (overall.ascent <= ypixels)) {
      return font_properties[i];
    }
  }
  return NULL;
}

/*
** Functions below are publics for the remainder of Xamine:
*/
/*
** Functional Description:
**    LoadFonts:
**     This function is responsible for loading fonts and information about
**    them into the font database.
** Formal Parameters:
**    Display *display:
**     X display handle reference.
*/
static void LoadFonts(Display *display)
{
  char **font_names;
  char **font_namelist;

  int  nfonts;

  /*
  ** First get the list of fonts that are in the font family used
  ** by Xamine:
  */
  font_names = XListFonts(display, XAMINE_FONT_FAMILY, 100, &nfonts);
  if(font_names == NULL) {
    fprintf(stderr, "This server does not have the font family %s\n",
	    XAMINE_FONT_FAMILY);
    fprintf(stderr, "Unfortunately Xamine cannot continue from that error\n");
    exit(-1);
  }
  font_namelist = font_names;
  /*
  ** now load the fonts into the database one by one:
  */
  fonts = new FontDatabase(nfonts);

  while(nfonts > 0) {
    fonts->AddFont(*font_names, XLoadQueryFont(display, *font_names));
    font_names++;
    nfonts--;
  }

}

/*
** Functional Description:
**   Xamine_DrawFittedString:
**     This function draws a line of text to a window using a font family
**     that is stored in the fonts FontDatabase.  If the fonts database
**     has not yet been loaded then it will be.  The member of the font
**     family chosen will be the largest one which fits in the rectangle
**     provided by xl,yl, xh,yh.
**     If there are no fonts that will fit, then the string is silently
**     not drawn.
** Formal Parameters:
**    Display *display:
**     Points to the display server which we are displaying on.
**    Drawable drawable:
**     The window or pixmap into which the text will be drawn.
**    GC gc:
**     A graphical context used to draw the text.  Note that the font
**     description will be altered to the font actually chosen to draw
**     the text.
**    int xl,yl:
**     The baseline point (lower left) of the string.
**    int xh,yh:
**      The upper corner position of the string
** NOTE:  yl > yh due to the crappy coordinate scheme chosen by Xwindows.
**    char *string:
**       Pointer to the null terminated string to draw.
*/

void Xamine_DrawFittedString(Display *display, Drawable drawable,
			     GC gc,
			     int xl, int yl, int xh, int yh,
			     char *string)
{
  /*
  ** If the fonts database is empty, then we must load it with the
  ** XAMINE_FONT_FAMILY of fonts.
  */
  if(fonts == NULL) 
    LoadFonts(display);
  /*
  ** Select the font to use, return if there isn't a suitable one.
  */

  XFontStruct *font = fonts->SelectFont(string, (xh-xl), (yl-yh));
  if(font == NULL) return;

  /*
  ** Now set up the GC.
  */
  XSetFont(display, gc, font->fid);
  /*
  ** We write the text centered in the  bounding rectangle
  */
  int dir, ascent, descent;
  XCharStruct overall;
  XTextExtents(font, string, strlen(string), 
	       &dir, &ascent, &descent, &overall);
  int width = overall.width;
  int height= ascent+descent;
  int xloc =  (xh+xl-width)/2;
  int yloc =  (yh+yl+height)/2;

  XDrawString(display, drawable, gc, xloc, yloc, string, strlen(string));
}

/*
** Functional Description:
**    Xamine_SelectFont:
**      This function selects a font to fill a rectangle with text.
**      The font family is searched for the largest font that will fit
**      in the rectangle size given.
** Formal Parameters:
**   Display *d:
**      Display connection identifier.
**   char *string:
**      The string to fit.
**   int xextent, yextent:
**      The size of the rectangle to fit the string into.
** Returns:
**   XFontStruct * corresponding to font selected.  Note that 
**   NULL is returned if there is not a font small enough to fit
**   the text into the box.
*/
XFontStruct *Xamine_SelectFont(Display *d,
			       char *string, int xextent, int yextent)
{
  /*
  ** If the fonts database is empty, then we must load it with the
  ** XAMINE_FONT_FAMILY of fonts.
  */
  if(fonts == NULL) 
    LoadFonts(d);
  /*
  ** Select the font to use, return if there isn't a suitable one.
  */

  XFontStruct *font = fonts->SelectFont(string, xextent, yextent);

  return font;

}

/*
** Functional Description:
**   Xamine_DrawCenteredString:
**      Draw a string centered vertically and horizontally within a rectangle
**      of display space:
** Formal Parameters:
**   Display *display:
**     Identifies the display server which we are connected to.
**   Drawable drawable:
**     Identifies the drawable the string should be written into.
**   XfontStruct *font:
**     Identifies the font that should be used to write the text.
**   GC gc:
**     Graphical context that determines everything else about the text.
**   int xl, yl, xh,yh:
**     Describes the centering rectangle for the string.
**   char *string:
**     The string to write.
** NOTE:
**   If the font structure is a NULL pointer, no draw occurs.
*/
void Xamine_DrawCenteredString(Display *display, Drawable drawable,
			       XFontStruct *font, GC gc,
			       int xl, int yl, int xh, int yh,
			       char *string)
{
  if(font == NULL) return;

  /*
  ** Now set up the GC.
  */
  XSetFont(display, gc, font->fid);
  /*
  ** We write the text centered in the  bounding rectangle
  */
  int dir, ascent, descent;
  XCharStruct overall;
  XTextExtents(font, string, strlen(string), 
	       &dir, &ascent, &descent, &overall);
  int width = overall.width;
  int height= ascent+descent;
  int xloc =  (xh+xl-width)/2;
  int yloc =  (yh+yl+height)/2;

  XDrawString(display, drawable, gc, xloc, yloc, string, strlen(string));
  
}

/*
**  This is an overloaded version of Xamine_DrawCenteredString which uses
**  the font already loaded into the graphical context and takes a center
**  point rather than a bounding box.
** Formal Parameters:
**    Display *display:
**       Identifies the connection to the server.
**    Drawable drawable:
**       Identifies what we're drawing in.
**    GC gc:
**       Graphical context for the draw with the font already loaded in.
**    int xmid, ybase:
**       Midpoint of the baseline of the text.
**    char *string:
**       The string to draw.
*/
void Xamine_DrawCenteredStringImage(Display *display, Drawable drawable, 
			       GC gc, int xmid, int ybase, char *string)
{
  /* As with the primary version of this we first have to measure the
  ** string extent.
  */
  XCharStruct overall;
  int         dir, ascent, descent;
  GContext    gcontext = XGContextFromGC(gc);

  XQueryTextExtents(display, gcontext, string, strlen(string),
		    &dir, &ascent, &descent, &overall);
  int xbase = xmid - (overall.width >> 1); /* Locate left edge of text. */

  /* Now draw the string as an image:  */

  XDrawImageString(display, drawable, gc, xbase, ybase, 
		   string, strlen(string));
}

/*
** Functional Description:
**   Xamine_SelectSmallestFont:
**     This function returns the smallest font of the Xamine font set.
** Formal Parameters:
**     Display *d:
**       Display connection id
** Returns:
**   A pointer to the XFontStruct corresponding to the smallest font in
**   Xamine's font family.
*/
XFontStruct *Xamine_SelectSmallestFont(Display *d)
{
  /* If necessary load the fonts into the server: */

  if(fonts == NULL) 
    LoadFonts(d);

  /* Select the font to use by looking for the smallest: */

  return fonts->SelectSmallest();
}

/*
** Functional Description:
**   Xamine_GetFontCount:
**     If necessry loads the fonts and then returns the number of fonts
**     in the font database.
** Formal Parameters:
**    Display *d:
**     The display on which the font database is composed.
*/
int Xamine_GetFontCount(Display *d)
{
  if(fonts == NULL)
    LoadFonts(d);

  return fonts->Count();
}

/*
** Functional Description:
**   Xamine_GetFontByIndex:
**     If necessary loads the fonts.  Returns a font descriptor given an index.
** Formal Parameters:
**   Display *d:
**     Display identifier.
**  int n:
**     The index.
** Returns:
**   XFontStruct * or NULL if no such index.
*/
XFontStruct *Xamine_GetFontByIndex(Display *d, int n)
{
  if(fonts == NULL)
    LoadFonts(d);

  return fonts->GetFont(n);
}

/*
** Functional Description:
**   Xamine_GetFontNameByIndex:
**     This function returns the name of a font given the index.
**     If necessary, the fonts are loaded.
** Formal Parameters:
**   Display *d:
**      Display Identifier.
**   int n:
**      Font index.
** Returns:
**    char *name  of font or NULL if no corresponding font.
*/
XmStringCharSet Xamine_GetFontNameByIndex(Display *d, int n)
{
  if(fonts == NULL)
    LoadFonts(d);

  return (XmStringCharSet)fonts->GetName(n);

}

/*
** Functional Description:
**   Xamine_GetSmallestFontIndex:
**     Gets the index of the smallest sized font.
** Formal Parameters:
**    Display *d:
**     Display identifier.
** Returns:
**   Index of smallest font.
*/
int Xamine_GetSmallestFontIndex(Display *d)
{
  if(fonts == NULL)
    LoadFonts(d);

  return fonts->GetSmallestIndex();
}
