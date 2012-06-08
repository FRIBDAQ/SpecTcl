#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.


AC_INIT(RootFilterFormat, 1.0-003, fox@nscl.msu.edu)
AC_CONFIG_AUX_DIR(config)
AM_CONFIG_HEADER([configure.h])
AC_CANONICAL_HOST
AM_INIT_AUTOMAKE()

AC_CONFIG_MACRO_DIR([m4])

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


tcl_versions="8.5 8.4 8.3 8.2 8.1 8.0"   # First match;  so order new -> old.



tcl_header_dir=""
AC_MSG_CHECKING([for tcl header directory])

AC_ARG_WITH(tcl-header-dir,
	[  --with-tcl-header-dir=path Path to tcl headers],
	[tcl_header_dir=$withval ;
	tcl_headers_found="yes"],[tcl_headers_found="no"])


#
#  Look for tcl.h in the following places:
#  Note that the order is important for Darwin and OSF1.
	#
	# /sw/include             (Best for darwin).
	# /usr/local/tcl/include  (Best for NSCL Cygwin).
	# /usr/local/include      (Best for NSCL OSF1)
	# /usr/include            (Best for e.g. Linux).
	#  
	# For all of these directories,	
	#  look for tcl.h in:
	#    - the directory itself.
	#    - a tcl subdirectory to the directory.
	#    - subdirectories of the form tcl${version} where
	#      version is chosein from tcl_versions.
	#

tcl_h_testdirs="/sw/include /usr/local/tcl/include /usr/local/include \
	/usr/include"

if test $tcl_headers_found = "no"
then
   for d in $tcl_h_testdirs
   do
	for v in ${tcl_versions};
        do
	   if test $tcl_headers_found = "no"
           then
	      AC_CHECK_FILE([${d}/tcl${v}/tcl.h],
	                    [tcl_header_dir=${d}/tcl${v}
	                     tcl_headers_found="yes"])
           fi
        done
	if test $tcl_headers_found = "no"
	then
	   AC_CHECK_FILE([${d}/tcl.h],
	                 [tcl_header_dir=$d
	                  tcl_headers_found="yes"])
        fi
	if test $tcl_headers_found = "no"
	then
	   AC_CHECK_FILE([${d}/tcl/tcl.h],
	                 [tcl_header_dir=${d}/tcl
	                  tcl_headers_found="yes"])
	fi
   done
fi

if test $tcl_headers_found = "yes"; then
   TCLINCDIR=${tcl_header_dir}
   AC_MSG_RESULT(Using path $tcl_header_dir)
else
   AC_MSG_ERROR([can't find tcl.h try using --with-tcl-header-dir to help me])
fi


# --prefix is assumed to be where spectcl is unless overidden
#          with --with-spectcl-home
#

AC_ARG_WITH(spectcl-home,
	[  --with-spectcl-home=path Path to SpecTcl installatino],
	[SPECTCLHOME=$withval],
	[SPECTCLHOME=$prefix])

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

  # Root:


  if test -e $ROOTSYS/bin/root-config
  then
    ROOTCCSW=`${ROOTSYS}/bin/root-config --cflags`
    ROOTLDSW=`${ROOTSYS}/bin/root-config --libs`
  else
    ROOTCCSW="-I${ROOTSYS}/include -I${ROOTSYS}/include/root"
    ROOTLDSW="-L${ROOTSYS}/lib -L${ROOTSYS}/lib/root -lTree -lCore -lCint -ldl -Wl,\"-rpath=${ROOTSYS}/lib/root\""
  fi

  AC_SUBST(ROOTCCSW)
  AC_SUBST(ROOTLDSW)

  # Documentation:

  AC_SUBST(DOCBOOK)
  AC_SUBST(DVIPDF)



# Checks for library functions.


AC_OUTPUT(Makefile)
