%{
/*
**  Facility:
**    NSCL display system, window file I/O subsystem.
**  Abstract:
**    windfile.l  - This file contains a lex/flex description
**                 of the tokens which make up the context free language
**                 that is used to describe window files.
**  Author:
**    Ron Fox
**    NSCL
**    Michigan State University
**    East Lansing, MI 48824-1321
** Revision level:
**    @(#)windfile.l	8.1 6/23/95 
*/
static char *lexrevlevel = "@(#)windfile.l	8.1 6/23/95  - windows file lexical analyzer";
#include <stdlib.h>
#include "windio.h"
#include "dispwind.h"
#include "printer.h"
#include <config.h>
#include "windfile_lex.h"
#include "windlex.h"
#undef lex_output
#define lex_output(c) 

#ifdef DEBUG
#define dbgprintf(arg) printf(arg); fflush(stdout);
#else
#define dbgprintf(arg)
#endif
extern YYSTYPE yylval;
char* yytitle;
static void rd_yytitle();
int         yylex_line;
%}
%%
\"[^\"\n]*[\"\n]   {   if(yytext[strlen(yytext) - 1] != '"') {
	                  fprintf(stderr,"warning: Unterminated string %s \n",
		          yytext);
                        } else {
			  yytext[strlen(yytext)-1] = '\0'; /* remove last " */
			}
			strcpy(yylval.string,yytext+1);  /* Pass str unquoted */
			return QSTRING;
		      }
[Tt][Ii][Tt][Ll][Ee]               { rd_yytitle(); dbgprintf("TITLE ");return TITLE; }
[Gg][Ee][Oo][Mm][Ee][Tt][Rr][Yy]   { dbgprintf("GEOMETRY "); return GEOMETRY; }
[Ww][Ii][Nn][Dd][Oo][Ww]           { dbgprintf("WINDOW "); return WINDOW; }
[Zz][Oo][Oo][Mm][Ee][Dd]           { dbgprintf("ZOOMED ");return ZOOMED; }
([nN][oO])?[Aa][Xx][Ee][sS]      { dbgprintf("AXES "); 
                                  if( (yytext[0] == 'N') || (yytext[0] == 'n'))
                                      return NOAXES;
                                   else
                                      return AXES; }
[Tt][Ii][Cc][kK][sS]               { dbgprintf("TICKS "); return TICKS; }
[lL][aA][bB][Ee][lL][sS]           { dbgprintf("LABELS "); return LABELS; }
([nN][oO])?[lL][aA][bB][eE][lL]    { dbgprintf("LABEL "); 
                                  if( (yytext[0] == 'N') || (yytext[0] == 'n'))
                                      return NOLABEL;
                                   else
                                      return LABEL; }
[nN][aA][mM][eE]                   { dbgprintf("NAME ");  return NAME; }
[Nn][uU][mM][bB][eE][rR]           { dbgprintf("NUMBER "); return NUMBER; }
[dD][eE][sS][cC][rR][iI][pP][tT][iI][oO][nN] {dbgprintf("DESCRIPTION "); return DESCRIPTION; }
[pP][eE][aA][kK]                  { dbgprintf("PEAK "); return PEAK; }
[uU][pP][dD][aA][tT][eE]          { dbgprintf("UPDATE "); return UPDATE; }
[oO][bB][jJ][eE][cC][tT][sS]      { dbgprintf("OBJECTS "); return OBJECTS; }
([nN][oO])?[fF][lL][iI][pP][pP][eE][dD] { dbgprintf("FLIPPED "); 
                                  if( (yytext[0] == 'N') || (yytext[0] == 'n'))
                                      return NOFLIPPED;
                                   else
                                      return FLIPPED; }
