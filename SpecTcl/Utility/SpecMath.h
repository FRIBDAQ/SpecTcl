/*!
  This file exists as a replacement for #include'ing math.h on OSF machines.
  The problem is that math.h on some OSF versions redefine a struct exception
  which is already defined in C++. 
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* 
   If 'struct exception' is redefined in math.h, we need to make sure
   it doesn't get defined. Its definition is contingent upon _ANSI_C_SOURCE
   and __LANGUAGE_ASSEMBLY__ not being defined. We make sure that one of
   them is defined before including math.h
*/

#ifdef OSF
#if (!defined (_ANSI_C_SOURCE)) || (!defined(__LANGUAGE_ASSEMBLY__))
#ifndef _ANSI_C_SOURCE    /* Make sure both are defined */
#define _ANSI_C_SOURCE
#else
#define __LANGUAGE_ASSEMBLY__
#endif

/* Define other things that are missing because of the definitions above. Note
   That the following code is pulled straight from math.h */
#ifdef	_DEC_XPG                   
#define	_XOPEN_SOURCE_EXTENDED
#define	_XOPEN_SOURCE	500
#else
#ifdef	_DEC_XPG4R2
#define	_XOPEN_SOURCE_EXTENDED
#define	_XOPEN_SOURCE	420
#else
#define	_XOPEN_SOURCE	400
#endif
#endif
/* Set historical default environment. */
#define _AES_SOURCE
#if	defined(_PTHREAD_USE_D4)||defined(PTHREAD_USE_D4)
/* P1003.4a/D4 interface compatibility */
#define	_POSIX_C_SOURCE	199309L
#else
/* default to P1003.1c */
#define _POSIX_C_SOURCE 199506L
#endif	/* _PTHREAD_USE_D4 */
/* _XOPEN_SOURCE compatibility */
#define _POSIX_SOURCE
/* P1003.4 obsolete symbol compatibility */
#define _POSIX_4SOURCE
#endif
#include <math.h>
#define _OSF_SOURCE
#ifdef __LANGUAGE_ASSEMBLY__   // now undefine it so other includes are okay
#undef __LANGUAGE_ASSEMBLY__
#endif
#else
#include <math.h>   /* If exception not redefined, just include math.h */
#endif

