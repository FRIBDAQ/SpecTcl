AC_DEFUN([AX_TCL], [
dnl shamelessly stolen from http://www.opensource.apple.com/source/passwordserver_sasl-164/cyrus_sasl/cmulocal/tcl.m4
# --- BEGIN CMU_TCL ---
dnl To link against Tcl, configure does several things to make my life
dnl "easier".
dnl
dnl * maybe ask the user where they think Tcl lives, and try to find it
dnl * maybe ask the user what "tclsh" is called this week (i.e., "tclsh8.0")
dnl * run tclsh, ask it for a path, then run that path through sed
dnl * sanity check its result (many installs are a little broken)
dnl * try to figure out where Tcl is based on this result
dnl * try to guess where the Tcl include files are
dnl
dnl Notes from previous incarnations:
dnl > XXX MUST CHECK FOR TCL BEFORE KERBEROS V4 XXX
dnl > This is because some genius at MIT named one of the Kerberos v4
dnl > library functions log().  This of course conflicts with the
dnl > logarithm function in the standard math library, used by Tcl.
dnl
dnl > Checking for Tcl first puts -lm before -lkrb on the library list.
dnl

dnl Check for some information from the user on what the world looks like

dnl TCL locational arguments:


AC_ARG_WITH(tclconfig,[  --with-tclconfig=PATH   use tclConfig.sh from PATH
                          (configure gets Tcl configuration from here)],
        dnl trim tclConfig.sh off the end so we can add it back on later.
	TclLibBase=`echo ${withval} | sed s/tclConfig.sh\$//`)
AC_ARG_WITH(tcl,      [  --with-tcl=PATH         use Tcl from PATH],
	TclLibBase="${withval}/lib")
AC_ARG_WITH(tclsh,    [  --with-tclsh=TCLSH      use TCLSH as the tclsh program
                          (let configure find Tcl using this program)],
	TCLSH="${withval}")

dnl Tk/wish locational arguments:

AC_ARG_WITH(tkconfig, [  --with-tkconfig=PATH use tkConfig.sh from PATH
		      	   (configure gets Tk configuration from here)],
            TkLibBase=`echo ${withval} | sed s/tkConfig.sh\$//`)

AC_ARG_WITH(tk        [ --with-tk=PATH       USe Tk from PATH],
            TkLibBase="${withval}/lib")

AC_ARG_WITH(wish,     [--with-wish=WISH   use WISH as the wish program
                         (let cofigure find wish using this program)],
            WISH="${withval}")     
               



if test "$TCLSH" = "no" -o "$with_tclconfig" = "no" ; then
  AC_MSG_WARN([Tcl disabled because tclsh or tclconfig specified as "no"])
  with_tcl=no
fi

if test "$with_tcl" != "no"; then
  if test \! -z "$with_tclconfig" -a \! -d "$with_tclconfig" ; then
    AC_MSG_ERROR([--with-tclconfig requires a directory argument.])
  fi

  if test \! -z "$TCLSH" -a \! -x "$TCLSH" ; then
    AC_MSG_ERROR([--with-tclsh must specify an executable file.])
  fi

  if test -z "$TclLibBase"; then # do we already know?
    # No? Run tclsh and ask it where it lives.

    # Do we know where a tclsh lives?
    if test -z "$TCLSH"; then
      # Try and find tclsh.  Any tclsh.
      # If a new version of tcl comes out and unfortunately adds another
      # filename, it should be safe to add it (to the front of the line --
      # somef vendors have older, badly installed tclshs that we want to avoid
      # if we can)
      AC_PATH_PROGS(TCLSH, [tclsh8.6 tclsh8.5 tclsh8.4 tclsh8.3 tclsh8.2 tclsh8.1 tclsh8.0 tclsh], "unknown")
    fi

    # Do we know where to get a tclsh?
    if test "${TCLSH}" != "unknown"; then
      AC_MSG_CHECKING([where Tcl says it lives])
      #
      # See if Tcl can tell us where it is:
      #
      tclClaims=`echo puts \\\$tcl_library | ${TCLSH} | sed -e 's,[^/]*$,,'`
      AC_MSG_CHECKING([$tclClaims])
      if test -f $tclClaims/tclConfig.sh; then
        TclLibBase=${tclClaims}
      else
          #If not try /usr/lib/tcl$version where some (debian e.g.) put it:
	  
	  version=`echo puts \\\$tcl_version | ${TCLSH}`
          libloc=/usr/lib/tcl${version}
          AC_MSG_CHECKING($libloc)
          if test -f $libloc/tclConfig.sh; then
             TclLibBase=$libloc
          else 
             AC_MSG_ERROR([Can't find tclConfig.sh you'll need to use --with-tclconfig to tell me where it is])
          fi
      fi
      AC_MSG_RESULT($TclLibBase)
    fi
  fi

  if test -z "$TclLibBase" ; then
    AC_MSG_RESULT([can't find tclsh])
    AC_MSG_WARN([can't find Tcl installtion; use of Tcl disabled.])
    with_tcl=no
  else
    AC_MSG_CHECKING([for tclConfig.sh])
    # Check a list of places where the tclConfig.sh file might be.
    for tcldir in "${TclLibBase}" \
                  "${TclLibBase}/.." \
		  "${TclLibBase}"`echo ${TCLSH} | sed s/sh//` ; do
      if test -f "${tcldir}/tclConfig.sh"; then
        TclLibBase="${tcldir}"
        break
      fi
    done

    if test -z "${TclLibBase}" ; then
      AC_MSG_RESULT("unknown")
      AC_MSG_WARN([can't find Tcl configuration; use of Tcl disabled.])
      with_tcl=no
    else
      AC_MSG_RESULT(${TclLibBase}/)
    fi

    if test "${with_tcl}" != no ; then
      AC_MSG_CHECKING([Tcl configuration on what Tcl needs to compile])
      . ${TclLibBase}/tclConfig.sh
      AC_MSG_RESULT(ok)
      dnl       . ${TclLibBase}/tkConfig.sh
    fi

    if test "${with_tcl}" != no ; then
      dnl Now, hunt for the Tcl include files, since we don't strictly
      dnl know where they are; some folks put them (properly) in the 
      dnl default include path, or maybe in /usr/local; the *BSD folks
      dnl put them in other places.
      AC_MSG_CHECKING([where Tcl includes are])
      for tclinclude in "${TCL_PREFIX}/include/tcl${TCL_VERSION}" \
                        "${TCL_PREFIX}/include/tcl" \
                        "${TCL_PREFIX}/include" ; do
        if test -r "${tclinclude}/tcl.h" ; then
          TCL_CPPFLAGS="-I${tclinclude}"
          break
        fi
      done
      if test -z "${TCL_CPPFLAGS}" ; then
        AC_MSG_WARN(can't find Tcl includes; use of Tcl disabled.)
        with_tcl=no
      fi
      AC_MSG_RESULT(${TCL_CPPFLAGS})
    fi
    
    # Finally, pick up the Tcl configuration if we haven't found an
    # excuse not to.
    if test "${with_tcl}" != no; then
      TCL_LIBS="${TCL_LD_SEARCH_FLAGS} ${TCL_LIB_SPEC} ${TCL_LIBS}"
    fi
  fi
fi

AC_SUBST(TCL_DEFS)
AC_SUBST(TCL_LIBS)
AC_SUBST(TCL_CPPFLAGS)
AC_SUBST(TCLSH)

#
#   TK/Wish  If WISH is not define find it:
#

if test -z ${WISH}; then
   AC_MSG_CHECKING([wish interpreter])
   AC_PATH_PROGS(WISH, [wish8.6 wish8.5 wish8.4 wish8.3 wish8.2 wish8.1 wish8.0 wish], "unknown")   
   if test "${WISH}" = "unknown"; then
      AC_MSG_ERROR([Unable to find wish, help me out with --with-wish])
   else
      AC_MSG_RESULT([${WISH}])
   fi
fi

version=`echo puts \\\$tcl_version | ${TCLSH}`

# We can't use wish to find tkConfig.sh since it requires an X11 server to run.
# we're going to assume that it's either ../tk or ../tk${version} relative to
# TclLibBase...unless of course we've been handed it by the --with switches.

if test -z ${TkLibBase}; then   
   AC_MSG_CHECKING([tkConfig.sh])
   guesses="${TclLibBase}/../tk${version} ${TclLibBase}/../tk"
   for guess  in $guesses; do
       if test -r ${guess}/tkConfig.sh; then
       	  TkLibBase=${guess}
	  AC_MSG_RESULT([${TkLibBase}])
	  break
       fi
   done
fi

# Still not defined is an error:

if test -z ${TkLibBase}; then
   AC_MSG_ERROR([Can't find tkConfig.sh, use one of the -with-tk flags to help me out])
fi


# Source the config file

. ${TkLibBase}/tkConfig.sh

# Figure out and set the syms:

TK_LIBS="${TK_LD_SEARCH_FLAGS} ${TK_LIB_SPEC} ${TK_LIBS}"
TK_CPPFLAGS="${TK_INCLUDE_SPEC}"

AC_SUBST(WISH)
AC_SUBST(TK_DEFS)
AC_SUBST(TK_LIBS)
AC_SUBST(TK_CPPFLAGS)

# --- END CMU_TCL ---
]) dnl CMU_TCL
