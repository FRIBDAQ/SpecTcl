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

static const char* Copyright = "(C) Copyright Michigan State University 1994, All rights reserved";
/*
** Facility:
**   Xamine  - NSCL display program.
** Abstract:
**    dfltmgr.cc  - This file implements the pane defaults manager.
**                  The pane defaults manager is responsible for maintaining
**                  and providing access to a set of defaults to use when 
**                  putting a spectrum into a pane.   See dfltmgr.h for
**                  a summary of the public interfaces.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State Univeristy
**   East Lansing, MI 48824-1321
*/

/*
   Change Log:
   $Log$
   Revision 5.3  2006/11/17 16:08:38  ron-fox
   Defect 228 fix: Printer defaults could wind up only partially
   initialized.. this resulted in bad Xamine.Default files and
   could cause printing to fail or crash Xamine.

   Revision 5.2.2.1  2006/11/17 14:41:54  ron-fox
   Defect 228 - Xamine can make bad Xamine.Default files which can cause printing
   to fail because the print defauts can be set to whacky values.

   Revision 5.2  2005/06/03 15:18:55  ron-fox
   Part of breaking off /merging branch to start 3.1 development

   Revision 5.1.2.1  2004/12/21 17:51:13  ron-fox
   Port to gcc 3.x compilers.

   Revision 5.1  2004/11/29 16:55:35  ron-fox
   Begin port to 3.x compilers calling this 3.0

   Revision 4.6.4.1  2004/04/12 16:37:31  ron-fox
   - Use etc for etc stuff with link named Etc rather than the other way around.
   - Extract all Makefile definitions into separate include files so the user makefile
     becomes less variable with time.

   Revision 4.6  2003/11/07 21:32:17  ron-fox
   Unconditionally include config.h

   Revision 4.5  2003/08/25 16:25:30  ron-fox
   Initial starting point for merge with filtering -- this probably does not
   generate a goo spectcl build.

   Revision 4.4  2003/04/02 18:35:24  ron-fox
   Added support for central Xamine.Default files that live in any of:
   $SpecTclHome/etc or $SpecTclHome/etc as well as the user's $HOME/Xamine.Defaults.  The effect of having multiple files is cumulative.

*/

/*
** External include files required:
*/

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <memory.h>
#include <strings.h>
#include <sys/time.h>
#include <sys/types.h>

#include "panemgr.h"
#include "grobjdisplay.h"
#include "refreshctl.h"
#include "printer.h"
#include <time.h>



/*
** External functions used in the default file parse.
*/

extern int defaultfileparse();
extern void defaultfilerestart(FILE *fp);
extern void def_startlex();
extern FILE *defaultfilein;
#ifndef True
#define True -1
#define False 0
#endif
#include "dispwind.h"
#include "dfltmgr.h"

/*
** Module global storage:
*/
static win_attributed GenericDefaults;
static rendition_1d   rend1d = histogram;
static rendition_2d   rend2d = color;


/*
** Functional Description:
**   Xamine_GetDefaultGenericAttributes:
**     This function returns a pointer to the GenericDefaults object.
**     The intent is to provide a mechanism for the caller to modify
**     the generic defaults as, e.g. part of the Options menu entries.
** Returns:
**   Pointer to GenericDefaults
*/
win_attributed *Xamine_GetDefaultGenericAttributes()
{
  return &GenericDefaults;
}

/*
** Functional Description:
**   Xamine_SetDefault1DRendition:
**     This function allows the caller to set the value of the default
**     1-d rendition.
** Formal Parameters:
**     rendition_1d rend:
**       The new value for the rendition.
*/
void Xamine_SetDefault1DRendition(rendition_1d rend)
{
  rend1d = rend;
}

/* 
** Functional Description:
**   Xamine_SetDefault2DRendition:
**     This function allows the caller to set the value of the default
**     2-d rendition.
** Formal Parameters:
**     rendition_2d rend:
**       The new value for the rendition.
*/
void Xamine_SetDefault2DRendition(rendition_2d rend)
{
  rend2d = rend;
}

