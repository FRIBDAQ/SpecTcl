%{
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "grobjio.h"
#include "dispgrob.h"
#define yyleng grobyyleng
#define yytext grobyytext
void yyerror(char *txt);

int yylex();

static grobj_generic *current;
extern grobj_database *grobj_db;	/*  Pointer to graphical object database */
static int ix,iy;
static char *typenames[] = { " Invalid ",
                             "Cut",
			     "1-d Summing Region",
			     "Band",
			     "Contour",
			     "1-d Marker",
			     "2-d Summing region",
			     "2-d Marker"
                           };
%}
%union {
        int integer;
	char string[80];
      }

%token IDENTIFIER
%token OBJECT
%token ENDOBJECT
%token OBJECTNAME
%token BEGINPTS
%token ENDPTS
%token TITLE
%token CUT1D
%token SUM1D
%token MARK1D
%token CONTOUR
%token BAND
%token SUM2D
%token MARK2D
%token QSTRING
%token NOMATCH
%token INTEGER
%token SPECTRUM
%token ID
%token COMMA
%%

file:   | objects
        | title_clause objects
        ;
objects:  object
        | objects object
        ;

object:   object_start object_def ENDOBJECT
                 { char errmsg[80];
		   assert(current != (grobj_generic *)NULL);
		   /* Now reject any object which is not a 
		   ** summing region or a marker:
		   */
		   switch(current->type()) {
		   case marker_1d:            /* these are the legal types. */
		   case summing_region_1d:
		   case summing_region_2d:
		   case marker_2d:
		     if(grobj_db->enter(current) == (grobj_generic *)NULL)
		       yyerror("Graphical object database full\n");
		     break;
		   default:	          /* These are the illegal types; */
		     sprintf(errmsg,
			     "Gating graphical object of type %s ignored\n",
			     typenames[current->type()]);
		     yyerror(errmsg);
		     break;
		   }
		   delete current;
		 }
        ;

title_clause: TITLE QSTRING
                 { printf("name\n"); grobj_db->name(yylval.string); } /*  Set the database name. */
              ;


object_start:  OBJECT object_type
               ;

object_type:   CUT1D
                 { current = new grobj_cut1d; } 
            |  SUM1D
                 { current = new grobj_sum1d; } 
            |  MARK1D
                 { current = new grobj_mark1d; } 
            |  CONTOUR
                 { current = new grobj_contour; }
            |  BAND
                 { current = new grobj_band; }
            |  SUM2D 
                 { current = new grobj_sum2d; } 
            |  MARK2D
                 { current = new grobj_mark2d; }
            ;

object_def: characteristic
            | object_def characteristic

characteristic:  obj_name
               | point_list
               | spectrum_id
               ;

obj_name:   OBJECTNAME QSTRING
               { assert(current != (grobj_generic *)NULL);
		 current->setname(yylval.string);
	       }
         ;

point_list:  BEGINPTS points ENDPTS
         ;

spectrum_id:  SPECTRUM INTEGER
              { assert(current != (grobj_generic *) NULL);
                current->setspectrum(yylval.integer);
	      }
            ;

points:      /* Empty */
        | points point
        ;

point:  xval COMMA yval
           {
	     assert(current != (grobj_generic *)NULL);
	     if(current->addpt(ix,iy) == 0)
	       yyerror("Too many points in graphical object\n");
	   }
      ;

xval: INTEGER
           { ix = yylval.integer;
	   }
           ;
yval: INTEGER
           { iy = yylval.integer;
	   }
           ;



