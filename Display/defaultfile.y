%{
/*
**  Facility:
**    NSCL display system window file I/O subsystem.
**  Abstract:
**    winfile.y   - This file contains a YACC/BISON description of the
**                  grammar of the context free language which is used to
**                  describe window save files.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** Revision level:
**   @(#)defaultfile.y	2.1 12/22/93 
*/
static char *yaccrevlevel = "@(#)defaultfile.y	2.1 12/22/93  - windows file parser\n";
extern void windfileerror(char *text);
/* #define SHARED extern */
#include <assert.h>
#ifdef unix
#include <memory.h>
#endif
#ifdef VMS
#include <stdlib.h>
#endif
#include <stdio.h>

#include "windio.h"
#include "dispwind.h"
#include "dispshare.h"
#include "dfltmgr.h"
#include "grobjdisplay.h"
#include "panemgr.h"
#define defaultfileerror(text) windfileerror((text))
#define yylex         defaultfilelex
#define windfilelex() defaultfilelex
#define yyleng dfyyleng
#define yytext dfyytext
#define getlexrev() dfgetlexrev()
#define get_title() dfget_title()
#define win_startlex() def_startlex()
#define yynerrs defaultfilenerrs
static int ticks = 0, 
    labels= 0, 
    axes  = 0;	/* Axis label attribute flags. */
static int name        = 0, 
    number      = 0, 
    description = 0, 
    peak        = 0, 
    update      = 0, 
    objects     = 0; /* Title attr. flags */
static struct limit { int low;
		int high;
	      } xlim, ylim, limits;	/* Limits structures. */


extern int windfilelex_line;
static win_attributed *current = Xamine_GetDefaultGenericAttributes();

%}
%union {
   int integer;
   char string[80];
 }

%token  NAME
%token  NUMBER
%token  DESCRIPTION
%token  PEAK
%token  UPDATE
%token  OBJECTS
%token FLIPPED
%token NOFLIPPED
%token TITLE
%token GEOMETRY
%token ZOOMED
%token WINDOW
%token  ZOOMED
%token AXES
%token NOAXES
%token  TICKS
%token  LABELS
%token LABEL
%token NOLABEL
%token REDUCTION
%token  SAMPLED
%token  SUMMED
%token  AVERAGED
%token SCALE
%token  AUTO
%token  MANUAL
%token COUNTSAXIS
%token  LOG
%token  LINEAR
%token FLOOR
%token CEILING
%token EXPANDED
%token RENDITION
%token  SMOOTHED
%token  HISTOGRAM
%token  POINTS
%token  LINE
%token  SCATTER
%token  BOX
%token  COLOR
%token  CONTOUR
%token  LEGO
%token  REFRESH
%token <integer> INTEGER
%token COMMA
%token ENDWINDOW
%token ENDLINE
%token ATTRIBUTES
%token ENDATTRIBUTES
%token RENDITION_1D
%token RENDITION_2D
%token SUPERIMPOSE
%token GROBJFONT
%token UNMATCHED
%token DEFAULTFILE

%token <string> QSTRING
%%
setup_file:    setup_filel | blankline setup_filel
               ;

setup_filel:	description
		;

description:	window_clause endwindow_clause rendition_clauses
		| window_clause attribute_clauses endwindow_clause 
                  rendition_clauses
                | window_clause endwindow_clause rendition_clauses font_clauses
                | window_clause attribute_clauses endwindow_clause 
                  rendition_clauses font_clauses

		;

window_clause: ATTRIBUTES blankline
		;

endwindow_clause: ENDATTRIBUTES blankline
  		;

attribute_clauses: attribute_clause | attribute_clauses attribute_clause
		;

attribute_clause: axes_clause      | labels_clause  | flipped_clause 
		| reduction_clause | scale_clause   | countsaxis_clause
		| floor_clause     | ceiling_clause 
		 | refresh_clause
		;

axes_clause:      AXES axis_attributes blankline
                  {
		    assert(current != NULL);
		    if(ticks)  current->tickson();
		    if(labels) current->axis_labelson();
		    if(axes)   current->axeson();
		    ticks = labels = axes = 0;
		  }
                | NOAXES axis_attributes blankline
                  {
		    if(ticks) current->ticksoff();
		    if(labels)current->axis_labelsoff();
		    if(axes)  current->axesoff();
		    ticks = labels = axes = 0;
		  }
               | AXES blankline 
                 {
		   assert(current != NULL);
		   current->set_axes(TRUE, TRUE, TRUE);
		 }
               | NOAXES blankline
                 {
		   assert(current != NULL);
		   current->set_axes(FALSE,FALSE,FALSE);
		 }

		;

axis_attributes: axis_attribute
               | axis_attributes axis_attribute
		;

