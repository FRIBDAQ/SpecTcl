static const char* Copyright = "(C) Copyright Michigan State University 1994, All rights reserved";
/*
** Facility:
**    Display processor window pane class
** Abstract:
**    dispwind.cc   - This file defines non-inline functions supported
**                    on the window win_xxx classes.
** Author:
**    Ron Fox
**    NSCL
**    Michigan State University
**    East Lansing, MI 48824-1321
** Revision data:
**    @(#)dispwind.cc	2.2 1/28/94 
*/
static const  char *revision="@(#)dispwind.cc	2.2 1/28/94 ";

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include "dispwind.h"
#include <stdexcept>
#include <SpectrumQueryInterface.h>


int win_db::m_ReadOnce = 0;         // True if flex was used.
void windfilerestart(FILE *fp);

std::string win_geometry::getIdentifier(win_definition* pWin)
{
    std::string identifier;

    if (! pWin->getSpectrumName().empty()) {

        identifier = pWin->getSpectrumName();

    } else {

        if (m_pInterface) {
          Win::SpectrumQueryResults info 
            = m_pInterface->getSpectrumInfo(pWin->spectrum());

          identifier = info.s_name;
        } else {
            identifier = "NotAvailable";
        }
    }

    return identifier;
}

/*
** Method Description:
**   win_attributed::set_defaults() - This method function sets the default
**                                    attributes for a window with display
**                                    attributes.  It is a virtual function
**                                    and will probably be called explicitly
**                                    from override functions in derived
**                                    classes.
**/
void win_attributed::set_defaults()
{
  set_axes(TRUE, TRUE, TRUE);	/* Full axis labelling. */
  set_titles(TRUE, TRUE, FALSE, FALSE, FALSE, FALSE); /* Full spectrum labelling */
  normal();			/* Normal spectrum orientation */
  sample();			/* Reduce using sampling. */
  setfs(256);
  linear();			/* Linear counts axis. */
  nofloor(); noceiling();	/* Don't cut off the heights. */
  update_time = 0;
  noautoupdate();		/* Turn off autoupdate. */
  setmapped(TRUE);
}

/*
** Method description:
**   win_attributed::write   - This method writes out the contents of the
**                             object to file in a format which can be handled
**                             by the windfile attribute grammar described
**                             in windfile.y and windfile.l
**                             Normally this function is called as part of
**                             the win_db::write function.
** Formal Parameters:
**   FILE *f                - Stream file pointer pointing to write to.
** Returns:
**   EOF                    - If failure
**   n                      - number of bytes written if successful.
*/

