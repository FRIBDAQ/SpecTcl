#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT(firstof, 1.0-001, fox@nscl.msu.edu)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_SRCDIR([CFirstofEventProcessor.h])
AC_CONFIG_HEADER([configure.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AM_INIT_AUTOMAKE([])
AC_PROG_LIBTOOL


# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

AC_C_CONST

#--------------------------------------------------------
#  
# Locate the Tcl headers and libraries.
#
AX_TCL



#-----------------------------------------------------------------
# The user must provide the SpecTcl install directory 
# else it defaults to --prefix


# --prefix is assumed to be where spectcl is unless overidden
#          with --with-spectcl-home
#

AC_ARG_WITH(spectcl-home,
	[  --with-spectcl-home=path Path to SpecTcl installatino],
	[SPECTCLHOME=$withval],
	[SPECTCLHOME=$prefix])

#------------------------------------------------------------------
# In order to make documentation, we need a docbooktopdf
# installed.  If it's not installed, we'll make some substitutions
# that will make the build succeed.

AC_PATH_PROG([DOCBOOK], [docbook2dvi],
	     [docbook2dvi], [touch $$@])
AC_PATH_PROG([DVIPDF], [dvipdf],
             [dvipdf], [touch $$@])


#---------------------------------------------------------------------
#
#  Export makefile macros.


  # TCL:


  TCLCCSW="-I${TCLINCDIR}"
  AC_SUBST(TCLCCSW)

  # SpecTcl:

  SPECTCLCCSW="-I${SPECTCLHOME}/include"
  AC_SUBST(SPECTCLCCSW)

  # Documentation:

  AC_SUBST(DOCBOOK)
  AC_SUBST(DVIPDF)



#----------------------------------------------------------------------
#
#  Generate the makefiles etc.


AC_CONFIG_FILES([Makefile])
AC_OUTPUT
