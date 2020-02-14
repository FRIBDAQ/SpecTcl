#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.


AC_INIT(constparam, 1.0-001, fox@nscl.msu.edu)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIRS([m4])
AM_CONFIG_HEADER([configure.h])
AC_CANONICAL_HOST
AM_INIT_AUTOMAKE()

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



# In order to make documentation, we need a docbooktopdf
# installed.  If it's not installed, we'll make some substitutions
# that will make the build succeed.

AC_PATH_PROG([DOCBOOK], [docbook2pdf],
	     [docbook2pdf], [touch $$@])

# Makefile macros exported:

  # TCL:


  TCLCCSW="-I${TCLINCDIR}"
  AC_SUBST(TCLCCSW)

  # SpecTcl:

  SPECTCLCCSW="-I${SPECTCLHOME}/include"
  AC_SUBST(SPECTCLCCSW)
  
  # Documentation:

  AC_SUBST(DOCBOOK)

  

# Checks for library functions.


AC_OUTPUT(Makefile)