axis_attribute:	 TICKS 
                   {
		     ticks = TRUE;
		   }
               | LABELS 
                   {
		     labels = TRUE;
		   }
               | AXES
                   {
		     axes = TRUE;
		   }
		;

labels_clause:	LABEL blankline 
                 {
		   assert(current != NULL);
		   current->set_titles(TRUE, TRUE, TRUE, TRUE, TRUE, TRUE);
		 }
               | NOLABEL blankline
                 {
		   assert(current != NULL);
		   current->set_titles(FALSE, FALSE, FALSE,
					 FALSE, FALSE, FALSE);
		 }
                | LABEL label_attributes blankline
                  {
		    assert(current != NULL);
		    if(name)        current->dispname();
		    if(number)      current->dispid();
		    if(description) current->dispid();
		    if(peak)        current->disppeak();
		    if(update)      current->dispupd();
		    if(objects)     current->labelobj();
		    name = number = description = peak = update = objects = 0;
		  }
                | NOLABEL label_attributes blankline
                  {
		    assert(current != NULL);
		    if(name)        current->hidename();
		    if(number)      current->hideid();
		    if(description) current->hidedescr();
		    if(peak)        current->hidepeak();
		    if(update)      current->hideupd();
		    if(objects)     current->unlabelobj();
		    name = number = description = peak = update = objects = 0;
		  }
		;

label_attributes: label_attribute | label_attributes label_attribute
  		;

label_attribute:  NAME 
                   {
		     name = TRUE;
		   }
                | NUMBER 
                   {
		     number = TRUE;
		   }
                | DESCRIPTION {
		  description = TRUE;
		}
                | PEAK {
		  peak  = TRUE;
		}
                | UPDATE {
		  update = TRUE;
		}
                | OBJECTS {
		  objects = TRUE;
		}
                ;

flipped_clause:	FLIPPED blankline
                { 
		  assert(current != NULL);
		  current->sideways();
		}
		 
              | NOFLIPPED blankline 
                {
		  assert(current != NULL);
		  current->normal();
		} 
		;

reduction_clause: REDUCTION reduction_attribute blankline
		;

reduction_attribute:  SAMPLED {
                        assert(current != NULL);
			current->sample();
		      }
                    | SUMMED {
		      assert(current != NULL);
		      current->sum();
		    }
                    | AVERAGED {
		      assert(current != NULL);
		      current->average();
		    }
		;

scale_clause: SCALE scale_attribute blankline
		;

scale_attribute: AUTO {
                       assert(current != NULL);
		       current->autoscale();
		     }
               | MANUAL INTEGER
                 {
		   assert(current != NULL);
		   current->setfs($2);
		 }
		;

countsaxis_clause:  COUNTSAXIS countsaxis_attributes blankline
		;

countsaxis_attributes: LOG {
			     assert(current != NULL);
			     current->log();
			   }
                       | LINEAR {
			 assert(current != NULL);
			 current->linear();
		       }
		;

floor_clause: FLOOR INTEGER blankline
                 {
		   assert(current != NULL);
		   current->setfloor($2);
		 }
		;

ceiling_clause: CEILING INTEGER blankline
                {
		  assert(current != NULL);
		  current->setceiling($2);
		}
		;

refresh_clause:   REFRESH INTEGER blankline
                {
		  assert(current != NULL);
		  current->update_interval($2);	/* Set the update interval. */
                }
                ;


rendition_clauses: RENDITION_1D rend1_attribute blankline RENDITION_2D 
                   rend2_attribute blankline

rend1_attribute:  SMOOTHED 
                  { 
		    Xamine_SetDefault1DRendition(smoothed);
		  }
                | HISTOGRAM 
                  {
		    Xamine_SetDefault1DRendition(histogram);
		  }
                | POINTS 
                  { 
		    Xamine_SetDefault1DRendition(points);
		  }
                | LINE
                  {
		    Xamine_SetDefault1DRendition(lines);
		  }
		;

rend2_attribute:  SCATTER 
                  { 
		    Xamine_SetDefault2DRendition(scatter);
		  }
                | BOX
                  { 
		    Xamine_SetDefault2DRendition(boxes);
		  }
                | COLOR 
                  { 
		    Xamine_SetDefault2DRendition(color);
		  }
                | CONTOUR 
                  {
		    Xamine_SetDefault2DRendition(contour);
		  }
                | LEGO
                  {
		    Xamine_SetDefault2DRendition(lego);
		  }
		;

font_clauses: GROBJFONT INTEGER blankline
               {
		 Display *d;
		 d = XtDisplay(Xamine_Getpanemgr()->getid());
		 Xamine_SetObjectLabelIndex(d, $2);
               }

blankline:  ENDLINE | ENDLINE blankline
          ;
%%