int win_attributed::write(FILE *f)
{
  int nbytes;		         /* Number of bytes written so-far. */
  int wbytes;			/* Number of bytes (or EOF) for this write */
  win_attributed deflt;		/* Holds the defaults. */
  char scale_txt[132];		/* Text which will hold the scaling line  */
  char msg_txt[132];		/* Error message text buffer */

  nbytes = 0;

  /*  Write out the set of axis label attributes which are different from  */
  /*  the default attributes (instantiated in the deflt object)             */

  if(axis != deflt.showaxes()) {
    wbytes = fprintf(f, "%sAXES AXES\n", axis ? "   " : "   NO");
    if(wbytes == EOF) return EOF;
    nbytes += wbytes;
  }
  if(ticks != deflt.showticks()) {
    wbytes = fprintf(f, "%sAXES TICKS\n", ticks ? "   " : "   NO");
    if(wbytes == EOF) return EOF;
    nbytes += wbytes;
  }
  if(axis_labels != deflt.labelaxes()) {
    wbytes = fprintf(f, "%sAXES LABELS\n", axis_labels ? "   " : "   NO");
    if(wbytes == EOF) return EOF;
    nbytes += wbytes;
  }
  /* Write out the set of spectrum labels which are different from the */
  /* default attributes (as instantiated in the deflt object)  */

  if(name != deflt.showname()) {
    wbytes = fprintf(f, "%sLABEL  NAME\n", name ? "   " : "   NO");
    if(wbytes == EOF) return EOF;
    nbytes += wbytes;
  }
  if(number != deflt.shownum()) {
    wbytes = fprintf(f, "%sLABEL NUMBER\n", number ? "   " : "   NO");
    if(wbytes == EOF) return EOF;
    nbytes += wbytes;
  }
  if(describe != deflt.showdescrip()) {
    wbytes = fprintf(f, "%sLABEL DESCRIPTION\n", describe ? "   " : "   NO");
    if(wbytes == EOF) return EOF;
    nbytes += wbytes;
  }
  if(maxpeak != deflt.showpeak()) {
    wbytes = fprintf(f, "%sLABEL PEAK\n", maxpeak ? "  " : "   NO");
    if(wbytes == EOF) return EOF;
    nbytes += wbytes;
  }
  if(update_info != deflt.showupdt()) {
    wbytes = fprintf(f, "%sLABEL UPDATE\n", update_info ? "   " : "   NO");
    if(wbytes == EOF) return EOF;
    nbytes += wbytes;
  }
  if(label_objects != deflt.showlbl()) { 
    wbytes = fprintf(f, "%sLABEL OBJECTS\n", label_objects ? "   " : "   NO");
    if(wbytes == EOF) return EOF;
    nbytes += wbytes;
  }
  /* If the flipped attribute is not default, then write it out: */

  if(flipped != deflt.isflipped()) {
    wbytes = fprintf(f, "%sFLIPPED\n", flipped? "   " : "   NO");
    if(wbytes == EOF) return EOF;
    nbytes += wbytes;
  }
  /* If the reduction mode isn't the same as the default mode, then write it
  ** out
  */
  if(reduction != deflt.getreduction()) {
    switch(reduction) {
    case sampled:
      wbytes = fprintf(f, "   REDUCTION SAMPLED\n");
      break;
    case summed:
      wbytes = fprintf(f, "   REDUCTION SUMMED\n");
      break;
    case averaged:
      wbytes = fprintf(f, "   REDUCTION AVERAGED\n");
      break;
    default:
      sprintf(msg_txt, "BUGBUGBUG >>> Invalid reduction mode: %d <<<<\n %s", 
	      reduction,
	      "Please save the partially written file for analysis\n");
      fclose(f);
//      Xamine_error_msg(Xamine_Getpanemgr(), msg_txt);
      throw std::runtime_error(msg_txt);
      wbytes = EOF;		/* Ensure the write fails. */
    }
    if(wbytes == EOF) return EOF;
    nbytes += wbytes;
  }
  /* Write the scale attribute no matter what: */
  
  sprintf(scale_txt, "Manual %d", full_scale);
  wbytes = fprintf(f, "   SCALE %s\n", manual ? scale_txt :
		                             " AUTO");

  if(wbytes == EOF) return EOF;
  nbytes += wbytes;

  /* Set axis count attributes if not default: */

  if(log_scale != deflt.islog()) {
    wbytes = fprintf(f, "   COUNTSAXIS %s\n", log_scale ? " LOG" : " LINEAR");
    if(wbytes == EOF) return EOF;
    nbytes += wbytes;
  }
  /* If there are floor and ceiling values, then set them: */

  if(has_floor) {
    wbytes = fprintf(f, "   Floor %d\n", floor);
    if(wbytes == EOF) return EOF;
    nbytes += wbytes;

  }
  if(has_ceiling) {
    wbytes = fprintf(f, "   Ceiling %d\n", ceiling);
    if(wbytes == EOF) return EOF;
    nbytes += wbytes;

  }
  /* If the auto update is set then write that too: */

  if(auto_update) {
    wbytes = fprintf(f, "   Refresh %d\n", update_period);
    if(wbytes == EOF) return EOF;
    nbytes += wbytes;
  }

  /* If the window is mapped, write that as well */
  
  wbytes = fprintf(f, "   MAPPED %d\n", user_mapping);
  if(wbytes == EOF) return EOF;
  nbytes += wbytes;

  return nbytes;				    
}

