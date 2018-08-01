/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


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
