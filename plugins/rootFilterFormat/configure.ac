#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.


AC_INIT(RootFilterFormat, 1.0-004, fox@nscl.msu.edu)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIRS([m4])
AM_CONFIG_HEADER([configure.h])
AC_CANONICAL_HOST
AM_INIT_AUTOMAKE()

#AC_CONFIG_MACRO_DIR([m4])

AC_EXEEXT
AC_OBJEXT

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST


# Checks for libraries.

# Checks for header files.

AX_TCL

# --prefix is assumed to be where spectcl is unless overidden
#          with --with-spectcl-home
#


AC_ARG_WITH(spectcl-home,
	[  --with-spectcl-home=path Path to SpecTcl installatino],
	[SPECTCLHOME=$withval],
	[SPECTCLHOME=$prefix])

##
# We could find the root bits and pieces if root-config is in the path./
#

AC_CHECK_PROG(rootinstalled, [root-config], [1], [0])
if test $rootinstalled = 1
then
	ROOTCCSW=`root-config --cflags`
	ROOTLDSW=`root-config --libs`
else


# --rootsys is required as we have no idea where to look for it.

AC_ARG_WITH(rootsys,
	[  --with-rootsys=path Path to Root installation],
	[ROOTSYS=$withval],
	[ROOTSYS=""])

if test -d $ROOTSYS
then
  AC_MSG_RESULT([ Root is installedi n $ROOTSYS])
else
   AC_MSG_ERROR([ You must tell me where root is installed via --with-rootsys])
fi
  # Root:


  if test -e $ROOTSYS/bin/root-config
  then
    ROOTCCSW=`${ROOTSYS}/bin/root-config --cflags`
    ROOTLDSW=`${ROOTSYS}/bin/root-config --libs`
  else
    ROOTCCSW="-I${ROOTSYS}/include -I${ROOTSYS}/include/root"
    ROOTLDSW="-L${ROOTSYS}/lib -L${ROOTSYS}/lib/root -lTree -lCore -lCint -ldl -Wl,\"-rpath=${ROOTSYS}/lib/root\""
  fi
fi
  AC_SUBST(ROOTCCSW)
  AC_SUBST(ROOTLDSW)
# In order to make documentation, we need a docbooktopdf
# installed.  If it's not installed, we'll make some substitutions
# that will make the build succeed.

AC_PATH_PROG([DOCBOOK], [docbook2dvi],
	     [docbook2dvi], [touch $$@])
AC_PATH_PROG([DVIPDF], [dvipdf],
             [dvipdf], [touch $$@])


# Makefile macros exported:

  # TCL:


  TCLCCSW="-I${TCLINCDIR}"
  AC_SUBST(TCLCCSW)

  # SpecTcl:

  SPECTCLCCSW="-I${SPECTCLHOME}/include"
  AC_SUBST(SPECTCLCCSW)



  # Documentation:

  AC_SUBST(DOCBOOK)
  AC_SUBST(DVIPDF)



# Checks for library functions.


AC_OUTPUT(Makefile)