/*
** Method description:
**    win_1d::set_defaults  - A virtual function which sets the
**                            default values of a 1-d display window.
**                            To do this we first activate 
**                            win_attributed::set_defaults, and then
**                            set default values for expanded and rendition:
*/
void win_1d::set_defaults()
{
  win_attributed::set_defaults(); /* Set parent class defaults */
  unexpand();			/* The spectrum is not in the expanded state */
  bars();			/* Display spectrum as a histogram.         */
  unmap();
}

/*
** Method Description:
**    win_1d::write      - A virtual function which writes the
**                         attributes of a 1-d window to file in a syntax
**                         acceptable to the parse grammer in windfile.y
** Formal Parameters:
**   FILE *f    - Pointer to stream file open for write.
** Returns:
**    EOF    - Failure
**    other  - Number of bytes written to file.
*/
int win_1d::write(FILE *f)
{
  int nbytes;			/* Total number of bytes written. */
  int wbytes;			/* # bytes or (EOF) for this write. */
  win_1d deflt;			/* Instance containing default values. */
  char msg_txt[256];		/* Error message text buffer. */

  nbytes = 0;

  /* Use anscestral write function to write the invariantly formed part of
  ** The object.
  */
  wbytes = win_attributed::write(f); /* Write invariant part of object. */
  if(wbytes == EOF) return EOF;
  nbytes += wbytes;

  /*  Write the expansion line if the spectrum is expanded. */

  if(expanded) {
    wbytes = fprintf(f, "   Expanded %d,%d\n", xlow, xhigh);
    if(wbytes == EOF) return EOF;
    nbytes += wbytes;
  }
  /*  Write the spectrum rendition if it isn't the default */

  if(rendition != deflt.getrend()) {
    switch(rendition) {
    case smoothed:
      wbytes = fprintf(f, "  Rendition SMOOTHED\n");
      break;
    case histogram:
      wbytes = fprintf(f, "   Rendition HISTOGRAM\n");
      break;
    case points:
      wbytes = fprintf(f, "   Rendition POINTS\n");
      break;
    case lines:
      wbytes = fprintf(f, "   Rendition Line\n");
      break;
    default:
      sprintf(msg_txt, "BUGBUGBUG>>> Invalid rendition for 1d %d\n %s", 
	      rendition,
	      "Please save the partially written file for analysis\n");
      throw std::runtime_error(msg_txt);
      wbytes = EOF;
    }
    if(wbytes == EOF) return EOF;
    nbytes += wbytes;
  }
  /* Write the superpositions if there are any:    */

  if(additional_spectra.Count() != 0) {
      SuperpositionListIterator sli(additional_spectra);
      while(!sli.Last()) {
          int id = (sli.Next()).Spectrum();
          if (id >= 0) {
              Win::SpectrumQueryResults info;
              info = m_pParent->getSpectrumInterface()->getSpectrumInfo(id);
              wbytes = fprintf(f,"   Superimpose \"%s\"\n", info.s_name.c_str());
          } else {
              std::string name = sli.Next().SpectrumName();
              if (! name.empty()) {
                wbytes = fprintf(f, "   Superimpose \"%s\"\n", name.c_str());
              } else {
                // skip this if we cannot properly label it.
                wbytes = 0;
              }

          }
          if(wbytes == EOF) {
              sprintf(msg_txt,"%s%s%s",
                      "Failed to write a Superimpose record.\n",
                      "Please save the partially written fiel for analysis\n",
                      "Check quotas and free disk space as well as file name\n");
              throw std::runtime_error(msg_txt);
          }
          else {
              nbytes += wbytes;
          }
      }
  }

  return nbytes;
}


