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
static char *sccsinfo="@(#)dfltmgr.cc	8.1 6/23/95 ";


/*
** External include files required:
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef unix
#include <memory.h>
#include <strings.h>
#include <sys/time.h>
#include <sys/types.h>
#endif
#ifdef VMS
#include <string.h>
#include <time.h>
#include <types.h>
#define unlink remove
#endif

#include "panemgr.h"
#include "grobjdisplay.h"
#include "refreshctl.h"

#ifdef ultrix
#include <time.h>
extern "C" {
  time_t time(time_t *tloc);
}
#endif
extern "C" {
  void exit(int); 
}

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
static char *ConstructFilename() 
{
  char *dir;
  char *name;

  dir = getenv(XAMINE_DEFAULTS_DIRECTORY);
  if(dir == NULL) dir = "";	/* Point to an empty string if no dir. */
  name = new char[strlen(dir)+strlen(XAMINE_DEFAULTS_FILE) + 1];
  strcpy(name, dir);
  strcat(name, XAMINE_DEFAULTS_FILE);
  return name;
}

/*
** Functional Description:
**   Xamine_ReadDefaultProperties:
**     This function reads the default set of properties from file.
**     The defaults filename is given by XAMINE_DEFAULTS_FILE,
**     it will be written to XAMINE_DEFAULTS_DIRECTORY.
** Returns:
**    True   - Success
**    False  - Failure
*/
int Xamine_ReadDefaultProperties()
{
  char *filename = ConstructFilename();
  FILE *config;
  int status;
  static int FirstTime = TRUE;

  /* Open the file.. if we can.. */

  config = fopen(filename, "r");
  delete filename;
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

  filename = ConstructFilename(); /* Glue together the filename. */
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

  fprintf(fp, "\n# Xamine default file saved by %s on %s\n",
	  sccsinfo, ctime(&t));

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
