#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.


AC_INIT(mapvalues, 1.0-001, fox@nscl.msu.edu)
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
	[  --with-spectcl-home=path Path to SpecTcl installation],
	[SPECTCLHOME=$withval],
	[SPECTCLHOME=$prefix])


# The Gnu scientific library is used for 
# to make the fits.  We need to be able to find
# <gsl.h> and libgsl, -libgslcblas
#

AC_ARG_WITH(gsl-home,
	    [  --with-gsl-home=path Path to GNU Scientific libary installation],
	    [GSLHOME=$withval],
	    [GSLHOME="none"])

# if not explcitly specified, we are going to assume that if we can find
# the lib we can find the header...

if test "$GSLHOME" = "none"  
then


   AC_CHECK_HEADER([gsl/gsl_fit.h],
		   [GSLHOME=""], [GSLHOME="none"])
   AC_CHECK_LIB([gsl], [gsl_fit_linear],
		[GSLHOME=""], [GSLHOME="none"], [-lgslcblas])
fi



if test "$GSLHOME" = "none"
 then
   AC_MSG_ERROR([Can't find the GNU Scientific library install it or use --with-gsl-home to tell me where it is])
else
   if test "$GSLHOME" = "" 
   then
      GSL_CXXFLAGS=""
      GSL_LDFLAGS="-lgsl -lgslcblas"
   else
      GSL_CXXFLAGS="-I$GSLHOME/include"
      GSL_LDFLAGS="-L$GSLHOME/lib -lgsl -lgslcblas"
      
   fi
fi



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
  AC_SUBST(TCLSH_CMD)

  # SpecTcl:

  SPECTCLCCSW="-I${SPECTCLHOME}/include"
  AC_SUBST(SPECTCLCCSW)
  
  # GNU scientific library:

  AC_SUBST(GSL_CXXFLAGS)
  AC_SUBST(GSL_LDFLAGS)

  # Documentation:

  AC_SUBST(DOCBOOK)
  AC_SUBST(DVIPDF)
  

# Checks for library functions.


AC_OUTPUT(Makefile)
