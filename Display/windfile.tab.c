
/*  A Bison parser, made from windfile.y
 by  GNU Bison version 1.25
  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse windfileparse
#define yylex windfilelex
#define yyerror windfileerror
#define yylval windfilelval
#define yychar windfilechar
#define yydebug windfiledebug
#define yynerrs windfilenerrs
#define	NAME	258
#define	NUMBER	259
#define	DESCRIPTION	260
#define	PEAK	261
#define	UPDATE	262
#define	OBJECTS	263
#define	FLIPPED	264
#define	NOFLIPPED	265
#define	TITLE	266
#define	GEOMETRY	267
#define	ZOOMED	268
#define	WINDOW	269
#define	AXES	270
#define	NOAXES	271
#define	TICKS	272
#define	LABELS	273
#define	LABEL	274
#define	NOLABEL	275
#define	REDUCTION	276
#define	SAMPLED	277
#define	SUMMED	278
#define	AVERAGED	279
#define	SCALE	280
#define	AUTO	281
#define	MANUAL	282
#define	COUNTSAXIS	283
#define	LOG	284
#define	LINEAR	285
#define	FLOOR	286
#define	CEILING	287
#define	EXPANDED	288
#define	RENDITION	289
#define	SMOOTHED	290
#define	HISTOGRAM	291
#define	POINTS	292
#define	LINE	293
#define	SCATTER	294
#define	BOX	295
#define	COLOR	296
#define	CONTOUR	297
#define	LEGO	298
#define	REFRESH	299
#define	INTEGER	300
#define	COMMA	301
#define	ENDWINDOW	302
#define	ENDLINE	303
#define	ATTRIBUTES	304
#define	ENDATTRIBUTES	305
#define	RENDITION_1D	306
#define	RENDITION_2D	307
#define	SUPERIMPOSE	308
#define	GROBJFONT	309
#define	UNMATCHED	310


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
#ifndef YYSTYPE
#define YYSTYPE int
#endif
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		142
#define	YYFLAG		-32768
#define	YYNTBASE	56

#define YYTRANSLATE(x) ((unsigned)(x) <= 310 ? yytranslate[x] : 94)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
    46,    47,    48,    49,    50,    51,    52,    53,    54,    55
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     5,     7,    10,    13,    17,    19,    22,    24,
    26,    32,    39,    44,    46,    49,    52,    56,    64,    67,
    69,    72,    74,    76,    78,    80,    82,    84,    86,    88,
    90,    92,    94,    96,   100,   104,   107,   110,   112,   115,
   117,   119,   121,   124,   127,   131,   135,   137,   140,   142,
   144,   146,   148,   150,   152,   155,   158,   162,   164,   166,
   168,   172,   174,   177,   181,   183,   185,   189,   193,   197,
   202,   206,   208,   210,   214,   218,   222,   224,   226,   228,
   230,   232,   234,   236,   238,   240,   244,   246
};

static const short yyrhs[] = {    57,
     0,    93,    57,     0,    60,     0,    60,    58,     0,    60,
    63,     0,    60,    58,    63,     0,    59,     0,    59,    93,
     0,    11,     0,    61,     0,    12,    45,    46,    45,    93,
     0,    12,    45,    46,    45,    62,    93,     0,    13,    45,
    46,    45,     0,    64,     0,    63,    64,     0,    65,    66,
     0,    65,    67,    66,     0,    14,    45,    46,    45,    46,
    45,    93,     0,    47,    93,     0,    68,     0,    67,    68,
     0,    69,     0,    72,     0,    75,     0,    76,     0,    78,
     0,    80,     0,    82,     0,    83,     0,    85,     0,    89,
     0,    84,     0,    92,     0,    15,    70,    93,     0,    16,
    70,    93,     0,    15,    93,     0,    16,    93,     0,    71,
     0,    70,    71,     0,    17,     0,    18,     0,    15,     0,
    19,    93,     0,    20,    93,     0,    19,    73,    93,     0,
    20,    73,    93,     0,    74,     0,    73,    74,     0,     3,
     0,     4,     0,     5,     0,     6,     0,     7,     0,     8,
     0,     9,    93,     0,    10,    93,     0,    21,    77,    93,
     0,    22,     0,    23,     0,    24,     0,    25,    79,    93,
     0,    26,     0,    27,    45,     0,    28,    81,    93,     0,
    29,     0,    30,     0,    31,    45,    93,     0,    32,    45,
    93,     0,    44,    45,    93,     0,    33,    86,    87,    93,
     0,    33,    88,    93,     0,    88,     0,    88,     0,    45,
    46,    45,     0,    34,    90,    93,     0,    34,    91,    93,
     0,    35,     0,    36,     0,    37,     0,    38,     0,    39,
     0,    40,     0,    41,     0,    42,     0,    43,     0,    53,
    45,    93,     0,    48,     0,    48,    93,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   114,   114,   117,   117,   118,   118,   121,   122,   125,   130,
   133,   138,   145,   151,   152,   156,   157,   160,   183,   190,
   190,   193,   193,   193,   194,   194,   194,   195,   195,   195,
   196,   196,   196,   199,   207,   214,   219,   227,   228,   231,
   235,   239,   245,   250,   256,   267,   280,   280,   283,   287,
   291,   294,   297,   300,   305,   311,   318,   321,   325,   329,
   335,   338,   342,   349,   352,   356,   362,   369,   376,   383,
   406,   427,   431,   435,   441,   442,   444,   452,   460,   468,
   478,   486,   494,   502,   510,   521,   545,   545
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","NAME","NUMBER",
"DESCRIPTION","PEAK","UPDATE","OBJECTS","FLIPPED","NOFLIPPED","TITLE","GEOMETRY",
"ZOOMED","WINDOW","AXES","NOAXES","TICKS","LABELS","LABEL","NOLABEL","REDUCTION",
"SAMPLED","SUMMED","AVERAGED","SCALE","AUTO","MANUAL","COUNTSAXIS","LOG","LINEAR",
"FLOOR","CEILING","EXPANDED","RENDITION","SMOOTHED","HISTOGRAM","POINTS","LINE",
"SCATTER","BOX","COLOR","CONTOUR","LEGO","REFRESH","INTEGER","COMMA","ENDWINDOW",
"ENDLINE","ATTRIBUTES","ENDATTRIBUTES","RENDITION_1D","RENDITION_2D","SUPERIMPOSE",
"GROBJFONT","UNMATCHED","setup_file","setup_filel","ident","title","geometry",
"geometry_line","zoom_clause","descriptions","description","window_clause","endwindow_clause",
"attribute_clauses","attribute_clause","axes_clause","axis_attributes","axis_attribute",
"labels_clause","label_attributes","label_attribute","flipped_clause","reduction_clause",
"reduction_attribute","scale_clause","scale_attribute","countsaxis_clause","countsaxis_attributes",
"floor_clause","ceiling_clause","refresh_clause","expanded_clause","xlimit",
"ylimit","limits","rendition_clause","rend1_attribute","rend2_attribute","superposition_clause",
"blankline", NULL
};
#endif

static const short yyr1[] = {     0,
    56,    56,    57,    57,    57,    57,    58,    58,    59,    60,
    61,    61,    62,    63,    63,    64,    64,    65,    66,    67,
    67,    68,    68,    68,    68,    68,    68,    68,    68,    68,
    68,    68,    68,    69,    69,    69,    69,    70,    70,    71,
    71,    71,    72,    72,    72,    72,    73,    73,    74,    74,
    74,    74,    74,    74,    75,    75,    76,    77,    77,    77,
    78,    79,    79,    80,    81,    81,    82,    83,    84,    85,
    85,    86,    87,    88,    89,    89,    90,    90,    90,    90,
    91,    91,    91,    91,    91,    92,    93,    93
};

static const short yyr2[] = {     0,
     1,     2,     1,     2,     2,     3,     1,     2,     1,     1,
     5,     6,     4,     1,     2,     2,     3,     7,     2,     1,
     2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     3,     3,     2,     2,     1,     2,     1,
     1,     1,     2,     2,     3,     3,     1,     2,     1,     1,
     1,     1,     1,     1,     2,     2,     3,     1,     1,     1,
     3,     1,     2,     3,     1,     1,     3,     3,     3,     4,
     3,     1,     1,     3,     3,     3,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     3,     1,     2
};

static const short yydefact[] = {     0,
     0,    87,     1,     3,    10,     0,     0,    88,     9,     0,
     4,     7,     5,    14,     0,     2,     0,     0,     6,     8,
    15,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    16,     0,    20,
    22,    23,    24,    25,    26,    27,    28,    29,    32,    30,
    31,    33,     0,     0,    55,    56,    42,    40,    41,     0,
    38,    36,     0,    37,    49,    50,    51,    52,    53,    54,
     0,    47,    43,     0,    44,    58,    59,    60,     0,    62,
     0,     0,    65,    66,     0,     0,     0,     0,     0,    72,
    77,    78,    79,    80,    81,    82,    83,    84,    85,     0,
     0,     0,    19,     0,    17,    21,     0,     0,    11,     0,
    39,    34,    35,    48,    45,    46,    57,    63,    61,    64,
    67,    68,     0,     0,    73,    71,    75,    76,    69,    86,
     0,    12,     0,    74,    70,     0,     0,    13,    18,     0,
     0,     0
};

static const short yydefgoto[] = {   140,
     3,    11,    12,     4,     5,   108,    13,    14,    15,    38,
    39,    40,    41,    60,    61,    42,    71,    72,    43,    44,
    79,    45,    82,    46,    85,    47,    48,    49,    50,    89,
   124,    90,    51,   100,   101,    52,     6
};

static const short yypact[] = {    -4,
   -44,   -39,-32768,     5,-32768,    15,   -35,-32768,-32768,   -17,
    19,   -39,    19,-32768,    21,-32768,   -16,    -7,    19,-32768,
-32768,   -39,   -39,    -3,    -3,    -1,    -1,    40,    -9,    37,
    12,    25,    26,    54,    28,   -39,    30,-32768,    21,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,     0,    31,-32768,-32768,-32768,-32768,-32768,    -3,
-32768,-32768,    -3,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
    -1,-32768,-32768,    -1,-32768,-32768,-32768,-32768,   -39,-32768,
    33,   -39,-32768,-32768,   -39,   -39,   -39,     4,    26,   -39,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   -39,
   -39,   -39,-32768,   -39,-32768,-32768,    34,   -39,-32768,    14,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,    36,   -39,-32768,-32768,-32768,-32768,-32768,-32768,
    41,-32768,    56,-32768,-32768,    58,   -39,-32768,-32768,    43,
    82,-32768
};

static const short yypgoto[] = {-32768,
    80,-32768,-32768,-32768,-32768,-32768,    93,    13,-32768,    66,
-32768,    68,-32768,    83,   -25,-32768,    84,   -15,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,    20,-32768,-32768,-32768,-32768,    -2
};


#define	YYLAST		135


static const short yytable[] = {     8,
     7,    65,    66,    67,    68,    69,    70,     1,     2,    20,
    17,    57,   107,    58,    59,     9,    80,    81,    10,    55,
    56,    62,    64,    73,    75,    21,     1,    18,    53,    22,
    23,    21,    10,   103,   111,    24,    25,   111,    54,    26,
    27,    28,   141,     2,     2,    29,     2,     2,    30,   123,
   109,    31,    32,    33,    34,   114,    86,   112,   114,   133,
   113,    76,    77,    78,    35,    83,    84,    36,   115,    87,
    88,   116,   102,    37,   104,   110,   117,   118,   131,   119,
   134,   142,   120,   121,   122,    16,   136,   126,    91,    92,
    93,    94,    95,    96,    97,    98,    99,   127,   128,   129,
   137,   130,   138,    19,   105,   132,   106,    63,   125,     0,
    74,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   135,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   139
};

static const short yycheck[] = {     2,
    45,     3,     4,     5,     6,     7,     8,    12,    48,    12,
    46,    15,    13,    17,    18,    11,    26,    27,    14,    22,
    23,    24,    25,    26,    27,    13,    12,    45,    45,     9,
    10,    19,    14,    36,    60,    15,    16,    63,    46,    19,
    20,    21,     0,    48,    48,    25,    48,    48,    28,    46,
    53,    31,    32,    33,    34,    71,    45,    60,    74,    46,
    63,    22,    23,    24,    44,    29,    30,    47,    71,    45,
    45,    74,    45,    53,    45,    45,    79,    45,    45,    82,
    45,     0,    85,    86,    87,     6,    46,    90,    35,    36,
    37,    38,    39,    40,    41,    42,    43,   100,   101,   102,
    45,   104,    45,    11,    39,   108,    39,    25,    89,    -1,
    27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   124,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,   137
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */


/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifndef YYPARSE_PARAM
int yyparse (void);
#endif
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif



/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 9:
{ 
                  database->settitle(get_windfiletitle()); ;
    break;}
case 11:
{ database->cleardb();
		     int a = yyvsp[-3], b= yyvsp[-1];
		     database->setx(yyvsp[-3]); database->sety(yyvsp[-1]);
                   ;
    break;}
case 12:
{ database->cleardb();
                     database->setx(yyvsp[-4]); database->sety(yyvsp[-2]);
		     database->zoom(x,y);
                   ;
    break;}
case 13:
{ x = yyvsp[-2];
                   y = yyvsp[0];
		 ;
    break;}
case 18:
{
		    if(specisundefined(yyvsp[-1])) {
		       yyerror("Spectrum is not defined\n");
		       return -1;
		    }
		    else {
			    if(specis1d(yyvsp[-1])) {
			      database->define1d(yyvsp[-5],yyvsp[-3], yyvsp[-1]);
			    } else {
			      database->define2d(yyvsp[-5],yyvsp[-3],yyvsp[-1]);
			    }
			    x = yyvsp[-5];
			    y = yyvsp[-3];
			    current = database->getdef(x,y);
			    if(current == NULL) {
			      yyerror("Internal consistency error -- window is not defined\n");
			      return -1;
			    }
			  }
		    ;
    break;}