[rR][eE][dD][uU][cC][tT][iI][oO][nN] { dbgprintf("REDUCTION "); return REDUCTION; }
[sS][aA][mM][pP][lL][eE][dD]      { dbgprintf("SAMPLED "); return SAMPLED; }
[sS][uU][mM][mM][eE][dD]          { dbgprintf("SUMMED "); return SUMMED; }
[aA][vV][eE][rR][aA][gG][eE][dD]  { dbgprintf("AVERAGED "); return AVERAGED; }
[sS][cC][aA][lL][eE]              { dbgprintf("SCALE "); return SCALE; }
[aA][uU][tT][oO]                  { dbgprintf("AUTO "); return AUTO; }
[Mm][Aa][Pp][Pp][Ee][Dd]          { dbgprintf("MAPPED "); return MAPPED; }
[mM][aA][nN][uU][aA][lL]	  { dbgprintf("MANUAL "); return MANUAL; }
[cC][oO][uU][nN][tT][sS][aA][xX][iI][sS] {dbgprintf("COUNTSAXIS "); return COUNTSAXIS; }
[lL][oO][gG]                      { dbgprintf("LOG "); return LOG; }
[lL][iI][nN][eE][aA][rR]          { dbgprintf("LINEAR "); return LINEAR; }
[fF][lL][oO][oO][rR]                  { dbgprintf("FLOOR "); return FLOOR; }
[cC][eE][iI][lL][iI][nN][gG]      { dbgprintf("CEILING "); return CEILING; }
[eE][xX][pP][aA][nN][dD][eE][dD]  { dbgprintf("EXPANDED "); return EXPANDED; }
[rR][eE][nN][dD][iI][tT][iI][oO][nN] { dbgprintf("RENDITION "); return RENDITION; }
[sS][mM][oO][oO][tT][hH][eE][dD]  { dbgprintf("SMOOTHED ");  return SMOOTHED; }
[hH][iI][sS][tT][oO][gG][rR][aA][mM] { dbgprintf("HISTOGRAM "); return HISTOGRAM; }
[pP][oO][iI][nN][tT][sS]          { dbgprintf("POINTS ");  return POINTS; }
[lL][iI][nN][eE]                  { dbgprintf("LINE "); return LINE; }
[sS][cC][aA][tT][tT][eE][rR]      { dbgprintf("SCATTER "); return SCATTER; }
[bB][oO][xX]                     { dbgprintf("BOX "); return BOX; }
[cC][oO][lL][oO][rR]             { dbgprintf("COLOR "); return COLOR; }
[cC][oO][nN][tT][oO][uU][rR]     { dbgprintf("CONTOUR "); return CONTOUR; }
[lL][eE][gG][oO]                 { dbgprintf("LEGO "); return LEGO; }
[rR][eE][fF][rR][eE][sS][hH]     { dbgprintf("REFRESH"); return REFRESH; }
[eE][nN][dD][wW][iI][nN][dD][oO][wW]  {dbgprintf("ENDWINDOW "); return ENDWINDOW; }
[Aa][Tt][Tt][rR][iI][Bb][Uu][Tt][Ee][Ss] { dbgprintf("ATTRIBUTES ");
                                           return ATTRIBUTES;
                                 }
[Ee][Nn][Dd][Aa][Tt][tT][Rr][Ii][Bb][Uu][Tt][Ee][Ss] {
                                       dbgprintf("ENDATTRIBUTES ");
                                       return ENDATTRIBUTES;
                                  }
[Rr][Ee][Nn][Dd][Ii][Tt][Ii][Oo][Nn]_1[Dd] {
                                        dbgprintf("RENDITION_1D ");
                                        return RENDITION_1D;
                                  }
[Rr][Ee][Nn][Dd][Ii][Tt][Ii][Oo][Nn]_2[Dd] {
                                        dbgprintf("RENDITION_2D ");
                                        return RENDITION_2D;
                                  }
[Ss][uU][pP][eE][rR][iI][mM][pP][oO][sS][eE] {
                                        dbgprintf("Superimpose");
                                        return SUPERIMPOSE;
                                  }
[Oo][Bb][Jj][Ee][Cc][Tt]_[Ll][Aa][Bb][Ee][Ll]_[Ff][Oo][Nn][Tt] {
                                        dbgprintf("GROBJFONT");
                                        return GROBJFONT;
                                }
[+-]?[0-9]+                     { dbgprintf("INTEGER ");
				  yylval.integer = atoi(yytext);
#ifdef DEBUG
				  printf("%d \n", yylval);
#endif
                                  return INTEGER; }
[+-]?[0-9]*"."[0-9]+ {
                                  dbgprintf("REAL ");
                                  yylval.real = atof(yytext);
#ifdef DEBUG
                                  printf("%f \n", yylval);
#endif
                                  return REAL;
}
\,                                {dbgprintf("COMMA ");  return COMMA; }
^\n                             {dbgprintf("EMPTYLINE\n "); yylex_line++; 
                                 return ENDLINE; }
\n                               { dbgprintf("ENDLINE \n"); yylex_line++;
                                   return ENDLINE; }
#.*\n                           {dbgprintf("COMMENT\n"); yylex_line++;
                                  return ENDLINE; }
[Ee][nN][dD]                       { dbgprintf("END "); return 0; }
[ \t]+                          ;
.                              { return UNMATCHED; }

%%

/*
** win_startlex  - Restart the lexer:
*/
void yy_startlex()
{
  yy_init  = 1;
  yy_start = 0; 
  yylex_line = 0;
}
/*
** rd_yytitle -- Function to get the yytitle string:
*/

static void rd_yytitle()
{
  int i;
  int c;
  i = 0;
  memset(yytitle, 0, sizeof(yytitle));
  c = yyinput();
  while( (c != '\n') && (c != EOF) )
  {
    if(i < WINDOW_MAXTITLE-1) {
       yytitle[i] = (char)c;
       i++;
    }
    c = yyinput();
  }
  yylex_line++;
}
/*
** get_yytitle  -- function to give the yytitle string to the caller:
*/

char *get_yytitle()
{
   return yytitle;
}
/*
** Get lexer revision level:
*/

char *yygetlexrev()
{
  return lexrevlevel;
}
