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
#ifndef _GROBJIO_H_INSTALLED
#define _GROBJIO_H_INSTALLED
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
