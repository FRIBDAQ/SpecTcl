#ifndef DEFAULTFILE_TAB_H
# define DEFAULTFILE_TAB_H

typedef union {
   int integer;
   char string[80];
 } YYSTYPE;
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
# define	DEFAULTFILE	310
# define	QSTRING	311


extern YYSTYPE defaultfilelval;

#endif /* not DEFAULTFILE_TAB_H */
