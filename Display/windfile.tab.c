/* A Bison parser, made from windfile.y
   by GNU bison 1.35.  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse windfileparse
#define yylex windfilelex
#define yyerror windfileerror
#define yylval windfilelval
#define yychar windfilechar
#define yydebug windfiledebug
#define yynerrs windfilenerrs
# define	NAME	257
# define	NUMBER	258
# define	DESCRIPTION	259
# define	PEAK	260
# define	UPDATE	261
# define	OBJECTS	262
# define	FLIPPED	263
# define	NOFLIPPED	264
# define	TITLE	265
# define	GEOMETRY	266
# define	ZOOMED	267
# define	WINDOW	268
# define	AXES	269
# define	NOAXES	270
# define	TICKS	271
# define	LABELS	272
# define	LABEL	273
# define	NOLABEL	274
# define	REDUCTION	275
# define	SAMPLED	276
# define	SUMMED	277
# define	AVERAGED	278
# define	SCALE	279
# define	AUTO	280
# define	MANUAL	281
# define	COUNTSAXIS	282
# define	LOG	283
# define	LINEAR	284
# define	FLOOR	285
# define	CEILING	286
# define	EXPANDED	287
# define	RENDITION	288
# define	SMOOTHED	289
# define	HISTOGRAM	290
# define	POINTS	291
# define	LINE	292
# define	SCATTER	293
# define	BOX	294
# define	COLOR	295
# define	CONTOUR	296
# define	LEGO	297
# define	REFRESH	298
# define	INTEGER	299
# define	COMMA	300
# define	ENDWINDOW	301
# define	ENDLINE	302
# define	ATTRIBUTES	303
# define	ENDATTRIBUTES	304
# define	RENDITION_1D	305
# define	RENDITION_2D	306
# define	SUPERIMPOSE	307
# define	GROBJFONT	308
# define	UNMATCHED	309
# define	QSTRING	310


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

#ifndef YYSTYPE
typedef union {
  int integer;
  char string[80];
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
#ifndef YYDEBUG
# define YYDEBUG 0
#endif



#define	YYFINAL		144
#define	YYFLAG		-32768
#define	YYNTBASE	57

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 310 ? yytranslate[x] : 96)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     2,     5,     7,    10,    13,    17,    19,    22,
      24,    26,    32,    39,    44,    46,    49,    52,    56,    64,
      66,    68,    71,    73,    76,    78,    80,    82,    84,    86,
      88,    90,    92,    94,    96,    98,   100,   104,   108,   111,
     114,   116,   119,   121,   123,   125,   128,   131,   135,   139,
     141,   144,   146,   148,   150,   152,   154,   156,   159,   162,
     166,   168,   170,   172,   176,   178,   181,   185,   187,   189,
     193,   197,   201,   206,   210,   212,   214,   218,   222,   226,
     228,   230,   232,   234,   236,   238,   240,   242,   244,   248,
     250
};
static const short yyrhs[] =
{
      58,     0,    95,    58,     0,    61,     0,    61,    59,     0,
      61,    64,     0,    61,    59,    64,     0,    60,     0,    60,
      95,     0,    11,     0,    62,     0,    12,    45,    46,    45,
      95,     0,    12,    45,    46,    45,    63,    95,     0,    13,
      45,    46,    45,     0,    65,     0,    64,    65,     0,    66,
      68,     0,    66,    69,    68,     0,    14,    45,    46,    45,
      46,    67,    95,     0,    45,     0,    56,     0,    47,    95,
       0,    70,     0,    69,    70,     0,    71,     0,    74,     0,
      77,     0,    78,     0,    80,     0,    82,     0,    84,     0,
      85,     0,    87,     0,    91,     0,    86,     0,    94,     0,
      15,    72,    95,     0,    16,    72,    95,     0,    15,    95,
       0,    16,    95,     0,    73,     0,    72,    73,     0,    17,
       0,    18,     0,    15,     0,    19,    95,     0,    20,    95,
       0,    19,    75,    95,     0,    20,    75,    95,     0,    76,
       0,    75,    76,     0,     3,     0,     4,     0,     5,     0,
       6,     0,     7,     0,     8,     0,     9,    95,     0,    10,
      95,     0,    21,    79,    95,     0,    22,     0,    23,     0,
      24,     0,    25,    81,    95,     0,    26,     0,    27,    45,
       0,    28,    83,    95,     0,    29,     0,    30,     0,    31,
      45,    95,     0,    32,    45,    95,     0,    44,    45,    95,
       0,    33,    88,    89,    95,     0,    33,    90,    95,     0,
      90,     0,    90,     0,    45,    46,    45,     0,    34,    92,
      95,     0,    34,    93,    95,     0,    35,     0,    36,     0,
      37,     0,    38,     0,    39,     0,    40,     0,    41,     0,
      42,     0,    43,     0,    53,    45,    95,     0,    48,     0,
      48,    95,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   120,   120,   123,   123,   124,   124,   127,   128,   131,
     136,   139,   144,   151,   157,   158,   162,   163,   166,   190,
     194,   204,   211,   211,   214,   214,   214,   215,   215,   215,
     216,   216,   216,   217,   217,   217,   220,   228,   235,   240,
     248,   249,   252,   256,   260,   266,   271,   277,   288,   301,
     301,   304,   308,   312,   315,   318,   321,   326,   332,   339,
     342,   346,   350,   356,   359,   363,   370,   373,   377,   383,
     390,   397,   404,   427,   448,   452,   456,   462,   463,   465,
     473,   481,   489,   499,   507,   515,   523,   531,   542,   566,
     566
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "NAME", "NUMBER", "DESCRIPTION", "PEAK", 
  "UPDATE", "OBJECTS", "FLIPPED", "NOFLIPPED", "TITLE", "GEOMETRY", 
  "ZOOMED", "WINDOW", "AXES", "NOAXES", "TICKS", "LABELS", "LABEL", 
  "NOLABEL", "REDUCTION", "SAMPLED", "SUMMED", "AVERAGED", "SCALE", 
  "AUTO", "MANUAL", "COUNTSAXIS", "LOG", "LINEAR", "FLOOR", "CEILING", 
  "EXPANDED", "RENDITION", "SMOOTHED", "HISTOGRAM", "POINTS", "LINE", 
  "SCATTER", "BOX", "COLOR", "CONTOUR", "LEGO", "REFRESH", "INTEGER", 
  "COMMA", "ENDWINDOW", "ENDLINE", "ATTRIBUTES", "ENDATTRIBUTES", 
  "RENDITION_1D", "RENDITION_2D", "SUPERIMPOSE", "GROBJFONT", "UNMATCHED", 
  "QSTRING", "setup_file", "setup_filel", "ident", "title", "geometry", 
  "geometry_line", "zoom_clause", "descriptions", "description", 
  "window_clause", "spectrum", "endwindow_clause", "attribute_clauses", 
  "attribute_clause", "axes_clause", "axis_attributes", "axis_attribute", 
  "labels_clause", "label_attributes", "label_attribute", 
  "flipped_clause", "reduction_clause", "reduction_attribute", 
  "scale_clause", "scale_attribute", "countsaxis_clause", 
  "countsaxis_attributes", "floor_clause", "ceiling_clause", 
  "refresh_clause", "expanded_clause", "xlimit", "ylimit", "limits", 
  "rendition_clause", "rend1_attribute", "rend2_attribute", 
  "superposition_clause", "blankline", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,    57,    57,    58,    58,    58,    58,    59,    59,    60,
      61,    62,    62,    63,    64,    64,    65,    65,    66,    67,
      67,    68,    69,    69,    70,    70,    70,    70,    70,    70,
      70,    70,    70,    70,    70,    70,    71,    71,    71,    71,
      72,    72,    73,    73,    73,    74,    74,    74,    74,    75,
      75,    76,    76,    76,    76,    76,    76,    77,    77,    78,
      79,    79,    79,    80,    81,    81,    82,    83,    83,    84,
      85,    86,    87,    87,    88,    89,    90,    91,    91,    92,
      92,    92,    92,    93,    93,    93,    93,    93,    94,    95,
      95
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     1,     2,     1,     2,     2,     3,     1,     2,     1,
       1,     5,     6,     4,     1,     2,     2,     3,     7,     1,
       1,     2,     1,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     3,     3,     2,     2,
       1,     2,     1,     1,     1,     2,     2,     3,     3,     1,
       2,     1,     1,     1,     1,     1,     1,     2,     2,     3,
       1,     1,     1,     3,     1,     2,     3,     1,     1,     3,
       3,     3,     4,     3,     1,     1,     3,     3,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     1,
       2
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       0,     0,    89,     1,     3,    10,     0,     0,    90,     9,
       0,     4,     7,     5,    14,     0,     2,     0,     0,     6,
       8,    15,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    16,     0,
      22,    24,    25,    26,    27,    28,    29,    30,    31,    34,
      32,    33,    35,     0,     0,    57,    58,    44,    42,    43,
       0,    40,    38,     0,    39,    51,    52,    53,    54,    55,
      56,     0,    49,    45,     0,    46,    60,    61,    62,     0,
      64,     0,     0,    67,    68,     0,     0,     0,     0,     0,
      74,    79,    80,    81,    82,    83,    84,    85,    86,    87,
       0,     0,     0,    21,     0,    17,    23,     0,     0,    11,
       0,    41,    36,    37,    50,    47,    48,    59,    65,    63,
      66,    69,    70,     0,     0,    75,    73,    77,    78,    71,
      88,     0,    12,     0,    76,    72,     0,    19,    20,     0,
      13,    18,     0,     0,     0
};

static const short yydefgoto[] =
{
     142,     3,    11,    12,     4,     5,   108,    13,    14,    15,
     139,    38,    39,    40,    41,    60,    61,    42,    71,    72,
      43,    44,    79,    45,    82,    46,    85,    47,    48,    49,
      50,    89,   124,    90,    51,   100,   101,    52,     6
};

static const short yypact[] =
{
      -4,   -44,   -39,-32768,    24,-32768,    27,   -35,-32768,-32768,
     -12,    29,   -39,    29,-32768,    21,-32768,     5,    11,    29,
  -32768,-32768,   -39,   -39,    -3,    -3,    -1,    -1,    -5,     2,
      41,    15,    17,    19,    54,    22,   -39,    28,-32768,    21,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,     0,    30,-32768,-32768,-32768,-32768,-32768,
      -3,-32768,-32768,    -3,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,    -1,-32768,-32768,    -1,-32768,-32768,-32768,-32768,   -39,
  -32768,    31,   -39,-32768,-32768,   -39,   -39,   -39,    32,    19,
     -39,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
     -39,   -39,   -39,-32768,   -39,-32768,-32768,    34,   -39,-32768,
      35,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,    37,   -39,-32768,-32768,-32768,-32768,-32768,
  -32768,    40,-32768,   -29,-32768,-32768,    42,-32768,-32768,   -39,
  -32768,-32768,   101,   103,-32768
};

static const short yypgoto[] =
{
  -32768,    98,-32768,-32768,-32768,-32768,-32768,    94,    13,-32768,
  -32768,    68,-32768,    69,-32768,    84,     3,-32768,    83,   -15,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,    23,-32768,-32768,-32768,-32768,    -2
};


#define	YYLAST		137


static const short yytable[] =
{
       8,     7,    65,    66,    67,    68,    69,    70,     1,     2,
      20,    17,    57,   107,    58,    59,   137,    76,    77,    78,
      55,    56,    62,    64,    73,    75,    21,   138,    80,    81,
      22,    23,    21,    18,   103,     9,    24,    25,    10,     1,
      26,    27,    28,    10,     2,     2,    29,     2,     2,    30,
      53,   109,    31,    32,    33,    34,   114,    54,   112,   114,
      86,   113,    87,   111,    88,    35,   111,   102,    36,   115,
      83,    84,   116,   104,    37,   110,   118,   117,   123,   131,
     119,   133,   134,   120,   121,   122,   136,   140,   126,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   127,   128,
     129,   143,   130,   144,    16,    19,   132,   105,   106,    63,
      74,     0,   125,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   135,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   141
};

static const short yycheck[] =
{
       2,    45,     3,     4,     5,     6,     7,     8,    12,    48,
      12,    46,    15,    13,    17,    18,    45,    22,    23,    24,
      22,    23,    24,    25,    26,    27,    13,    56,    26,    27,
       9,    10,    19,    45,    36,    11,    15,    16,    14,    12,
      19,    20,    21,    14,    48,    48,    25,    48,    48,    28,
      45,    53,    31,    32,    33,    34,    71,    46,    60,    74,
      45,    63,    45,    60,    45,    44,    63,    45,    47,    71,
      29,    30,    74,    45,    53,    45,    45,    79,    46,    45,
      82,    46,    45,    85,    86,    87,    46,    45,    90,    35,
      36,    37,    38,    39,    40,    41,    42,    43,   100,   101,
     102,     0,   104,     0,     6,    11,   108,    39,    39,    25,
      27,    -1,    89,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   124,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   139
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */

/* Skeleton output parser for bison,

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software
   Foundation, Inc.

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

/* This is the parser code that is written into each bison parser when
   the %semantic_parser declaration is not specified in the grammar.
   It was written by Richard Stallman by simplifying the hairy parser
   used when %semantic_parser is specified.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

#if ! defined (yyoverflow) || defined (YYERROR_VERBOSE)

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || defined (YYERROR_VERBOSE) */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYLTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
# if YYLSP_NEEDED
  YYLTYPE yyls;
# endif
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAX (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# if YYLSP_NEEDED
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE) + sizeof (YYLTYPE))	\
      + 2 * YYSTACK_GAP_MAX)
# else
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAX)
# endif

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAX;	\
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif


#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");			\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).

   When YYLLOC_DEFAULT is run, CURRENT is set the location of the
   first token.  By default, to implement support for ranges, extend
   its range to the last symbol.  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)       	\
   Current.last_line   = Rhs[N].last_line;	\
   Current.last_column = Rhs[N].last_column;
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#if YYPURE
# if YYLSP_NEEDED
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, &yylloc, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval, &yylloc)
#  endif
# else /* !YYLSP_NEEDED */
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval)
#  endif
# endif /* !YYLSP_NEEDED */
#else /* !YYPURE */
# define YYLEX			yylex ()
#endif /* !YYPURE */


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)
/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif

#ifdef YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif
#endif



/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
#  define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL
# else
#  define YYPARSE_PARAM_ARG YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
# endif
#else /* !YYPARSE_PARAM */
# define YYPARSE_PARAM_ARG
# define YYPARSE_PARAM_DECL
#endif /* !YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
# ifdef YYPARSE_PARAM
int yyparse (void *);
# else
int yyparse (void);
# endif
#endif

/* YY_DECL_VARIABLES -- depending whether we use a pure parser,
   variables are global, or local to YYPARSE.  */

#define YY_DECL_NON_LSP_VARIABLES			\
/* The lookahead symbol.  */				\
int yychar;						\
							\
/* The semantic value of the lookahead symbol. */	\
YYSTYPE yylval;						\
							\
/* Number of parse errors so far.  */			\
int yynerrs;

#if YYLSP_NEEDED
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES			\
						\
/* Location data for the lookahead symbol.  */	\
YYLTYPE yylloc;
#else
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES
#endif


/* If nonreentrant, generate the variables here. */

#if !YYPURE
YY_DECL_VARIABLES
#endif  /* !YYPURE */

int
yyparse (YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  /* If reentrant, generate the variables here. */
#if YYPURE
  YY_DECL_VARIABLES
#endif  /* !YYPURE */

  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yychar1 = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack. */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;

#if YYLSP_NEEDED
  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
#endif

#if YYLSP_NEEDED
# define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
# define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  YYSIZE_T yystacksize = YYINITDEPTH;


  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
#if YYLSP_NEEDED
  YYLTYPE yyloc;
#endif

  /* When reducing, the number of symbols on the RHS of the reduced
     rule. */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;
#if YYLSP_NEEDED
  yylsp = yyls;
#endif
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  */
# if YYLSP_NEEDED
	YYLTYPE *yyls1 = yyls;
	/* This used to be a conditional around just the two extra args,
	   but that might be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
# else
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);
# endif
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);
# if YYLSP_NEEDED
	YYSTACK_RELOCATE (yyls);
# endif
# undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
#if YYLSP_NEEDED
      yylsp = yyls + yysize - 1;
#endif

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
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
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yychar1 = YYTRANSLATE (yychar);

#if YYDEBUG
     /* We have to keep this `#if YYDEBUG', since we use variables
	which are defined only if `YYDEBUG' is set.  */
      if (yydebug)
	{
	  YYFPRINTF (stderr, "Next token is %d (%s",
		     yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise
	     meaning of a token, for further debugging info.  */
# ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
# endif
	  YYFPRINTF (stderr, ")\n");
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
  YYDPRINTF ((stderr, "Shifting token %d (%s), ",
	      yychar, yytname[yychar1]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to the semantic value of
     the lookahead token.  This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

#if YYLSP_NEEDED
  /* Similarly for the default location.  Let the user run additional
     commands if for instance locations are ranges.  */
  yyloc = yylsp[1-yylen];
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
#endif

#if YYDEBUG
  /* We have to keep this `#if YYDEBUG', since we use variables which
     are defined only if `YYDEBUG' is set.  */
  if (yydebug)
    {
      int yyi;

      YYFPRINTF (stderr, "Reducing via rule %d (line %d), ",
		 yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (yyi = yyprhs[yyn]; yyrhs[yyi] > 0; yyi++)
	YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
      YYFPRINTF (stderr, " -> %s\n", yytname[yyr1[yyn]]);
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



  yyvsp -= yylen;
  yyssp -= yylen;
#if YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;
#if YYLSP_NEEDED
  *++yylsp = yyloc;
#endif

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("parse error, unexpected ") + 1;
	  yysize += yystrlen (yytname[YYTRANSLATE (yychar)]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "parse error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[YYTRANSLATE (yychar)]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exhausted");
	}
      else
#endif /* defined (YYERROR_VERBOSE) */
	yyerror ("parse error");
    }
  goto yyerrlab1;


/*--------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action |
`--------------------------------------------------*/
yyerrlab1:
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;
      YYDPRINTF ((stderr, "Discarding token %d (%s).\n",
		  yychar, yytname[yychar1]));
      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;


/*-------------------------------------------------------------------.
| yyerrdefault -- current state does not do anything special for the |
| error token.                                                       |
`-------------------------------------------------------------------*/
yyerrdefault:
#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */

  /* If its default is to accept any token, ok.  Otherwise pop it.  */
  yyn = yydefact[yystate];
  if (yyn)
    goto yydefault;
#endif


/*---------------------------------------------------------------.
| yyerrpop -- pop the current state because it cannot handle the |
| error token                                                    |
`---------------------------------------------------------------*/
yyerrpop:
  if (yyssp == yyss)
    YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#if YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "Error: state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

/*--------------.
| yyerrhandle.  |
`--------------*/
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

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

/*---------------------------------------------.
| yyoverflowab -- parser overflow comes here.  |
`---------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
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