case 19:
{
		     x = -1; y = -1;
		     current = NULL;
		   ;
    break;}
case 34:
{
		    assert(current != NULL);
		    if(ticks)  current->tickson();
		    if(labels) current->axis_labelson();
		    if(axes)   current->axeson();
		    ticks = labels = axes = 0;
		  ;
    break;}
case 35:
{
		    if(ticks) current->ticksoff();
		    if(labels)current->axis_labelsoff();
		    if(axes)  current->axesoff();
		    ticks = labels = axes = 0;
		  ;
    break;}
case 36:
{
		   assert(current != NULL);
		   current->set_axes(TRUE, TRUE, TRUE);
		 ;
    break;}
case 37:
{
		   assert(current != NULL);
		   current->set_axes(FALSE,FALSE,FALSE);
		 ;
    break;}
case 40:
{
		     ticks = TRUE;
		   ;
    break;}
case 41:
{
		     labels = TRUE;
		   ;
    break;}
case 42:
{
		     axes = TRUE;
		   ;
    break;}
case 43:
{
		   assert(current != NULL);
		   current->set_titles(TRUE, TRUE, TRUE, TRUE, TRUE, TRUE);
		 ;
    break;}
case 44:
{
		   assert(current != NULL);
		   current->set_titles(FALSE, FALSE, FALSE,
					 FALSE, FALSE, FALSE);
		 ;
    break;}