/*
**  Method description:
**    win_2d::set_defaults  - A virtual function which sets the default values
**                            of a 2-d display window.  To do this we first
**                            activate win_attributed:: set defaults, and then
**                            set default values expanded and rendition:
**/
void win_2d::set_defaults()
{
  win_attributed::set_defaults();
  unexpand();
  scatterplot();
  unmap();
}

/*
** Method description:
**    win_2d::write()   - Writes a 2-d object out to a file using a format
**                        which can be parsed by the grammar definition in
**                        windfile.y
** Formal Parameters:
**    FILE *f           - File stream pointer open on the file to write.
** Returns:
**    EOF     - If there was an error.
**    n       - Number of bytes written 
*/
int win_2d::write(FILE *f)
{
  int nbytes;			/* Number of bytes written so far. */
  int wbytes;			/* Number of bytes written this write */
  win_2d deflt;			/* Instance to provide default settings. */
  char msg_txt[256];		/* Error message text. */
  /*
  ** First call our anscestor write method to write the invariant part
  ** of our object:
  */
  wbytes = win_attributed::write(f);
  if(wbytes == EOF) return EOF;
  nbytes = wbytes;

  /*
  ** If expanded, write the expansion data: 
  */

  if(expanded) {
    wbytes = fprintf(f, "   Expanded %d,%d  %d,%d\n",
		     xlow,xhigh, ylow, yhigh);
    if(wbytes == EOF) return EOF;
    nbytes += wbytes;
  }
  /* Put out the rendition if it's not the default: */

  if(rendition != deflt.getrend()) {
    switch(rendition) {
    case scatter:
      wbytes = fprintf(f, "   Rendition Scatter\n");
      break;
    case boxes:
      wbytes = fprintf(f, "   Rendition Box\n");
      break;
    case color:
      wbytes = fprintf(f, "   Rendition Color\n");
      break;
    case contour:
      wbytes = fprintf(f, "   Rendition Contour\n");
      break;
    case lego:
      wbytes = fprintf(f, "   Rendition Lego\n");
      break;
    default:
      sprintf(msg_txt, "BUGBUGBUG >>>> Invalid 2-d rendition %d\n %s", 
	     rendition,
	     "Please save the partial window file for analysis\n");
      throw std::runtime_error(msg_txt);
      wbytes = EOF;
    }
    if(wbytes == EOF) return EOF;
    nbytes += wbytes;
  }

  return nbytes;
}

/*
** Method description:
**     win_db::init()   - Initializer called by all constructors.
**/
void win_db::init()
{
  unzoom();                     // not zoomed.
  clrtitle();                   // untitled.
  for(int i=0; i < WINDOW_MAXAXIS; i++)
    for(int j=0; j < WINDOW_MAXAXIS; j++)
      windows[i][j] = (win_definition *)NULL;
}

/**
** Method description:
**    win_db::define1d   - Puts a 1-d spectrum into a window.
** Formal Parameters:
**    int x,y:
**       The coordinates of the windows.
**    int specnum:
**        The number of the spectrum to put in the window.
*/
void win_db::define1d(int x, int y, int specnum)
{
  assert(exists(x,y));		/* Require the window to exist. */
  if(defined(x,y))  {
    delete windows[x][y];	/* Get rid of old definitions */
    windows[x][y] = (win_definition *)NULL;
  }
  windows[x][y] = (win_definition *)new win_1d(specnum);
  windows[x][y]->setParent(this);
}

void win_db::define1d(int x, int y, const std::string& spectrumName)
{
  assert(exists(x,y));		/* Require the window to exist. */
  if(defined(x,y))  {
    delete windows[x][y];	/* Get rid of old definitions */
    windows[x][y] = (win_definition *)NULL;
  }
  
  windows[x][y] = new win_1d;
  windows[x][y]->setSpectrumName(spectrumName);
  windows[x][y]->setParent(this);
}

