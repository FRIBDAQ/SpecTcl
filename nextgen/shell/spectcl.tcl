package provide spectclsh 1.0

package require SpecTcl

#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2009.
#
#    You may use this software under the terms of the GNU public license
#    (GPL).  The terms of this license are described at:
#
#     http://www.gnu.org/licenses/gpl.txt
#
#    Author:
#             Ron Fox
#	     NSCL
#	     Michigan State University
#	     East Lansing, MI 48824-1321


#-----------------------------------------------------------------
#  This software represents the SpecTcl shell.
#  It provides a command interface to SpecTcl.
#  It can be used in two separate ways:
#  spectcl command
#  spectcl 
#
#   The first case executes the command and exits.
#   The second case prompts for commands and continues to 
#   execute them until eof (control-D).
# 
# See the software documentation for information about the supported commnds.
# They are generally (but not quite) the commands provided by the Tcl bindings.
#

#  If this is not supplied the following are used to located the experiment
#  database:
#   SPECTCL_EXPERIMENT - an environment variable pointing to the spectcl experiment
#                        database.
#   ~/.spectcl - A text file which is interpreted as a script that may have a
#                line like 'set experiment path'
#   ./.spectcl - A text file which is interpreted as a script that may also have
#                'set experiment path'
#
#  Precedence is in this order (high to low)
#   ./.spectcl, ~/.spectcl, SPECTCL_EXPERIMENT
#
#
package provide spectclsh 1.0

package require SpecTcl
package require report
package require struct::matrix

#  Report styles:
catch {
::report::defstyle simpletable {} {
    data set [split "[string repeat "| "   [columns]]|"]
    top set  [split "[string repeat "+ - " [columns]]+"]
    bottom set [top get]
    top    enable 
    bottom enable
}

::report::defstyle captionedtable {{n 1}} {
    simpletable
    topdata    set [data get]
    topcapsep  set [top get]
    topcapsep  enable
    tcaption   $n
}
}


set here [file dirname [info script]]
set libs [file normalize [file join $here ../lib]]
lappend auto_path $libs

package require SpecTcl
global  exphandle;	# Handle open on the SpecTcl database.

#------------------------------------------------------------------------------
#
#  opens the epxeriment database
#
#   argv after the command parameters have been consumed
# Errors:
#   if unable to determine the path 
#   or the final database  path fails to open as a database handle.
#   Note that if the database does not exist but is a valid writable path,
#   it is created.
#
# Side-effects
#   global variable 'handle' is set with the database handle if successful.
#
proc openExperiment arglist {
    # Get the experiment database name:
    #
    # From the environment?
    #
    if {[array names ::env SPECTCL_EXPERIMENT] ne ""} {
	set path $::env(SPECTCL_EXPERIMENT)
    }
    # From the .rc file
    set globalrc [file join ~/.spectcl]
    if {[file exists $globalrc]} {
	source $globalrc
    }
    if {[file exists .spectcl]} {
	source .spectcl
    }
    if {[info exists experiment]} {
	set path $experiment
    }

   #  If there's no path that's  an error:

    if {![info exists path]} {
	error "No experiment database path supplied in environment, .spectcl's or on command line"
    }
 
    # Open or create:

    if {[file exists $path]} {
	set ::exphandle [::spectcl::expopen $path]
    }  else {
	set ::exphandle [::spectcl::expcreate $path]
    }
    return $arglist
}
#
#  Return a list of the unqualified SpecTcl command names...these are the valid
#  commands you can pass to experiment-command
#
proc unqualifiedCommands {} {
    set qualifiedCommands [info command ::spectcl::*]
    set unqualified [list]
    foreach q $qualifiedCommands {
	set lastsep [string last :: $q]
	incr lastsep 2
	lappend unqualified [string range $q $lastsep end]
    }
    return $unqualified
}
#-------------------------------------------------------------------------------
#  Simple wrappers for Spectcl commands..which hide the need for a handle:
#

#
#  Proc to define a command that is a simple shell of a ::spectcl:: 
#  command.
#
proc experiment-command {name} {
    if {[info command ::spectcl::$name] eq ""} {
	set cmds [unqualifiedCommands]
	error "$name is not a valid spectcl command use one of '$cmds'"
    }
    proc $name args {
	set command [info level 0]
	set command [lindex $command 0]
	set subcommand [lindex $args 0]
	set tail       [lrange $args 1 end]
	::spectcl::$command $subcommand $::exphandle {*}$tail
    }
}


experiment-command expuuid
experiment-command run
experiment-command parameter

#----------------------------------------------------------------------------
#
#  Human readable listers.
#

# 
#  Report the set of parameters in nice human readable form.
#
# Parameters:
#  pattern - defaults to * defines the set of parameters to match:
#
proc plist {{pattern *}} {
    set data [parameter list $pattern]

    ::struct::matrix reportData
    reportData add columns 4
    reportData insert row 0 [list Name Units {Low Limit} {High Limit}]

    foreach item $data {
	set row [list]
	foreach datum $item {
	    lappend row $datum
	}
	reportData insert row  end $row
    }

    #  Generate the report:

    ::report::report r 4 style captionedtable 1
    set result [r printmatrix reportData]

    reportData destroy
    r destroy

    return $result
}

#
#  Report on the runs in human readable form:
#
proc rlist {} {
    set data [run list]
    ::struct::matrix reportData
    reportData add columns 4
    reportData insert row 0 [list Number Title {Start Time} {End Time}]

    foreach item $data {
	set row [lindex $item 0]
	set rundata [lindex $item 1]
	foreach datum $rundata {
	    lappend row $datum
	}
	reportData insert row end $row
    }
    #  Generate the report:

    ::report::report r 4 style captionedtable 1
    set result [r printmatrix reportData]

    reportData destroy
    r destroy

    return $result
}    

#------------------------------------------------------------------------------
#
#  shell entry point.

set command [openExperiment $argv]

set tcl_prompt1 {
puts -nonewline "spectcl> "
flush stdout
}

set tcl_prompt2 {
puts -nonewline "spectcl>... "
flush stdout
}

puts -nonewline ""

