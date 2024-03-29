# Process this file with autoconf to produce a configure script.

AC_INIT(SpecTcl,5.13-013, daqhelp@frib.msu.edu)

#AC_CONFIG_SRCDIR(SpecTcl/MySpecTclApp.cpp)
AC_CONFIG_AUX_DIR(config)

AM_INIT_AUTOMAKE([foreign])
AM_CONFIG_HEADER(config.h)
AC_CANONICAL_HOST
AC_CONFIG_MACRO_DIR([m4])

AC_EXEEXT
AC_OBJEXT

AX_CXX_COMPILE_STDCXX_11([noext])

# we use libtool, thus we need to include it
LT_INIT

#  This should track the version in
#  AM_INIT_AUTOMAKE it will be used to
#  define the version information in shared
#  images.

SOVERSION="4:0"
#

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

# PYthon C/C++ extension environment build flags:

PKG_CHECK_MODULES(PYTHON, [python3-embed], [PYTHON_CXXFLAGS=`pkg-config --cflags python3-embed`; PYTHON_LDFLAGS=`pkg-config --libs python3-embed`],
			  [PYTHON_CXXFLAGS=`pkg-config python3 --cflags`, PYTHON_LDFLAGS=`pkg-config python3 --libs`])
AC_SUBST(PYTHON_CXXFLAGS)
AC_SUBST(PYTHON_LDFLAGS)

#PYTHON_CXXFLAGS=`pkg-config python3 --cflags`
#AC_SUBST(PYTHON_CXXFLAGS)
#PYTHON_LDFLAGS=`pkg-config python3 --libs`
#AC_SUBST(PYTHON_LDFLAGS)

# needed for autotools version earlier than 1.14
AM_PROG_CC_C_O

# The following are for doctools things:

AC_PATH_PROG(HCDOCBOOK,   docbook2dvi,  echo)
AC_PATH_PROG(HTMLDOCBOOK, docbook2html, echo)
AC_PATH_PROG(DVIPDF,      dvipdf,       echo)
AC_PATH_PROG(MANDOCBOOK,  xmlto,        echo)

#   Starting with SpecTcl 5.0 we have a new style startup.
#   Let's let the compiler know so that people can
#   conditionalize their MySpecTclApp.cpp's accordingly.


AH_TEMPLATE([SPECTCL_5_INIT],
	[Let the compiler know that SpecTcl 5 style initialization is in evfect])
