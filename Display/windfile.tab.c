
/*  A Bison parser, made from windfile.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse windfileparse
#define yylex windfilelex
#define yyerror windfileerror
#define yylval windfilelval
#define yychar windfilechar
#define yydebug windfiledebug
#define yynerrs windfilenerrs
#define	NAME	257
#define	NUMBER	258
#define	DESCRIPTION	259
#define	PEAK	260
#define	UPDATE	261
#define	OBJECTS	262
#define	FLIPPED	263
#define	NOFLIPPED	264
#define	TITLE	265
#define	GEOMETRY	266
#define	ZOOMED	267
#define	WINDOW	268
#define	AXES	269
#define	NOAXES	270
#define	TICKS	271
#define	LABELS	272
#define	LABEL	273
#define	NOLABEL	274
#define	REDUCTION	275
#define	SAMPLED	276
#define	SUMMED	277
#define	AVERAGED	278
#define	SCALE	279
#define	AUTO	280
#define	MANUAL	281
#define	COUNTSAXIS	282
#define	LOG	283
#define	LINEAR	284
#define	FLOOR	285
#define	CEILING	286
#define	EXPANDED	287
#define	RENDITION	288
#define	SMOOTHED	289
#define	HISTOGRAM	290
#define	POINTS	291
#define	LINE	292
#define	SCATTER	293
#define	BOX	294
#define	COLOR	295
#define	CONTOUR	296
#define	LEGO	297
#define	REFRESH	298
#define	INTEGER	299
#define	COMMA	300
#define	ENDWINDOW	301
#define	ENDLINE	302
#define	ATTRIBUTES	303
#define	ENDATTRIBUTES	304
#define	RENDITION_1D	305
#define	RENDITION_2D	306
#define	SUPERIMPOSE	307
#define	GROBJFONT	308
#define	UNMATCHED	309
#define	QSTRING	310


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

typedef union {
  int integer;
  char string[80];
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		144
#define	YYFLAG		-32768
#define	YYNTBASE	57

#define YYTRANSLATE(x) ((unsigned)(x) <= 310 ? yytranslate[x] : 96)

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
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
    47,    48,    49,    50,    51,    52,    53,    54,    55,    56
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     5,     7,    10,    13,    17,    19,    22,    24,
    26,    32,    39,    44,    46,    49,    52,    56,    64,    66,
    68,    71,    73,    76,    78,    80,    82,    84,    86,    88,
    90,    92,    94,    96,    98,   100,   104,   108,   111,   114,
   116,   119,   121,   123,   125,   128,   131,   135,   139,   141,
   144,   146,   148,   150,   152,   154,   156,   159,   162,   166,
   168,   170,   172,   176,   178,   181,   185,   187,   189,   193,
   197,   201,   206,   210,   212,   214,   218,   222,   226,   228,
   230,   232,   234,   236,   238,   240,   242,   244,   248,   250
};

static const short yyrhs[] = {    58,
     0,    95,    58,     0,    61,     0,    61,    59,     0,    61,
    64,     0,    61,    59,    64,     0,    60,     0,    60,    95,
     0,    11,     0,    62,     0,    12,    45,    46,    45,    95,
     0,    12,    45,    46,    45,    63,    95,     0,    13,    45,
    46,    45,     0,    65,     0,    64,    65,     0,    66,    68,
     0,    66,    69,    68,     0,    14,    45,    46,    45,    46,
    67,    95,     0,    45,     0,    56,     0,    47,    95,     0,
    70,     0,    69,    70,     0,    71,     0,    74,     0,    77,
     0,    78,     0,    80,     0,    82,     0,    84,     0,    85,
     0,    87,     0,    91,     0,    86,     0,    94,     0,    15,
    72,    95,     0,    16,    72,    95,     0,    15,    95,     0,
    16,    95,     0,    73,     0,    72,    73,     0,    17,     0,
    18,     0,    15,     0,    19,    95,     0,    20,    95,     0,
    19,    75,    95,     0,    20,    75,    95,     0,    76,     0,
    75,    76,     0,     3,     0,     4,     0,     5,     0,     6,
     0,     7,     0,     8,     0,     9,    95,     0,    10,    95,
     0,    21,    79,    95,     0,    22,     0,    23,     0,    24,
     0,    25,    81,    95,     0,    26,     0,    27,    45,     0,
    28,    83,    95,     0,    29,     0,    30,     0,    31,    45,
    95,     0,    32,    45,    95,     0,    44,    45,    95,     0,
    33,    88,    89,    95,     0,    33,    90,    95,     0,    90,
     0,    90,     0,    45,    46,    45,     0,    34,    92,    95,
     0,    34,    93,    95,     0,    35,     0,    36,     0,    37,
     0,    38,     0,    39,     0,    40,     0,    41,     0,    42,
     0,    43,     0,    53,    45,    95,     0,    48,     0,    48,
    95,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   120,   120,   123,   123,   124,   124,   127,   128,   131,   136,
   139,   144,   151,   157,   158,   162,   163,   166,   190,   194,
   204,   211,   211,   214,   214,   214,   215,   215,   215,   216,
   216,   216,   217,   217,   217,   220,   228,   235,   240,   248,
   249,   252,   256,   260,   266,   271,   277,   288,   301,   301,
   304,   308,   312,   315,   318,   321,   326,   332,   339,   342,
   346,   350,   356,   359,   363,   370,   373,   377,   383,   390,
   397,   404,   427,   448,   452,   456,   462,   463,   465,   473,
   481,   489,   499,   507,   515,   523,   531,   542,   566,   566
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
"GROBJFONT","UNMATCHED","QSTRING","setup_file","setup_filel","ident","title",
"geometry","geometry_line","zoom_clause","descriptions","description","window_clause",
"spectrum","endwindow_clause","attribute_clauses","attribute_clause","axes_clause",
"axis_attributes","axis_attribute","labels_clause","label_attributes","label_attribute",
"flipped_clause","reduction_clause","reduction_attribute","scale_clause","scale_attribute",
"countsaxis_clause","countsaxis_attributes","floor_clause","ceiling_clause",
"refresh_clause","expanded_clause","xlimit","ylimit","limits","rendition_clause",
"rend1_attribute","rend2_attribute","superposition_clause","blankline", NULL
};
#endif

static const short yyr1[] = {     0,
    57,    57,    58,    58,    58,    58,    59,    59,    60,    61,
    62,    62,    63,    64,    64,    65,    65,    66,    67,    67,
    68,    69,    69,    70,    70,    70,    70,    70,    70,    70,
    70,    70,    70,    70,    70,    71,    71,    71,    71,    72,
    72,    73,    73,    73,    74,    74,    74,    74,    75,    75,
    76,    76,    76,    76,    76,    76,    77,    77,    78,    79,
    79,    79,    80,    81,    81,    82,    83,    83,    84,    85,
    86,    87,    87,    88,    89,    90,    91,    91,    92,    92,
    92,    92,    93,    93,    93,    93,    93,    94,    95,    95
};

static const short yyr2[] = {     0,
     1,     2,     1,     2,     2,     3,     1,     2,     1,     1,
     5,     6,     4,     1,     2,     2,     3,     7,     1,     1,
     2,     1,     2,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     3,     3,     2,     2,     1,
     2,     1,     1,     1,     2,     2,     3,     3,     1,     2,
     1,     1,     1,     1,     1,     1,     2,     2,     3,     1,
     1,     1,     3,     1,     2,     3,     1,     1,     3,     3,
     3,     4,     3,     1,     1,     3,     3,     3,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     3,     1,     2
};

static const short yydefact[] = {     0,
     0,    89,     1,     3,    10,     0,     0,    90,     9,     0,
     4,     7,     5,    14,     0,     2,     0,     0,     6,     8,
    15,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    16,     0,    22,
    24,    25,    26,    27,    28,    29,    30,    31,    34,    32,
    33,    35,     0,     0,    57,    58,    44,    42,    43,     0,
    40,    38,     0,    39,    51,    52,    53,    54,    55,    56,
     0,    49,    45,     0,    46,    60,    61,    62,     0,    64,
     0,     0,    67,    68,     0,     0,     0,     0,     0,    74,
    79,    80,    81,    82,    83,    84,    85,    86,    87,     0,
     0,     0,    21,     0,    17,    23,     0,     0,    11,     0,
    41,    36,    37,    50,    47,    48,    59,    65,    63,    66,
    69,    70,     0,     0,    75,    73,    77,    78,    71,    88,
     0,    12,     0,    76,    72,     0,    19,    20,     0,    13,
    18,     0,     0,     0
};

static const short yydefgoto[] = {   142,
     3,    11,    12,     4,     5,   108,    13,    14,    15,   139,
    38,    39,    40,    41,    60,    61,    42,    71,    72,    43,
    44,    79,    45,    82,    46,    85,    47,    48,    49,    50,
    89,   124,    90,    51,   100,   101,    52,     6
};

static const short yypact[] = {    -4,
   -44,   -39,-32768,    24,-32768,    27,   -35,-32768,-32768,   -12,
    29,   -39,    29,-32768,    21,-32768,     5,    11,    29,-32768,
-32768,   -39,   -39,    -3,    -3,    -1,    -1,    -5,     2,    41,
    15,    17,    19,    54,    22,   -39,    28,-32768,    21,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,     0,    30,-32768,-32768,-32768,-32768,-32768,    -3,
-32768,-32768,    -3,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
    -1,-32768,-32768,    -1,-32768,-32768,-32768,-32768,   -39,-32768,
    31,   -39,-32768,-32768,   -39,   -39,   -39,    32,    19,   -39,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   -39,
   -39,   -39,-32768,   -39,-32768,-32768,    34,   -39,-32768,    35,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,    37,   -39,-32768,-32768,-32768,-32768,-32768,-32768,
    40,-32768,   -29,-32768,-32768,    42,-32768,-32768,   -39,-32768,
-32768,   101,   103,-32768
};

static const short yypgoto[] = {-32768,
    98,-32768,-32768,-32768,-32768,-32768,    94,    13,-32768,-32768,
    68,-32768,    69,-32768,    84,     3,-32768,    83,   -15,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,    23,-32768,-32768,-32768,-32768,    -2
};


#define	YYLAST		137


static const short yytable[] = {     8,
     7,    65,    66,    67,    68,    69,    70,     1,     2,    20,
    17,    57,   107,    58,    59,   137,    76,    77,    78,    55,
    56,    62,    64,    73,    75,    21,   138,    80,    81,    22,
    23,    21,    18,   103,     9,    24,    25,    10,     1,    26,
    27,    28,    10,     2,     2,    29,     2,     2,    30,    53,
   109,    31,    32,    33,    34,   114,    54,   112,   114,    86,
   113,    87,   111,    88,    35,   111,   102,    36,   115,    83,
    84,   116,   104,    37,   110,   118,   117,   123,   131,   119,
   133,   134,   120,   121,   122,   136,   140,   126,    91,    92,
    93,    94,    95,    96,    97,    98,    99,   127,   128,   129,
   143,   130,   144,    16,    19,   132,   105,   106,    63,    74,
     0,   125,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   135,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   141
};

static const short yycheck[] = {     2,
    45,     3,     4,     5,     6,     7,     8,    12,    48,    12,
    46,    15,    13,    17,    18,    45,    22,    23,    24,    22,
    23,    24,    25,    26,    27,    13,    56,    26,    27,     9,
    10,    19,    45,    36,    11,    15,    16,    14,    12,    19,
    20,    21,    14,    48,    48,    25,    48,    48,    28,    45,
    53,    31,    32,    33,    34,    71,    46,    60,    74,    45,
    63,    45,    60,    45,    44,    63,    45,    47,    71,    29,
    30,    74,    45,    53,    45,    45,    79,    46,    45,    82,
    46,    45,    85,    86,    87,    46,    45,    90,    35,    36,
    37,    38,    39,    40,    41,    42,    43,   100,   101,   102,
     0,   104,     0,     6,    11,   108,    39,    39,    25,    27,
    -1,    89,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   124,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   139
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */

