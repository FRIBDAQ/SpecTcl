/*
** Facility:
**   Xamine -- NSCL Display program.
** Abstract:
**   windio.h:
**     Defines stuff needed by the parser and lexical analyzers for window
**     file I/O to allow multipler flex and bison generated parsers and lexers.
** Author:
**    Ron Fox
**    NSCL
**    Michigan State University
**    East Lansing, MI 48824-1321
** SCCINFO:
**    @(#)windio.h	8.1 6/23/95 
*/
#ifndef WINDIO_H_INSTALLED
#define WINDIO_H_INSTALLED
/*
** The following defines specialize the yacc and lex names
** to be a windfile parser and lexer.
*/

#define yyoutput dispwind_yyoutput
#define yyunput  dispwind_yyunput
#define yylook   dispwind_yylook
#ifndef yywrap
#define yywrap   dispwind_yywrap
#endif
#define yyreject dispwind_yyreject
#define yyback   dispwind_yyback

int yylex();
void yyoutput(int);
void yyunput(int);
int yylook();
int yyreject();
int yyback(int *, int);

#endif