/*
** Functional Description:
**   Xamine_Construct1dDefaultProperties:
**     This function builds a default property block for a 1-d spectrum.
** Formal Parameters:
**   win_1d *properties:
**      A buffer into which the properties will be built.
*/
void Xamine_Construct1dDefaultProperties(win_1d *properties)
{
  win_1d *p = new win_1d(GenericDefaults);

  p->setrend(rend1d);
  memcpy(properties, p, sizeof(win_1d));
  properties->setmapped(GenericDefaults.ismapped());
  delete p;
} 
/*
** Functional Description:
**   Xamine_Construct2dDefaultProperties:
**      This function builds a default property block for a 2-d spectrum.
** Formal Parameters:
**    win_2d *properties:
**      A buffer into which the properties will be built
*/
void Xamine_Construct2dDefaultProperties(win_2d *properties)
{
  win_2d *p = new win_2d(GenericDefaults);

  p->setrend(rend2d);
  memcpy(properties, p, sizeof(win_2d));
  properties->setmapped(GenericDefaults.ismapped());
  delete p;
}

/*
** Functional Description:
**   ConstructFilename:
**     This local function constructs the defaults file filename.
**     This is done by first getting the value of the environment variable
**     corresponding to the destination directory and then concatenating
**     that with the filename.  The result is a dynamically allocated
**     string which must be delete'd.
*/
static char *ConstructFilename(const char* dir) 
{
  char *name;

  name = new char[strlen(dir)+strlen(XAMINE_DEFAULTS_FILE) + 1];
  strcpy(name, dir);
  strcat(name, XAMINE_DEFAULTS_FILE);
  return name;
}
/*!
   Read a default file given its filename:
   \param filename (const char* [in])
     Name of the default file to read.

   \retval int
    - True - success.
    - False- Failure.
*/
int
Xamine_ReadDefaultFile(const char* filename)
{
  FILE *config;
  int status;
  static int FirstTime = TRUE;

  /* Open the file.. if we can.. */

  config = fopen(filename, "r");
  if(config == NULL) return FALSE;

  if(!FirstTime)
    defaultfilerestart(config);
  else
    FirstTime = FALSE;
  defaultfilein = config;
  status = !defaultfileparse();
  fclose(config);

  return (status != 0);			/* BUGBUGBUG - Stub for now. */
}

/*!
     This function reads the default set of properties from file.
     We first attempt to read in a defaults file in HOME/etc,
     then HOME/etc.  Then we superimpose on that the user's default file:

     The defaults filename is given by XAMINE_DEFAULTS_FILE,
     it will be written to XAMINE_DEFAULTS_DIRECTORY.

    the purpose of this two step default scheme is to allow for system
    wide as well as user specific defaults. While this may be less important
    for Spectrum windows, it is very important for the printer defaults.
 
 \retval int
    - True   - Success
    - False  - Failure
*/
int Xamine_ReadDefaultProperties()
{
  // Try the system wide defaults files:

  char* dir = new char[strlen(HOME)+strlen("/etc") + 1];
  
  // Try in INSTDIR/etc...


  strcpy(dir, HOME);
  strcat(dir, "/etc");		// Old style home dir...
  char* pFilename = ConstructFilename(dir);

  Xamine_ReadDefaultFile(pFilename); // No penalty for failure.

  delete []pFilename;
  
  // Try in INSTDIR/etc:

  strcpy(dir, HOME);
  strcat(dir, "/etc");
  pFilename = ConstructFilename(dir);

  Xamine_ReadDefaultFile(pFilename); // No penalty for failure.

  delete []pFilename;
  delete []dir;
 
  // Read the user's filename


  dir = getenv(XAMINE_DEFAULTS_DIRECTORY);
  if(!dir) dir = "";		// If no env var, use current dir.

  char *filename = ConstructFilename(dir);
  int stat =  Xamine_ReadDefaultFile(filename);

  delete []filename;
  return stat;

}
#ifdef FLEXV2
void defaultfilerestart(FILE *config)
{
  defaultfilein = config;
  def_startlex();
}
#endif