/*
** Method description:
**   win_db::define2d  - Puts a 2-d spectrum into a window.
** Formal Parameters:
**   int x,y:
**   int specnum:
*/
void win_db::define2d(int x, int y, int specnum)
{
  assert(exists(x,y));		/* Require that the window exist. */
  if(defined(x,y)) {
    delete windows[x][y];	/* Get rid of the old definition */
    windows[x][y] = (win_definition *)NULL;
  }
  windows[x][y] = (win_definition *)new win_2d(specnum);
}

void win_db::define2d(int x, int y, const std::string& spectrumName)
{
  assert(exists(x,y));		/* Require that the window exist. */
  if(defined(x,y)) {
    delete windows[x][y];	/* Get rid of the old definition */
    windows[x][y] = (win_definition *)NULL;
  }
  windows[x][y] = new win_2d;
  windows[x][y]->setSpectrumName(spectrumName);
}

/*
** Method description:
**   win_db::undefine     - Remove a spectrum from a window.
** Formal Parameters:
**   int x,y:
**    specifies the window.
*/
void win_db::undefine(int x, int y)
{
  assert(exists(x,y));
  if(defined(x,y)) {
    delete windows[x][y];
    windows[x][y] = (win_definition *)NULL;
  }
}

/*
** Method Description:
**   win_db::write()   - This method writes a window database to file.
**                       We rely heavily on the write methods of the objects
**                       which make up the window database.  Essentially
**                       we only need to know how to write out the window
**                       line for each window definition and a comment
**                       indicating who and what we are and when we wrote this
**                       file.
** Formal Parameters:
**    char *filename   - Name of the file to write.
** Return Value:
**    TRUE   - If it worked.
**    FALSE  - If it failed.
*/

int win_db::write(const char *filename)
{
  FILE *config;
  time_t tm;
  int i,j;

  /*
  ** Open the file:
  **/

  config = fopen(filename, "w");
  if(config == NULL) {
    return FALSE;
  }
  /* Write the geometry, title (if present) and a comment header: */

  if(win_geometry::write(config) == EOF) {
    fclose(config);
    return FALSE;
  }
  if(titled) {
    if(fprintf(config,"TITLE %s\n", title) == EOF) {
      fclose(config);
      return FALSE;
    }
  }
  /* Write a comment header which includes the time and our version: */

  time(&tm);
  if(fprintf(config, "# %s Written by %s at %s\n", 
	     filename, revision, ctime(&tm))   ==  EOF) {
    fclose(config);
    return FALSE;
  }
 
  /* Write a window block for each defined window in the window array: */

  for(j = 0; j < win_ny; j++) {
    for(i = 0; i < win_nx; i++) {
        if(defined(i,j)) {
            if(fprintf(config, "Window  %d,%d,\"%s\"\n", i,j,
                       getIdentifier(windows[i][j]).c_str())
                    == EOF){
                fclose(config);
                return FALSE;
            }

            if(windows[i][j]->write(config) == EOF) {
                fclose(config);
                return FALSE;
            }
            if(fprintf(config, "Endwindow\n") == EOF) {
                fclose(config);
                return FALSE;
            }
        }
    }
  }
  fclose(config);
  return TRUE;
}

/*
** Method description:
**   win_db:read()     - This method and the ancillary functions on this
**                       page together with the LEX generated lexical analyzer
**                       in windfile.l and the YACC generated parser in
**                       windfile.y work together to read a window definition
**                       file into the database represented by this object
**                       instance.
** Formal Parameters:
**    char *filename    - The name of the file to read.
** Returns:
**    FALSE    - If it failed.
**    TRUE     - If it worked.
*/
extern FILE *windfilein;
win_db *database;
int windfileparse();
void windfileerror(char *c);
void win_startlex();

