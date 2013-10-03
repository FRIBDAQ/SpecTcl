dnl 
dnl - Autoconf macro to check for the existence of GNU gengetopt.
dnl   --with-gengetopt-path can be provided which is used as the path
dnl   instead of any existing or nonexistent gengetopt.
dnl
dnl   If this all exists, GENGETOPT is AC_SUSBST-ed into the Makefile.


AC_DEFUN([AX_GENGETOPT],[

	# Withval overrides:

	AC_ARG_WITH(gengetopt-path,
		[ --with-gengetopt-path=path-to-gengetopt-executable],
		[GENGETOPT=$withval],
		GENGETOPT="")
		
        if test -z "$GENGETOPT"
	then 
	     AC_PATH_PROGS([GENGETOPT], gengetopt)
	     if test -z "$GENGETOPT"
	     then
		AC_MSG_ERROR([ gnu gengetopt tested for but not found])
	     fi
	fi
	AC_MSG_RESULT([Using $GENGETOPT for gnu gengetoptcommand])
	AC_SUBST(GENGETOPT)
])