AC_DEFINE(SPECTCL_5_INIT)

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADER(time.h)
AC_CHECK_HEADERS([fcntl.h limits.h malloc.h memory.h stddef.h stdlib.h strings.h string.h sys/file.h sys/ioctl.h sys/socket.h sys/time.h unistd.h values.h windows.h machine/hal_sysinfo.h sys/proc.h sys/mtio.h sys/mman.h sys/ipc.h sys/shm.h machine/param.h sys/un.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STAT
AC_HEADER_TIME
AC_STRUCT_TM
AC_C_CONST
AC_C_INLINE
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SIGNAL

# Checks for library functions.
AC_FUNC_MMAP
AC_REPLACE_FUNCS([atexit dup2 ftruncate getpagesize memset putenv])
AC_REPLACE_FUNCS([strchr strcspn strerror strrchr bzero mkstemp stat]) 
AC_REPLACE_FUNCS([malloc memcmp strftime alloca vfork strtok_r])
AC_CHECK_FUNCS([select socket strspn fork drand48 seed48]) 
AC_CHECK_FUNCS([pipe sbrk shm_open shmget fcntl])
AC_CHECK_FUNCS([floor getcwd gethostname gettimeofday mbrlen])
AC_CHECK_FUNCS(time)
AC_CHECK_LIB(m, pow)
AC_CHECK_LIB(m, sqrt)
AC_CHECK_LIB(m, logb)
AC_CHECK_FUNCS([strdup strstr strtol tzset mktime strtod timezone])
AC_CHECK_LIB(rt, shm_open, [RTLIB=-lrt
AC_DEFINE([HAVE_SHM_OPEN], [1])],[RTLIB=""])
AC_SUBST(RTLIB)


#  The following are needed for strict ANSI c++ compilers The major features
#  are whether or not there are .h -less headers and if there's an std
#  namespace we'll need to import.

#  See if the C runtime libs have .h's on them or
#  if h-less versions can be used for what we need:
#

AC_LANG_PUSH(C++)
AC_CHECK_HEADER(iostream, [AC_DEFINE([HAVE_HLESS_IOSTREAM],1,[<iostream> exists])],[])
AC_CHECK_HEADER(istream,  [AC_DEFINE([HAVE_HLESS_ISTREAM],1,[<istream> exists])],[])
AC_CHECK_HEADER(ostream,  [AC_DEFINE([HAVE_HLESS_OSTREAM],1,[<ostream> exists])],[])
AC_CHECK_HEADER(streambuf,[AC_DEFINE([HAVE_HLESS_STREAMBUF],1,[<streambuf> exists])],[])
AC_CHECK_HEADER(fstream,  [AC_DEFINE([HAVE_HLESS_FSTREAM],1,[<fstream> exists])],[])
AC_CHECK_HEADER(iomanip,  [AC_DEFINE([HAVE_HLESS_IOMANIP],1,[<iomanip> exists])],[])
AC_CHECK_HEADER(sstream,  [AC_DEFINE([HAVE_HLESS_SSTREAM],1,[<sstream> exists])],[])
AC_CHECK_HEADER(typeinfo, [AC_DEFINE([HAVE_HLESS_TYPEINFO],1,[<typeinfo> exists])],[])




#   Is there an std namespace to import from?

AC_COMPILE_IFELSE([AC_LANG_SOURCE([using namespace std;])], [AC_DEFINE([HAVE_STD_NAMESPACE],[1],[1 if std namespace defined])])
AH_VERBATIM([VHAVE_STD_NAMESPACE],[
/* Define STD macro according to HAVE_STD_NAMESPACE value */
#ifdef HAVE_STD_NAMESPACE
#define STD(name)  std::name
#else
#define STD(name) name
#endif])



AC_LANG_POP(C++)

#
#  By default nscldaq will be in /usr/opt/daq/current
#  This can be overidden with --with-nscldaq-root.
#  We're not going to check to see if there really is an nscldaq since it's valid
#  to install SpecTcl on a daq-less system for analysis.

AC_ARG_WITH([nscldaq-root],
    [  --with-nscldaq-root=path Path to the NSCLDAQ installation (default=/usr/opt/daq/current)],
    [daqroot="$withval"], [daqroot="/usr/opt/daq/current"])

DAQROOT=$daqroot

AC_SUBST(DAQROOT)



# -------------------- X11 tests

# Locate the X11 header files and the X11 library archive. Try the
# ac_path_x macro first, but if it doesn't find the X stuff then check
# a list of possible directories.
#  Assume that if the stuff is in the 'normal places' it doesn't need any
#  -rpath-ing.

AC_PATH_X
AC_PATH_XTRA

if test "$x_includes" != ""
then
  XINCLUDES="-I$x_includes"
else
  XINCLUDES=""
fi
if test "$x_libraries" != ""
then
  XLIBSW="-L$x_libraries"
else
  XLIBSW=""
fi



#
#   Locate motif headers and libraries.  (Xm.h and libXm.{so,dylib,a}).
#
motif_headerdir=""
motif_h_found="no"
AC_MSG_CHECKING([for Xm/Xm.h])

AC_ARG_WITH(motif-header-dir,
	[  --with-motif-header-dir=path Path to Xm header directory],
	[motif_headerdir="$withval"; motif_h_found="yes"])

# first try is for the headers in the same dir as the X headers:

if test $motif_h_found = no
then
	# Special case:  if it's in x_includes, no extra sws.
	
	mdir="$x_includes"
	if test "$mdir" = ""
        then
          mdir="/usr/include"
        fi
	AC_CHECK_FILE([${mdir}/Xm/Xm.h],
	               [motif_h_found="yes"],
	               [motif_h_found="no"])
fi

#  Next search:
#      /usr/include/X11
#      /usr/include/X11R6
#      /usr/X11/include
#      /usr/X11R6/include
#      /usr/include/Xm
#      /sw/include             (Darwin lesstif).
#      /usr/local/include      (default lesstif installation).
#
#
#  If in /usr/include we don't need anything:



if test $motif_h_found = no
then
   motif_header_dirs="/usr/include/X11 /usr/include/X11R6 /usr/X11/include /usr/X11R6/include /usr/include /sw/include /usr/local/include"
   for d in ${motif_header_dirs}
   do
	AC_CHECK_FILE([${d}/Xm/Xm.h],
	              [motif_h_found="yes"; \
	               motif_headerdir="$d"])
       if test $motif_h_found = yes
       then
	break
       fi
   done
fi

#
#  If motif_h_found and nonblank motif_headerdir prepend
#  with -I to make MOTIF_INCLUDES (blank means XINCLUDES has it
#  covered.  If not motif_h_found error and ask for help.
#
if test $motif_h_found = yes
then
   if test "$motif_headerdir" = ""
   then
	AC_MSG_RESULT([Xm/Xm.h is relative to $x_includes])
   else
        AC_MSG_RESULT([Xm/Xm.h is relative to $motif_headerdir])
	MOTIF_INCLUDES="-I$motif_headerdir"
   fi
else
   AC_MSG_ERROR([ Can't find Xm/Xm.h try using --with-motif-header-dir=path])
fi

#
#   Look for libXm.{so,dylib,a}
#   Our search order is:
#   --with-motif-lib=path    (user always can override).
#   $x_libraries             (Pretty  normal place e.g. openmotif).
#   /usr/lib                 (e.g. OSF1).
#   /sw/lib                  (Darwin lesstif installation).
#   /usr/local/lesstif/lib   (normal lesstif install wo links?).
#      Architecture specific place in linux.
#
#  In this case assume we need an -rpath unless the extension is .dylib in which case 
#  it's OS-X which doesn't require that sort of frippery.
#
libsysdir=""
if test "$host_os" == "linux-gnu"
then
   kernel_full_version="$(uname -r)"
   kernel_version="$(echo $kernel_full_version | cut -f1,2 -d.)"
   AC_MSG_RESULT([Building on $host_os kernel $kernel_version])  

   arch="$(uname -m)"
   case $arch in
       i[[3456]]86)
           libsysdir="/usr/lib/i386-linux-gnu"
           ;;
       x86_64)
           libsysdir="/usr/lib/x86_64-linux-gnu"
           ;;
       *)

           ;;
   esac
fi
   

libexts=".so .dylib .a"        # prefer shared libs to static.
motiflibdirs="/usr/X11/lib /usr/X11R6/lib  /sw/lib /usr/local/lesstif/lib /usr/lib $libsysdir"
motif_libdir=""
motif_lib_found="no"
Motif_Rpath=""

AC_MSG_CHECKING([for libXm])
AC_ARG_WITH(motif-lib,
	   [  --with-motif-lib=path Path to libXm],
	   [motif_libdir=""
	    motif_lib_found="yes"]
	   )

#  If necessary, try in $x_libraries:


for ext in $libexts
do
     if test "$x_libraries" != ""
     then
         AC_CHECK_FILE([$x_libraries/libXm${ext}],
	               [motif_lib_found="yes"])
      else
	AC_CHECK_FILE([/usr/lib/libXm${ext}],
	               [motif_lib_found="yes"])
      fi	       

     # If found break from the loop:

   if test $motif_lib_found = "yes"
   then
      break
   fi
done

#  If necessary  try in the list of good directories:
#  Note that .so's require -rpath.

if test $motif_lib_found = "no"
then
for lib in $motiflibdirs
do
  for ext in $libexts
  do
	AC_CHECK_FILE([$lib/libXm${ext}],
	             [motif_lib_found="yes"
	              motif_libdir="$lib"])
     if test $motif_lib_found = "yes"
     then
        if test $ext = ".so"
        then
           Motif_Rpath="-Wl,\"-rpath=$lib\""
        fi
        break
     fi
  done
done
fi

#  If found  set MOTIF_LIBSW accordingly.  Note this will be
#  empty if found in with the x libraries.
#

if test $motif_lib_found = "yes"
then
    if test "$motif_libdir" = ""
    then
	AC_MSG_RESULT([Found libXm in $x_libraries])
    else
        AC_MSG_RESULT([Found libXm in $motif_libdir])
	MOTIF_LIBSW="-L$motif_libdir $Motif_Rpath"
    fi
else
   AC_MSG_ERROR([Can't fine libXm give me a hand with --with-motif-lib])
fi


# Checks for include declarations


# These checks are for declarations within specific system headers. For
# instance, MTSETBLK is not declared in sys/mtio.h on all platforms, but
# is needed on Linux.

	# MTSETBLK in sys/mtio.h
AC_MSG_CHECKING([for MTSETBLK in sys/mtio.h])
    AC_TRY_COMPILE([
#include <sys/mtio.h>
#ifndef MTSETBLK
#  error mtsetblk not defined
#endif
], , my_cv_have_mtsetblk=yes, my_cv_have_mtsetblk=no)
    if test $my_cv_have_mtsetblk = yes ; then
	AC_DEFINE(HAVE_DECL_MTSETBLK, 1, [True if MTSETBLK defined in sys/mtio.h])
    fi
AC_MSG_RESULT($my_cv_have_mtsetblk)

	# MTOVERRUN in sys/mtio.h
AC_MSG_CHECKING([for MTOVERRUN in sys/mtio.h])
    AC_TRY_COMPILE([
#include <sys/mtio.h>
#ifndef MTOVERRUN
# error overrun not defined
#endif
], , my_cv_have_mtoverrun=yes, my_cv_have_mtoverrun=no)
    if test $my_cv_have_mtoverrun = yes ; then
	AC_DEFINE(HAVE_DECL_MTOVERRUN, 1, [True if MTOVERRUN defined in sys/mtio.h])
    fi
AC_MSG_RESULT($my_cv_have_mtoverrun)


	# MTCSE in sys/mtio.h
AC_MSG_CHECKING([for MTCSE in sys/mtio.h])
    AC_TRY_COMPILE([
#include <sys/mtio.h>
#ifndef MTCSE
# error mtcse not defined
#endif
], , my_cv_have_mtcse=yes, my_cv_have_mtcse=no)
    if test $my_cv_have_mtcse = yes ; then
	AC_DEFINE(HAVE_DECL_MTCSE, 1, [True if MTCSE defined in sys/mtio.h])
    fi
AC_MSG_RESULT($my_cv_have_mtcse)

	# PAGESIZE in limits.h
AC_MSG_CHECKING([for PAGESIZE in limits.h])
    AC_TRY_COMPILE([
#include <limits.h>
#ifndef PAGESIZE
# error pagesize not defined
#endif
], , my_cv_have_pagesize=yes, my_cv_have_pagesize=no)
    if test $my_cv_have_pagesize = yes ; then
	AC_DEFINE(HAVE_DECL_PAGESIZE, 1, [True if PAGESIZE is defined in limits.h as on HP-UX])
    fi
AC_MSG_RESULT($my_cv_have_pagesize)

	# PADSIZE in limits.h
AC_MSG_CHECKING([for PADSIZE in limits.h])
    AC_TRY_COMPILE([
#include <limits.h>
#ifndef PADSIZE
# error padsize not defined
#endif
], , my_cv_have_padsize=yes, my_cv_have_padsize=no)
    if test $my_cv_have_padsize = yes ; then
	AC_DEFINE(HAVE_DECL_PADSIZE, 1, [True if PADSIZE defined in limits.h])
    fi
AC_MSG_RESULT($my_cv_have_padsize)

	# INT_MAX in limits.h
AC_MSG_CHECKING([for INT_MAX in limits.h])
    AC_TRY_COMPILE([
#include <limits.h>
#ifndef INT_MAX
# error intmax not defined
#endif
], , my_cv_have_int_max=yes, my_cv_have_int_max=no)
    if test $my_cv_have_int_max = yes ; then
	AC_DEFINE(HAVE_DECL_INT_MAX, 1, [True if INT_MAX defined in limits.h])
    fi
AC_MSG_RESULT($my_cv_have_int_max)

	# MAXINT in values.h
AC_MSG_CHECKING([for MAXINT in values.h])
    AC_TRY_COMPILE([
#include <values.h>
#ifndef MAXINT
# error maxint not defined
#endif
], , my_cv_have_maxint=yes, my_cv_have_maxint=no)
    if test $my_cv_have_maxint = yes ; then
	AC_DEFINE(HAVE_DECL_MAXINT, 1, [True if MAXINT defined in values.h])
    fi
AC_MSG_RESULT($my_cv_have_maxint)

	# Check for 64 bit longs
AC_MSG_CHECKING([whether long int is 64 bits])
    AC_TRY_RUN([
int main(int argc, char **argv) {
	if(sizeof(long) == 8)
		return 0;
	else
		return 1;
}  
], [AC_DEFINE(HAVE_TRUE_SIXTY_FOUR, 1, [True if long int is 8 bits]) AC_MSG_RESULT(yes)], AC_MSG_RESULT(no), AC_MSG_RESULT(assuming not on target machine))

# Determine the particular platform we are running on. Specifically,
# we need to know if we are running on a SPARC processor, so that we
# can do the tape reading appropriately.

AC_MSG_CHECKING([for SPARC processor type])
case $host in
  *sparc*)
	AC_DEFINE(SPARC, 1, [Set to 1 if SPARC Processor])
	echo "yes"
	;;
  *) echo "no" ;;
esac

# Determine what OS we are running so we know whether or not to 
# declare some things extern "C". Also helpful for some Cygwin
# and Ultrix specific calls. 

LD_EXTRA_FLAGS=""
X11EXTRA_LDFLAGS=""
AC_MSG_CHECKING([for supported OS])
OS="$host_os"                          # Default guess.
case $host_os in
  *cygwin* ) 
	AC_DEFINE(CYGWIN, 1, [Set to 1 if Cygwin environment])
	AC_MSG_RESULT([Cygwin...  xdr.h broken for c++])

	# cygwin we have doesn't seem to get the object extension
	# quite right:

	OBJEXT="o"
	AC_SUBST(OBJEXT)
	AC_DEFINE(BROKEN_XDR_H, 1,
	         [defined if xdr.h unusuable for c++])


	# Windows doesn't allow undefined symbols at link time
	#
	LD_EXTRA_FLAGS="$LD_EXTRA_FLAGS -no-undefined -lrpc" 
	X11EXTRA_LDFLAGS="-lSM -lICE"
	;;
  *linux* )
	AC_DEFINE(LINUX, 1, [Set to 1 if Linux environment])
	AC_MSG_RESULT([Linux])
	;;
  *ultrix* )
	AC_DEFINE(ULTRIX, 1, [Set to 1 if Ultrix environment])
	AC_MSG_RESULT([Ultrix])
	;;
  *osf* )
	AC_DEFINE(OSF, 1, [Set to 1 if OSF environment])
	AC_MSG_RESULT([OSF1])
	WCHAR_T_DEF=-D_WCHAR_T
	OBJEXT=o

	# Missing on OSF1 for some reason
	AC_SUBST(OBJEXT)

	# This does away with "weak symbol" warnings
	LD_EXTRA_FLAGS="$LD_EXTRA_FLAGS -Wl,\"-S\""
	;;
  *darwin* )
	OBJEXT="o"
	AC_SUBST(OBJEXT)
	AC_DEFINE(DARWIN, 1, [Set to 1 if Darwin environment])
	AC_DEFINE(BROKEN_XDR_H, 1,
		  [defined if xdr.h is unusable for c++])
	AC_MSG_RESULT([Darwin .. xdr.h broken for c++])
	OS="darwin"              # It's crapped up with versions otherwise.
	;;
  * )
	AC_MSG_RESULT([ok])
	;;
