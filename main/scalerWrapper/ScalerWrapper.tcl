#!/bin/sh
# -*- tcl -*-
# The next line is executed by /bin/sh, but not tcl \
exec tclsh "$0" ${1+"$@"}

#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2014.
#
#    You may use this software under the terms of the GNU public license
#    (GPL).  The terms of this license are described at:
#
#     http://www.gnu.org/licenses/gpl.txt
#
#    Authors:
#             Ron Fox
#             Giordano Cerriza
#	     NSCL
#	     Michigan State University
#	     East Lansing, MI 48824-1321


##
# @file ScalerWrapper.tcl
# @brief Wraps NSCLDAQ scaler display program.
# @author Ron Fox <fox@nscl.msu.edu>
#

##
#   This wrapper script provides access to the new NSCL Scaler program
#   using and API identical to the old program.
#
#   The idea is that SpecTcl event processors can invoke the old style
#   scaler program methods for flagging begin/end/pause and resumes as well
#   as providing new scaler values, and these will be displayed using the
#   "New" NSCLDAQ scaler program.

#  Requirements:
#    -  Installation of NSCLDAQ.
#    -  Environment variables set up as per daqsetup.bash (really only need DAQROOT).
#    -  Configuration filename stored in the global variable ::ScalerConfigFile.
#
#

package provide ScalerWrapper 1.0

# Add NSCLDAQ's pacakges to the search path:

lappend auto_path [file join $::env(DAQROOT) TclLibs]

set ::env(SCALER_RING) "";             # Disables the attempt to get data from rings.
set argv $::ScalerConfigFile

array set ::Scaler_Increments [list]
array set ::Scaler_Totals     [list];   #Unused I think.
set       ::RunStateName  "**Unknown**"
set       ::ElapsedRunTime 0
set       ::ScalerDeltaTime 0

set ::scalerWin [toplevel .scalers]

package require scalermain

##
# Update
#   Called to update the display.  We construct a scaler event dict and invoke
#   handleData which will decode and update the display.
#
#   We need the following global variables set:
#   -  Scaler_Increments - array of scaler channel increments:
#   -  ElapsedRunTime   - Number of seconds the run has been going.
#   -  ScalerDeltaTime   - Number of seconds over which scalers have incremented.
#
proc Update {} {
    set start [expr {$::ElapsedRunTime - $::ScalerDeltaTime}]
    
    # Construct the event... except for the scalers themselves:
    
    set event [dict create                                                      \
        type Scaler start $start end $::ElapsedRunTime realtime [clock seconds] \
        divisor 1 incremental 1 scalers [list]        
    ]
    #  Now the scaler increments.
    
    set nScalers [llength [array  names ::Scaler_Increments]]
    for {set i 0} {$i < $nScalers} {incr i} {
        dict lappend event scalers $::Scaler_Increments($i)
    }
    handleData $event
    
}

##
# BeginRun
#   Log a begin run:
#   -   RunNumber     - must have the new run number.
#   -   StartTime     - Time at which the run began.
#   -   ElapsedRunTime -  must have the new elapsed run time
#   -   RunTitle      -  Must have the new run's title
#
proc BeginRun {} {
    
    set event  [dict create  \
        type "Begin Run"                            \
        run        $::RunNumber                     \
        timeoffset $::ElapsedRunTime               \
        realtime   $::StartTime                    \
        title      $::RunTitle                     \
    ]
    
    puts $event
    handleData $event
}


##
# EndRun
#    Log an end run.  The same stuff has to be set as for BeginRun:
#
proc EndRun {} {
    #  Create the event:
    
    set event  [dict create  \
        type "End Run"                            \
        run        $::RunNumber                     \
        timeoffset $::ElapsedRunTime               \
        realtime   $::StartTime                    \
        title      $::RunTitle                     \
    ]
    
    puts $event
    handleData $event    
}

proc PauseRun {} {}
proc ResumeRun {} {}