int win_db::read(const char *filename)
{
  FILE *config;
  int status;

  /*
  ** Open the file on stdin (where yylex() expects to read the damned thing) 
  */

  config = fopen(filename, "r");
  if(config == NULL) return FALSE;

  if(m_ReadOnce) windfilerestart(config);
  windfilein  = config;
  m_ReadOnce = -1;                            // Indicate we've read once.

  /*
  ** Now do the parse: 
  */
  database = this;
  status = !windfileparse();

  /*
  ** Close the file and return the status:
  */

  fclose(config);

  return status;
}

/*
** yyerror() called with an error message argument when yyparse() detects
** an error:
*/
extern int windfilelex_line;		/* Current line number lexer is on. */
void windfileerror(char *s)
{
  char msg_txt[256];
  /*
  **  BUGBUGBUG   - Must be made a bit more general to support
  **                e.g. errors in X-windows programs.
  */
  sprintf(msg_txt, "Error on line %d: %s", 
	  windfilelex_line, s);
//  Xamine_error_msg(Xamine_Getpanemgr(), msg_txt);
  throw std::runtime_error(msg_txt);
}
extern "C" {
int windfilewrap()
{
  return 1;
}
}
int defaultfilewrap()
{
  return 1;
}

/*
** Method Description:
**   win_attributed::setall:
**     A copy function for attributed windows.  The copy is done in a manner
**     which preserves the type of the destination including the virtual
**     function table.
**   win_attributed::setattribs:
**     Just sets the attribues portion of the target from the source.
** Formal Parameters:
**   win_attributred &that:
**     Source for new attributes.
*/
void  win_attributed::setall(win_attributed &that)
{
  /*  Copy win_definition attributes: */

  win_spnum = that.spectrum();
  axis      = that.showaxes();
  ticks     = that.showticks();
  axis_labels 
            = that.labelaxes();
  name      = that.showname();
  number    = that.shownum();
  describe  = that.showdescrip();
  maxpeak   = that.showpeak();
  update_info
            = that.showupdt();
  label_objects
            = that.showlbl();
  flipped   = that.isflipped();
  reduction = that.getreduction();
  manual    = that.manuallyscaled();
  full_scale= that.getfsval();
  log_scale = that.islog();
  has_floor = that.hasfloor();
  has_ceiling
            = that.hasceiling();
  floor     = that.getfloor();
  ceiling   = that.getceiling();
  auto_update = that.autoupdate_enabled();
  if(auto_update)
    update_period = that.update_interval();

}
void win_attributed::setattribs(win_attributed &that)
{
  axis      = that.showaxes();
  ticks     = that.showticks();

  axis_labels 
            = that.labelaxes();
  name      = that.showname();
  number    = that.shownum();
  describe  = that.showdescrip();
  maxpeak   = that.showpeak();
  update_info
            = that.showupdt();
  label_objects
            = that.showlbl();
  flipped   = that.isflipped();
  reduction = that.getreduction();
  manual    = that.manuallyscaled();
  full_scale= that.getfsval();
  log_scale = that.islog();
  has_floor = that.hasfloor();
  has_ceiling
            = that.hasceiling();
  floor     = that.getfloor();
  ceiling   = that.getceiling();
  auto_update = that.autoupdate_enabled();
  if(auto_update)
    update_period = that.update_interval();
}

/*
** Functional Description:
**   win_1d::RemoveSuperposition:
**     This method removes a spectrum from the superposition list if in
**     fact it is on the list.
** Formal Parameters:
**    int spno:
**       Number of the spectrum.
*/
void win_1d::RemoveSuperposition(int spno)
{
  /* additional_spectra is the superposition list of superimposed spectra */

  SuperpositionListIterator i(additional_spectra);
  
  while(!i.Last()) {
    Superposition s(i.Next());	// Get the next superposition...
    if(spno == s.Spectrum()) {
      i.DeleteCurrent();	// Delete matching spectrum and...
      return;			// Return to caller
    }
  }
}