esac

# Locate Tcl/Tk and setup compile/link switches for them too:

AX_TCL

#--------------------- sqlite3 tests - needed for dbconfig:

AX_LIB_SQLITE3			dnl The base library.


AC_MSG_CHECKING([For sqlite3 Tcl package])

sqlitefile=`tempfile`

echo package require sqlite3 >$sqlitefile



$TCLSH $sqlitefile 2>/dev/null

status=$?

if test  $status -ne 0 
then
	rm -f $sqlitefile
	AC_MSG_RESULT([no])
	AC_MSG_FAILURE([Missing sqlite3 TCL Package required to buid SpecTcl],1)
fi
rm -f $sqlitefile
AC_MSG_RESULT([yes])


# Look for image magick convert

# Try to find ImageMagick's convert program, used for converting postscript
# files created by Gri into JPEGs or PNGs. This is only useful if Gri is
# present as well.

AC_CHECK_PROG(have_convert, convert, "yes", "no")
if test $have_convert = "no" ; then
	AC_MSG_WARN(Cannot print to JPG or PNG files without ImageMagick convert.)
else
	AC_DEFINE(HAVE_CONVERT, 1, [True if ImageMagick's convert is present])
fi

#  libgd: We'll only hunt for the headers in /usr/include /usr/local/include
#         which I >think< will be automatically searched by the compiler.
#         libs we will also only look for in /usr/lib and /usr/local/lib
#         If not found we invite the user to use --with-gd-headerdir
#         and  --with-gd-libdir
#

AC_MSG_CHECKING([   libgd used to do wysiwyg printing in Xamine])

AC_ARG_WITH(gd-headerdir,
	   [   --with-gd-headerdir=path Path to the gd.h header],
	   [gdheaderdir=$withval],
	   [gdheaderdir=""])
AC_ARG_WITH(gd-libdir,
	   [   --with-gd-libdir=path Path to libgd],
	   [gdlibdir=$withval],
	   [gdlibdri=""])


# If user supplied a directory for gd.h, keep them honest:

if test "$gdheaderdir" != ""
then
   AC_MSG_RESULT([ User provided --with-gd-headerdir=$gdheaderdir])
   if test -e $gdheaderdir/gd.h
   then
      GDINCLUDES="-I$gdheaderdir"
   else
      AC_MSG_ERROR([ User provided --with-gd-headerdir=$gdheaderdir does not contain gd.h])
   fi
else
#   Need to hunt in 'normal places'.
   
   AC_CHECK_HEADER(gd.h, [gdheaderdir="found in -I path"])   

    if test "$gdheaderdir" != ""
    then
       AC_MSG_RESULT([gd.h $gdheaderdir])
       GDINCLUDES=""
    else
       AC_MSG_ERROR([Unable to locate gd.h, try --with-gd-headerdir to help me out])
    fi
fi

# if user supplied a directory for libgd keep them honest.  We'll look for the
# static lib libgd.a  - since .so is not the only shared object header supported.
#

if test "$gdlibdir" != ""
then
   AC_MSG_RESULT([ User providied --with-gd-libdir=$gdlibdir])
   if test -e $gdlibdir/libgd.a
   then
	GDLDSW="-L$gdlibdir -lgd"
        
        # if there's an .so then we'll ned an -rpath:

        if test -e $gdlibdir/libgd.so
        then
	   GDLDSW="${GDLDSW} -Wl,\"-rpath=${gdlibdir}\""
        fi
   else
	AC_MSG_ERROR([ User provided --with-gd-libdir=$gdlibdir does not contain libgd.a])
   fi
else
#  Need to hunt in the normal places.

   AC_CHECK_LIB(gd, gdImageCreate, [gdlibdir="found in -L path"])
   if test "$gdlibdir" != ""
   then
      AC_MSG_RESULT([libgd $gdlibdir])
      GDLDSW="-lgd"
   else
      AC_MSG_ERROR([Unable to locate libgd try --with-gd-libdir to help me out])
   fi
fi


# Look for Gri to allow production quality printing

# If Gri is present, then printing will be enabled. Also required is the
# file gri.cmd, which tell's gri how to behave on command files. If Gri
# is not present, printing will simply be disabled.

# regardless, in order to build the distro, we need to configure
# our version of gri.


AC_ARG_WITH(gri-dir,
	[  --with-gri-dir=path Path to Gri binary],
	[gri_dir=$withval ; gri_found="yes"], 
	[gri_found="no"])


if test $gri_found = "no" ; then
   AC_MSG_CHECKING([for gri...])
fi

  # /usr/bin
if test $gri_found = "no" ; then
   AC_CHECK_FILE([/usr/bin/gri],
	[gri_dir="/usr/bin";
	gri_found="yes"])
fi

  # /usr/local/bin
if test $gri_found = "no" ; then
   AC_CHECK_FILE([/usr/local/bin/gri],
	[gri_dir="/usr/local/bin";
	gri_found="yes"])
fi

  # I give up at this point, if necessary the Gri packaged with SpecTcl is
  # configured here and built.
if test $gri_found = "yes" ; then
   AC_MSG_RESULT([yes])
   mygri="no"
   GRI=""
else
   AC_MSG_WARN([Can't find installed gri. If it's installed use --with-gri-dir to tell me where it is
Without gri, you will not be able to generate printed/postscript spectra])
fi



if test $gri_found = "yes" 
then
	AC_DEFINE(HAVE_GRI, 1, [True if Gri is available])
fi



AM_PATH_CPPUNIT
AM_CONDITIONAL([CPPUNIT], [test -n "${CPPUNIT_LIBS}"] )
AM_CONDITIONAL([CPPUNITDONOTBUILD], [test -n ""])
AM_CONDITIONAL([THIS_IS_NOT_DEFINED], [test -n ""])

# We use mandb to index the man pages if it exists... not sure what the 
# alternatives are on other systems so transform mandb -> echo if
# we can't find it:

AC_CHECK_PROG(MANDB, mandb, mandb, echo)

#
#   We'd like to compile with -pedantic if supported
#   Intel looks like gcc so we check explicitly for a gcc
#   named compiler
#

AC_MSG_CHECKING([gcc so use -pedantic?])

if `echo $CC | grep -q gcc` 
then 
     CFLAGS="$CFLAGS -pedantic -Wno-long-long"
     CXXFLAGS="$CXXFLAGS -pedantic -Wno-long-long"
     AC_MSG_RESULT(yes)
else  
     AC_MSG_RESULT(no)
fi

##
#  There's a utility that uses gengetopt:

AX_GENGETOPT

##
# Check for zlib for the zip/unzip commands.

AX_CHECK_ZLIB([AC_MSG_RESULT([Have zlib])], [AC_MSG_ERROR([zlib is a prerequisite for this version of SpecTcl])])

# Check that the unified format library has been incorporated into
# the source tree and configura/build it into $prefix/unifiedformat
#
AC_CHECK_PROG([havecmake], cmake, [1], [0])
if `test ${havcmake} = 0`
then
	AC_MSG_ERROR([cmake must be in the path to build SpecTcl])
fi

AC_CHECK_FILE([${srcdir}/unifiedformat/CMakeLists.txt], [
   rm -rf unifiedformat/build
   mkdir -p unifiedformat/build
   sourcedir="`realpath ${srcdir}`"
   (cd unifiedformat/build;  cmake ${sourcedir}/unifiedformat -DCMAKE_INSTALL_PREFIX=${prefix}/unifiedformat; make all test install)
   UFMT_CPPFLAGS="-I${prefix}/unifiedformat/include"
   UFMT_LDFLAGS="-L${prefix}/unifiedformat/lib -lNSCLDAQFormat -lV10Format -lV11Format -lV12Format -lAbstractFormat -Wl,-rpath=${prefix}/unifiedformat/lib"
   AC_SUBST(UFMT_CPPFLAGS)
   AC_SUBST(UFMT_LDFLAGS)
   ],
   [AC_MSG_ERROR([You need to incorporate the unified format library with unifiedfmt-incorp.sh])
   ])


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

AC_MSG_CHECKING([libtcl++ installed as package (e.g. .deb)])

AC_CHECK_FILE([/usr/include/TCLInterpreterObject.h], [systemtcl="yes"], [systemtcl="no"])

if test $systemtcl = "yes"
then
	LIBTCLPLUS_CFLAGS=""
	LIBEXCEPTION_LDFLAGS="-lException"
	LIBTCLPLUS_LDFLAGS="-ltclPlus ${LIBEXCEPTION_LDFLAGS}"
	AC_MSG_RESULT([Found in  system libraries])
else
  AC_CHECK_FILE([${prefix}/lib/libException.so],[libtclinstalled="yes"],[libtclinstalled="no"])
  AC_CHECK_FILE([${prefix}/lib/libtclPlus.so],[libexceptioninstalled="yes"],[libexceptioninstalled="no"])
  AC_CHECK_FILE([${prefix}/include/TCLInterpreterObject.h],[tclinterpobjinstalled="yes"],[tclinterpobjinstalled="no"])
  if [ [ $libtclinstalled = "no" ] || [ $libexceptioninstalled = "no" ] || [ $tclinterpobjinstalled = "no" ] ]
  then
    AC_CHECK_FILE([${srcdir}/libtcl], [havelibtcldir="yes"], [havelibtcldir="no"])
	  if test $havelibtcldir = "yes"
  	then
      AC_MSG_RESULT([Need to build local tagged copy])
      AC_CHECK_FILE([${srcdir}/libtcl/configure], [havelibtcldir="yes"], [havelibtcldir="no"])
		  (cd ${srcdir}/libtcl; ./configure --prefix=${prefix}; make clean install)
	  else
		  AC_MSG_ERROR([I seem to be missing libtcl, please use tcl++incorp.sh to fill it with a tagged release of libtclplus])
	  fi
  else
    AC_MSG_RESULT([Previously built version found in ${prefix}])
  fi
  LIBTCLPLUS_CFLAGS="-I${prefix}/include"
  LIBEXCEPTION_LDFLAGS="-L${prefix}/lib -lException -Wl,\"-rpath=${prefix}/lib\""
	LIBTCLPLUS_LDFLAGS="-L${prefix}/lib -ltclPlus ${LIBEXCEPTION_LDFLAGS}"

fi
AC_SUBST(LIBTCLPLUS_CFLAGS)
AC_SUBST(LIBTCLPLUS_LDFLAGS)
AC_SUBST(LIBEXCEPTION_LDFLAGS)

######################################## Checks for ZMQ
AX_ZMQ

######################################## Checks for Root 

AC_CHECK_PROG(ROOTCONFIGDFLT, [root-config], [root-config], [], [$PATH])


# override of default version
AC_ARG_WITH([rootsys],
	AS_HELP_STRING([--with-rootsys], [Path to ROOT install directory (a.k.a. ROOTSYS)]))

AC_PATH_PROG([ROOTCONFIG], [root-config], [notfound], [$with_rootsys/bin:$PATH])

if test "x$ROOTCONFIG" = "xnotfound" ; then 
  ROOTCONFIG=$ROOTCONFIGDFLT
  AC_MSG_NOTICE([Setting to $ROOTCONFIG])
fi

if test -z "$ROOTCONFIG" ; then
  AC_MSG_ERROR([Unable to locate root-config that is is required.])
else 
  AC_MSG_NOTICE([Using root-config ... $ROOTCONFIG])
fi

ROOT_CFLAGS=`$ROOTCONFIG --libs --cflags`
ROOT_LDFLAGS="`$ROOTCONFIG --glibs`  -Wl,-rpath=`$ROOTCONFIG --libdir` "
ROOT_LIBRARY_DIR=`$ROOTCONFIG --libdir`
AC_SUBST(ROOT_CFLAGS)
AC_SUBST(ROOT_LDFLAGS)
AC_SUBST(ROOT_LIBRARY_DIR)
AC_SUBST(ROOTCONFIG)

######################################  Checks for jsoncpp


PKG_CHECK_MODULES(JSONCPP, [jsoncpp], [], [AC_MSG_ERROR([jsoncpp package must be installed])])
AC_MSG_RESULT([$JSONCPP])

AC_SUBST(JSONCPP_CFLAGS)
AC_SUBST(JSONCPP_LIBS)

###################################### Checks for restclient-cpp
# Note there's no debian package for that.
# FRIB installs it in /usr/opt/restclient which shoulid have:
#  .../lib/librestclient-cpp.so
#  ../include/restclient-cpp/restclient.h
#     configure can overridet this location and will prompt for that
#     if not found using --with-librestclient=topdir.
# The package is on github at: https://github.com/mrtazz/restclient-cpp


AC_MSG_CHECKING([Checking for librestclient-cpp])
AC_MSG_NOTICE([If you need to install it it can be gotten at https://github.com/mrtazz/restclient-cpp])

AC_ARG_WITH([restclient-cpp],
	[AS_HELP_STRING([--with-restclient-cpp=topdir],[ Provide top installation directory of librestclient-cpp])],
	[restclient_topdir=$withval],[restclient_topdir="/usr/opt/restclient"])

#  Make sure we can find this beast:

AC_CHECK_FILE([${restclient_topdir}/lib/librestclient-cpp.so], [libfound="yes"], [libfound="no"])
AC_CHECK_FILE([${restclient_topdir}/include/restclient-cpp/restclient.h], [hdrfound="yes"], [hdrfound="no"])

if test "${libfound}" = "no"
then
   AC_MSG_FAILURE([Unable to find ${restclient_topdir}/lib/librestclient-cpp.so.  Use correct --with-restclient-cpp value.  If necessary, download/build from https://github.com/mrtazz/restclient-cpp])
fi

if test "${hdrfound}" = "no"
then
   AC_MSG_FAILURE([Unable to find ${restclient_topdir}/include/restclient-cpp/restclient.h.  Use correct --with-restclient-cpp value. If necessary, download/build from https://github.com/mrtazz/restclient-cpp])
fi

RESTCLIENT_LDFLAGS="-L${restclient_topdir}/lib -lrestclient-cpp -Wl,-rpath=${restclient_topdir}/lib"
RESTCLIENT_CPPFLAGS="-I${restclient_topdir}/include"

AC_SUBST(RESTCLIENT_LDFLAGS)
AC_SUBST(RESTCLIENT_CPPFLAGS)


# Check for 

# mandb:

AC_SUBST(MANDB)

# X flags

AC_SUBST(X11EXTRA_LDFLAGS)

AC_SUBST(XINCLUDES)
AC_SUBST(XLIBSW)
AC_SUBST(CFLAGS)

# Motif flags
AC_SUBST(MOTIF_INCLUDES)
AC_SUBST(MOTIF_LIBSW)
AC_SUBST(XTLIBSW)

# Gri directories
AC_SUBST(gri_dir)
AC_SUBST(MYGRIDIR)

# Tru64 specific
AC_SUBST(WCHAR_T_DEF)
AC_SUBST(LD_EXTRA_FLAGS)

AC_SUBST(ac_aux_dir)

# LIBOBJS and LTLILBOBJS set
AC_SUBST(LTLIBOBJS)

# Operating system designator:

AC_SUBST(OS)

#   .so version information.

AC_SUBST(SOVERSION)

# docbook tools:

AC_SUBST(HCDOCBOOK)
AC_SUBST(HTMLDOCBOOK)
AC_SUBST(DVIPDF)
AC_SUBST(MANDOCBOOK)

# Xamine uses libgd for wysiwyg printing:

  AC_SUBST(GDINCLUDES)
  AC_SUBST(GDLDSW)


# create output files.
# Removed           Tape/Makefile

AC_OUTPUT(Makefile 
          calibratedparams/Makefile 
          contrib/Makefile
          contrib/scriptable/Makefile
          contrib/Grace/Makefile
          contrib/multiread/Makefile
          contrib/treeparam/Makefile
          compress/Makefile
	  RootInterface/Makefile
          Core/Makefile 
          ccusb/Makefile
          ccusb/MakefileSkel
          XamineParsers/Makefile 
          Display/Makefile 
          docs/Makefile
          docs/webpage/Makefile
          docs/webpage/Commands/Makefile
          docs/webpage/Commands/_vti_cnf/Makefile
          docs/webpage/Programming/Makefile
          docs/diagrams/Makefile
          docs/docbook/Makefile
          docs/man/Makefile
          factories/Makefile
          filtsplit/Makefile
          Gui/Makefile
          Replace/Makefile
	  scalerWrapper/Makefile
          Scripts/Makefile 
          SpecTcl/Makefile
          splash/Makefile
          tkcon/Makefile
          TestFiles/Makefile
          treegui/Makefile
          TreeParam/Makefile 
          Utility/Makefile
          Utility/daqdefs.h 
          Utility/daqdefs.tcl 
          vmusb/Makefile
          vmusb/MakefileSkel 
          Python/Makefile
	  ddas/Makefile
	  ddas/src/Makefile
	  ddas/src/Makefile_user
	  packaged/Makefile
	  packaged/Makefile_skel
	  db/Makefile
	  PyQtGUI/Makefile
	  PyQtGUI/src/Makefile
	  PyQtGUI/sip/Makefile
	  PyQtGUI/gui/Makefile
	  PyQtGUI/test/Makefile
	  PyQtGUI/standalone/Makefile
	  restclient/Makefile
	  xaminerunner/Makefile
	  mirrorclient/Makefile
	  rootxamine/Makefile
	  )
          