/* This file comes from bison-1.28.  */

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
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

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

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
     unsigned int count;
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
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
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

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

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
  int yyfree_stacks = 0;

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
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
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
		     int a = yyvsp[-3].integer, b= yyvsp[-1].integer;
		     database->setx(yyvsp[-3].integer); database->sety(yyvsp[-1].integer);
                   ;
    break;}
case 12:
{ database->cleardb();
                     database->setx(yyvsp[-4].integer); database->sety(yyvsp[-2].integer);
		     database->zoom(x,y);
                   ;
    break;}
case 13:
{ x = yyvsp[-2].integer;
                   y = yyvsp[0].integer;
		 ;
    break;}
case 18:
{
		    if(specisundefined(specnum)) {
		       yyerror("Spectrum is not defined\n");
		       return -1;
		    }
		    else {
			    if(specis1d(specnum)) {
			      database->define1d(yyvsp[-5].integer,yyvsp[-3].integer, specnum);
			    } else {
			      database->define2d(yyvsp[-5].integer,yyvsp[-3].integer,specnum);
			    }
			    x = yyvsp[-5].integer;
			    y = yyvsp[-3].integer;
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
	     specnum = yyvsp[0].integer;	/* Just return the number as specnum. */
	   ;
    break;}
case 20:
{
	     specnum = xamine_shared->getspecid(yylval.string); /* translate name->id */
	     if(specnum == -1){
	       yyerror("Spectrum name does not match a valid spectrum");
	       return -1;
	     }
           ;
    break;}
case 21:
{
		     x = -1; y = -1;
		     current = NULL;
		   ;
    break;}
case 36:
{
		    assert(current != NULL);
		    if(ticks)  current->tickson();
		    if(labels) current->axis_labelson();
		    if(axes)   current->axeson();
		    ticks = labels = axes = 0;
		  ;
    break;}
case 37:
{
		    if(ticks) current->ticksoff();
		    if(labels)current->axis_labelsoff();
		    if(axes)  current->axesoff();
		    ticks = labels = axes = 0;
		  ;
    break;}
case 38:
{
		   assert(current != NULL);
		   current->set_axes(TRUE, TRUE, TRUE);
		 ;
    break;}
case 39:
{
		   assert(current != NULL);
		   current->set_axes(FALSE,FALSE,FALSE);
		 ;
    break;}
case 42:
{
		     ticks = TRUE;
		   ;
    break;}
case 43:
{
		     labels = TRUE;
		   ;
    break;}
case 44:
{
		     axes = TRUE;
		   ;
    break;}
case 45:
{
		   assert(current != NULL);
		   current->set_titles(TRUE, TRUE, TRUE, TRUE, TRUE, TRUE);
		 ;
    break;}
case 46:
{
		   assert(current != NULL);
		   current->set_titles(FALSE, FALSE, FALSE,
					 FALSE, FALSE, FALSE);
		 ;
    break;}
case 47:
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
case 48:
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
case 51:
{
		     name = TRUE;
		   ;
    break;}
case 52:
{
		     number = TRUE;
		   ;
    break;}
case 53:
{
		  description = TRUE;
		;
    break;}
case 54:
{
		  peak  = TRUE;
		;
    break;}
case 55:
{
		  update = TRUE;
		;
    break;}
case 56:
{
		  objects = TRUE;
		;
    break;}
case 57:
{ 
		  assert(current != NULL);
		  current->sideways();
		;
    break;}
case 58:
{
		  assert(current != NULL);
		  current->normal();
		;
    break;}
case 60:
{
                        assert(current != NULL);
			current->sample();
		      ;
    break;}
case 61:
{
		      assert(current != NULL);
		      current->sum();
		    ;
    break;}
case 62:
{
		      assert(current != NULL);
		      current->average();
		    ;
    break;}
case 64:
{
                       assert(current != NULL);
		       current->autoscale();
		     ;
    break;}
case 65:
{
		   assert(current != NULL);
		   current->setfs(yyvsp[0].integer);
		 ;
    break;}
case 67:
{
			     assert(current != NULL);
			     current->log();
			   ;
    break;}
case 68:
{
			 assert(current != NULL);
			 current->linear();
		       ;
    break;}
case 69:
{
		   assert(current != NULL);
		   current->setfloor(yyvsp[-1].integer);
		 ;
    break;}
case 70:
{
		  assert(current != NULL);
		  current->setceiling(yyvsp[-1].integer);
		;
    break;}
case 71:
{
		  assert(current != NULL);
		  current->update_interval(yyvsp[-1].integer);	/* Set the update interval. */
                ;
    break;}
case 72:
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
case 73:
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
case 74:
{ memcpy(&xlim, &limits, sizeof(struct limit)); ;
    break;}
case 75:
{ memcpy(&ylim, &limits, sizeof(struct limit)); ;
    break;}
case 76:
{ limits.low = yyvsp[-2].integer;
		  limits.high= yyvsp[0].integer;
		;
    break;}
case 79:
{ assert(current);
		    if(!current->is1d()) {
		      windfileerror("Attempted to set 1-d rendition on 2-d spectrum\n");
		    } else {
		      ((win_1d *)current)->smooth();
		    }
		  ;
    break;}
case 80:
{ assert(current);
		    if(!current->is1d()) {
		      windfileerror("Attempted to set 1-d rendition on 2-d spectrum\n");
		    } else {
		      ((win_1d *)current)->bars();
		    }
		  ;
    break;}
case 81:
{ assert(current);
                    if(!current->is1d()) {
		      windfileerror("Attempted to set 1-d rendition on 2-d spectrum\n");
		    } else {
		      ((win_1d *)current)->pts();
		    }
		  ;
    break;}
case 82:
{ assert(current);
		    if(!current->is1d()) {
		      windfileerror("Attempted to set 1-d rendition on 2-d spectrum\n");
		    } else {
		      ((win_1d *)current)->line();
		    }
		  ;
    break;}
case 83:
{ assert(current);
		    if(current->is1d()) {
		      windfileerror("Attempted to set 2-d rendition on 1-d spectrum\n");
		    } else {
		      ((win_2d *)current)->scatterplot();
		    }
		  ;
    break;}
case 84:
{ assert(current);
		    if(current->is1d()) {
		      windfileerror("Attempted to set 2-d rendition on 1-d spectrum\n");
		    } else {
		      ((win_2d *)current)->boxplot();
		    }
		  ;
    break;}
case 85:
{ assert(current);
		    if(current->is1d()) {
		      windfileerror("Attempted to set 2-d rendition on 1-d spectrum\n");
		    } else {
		      ((win_2d *)current)->colorplot();
		    }
		  ;
    break;}
case 86:
{ assert(current);
		    if(current->is1d()) {
		      windfileerror("Attempted to set 2-d rendition on 1-d spectrum\n");
		    } else {
		      ((win_2d *)current)->contourplot();
		    }
		  ;
    break;}
case 87:
{ assert(current);
		    if(current->is1d()) {
		      windfileerror("Attempted to set 2-d rendition on 1-d spectrum\n");
		    } else {
		      ((win_2d *)current)->legoplot();
		    }
		  ;
    break;}
case 88:
{
		     assert(current);
		     win_1d *at1 = (win_1d *)current;
		     if(!current->is1d()) {
		       windfileerror("Superimpose on 2-d spectrum ignored");
		     }
		     else if(!superimposable(current->spectrum(), yyvsp[-1].integer)) {
		       windfileerror("Incompatible superimpose ignored");
		     }
		     else if(superimposed(at1, yyvsp[-1].integer)) {
		       windfileerror("Redundant superimpose ignored");
		     }
		     else {
		       SuperpositionList &sl = at1->GetSuperpositions();
		       if(sl.Count() < MAX_SUPERPOSITIONS) {
			 sl.Add(yyvsp[-1].integer);
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

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
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