case 45:
{
		    assert(current != NULL);
		    if(name)        current->dispname();
		    if(number)      current->dispid();
		    if(description) current->dispid();
		    if(peak)        current->disppeak();
		    if(update)      current->dispupd();
		    if(objects)     current->labelobj();
		    name = number = description = peak = update = objects = 0;
		  ;
    break;}
case 46:
{
		    assert(current != NULL);
		    if(name)        current->hidename();
		    if(number)      current->hideid();
		    if(description) current->hidedescr();
		    if(peak)        current->hidepeak();
		    if(update)      current->hideupd();
		    if(objects)     current->unlabelobj();
		    name = number = description = peak = update = objects = 0;
		  ;
    break;}
case 49:
{
		     name = TRUE;
		   ;
    break;}
case 50:
{
		     number = TRUE;
		   ;
    break;}
case 51:
{
		  description = TRUE;
		;
    break;}
case 52:
{
		  peak  = TRUE;
		;
    break;}
case 53:
{
		  update = TRUE;
		;
    break;}
case 54:
{
		  objects = TRUE;
		;
    break;}
case 55:
{ 
		  assert(current != NULL);
		  current->sideways();
		;
    break;}
case 56:
{
		  assert(current != NULL);
		  current->normal();
		;
    break;}
case 58:
{
                        assert(current != NULL);
			current->sample();
		      ;
    break;}
case 59:
{
		      assert(current != NULL);
		      current->sum();
		    ;
    break;}