/*
** Functional Description:
**   Xamine_SaveDefaultProperties:
**     This function writes the default set of properties to file.
**     The defaults filename is given by XAMINE_DEFAULTS_FILE.
**     It will be written to XAMINE_DEFAULTS_DIRECTORY.
** Returns:
**   True   - Success
**   False  - Failure
*/
int Xamine_SaveDefaultProperties()
{
  char *filename;
  FILE *fp;

  /* Construct the filename and open the file for write: */
  char* dir = getenv(XAMINE_DEFAULTS_DIRECTORY);
  if(!dir) dir = "";		// If no env var, use current dir.
  filename = ConstructFilename(dir); /* Glue together the filename. */
  fp = fopen(filename, "r+");     /* Try to open existing for read/write */
  if(fp) {			  // Deal with filesystems which support
    fclose(fp);			  // File versioning by deleting the
    unlink(filename);		  // prior version.
  }
  fp =  fopen(filename, "w");	  /* This is done to deal with filesystems */

  if(fp == NULL) {
    delete filename;
    return False;
  }

  /* Write the header... */
  time_t t;

  time(&t);			/* Get the time in internal format.. */

  fprintf(fp, "\n# Xamine default file saved by "
	  "Xamine_SaveDefaultProperties on %s\n",
	  ctime(&t));

  /* Write the win_attributed part of the defaults: */

  fprintf(fp, "Attributes\n");
  GenericDefaults.write(fp);
  fprintf(fp, "EndAttributes\n");

  /* Write the renditions:  */

  char *rend_text;
  switch(rend1d) {
  case smoothed:
    rend_text = "Smoothed";
    break;
  case histogram:
    rend_text = "Histogram";
    break;
  case points:
    rend_text = "Points";
    break;
  case lines:
    rend_text = "Line";
    break;
  default:
    fprintf(stderr, "BUGBUGBUG>> Invalid 1-d default rendition at write\n");
    fprintf(stderr, "Please save the partially written file for analysis\n");
    fclose(fp);
    exit(-1);
  }
  fprintf(fp, "Rendition_1d %s\n", rend_text);


  switch(rend2d) {
  case scatter:
    rend_text = "Scatter";
    break;
  case boxes:
    rend_text = "Box";
    break;
  case color:
    rend_text = "Color";
    break;
  case contour:
    rend_text = "Contour";
    break;
  case lego:
    rend_text = "Lego";
    break;
  default:
    fprintf(stderr, "BUGBUGBUG>> Invalid 2-d default rendition at write\n");
    fprintf(stderr, "Please save the partially written file for analysis\n");
    fclose(fp);
    exit(-1);
  }


  fprintf(fp, "Rendition_2d %s\n", rend_text);


  /* Write the Grobj label font index */

  Display *d = XtDisplay(Xamine_Getpanemgr()->getid());
  int    idx = Xamine_GetObjectLabelIndex(d);

  fprintf(fp, "Object_Label_Font %d\n", idx);
  
  /* Write the printer dialog options */
  struct DefaultPrintOptions* dflts = Xamine_GetDefaultPrintOptions();
  if(dflts) {
    fprintf(fp, "PrintOptions\n");
    fprintf(fp, "   Layout %d\n", dflts->layout);
    fprintf(fp, "   PrintNum %d\n", dflts->num);
    fprintf(fp, "   Destination %d\n", dflts->dest);
    fprintf(fp, "   Resolution %d\n", dflts->res);
    fprintf(fp, "   FileType %d\n", dflts->file_type);
    fprintf(fp, "   PrintSize %.2f,%.2f\n", dflts->xlen, dflts->ylen);
    fprintf(fp, "   SpectrumOptions %d,%d,%d,%d\n", dflts->time_stamp, 
	    dflts->color_pal, dflts->contours, dflts->symbols);
    fprintf(fp, "   PrintCommand \"%s\"\n", dflts->print_cmd);
    fprintf(fp, "   PrintGeometry %s,%s\n", dflts->rows, dflts->cols);
    fprintf(fp, "EndPrintOptions\n");
  }

  /* Close the file, return the filename to free store and exit true. */

  fclose(fp);
  delete filename;		/* Delete the storage associated with it. */
  return True;
}

/*
** Functional Description:
**   Xamine_ApplyDefaultsEverywhere:
**       This function applies the default properties to all panes which
**       have spectra in them.
*/
void Xamine_ApplyDefaultsEverywhere()
{
  int rows = Xamine_Panerows();
  int cols = Xamine_Panecols();
  win_1d prop1;
  win_2d prop2;

  Xamine_Construct1dDefaultProperties(&prop1);
  Xamine_Construct2dDefaultProperties(&prop2);

  for(int r = 0; r < rows; r++) {
    for(int c = 0; c < cols; c++) {
      win_attributed *a;
      a = Xamine_GetDisplayAttributes(r,c);
      if(a != NULL) {
	if(a->is1d()) {
	  Xamine_SetDisplayAttributes(r,c, (win_attributed *)&prop1);
	}
	else {
	  Xamine_SetDisplayAttributes(r,c, (win_attributed *)&prop2);
	}
	Xamine_RedrawPane(c,r);
      }
    }
  }
}


