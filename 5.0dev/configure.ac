# Process this file with autoconf to produce a configure script.

AC_INIT(features/attach.feature)
AC_CONFIG_AUX_DIR(config)
AM_CONFIG_HEADER(config.h)
AC_CANONICAL_HOST
AM_INIT_AUTOMAKE(SpecTcl, 5.0, fox@nscl.msu.edu)
AC_CONFIG_MACRO_DIR([m4])

AC_EXEEXT
AC_OBJEXT

# we use libtool, thus we need to include it
LT_INIT


# Checks for programs.


AC_PROG_AWK
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AM_PROG_LEX
AC_PROG_YACC
AC_PROG_GCC_TRADITIONAL
AC_PROG_LIBTOOL

# Checks for header files.
AC_HEADER_STDC


# -------------------- X11 tests


# Locate Tcl/Tk and setup compile/link switches for them too:

AX_TCL

#  Ensure liburiparser is available:

AC_LANG(C)

AX_URIPARSER

AM_PATH_CPPUNIT
AM_CONDITIONAL([CPPUNIT], [test -n "${CPPUNIT_LIBS}"] )

AC_MSG_CHECKING([gcc so use -pedantic?])

if `echo $CC | grep -q gcc` 
then 
     CFLAGS="$CFLAGS -pedantic -Wno-long-long"
     CXXFLAGS="$CXXFLAGS -pedantic -Wno-long-long"
     AC_MSG_RESULT(yes)
else  
     AC_MSG_RESULT(no)
fi

#
#   libTclPlus and libException could have been installed from 
#   Ryan's Deb or I might need to install my own versions which
#   get exported into the build tree from a tag of the repository
#   into the libtcl subdirectory of this tree.
#    
#  Produce:
#     LIBTCLPLUS_CFLAGS - Compilation flags required to build against the
#                         appropriate one.
#     LIBTCLPLUS_LDFLAGS - Appropriate linker flags.
#
# 

AC_MSG_RESULT([Tcl cppflags are ${TCL_CPPFLAGS}])

AC_MSG_CHECKING([Checking for libtclPlus])
AC_CHECK_FILE([/usr/include/TCLInterpreterObject.h], [systemtcl="yes"], [systemtcl="no"])


#
#  IF systemtcl is yes we're pretty much done.. otherwise
#  we need to configure/build/install libtcl into prefix and then set
#  the flags:

if test $systemtcl = "yes"
then
	LIBTCLPLUS_CFLAGS=""
	LIBTCLPLUS_LDFLAGS="-ltclPlus -lException"
	AC_MSG_RESULT([Found in  system libraries])
else
	AC_MSG_RESULT([Need to build local tagged copy])
	AC_CHECK_FILE([./libtcl/configure], [havelibtcldir="yes"], [havelibtcldir="no"])
	if test $havelibtcldir = "yes"
	then
		(cd libtcl; ./configure --prefix=${prefix}; make clean install)
		LIBTCLPLUS_CFLAGS="-I${prefix}/include"
		LIBTCLPLUS_LDFLAGS=-"L${prefix}/lib -ltclPlus -lException -Wl,\"-rpath=${prefix}/lib\""
	else
		AC_MSG_ERROR([I seem to be missing libtcl, please use tcl++incorp.sh to fill it with a tagged release of libtclplus])
	fi

fi

AC_SUBST(LIBTCLPLUS_CFLAGS)
AC_SUBST(LIBTCLPLUS_LDFLAGS)


##
# See if the cucumber acceptance testing framework is defined:
#
AC_CHECK_PROG([have_cucumber], [cucumber], [yes], [no]) 

AM_CONDITIONAL([HAVE_CUCUMBER], [test ${have_cucumber} = "yes"])


AC_OUTPUT([Makefile]
	[SpecTclPackage/Makefile])