case 60:
{
		      assert(current != NULL);
		      current->average();
		    ;
    break;}
case 62:
{
                       assert(current != NULL);
		       current->autoscale();
		     ;
    break;}
case 63:
{
		   assert(current != NULL);
		   current->setfs(yyvsp[0]);
		 ;
    break;}
case 65:
{
			     assert(current != NULL);
			     current->log();
			   ;
    break;}
case 66:
{
			 assert(current != NULL);
			 current->linear();
		       ;
    break;}
case 67:
{
		   assert(current != NULL);
		   current->setfloor(yyvsp[-1]);
		 ;
    break;}
case 68:
{
		  assert(current != NULL);
		  current->setceiling(yyvsp[-1]);
		;
    break;}
case 69:
{
		  assert(current != NULL);
		  current->update_interval(yyvsp[-1]);	/* Set the update interval. */
                ;
    break;}
case 70:
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
		  ;
    break;}
case 71:
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
		  ;
    break;}
case 72:
{ memcpy(&xlim, &limits, sizeof(struct limit)); ;
    break;}
case 73:
{ memcpy(&ylim, &limits, sizeof(struct limit)); ;
    break;}
case 74:
{ limits.low = yyvsp[-2];
		  limits.high= yyvsp[0];
		;
    break;}
case 77:
{ assert(current);
		    if(!current->is1d()) {
		      windfileerror("Attempted to set 1-d rendition on 2-d spectrum\n");
		    } else {
		      ((win_1d *)current)->smooth();
		    }
		  ;
    break;}
case 78:
{ assert(current);
		    if(!current->is1d()) {
		      windfileerror("Attempted to set 1-d rendition on 2-d spectrum\n");
		    } else {
		      ((win_1d *)current)->bars();
		    }
		  ;
    break;}
case 79:
{ assert(current);
                    if(!current->is1d()) {
		      windfileerror("Attempted to set 1-d rendition on 2-d spectrum\n");
		    } else {
		      ((win_1d *)current)->pts();
		    }
		  ;
    break;}
case 80:
{ assert(current);
		    if(!current->is1d()) {
		      windfileerror("Attempted to set 1-d rendition on 2-d spectrum\n");
		    } else {
		      ((win_1d *)current)->line();
		    }
		  ;
    break;}
case 81:
{ assert(current);
		    if(current->is1d()) {
		      windfileerror("Attempted to set 2-d rendition on 1-d spectrum\n");
		    } else {
		      ((win_2d *)current)->scatterplot();
		    }
		  ;
    break;}
case 82:
{ assert(current);
		    if(current->is1d()) {
		      windfileerror("Attempted to set 2-d rendition on 1-d spectrum\n");
		    } else {
		      ((win_2d *)current)->boxplot();
		    }
		  ;
    break;}
case 83:
{ assert(current);
		    if(current->is1d()) {
		      windfileerror("Attempted to set 2-d rendition on 1-d spectrum\n");
		    } else {
		      ((win_2d *)current)->colorplot();
		    }
		  ;
    break;}
case 84:
{ assert(current);
		    if(current->is1d()) {
		      windfileerror("Attempted to set 2-d rendition on 1-d spectrum\n");
		    } else {
		      ((win_2d *)current)->contourplot();
		    }
		  ;
    break;}
case 85:
{ assert(current);
		    if(current->is1d()) {
		      windfileerror("Attempted to set 2-d rendition on 1-d spectrum\n");
		    } else {
		      ((win_2d *)current)->legoplot();
		    }
		  ;
    break;}
case 86:
{
		     assert(current);
		     win_1d *at1 = (win_1d *)current;
		     if(!current->is1d()) {
		       windfileerror("Superimpose on 2-d spectrum ignored");
		     }
		     else if(!superimposable(current->spectrum(), yyvsp[-1])) {
		       windfileerror("Incompatible superimpose ignored");
		     }
		     else if(superimposed(at1, yyvsp[-1])) {
		       windfileerror("Redundant superimpose ignored");
		     }
		     else {
		       SuperpositionList &sl = at1->GetSuperpositions();
		       if(sl.Count() < MAX_SUPERPOSITIONS) {
			 sl.Add(yyvsp[-1]);
		       }
		       else {
			 windfileerror("Too many superimposes, extra ignored");
		       }
		     }
		   ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */


  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}

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
