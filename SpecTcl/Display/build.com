$!
$!  Command file to build the VMS version of Xamine.
$!  This file works regardless of the architecture (VAX or AXP) and
$!  works regardless of the architecture of the last build.
$!  We depend on MMS to do the detailed build.
$! Author:
$!    Ron Fox
$!    NSCL
$!    Michigan State University
$!    East Lansing, MI 48824-1321
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$
$!   The file Last.build, if it exists, contains the name of the
$!   last architecture under which the build was performed.  If it doesn't
$!   exist, then we assume it was built on the 'wrong' one.
$
$ LastBuildArchitecture    = "UNKNOWN"
$ CurrentBuildArchitecture = F$EDIT(F$GETSYI("ARCH_NAME"), "UPCASE")
$
$ IF F$SEARCH("LAST.BUILD") .NES. ""		! This file contains the 
$ THEN						! ARCH_TYPE Of last build.
$    OPEN/READ arch LAST.BUILD
$    READ/END=IgnoreEOF arch LastBuildArchitecture
$    IgnoreEOF:
$    CLOSE arch
$ ENDIF
$
$!    If the current build architecture is different than the last one,
$!    Then we must get rid of *.obj, *.exe so as not to make MMS believe
$!    the wrong architecture's objects are current, and also we need to
$!    Copy the architecture specific files to appropriate places.
$!    and update the contents of the LAST.BUILD file.
$!
$ IF LastBuildArchitecture .NES. CurrentBuildArchitecture
$ THEN
$   WRITE SYS$OUTPUT "Deleting file from a previous build on wrong arch..."
$   DELETE *.obj;*,*.exe;*
$   WRITE SYS$OUTPUT "Copying architecture dependent files into place:
$   IF CurrentBuildArchitecture .EQS. "ALPHA"
$   THEN
$     COPY/LOG XAMINESHARE_AXPVMS.FOR XAMINESHARE.FOR
$     COPY/LOG ALPHA.MMS              DESCRIP.MMS
$     COPY/LOG XWINDOWS_ALPHA.OPT     XWINDOWS.OPT
$     COPY/LOG XAMINE_ALPHA.OPT       XAMINE.OPT
$   ELSE
$     COPY/LOG XAMINESHARE_VAXVMS.FOR XAMINESHARE.FOR
$     COPY/LOG VAX.MMS                DESCRIP.MMS
$     COPY/LOG XWINDOWS_VAX.OPT       XWINDOWS.OPT
$     COPY/LOG XAMINE_VAX.OPT	      XAMINE.OPT
$   ENDIF
$   WRITE SYS$OUTPUT "Updating prior build architecture file..."
$   OPEN/WRITE arch LAST.BUILD
$   WRITE/SYMBOL arch CurrentBuildArchitecture
$   CLOSE arch
$ ENDIF
$
$!     Now do the build.... the MMS file is also assumed to be 
$!     copied to DESCRIP.MMS
$
$ WRITE SYS$OUTPUT "Beginning build for ''CurrentBuildArchitecture'"
$ DEFINE/USER XAMINE_DIR SYS$DISK:[]
$ MMS
