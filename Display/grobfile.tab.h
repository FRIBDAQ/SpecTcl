#ifndef BISON_GROBFILE_TAB_H
# define BISON_GROBFILE_TAB_H

#ifndef YYSTYPE
typedef union {
        int integer;
	char string[80];
      } yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
# define	IDENTIFIER	257
# define	OBJECT	258
# define	ENDOBJECT	259
# define	OBJECTNAME	260
# define	BEGINPTS	261
# define	ENDPTS	262
# define	TITLE	263
# define	CUT1D	264
# define	SUM1D	265
# define	MARK1D	266
# define	CONTOUR	267
# define	BAND	268
# define	SUM2D	269
# define	MARK2D	270
# define	QSTRING	271
# define	NOMATCH	272
# define	INTEGER	273
# define	SPECTRUM	274
# define	ID	275
# define	COMMA	276


extern YYSTYPE grobjfilelval;

#endif /* not BISON_GROBFILE_TAB_H */
