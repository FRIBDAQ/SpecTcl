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

set unEncapsulableCommands [list expcreate expclose expopen]

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
package require csv
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
	set uq [string range $q $lastsep end]
	if {[lsearch $::unEncapsulableCommands $uq] == -1} {
	    lappend unqualified $uq
	}
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
    if {([info command ::spectcl::$name] eq "")    ||
	([lsearch $::unEncapsulableCommands $name] != -1)} {
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
#-----------------------------------------------------------------------------
# Event database manipulations:
#

#----------------------------------------------------------------------------
# 

#  The commands below define event database commands that have, as their first
#  parameter an experiment databas..and are directly exposed to the user:


experiment-command evtopen 
experiment-command evtclose
experiment-command evtruninfo




#
#  Create an event database.
# Parameters:
#    run  - Run  number that must already be defined in the experiment database.
#    name - Name of the database to be created.
#
proc evtcreate {run name} {
    ::spectcl::evtcreate $::exphandle $run $name
}

#
#  Attach an event database to the current experiment.
#  Parameters:
#    db      - Name of the database to attach
#    ?where> - Attachment point (if omitted the default point will be used.
# 
proc evtattach {db {where {}}} {
    if {$where eq ""} {
	::spectcl::attach $::exphandle $db
    } else {
	::spectcl::attach $::exphandle $db $where
    }
}
#
#  Detaches a run database.
# Parameters:
#    ?where?  - If supplied this is the attachment point; otherwise,
#               the default one is used.
#
proc evtdetach {{where {}}} {
    if {$where eq "" } {
	::spectcl::detach $::exphandle
    } else {
	::spectcl::detach $::exphandle $where
    }
}

#
#  Get information about an event database associated with our experiment database.
# Parameters:
#   filename - Event database file about which we want information.
# Implicit:
#    global exphandle
# Returns:
#   [list run-number title start-time end-time]
# Note that end-time may be empty if the run end time is not yet known.
#
proc evtinfo {filename} {
    evtattach $filename SPECTCLINTERNAL
    set result [::spectcl::evtruninfo $::exphandle SPECTCLINTERNAL]
    evtdetach SPECTCLINTERNAL
    return $result
}
#
#   Loads data from a csv file into an event database.  The event database
#   must already have been created.
# Parameters:
#   evtfile  - Event database to load.
#   source   - The csv source.  This can be either a file path or, if the
#              path starts with the pipe character '|', the remainder is the
#              name of a program that provides csv  data on stdout.
#              The fields for each line in the file are:
#              - trigger number of the event.
#              - name of a defined parameter.
#              - value of the parameter for that event.
#  progress  - Optional command invoked every few triggers to report progress.
#              This is run at the top level and is invoked as:
#              $progress $source triggers-processed-so-far
#  triggersPerClump
#            - Optional value that is the number of triggers that are atomically 
#              added to the database.  This also sets the number of triggers between
#              calls to progress.   Defaults to 100.
# Implicit inputs:
#    ::exphandle 
#
# Returns:
#   The number of triggers loaded.
#
proc evtload {evtfile source {progress {}} {triggersPerClump 100}} {
    set src               [open $source r]
    set triggersProcessed 0
    set events            [list]
    set thisEvent         [list]
    set lastTrigger       -1
    set evthandle          [::spectcl::evtopen $evtfile]
    while {1} {
	set line [gets $src]
	if {$line ne ""} {
	    set fields [::csv::split $line]
	    set trigger [lindex $fields 0]
	    set name    [lindex $fields 1]
	    set value   [lindex $fields 2]
	    if {(($trigger != $lastTrigger) && ($lastTrigger != -1)) || [eof $src]} {
		lappend events [linsert $thisEvent 0 $lastTrigger]
		set thisEvent   [list]
		incr triggersProcessed
		if {(($triggersProcessed % $triggersPerClump) == 0) || [eof $src]} {
		    ::spectcl::loadevents $::exphandle $evthandle $events
		    if {$progress ne ""} {
			uplevel #0 $progress $source $triggersProcessed
		    }
		    set events [list]
		}
	    }
	    set lastTrigger $trigger
	    lappend thisEvent [list $name $value]
	}
	if {[eof $src]} break
    }
    if {[llength $thisEvent] != 0} {
	lappend events [linsert $thisEvent 0 $lastTrigger]
    }
    if {[llength $events] != 0} {
	::spectcl::loadevents $::exphandle $evthandle $events
	if {$progress ne ""} {
	    uplevel #0 $progress $source $triggersProcessed
	}	
    }
    close $src
    ::spectcl::evtclose $evthandle
    return $triggersProcessed


}
#
#  Augment the data with a pseudo script
# parameters:
#    evtdb    - event databasefile
#    callback - proc to use to augment.
#              see the ::spectcl::augment command for more about this.
#
proc evtaugment {evtdb callback} {
    set handle [::spectcl::evtopen $evtdb]
    ::spectcl::augment $::exphandle $handle $callback
    ::spectcl::evtclose $handle
}
#    
#
#   Return the UUID of an event file
# Parameters:
#   file  - The file to get
# Returns:
#   Textualized UUID
#
proc evtuuid {file} {
    set handle [::spectcl::evtopen $file]
    set result [::spectcl::evtuuid $handle]
    ::spectcl::evtclose $handle

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

