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
**   @(#)windfile.y	8.1 6/23/95 
*/
static char *yaccrevlevel = "@(#)windfile.y	8.1 6/23/95  - windows file parser\n";
char *get_windfiletitle();
#include <assert.h>
#include <memory.h>
#include <stdio.h>

#include "windio.h"
#include "dispwind.h"
#include "dispshare.h"
#include "printer.h"
#ifndef DEBUG
extern win_db *database;	/* Setup by the caller. */
#else
win_db db, *database=&db;
#endif
extern int windfilelex_line;
static int superimposed(win_1d *a, int specid);
static int superimposable(int tgt, int specid);
int specis1d(int spec);
int specisundefined(int spec);
static int x,y;
static int specnum;
static win_attributed *current;
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

extern spec_shared *xamine_shared;
void windfileerror(char *c);
%}
%union {
  int integer;
  double real;
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
%token <real>    REAL
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
%token MAPPED

%token <string> QSTRING
%%
setup_file:    setup_filel | blankline setup_filel
                ;

setup_filel:	 geometry | geometry ident
                 | geometry descriptions  | geometry ident descriptions 
		;

ident:		title 
                | title blankline
                ;

title:         TITLE
                { 
                  database->settitle(get_windfiletitle()); }
		;

geometry:       geometry_line
                ;

geometry_line:	GEOMETRY INTEGER COMMA INTEGER blankline
                   { database->cleardb();
		     int a = $2, b= $4;
		     database->setx($2); database->sety($4);
                   }
		| GEOMETRY INTEGER COMMA INTEGER zoom_clause blankline
                   { database->cleardb();
                     database->setx($2); database->sety($4);
		     database->zoom(x,y);
                   }
  		;

zoom_clause:    ZOOMED INTEGER COMMA INTEGER
                 { x = $2;
                   y = $4;
		 }
		;

descriptions:	description
              | descriptions description
	      ;


description:	window_clause endwindow_clause 
		| window_clause attribute_clauses endwindow_clause
		;

window_clause: WINDOW INTEGER COMMA INTEGER COMMA spectrum blankline
                  {
		    if(specisundefined(specnum)) {
		       yyerror("Spectrum is not defined\n");
		       return -1;
		    }
		    else {
			    if(specis1d(specnum)) {
			      database->define1d($2,$4, specnum);
			    } else {
			      database->define2d($2,$4,specnum);
			    }
			    x = $2;
			    y = $4;
			    current = database->getdef(x,y);
			    if(current == NULL) {
			      yyerror("Internal consistency error -- window is not defined\n");
			      return -1;
			    }
			  }
		    }
		;

spectrum:  INTEGER 
           {
	     specnum = $1;	/* Just return the number as specnum. */
	   }
         | QSTRING
           {
	     specnum = xamine_shared->getspecid(yylval.string); /* translate name->id */
	     if(specnum == -1){
	       yyerror("Spectrum name does not match a valid spectrum");
	       return -1;
	     }
           }
	;

endwindow_clause: ENDWINDOW blankline
                   {
		     x = -1; y = -1;
		     current = NULL;
		   }
  		;

attribute_clauses: attribute_clause | attribute_clauses attribute_clause
		;

attribute_clause: axes_clause      | labels_clause  | flipped_clause 
		| reduction_clause | scale_clause   | countsaxis_clause
		| floor_clause     | ceiling_clause | expanded_clause
		| rendition_clause | refresh_clause | superposition_clause
                | mapped_clause
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

mapped_clause: MAPPED INTEGER blankline {
  assert(current != NULL);
  current->setmapped($2);
};

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

expanded_clause:  EXPANDED xlimit  ylimit blankline
                  { assert(current);
		    if(current->is1d()) {
		      windfilelex_line--; /* Compensate for blankline. */
		      windfileerror("2-D expansion of a 1-d spectrum\n");
		      windfilelex_line++;
		    } else {
		      int spec = current->spectrum();
		      int topx = xamine_shared->getxdim(spec);
		      int topy = xamine_shared->getydim(spec);
		      if( (xlim.low < 0) || (xlim.high >= topx) ||
		          (ylim.low < 0) || (ylim.high >= topy)) {
			 windfilelex_line--;
			 windfileerror("2-d expansion limits out of range\n");
			 windfilelex_line++;
		      }
		      else {
		         ((win_2d *)current)->expand(xlim.low, xlim.high,
						     ylim.low, ylim.high);
                      }
		    }
		  }
 
                |  EXPANDED limits blankline 
                  { assert(current);
		    if(!current->is1d()) {
		      windfilelex_line--; /* compensate for blankline. */
		      windfileerror("1-D expansion of a 2-d spectrum\n");
		      windfilelex_line++;
		    } else {
		      int spec = current->spectrum();
		      int topx = xamine_shared->getxdim(spec);
		      if( (limits.low < 0) || (limits.high >= topx)) {
			 windfilelex_line--;
		         windfileerror("1-d Expansion limits outo f range\n");
			 windfilelex_line++;
		      }
		      else {
		         ((win_1d *)current)->expand(limits.low, limits.high);
		      }
		    }
		  }
		;

xlimit:         limits
                { memcpy(&xlim, &limits, sizeof(struct limit)); }
                ;

ylimit:         limits
                { memcpy(&ylim, &limits, sizeof(struct limit)); }
                ;

limits:		INTEGER COMMA INTEGER
                { limits.low = $1;
		  limits.high= $3;
		}
		;

rendition_clause: RENDITION rend1_attribute blankline 
		| RENDITION rend2_attribute blankline
		;
rend1_attribute:  SMOOTHED 
                  { assert(current);
		    if(!current->is1d()) {
		      windfileerror("Attempted to set 1-d rendition on 2-d spectrum\n");
		    } else {
		      ((win_1d *)current)->smooth();
		    }
		  }
                | HISTOGRAM 
                  { assert(current);
		    if(!current->is1d()) {
		      windfileerror("Attempted to set 1-d rendition on 2-d spectrum\n");
		    } else {
		      ((win_1d *)current)->bars();
		    }
		  }
                | POINTS 
                  { assert(current);
                    if(!current->is1d()) {
		      windfileerror("Attempted to set 1-d rendition on 2-d spectrum\n");
		    } else {
		      ((win_1d *)current)->pts();
		    }
		  }
                | LINE
                  { assert(current);
		    if(!current->is1d()) {
		      windfileerror("Attempted to set 1-d rendition on 2-d spectrum\n");
		    } else {
		      ((win_1d *)current)->line();
		    }
		  }
		;

rend2_attribute:  SCATTER 
                  { assert(current);
		    if(current->is1d()) {
		      windfileerror("Attempted to set 2-d rendition on 1-d spectrum\n");
		    } else {
		      ((win_2d *)current)->scatterplot();
		    }
		  }
                | BOX
                  { assert(current);
		    if(current->is1d()) {
		      windfileerror("Attempted to set 2-d rendition on 1-d spectrum\n");
		    } else {
		      ((win_2d *)current)->boxplot();
		    }
		  }
                | COLOR 
                  { assert(current);
		    if(current->is1d()) {
		      windfileerror("Attempted to set 2-d rendition on 1-d spectrum\n");
		    } else {
		      ((win_2d *)current)->colorplot();
		    }
		  }
                | CONTOUR 
                  { assert(current);
		    if(current->is1d()) {
		      windfileerror("Attempted to set 2-d rendition on 1-d spectrum\n");
		    } else {
		      ((win_2d *)current)->contourplot();
		    }
		  }
                | LEGO
                  { assert(current);
		    if(current->is1d()) {
		      windfileerror("Attempted to set 2-d rendition on 1-d spectrum\n");
		    } else {
		      ((win_2d *)current)->legoplot();
		    }
		  }
		;


superposition_clause:   SUPERIMPOSE spectrum blankline
                   {
		     assert(current);
		     win_1d *at1 = (win_1d *)current;
		     if(!current->is1d()) {
		       windfileerror("Superimpose on 2-d spectrum ignored");
		     }
		     else if(!superimposable(current->spectrum(), specnum)) {
		       windfileerror("Incompatible superimpose ignored");
		     }
		     else if(superimposed(at1, specnum)) {
		       windfileerror("Redundant superimpose ignored");
		     }
		     else {
		       SuperpositionList &sl = at1->GetSuperpositions();
		       if(sl.Count() < MAX_SUPERPOSITIONS) {
			 sl.Add(specnum);
		       }
		       else {
			 windfileerror("Too many superimposes, extra ignored");
		       }
		     }
		   }
                   ;

blankline:  ENDLINE | ENDLINE blankline 
;


%%
int specis1d(int spec)
{
  return ((xamine_shared->dsp_types[spec-1] == onedlong) ||
	  (xamine_shared->dsp_types[spec-1] == onedword));
}

/*
** Returns true if target and specrum can be superimposed.
** Target is assumed to already be known to be 1-d.
*/
static int superimposable(int target, int spectrum)
{
  if(!specis1d(spectrum)) return 0;

  if(xamine_shared->getxdim(spectrum) == xamine_shared->getxdim(target))
    return -1;

  return 0;
}
/*
** Returns true if the spectrum is already superimposed on the pane.
*/
static int superimposed(win_1d *a, int spectrum)
{

  if(spectrum == a->spectrum()) return -1;

  SuperpositionList &sl = a->GetSuperpositions();
  SuperpositionListIterator sli(sl);

  if(sl.Count() <= 0)  return 0;
  while(!sli.Last()) {
    if((sli.Next()).Spectrum() == a->spectrum()) return -1;
  }
  return 0;
}
char *getsyntaxrev()
{
  return yaccrevlevel;
}

int specisundefined(int spec)
{
  return (xamine_shared->dsp_types[spec-1] == undefined);
}
