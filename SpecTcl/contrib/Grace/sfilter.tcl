#!/usr/bin/tclsh
#
#   Tcl command file that takes stdin as an NSCL 1d spectrum file
#   and converts it into a space separated channel value bunch o pairs
#   on stdout.
#
#   $Log$
#   Revision 5.1  2004/11/29 16:56:21  ron-fox
#   Begin port to 3.x compilers calling this 3.0
#
#   Revision 1.1  2003/09/03 17:35:05  ron-fox
#   Added new contributed item:  $SpecTclHome/contrib/grace.tcl exports 1-d spectra to xmgrace Makefile.am updated to install this.
#   grace.tcl - script to setup the interface:
#       pack [source $SpecTclHome/contrib/Grace/grace.tcl]
#       click on the Export Grace... button, choose a spectrum.
#   sfilter.tcl - Script to filter the output of swrite to grace readable format.
#

#  First kill off lines until the '---' line.

    set line "   "
    while {[string index $line 0] != "-"} {
	set line [gets stdin]
    }
    
    #  Now each line consists of (channel)  value
    #  until we see (-1)  -1.
    #
    while {1} {
	set line [gets stdin]
	scan $line "(%d) %d" channel value
	if {$channel == -1} break
	puts "$channel $value"
    }
    puts ""
