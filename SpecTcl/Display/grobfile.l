%{
/*
** Facility:
**   Display program graphical object subsystem.
** Abstract:
**   grobfile.l     - This file contains a LEX definition for a lexical
**                    analyzer that scans input for the graphical object
**                    file format.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
**/
#include <config.h>
#include <stdio.h>		/* Include standard I/O functions too. */
#include <strings.h>
#include <ctype.h>		/* typeconversion functions./macros. */

#include "grobjio.h"
#include "dispgrob.h"
#include "grobfile_lex.h"		/* Include the lexical token defs */
int grob_lineno = 0;             /* Keeps track of line number */

static int ksearch(char *keyword);
static void spaces();
static void comment();
static void eoline();
extern YYSTYPE        yylval;
%}

qstring \"[^\"\n]*[\"\n"]
comment  #.*\n
spaces  [ \t]+
eol     \n
keyword [a-zA-Z][a-zA-Z0-9_-]*
number  [0-9]+

%%

{number}         { grobjfilelval.integer = atoi(yytext); return INTEGER; }
{keyword}        { return ksearch(yytext); }
{qstring}        { strcpy(grobjfilelval.string, &(yytext[1]));    /* Strip off the */
                   grobjfilelval.string[strlen(yytext)-2] = '\0'; /* quotes */
                   return QSTRING; }    /* Quoted string */

{spaces}         { spaces(); }
{comment}        { comment(); grob_lineno++; }             /* Swallow whitespace/comments. */

","                { return COMMA; }

{eol}            {grob_lineno++; eoline(); }      /*  newlines too */
.                { return NOMATCH; } /* Pass all other chars to parser */

%%
/*
** The table below contains a keyword lookup table:
*/

struct ktbl {
              char *key;
              int  keylen;
              int keyval;
            };
ktbl keywords[]  = {
                     {"object", 6, OBJECT},
                     {"endobject", 9, ENDOBJECT},
                     {"name", 4, OBJECTNAME},
                     {"beginpts", 8, BEGINPTS},
                     {"endpts", 6, ENDPTS},
                     {"title", 5, TITLE},
                     {"cut", 3, CUT1D},
                     {"sum-1d", 6, SUM1D},
                     {"mark-1d", 7, MARK1D},
                     {"contour", 7, CONTOUR},
                     {"band",    4, BAND},
                     {"sum-2d",  6, SUM2D},
                     {"mark-2d", 7, MARK2D},
		     {"spectrum", 8, SPECTRUM},
		     {"id", 2, ID},
                     {0,0,0}};

/*
** Functional Description:
**   ksearch   - This function is called whenever a keyword pattern is
**               matched by Lex.  We lookup the Lex scanned string and
**               compare it with legitimate lexical keywords.  If there's
**               a match, then we return the value of the TOKEN associated
**               with the match.  If there's no match, then we return the
**               value IDENTIFIER instead.
** Formal Parameters:
**    char *key   - The keyword to lookup.
** Returns:
**    Token value.
*/
int ksearch(char *key)
{
   ktbl *pt;
   char *c;
   int l;


   /* First convert the key to lower case for a case blind match */

   for(c = key; *c != '\0'; c++) {
     *c = tolower(*c);
   }
   /* Now hunt for the keyword in the table: */

   l = strlen(key);
   for (pt = keywords; pt->key != (char *)NULL; pt++ ) {
     if( (l == pt->keylen) && (strcmp(key, pt->key) == 0))
       return pt->keyval;
   }
   return  IDENTIFIER;
     
}


static void spaces()
{
}
static void comment()
{
}
static void eoline()
{
}
/*
** grobj_startlex  - restart lexer...
*/
void grobj_startlex()
{
  yy_init  = 1;
  yy_start = 0;
  grob_lineno = 0;
}

