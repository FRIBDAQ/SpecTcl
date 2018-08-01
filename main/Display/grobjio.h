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
**    grobjio.h:
**       This file contains definitions for the lexical analyzer and parser
**       generater by flex and bison to allow multiple lexers and parsers
**       in the same program.
** Author:
**     Ron Fox
**     NSCL
**     Michigan State University
**     East Lansing, MI 48824-1321
** Modified:
**     @(#)grobjio.h	2.1 12/22/93 
*/
#ifndef GROBJIO_H_INSTALLED
#define GROBJIO_H_INSTALLED
/*
** The following defines are to support multiple parsers
*/

#ifndef yyoutput
#define yyoutput grobj_yyoutput
#endif
#ifndef yyunput
#define yyunput  grobj_yyunput
#endif
#ifndef yylook
#define yylook   grobj_yylook
#endif
#ifndef yywrap
#define yywrap   grobj_yywrap
#endif
#ifndef yyreject
#define yyreject grobj_yyreject
#endif
#ifndef yyback
#define yyback   grobj_yyback
#endif
#ifndef yynerrs
#define yynerrs grobfilenerrs
#endif
int yylex();
void yyoutput(int);
void yyunput(int);
int yylook();
int yyreject();
int yyback(int *, int);

#endif
