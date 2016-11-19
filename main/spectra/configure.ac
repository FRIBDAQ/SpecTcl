AC_INIT([spectra], [0.3])

AC_CONFIG_AUX_DIR([config])

AM_INIT_AUTOMAKE([foreign])

AC_CONFIG_MACRO_DIR([m4])


#AM_CONFIG_HEADER(config.h)
AC_EXEEXT
AC_OBJEXT

AX_CXX_COMPILE_STDCXX_11([noext])

LT_INIT

AC_PROG_CXX

PKG_PROG_PKG_CONFIG

# ...

# Check for Qt libraries
PKG_CHECK_MODULES(QT, [QtCore, QtGui, QtNetwork], [], [AC_MSG_ERROR([Qt libraries are required.])])

# Retrieve Qt compilation and linker flags
CPPFLAGS="`$PKG_CONFIG --cflags-only-I QtCore QtGui QtNetwork` $CPPFLAGS"
LDFLAGS="`$PKG_CONFIG --libs-only-L QtCore QtGui QtNetwork` $LDFLAGS"
LIBS="`$PKG_CONFIG --libs-only-l QtCore QtGui QtNetwork` $LIBS"

if ! `$PKG_CONFIG --atleast-version=4.6.0 QtCore`; then
   AC_MSG_ERROR([Qt >= 4.6.0 is required.])
fi

AC_CHECK_PROGS(MOC, [moc-qt5 moc-qt4 moc])
AC_CHECK_PROGS(UIC, [uic-qt5 uic-qt4 uic])
AC_CHECK_PROGS(RCC, [rcc])
 if test -z "$MOC" || test -z "$UIC" || test -z "$RCC"; then
   AC_MSG_ERROR([Qt utility programs moc, uic, and rcc are required.])
fi


AC_CHECK_PROG(ROOTCONFIGDFLT, [root-config], [root-config], [], [$PATH])
AC_MSG_NOTICE([root-config search found : $ROOTCONFIGDFLT])

# override of default version
AC_ARG_WITH([rootsys],
	AS_HELP_STRING([--with-rootsys], [Path to ROOT install directory (a.k.a. ROOTSYS)]))

AC_MSG_NOTICE([with_rootsys is now $with_rootsys])

AC_PATH_PROG([ROOTCONFIG], [root-config], [notfound], [$with_rootsys/bin:$PATH])
#if test "x$with_rootsys" != "xno" -a "x$with_rootsys" != "xyes" -a "x$with_rootsys" != "x" ; then
#fi

if test "x$ROOTCONFIG" = "xnotfound" ; then 
  ROOTCONFIG=$ROOTCONFIGDFLT
  AC_MSG_NOTICE([Setting to $ROOTCONFIG])
fi

if test -z "$ROOTCONFIG" ; then
  AC_MSG_ERROR([Unable to locate root-config that is is required.])
fi

ROOT_CFLAGS=`$ROOTCONFIG --cflags`
ROOT_LDFLAGS=`$ROOTCONFIG --glibs`
AC_SUBST(ROOT_CFLAGS)
AC_SUBST(ROOT_LDFLAGS)


AC_OUTPUT([Makefile 
           src/Makefile])
