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
# @file trace.tcl
# @brief Support gate -trace, spectrum -trace and parameter -trace
# @author Ron Fox <fox@nscl.msu.edu>
#

#
#  Traces are problematic with REST interfaces since interactions are
#  client driven but traces are inherently server driven.
#  The interface we provide works as follows:
#  Clients indicate they are interested in tracing.
#  They receive a token from the server which must be passed in to the other
#  rest requests.  The server will then buffer all traces for that token
#  for as long as that client specifies.  The client can fetch the buffered
#  traces.
#
#  The URL's we're going to provide are:
#
#   /spectcl/trace/establish?retention=seconds - provides the client with the
#               token and begins buffering traces tossing traces older than
#               seconds seconds of history.  This prevents the trace buffer
#               from growing without bounds in case the client exits
#               without invoking:
#   /spectcl/trace/done?token=token-from-establish - stops buffering traces
#                      for the client represented by token.  The trace buffer
#                      is destroyed.
#   /spectcl/trace/fetch?token=token-from-establish - fetches traces that were
#                      buffered for the client represented by the token.
#                      The trace buffer is cleared.
#
#  So the normal sequence is that an application that needs traces
#  initially invokes /spectcl/trace/establish specifying a maximum time the
#  traces should be retained.  It then polls /spectcl/trace/fetch periodically
#  to obtain traces that occured.  Note that the retention period specified
#  should probably be some small multiple of whatever poll interval is used.
#  Prior to exiting the application should invoke
#  /spectcl/trace/done to clean up.  If it is unable or does not do that,
#  the server will continue to buffer traces for that client in-perpetuity
#  however the trace buffer will be limited by the retention period
#  specified when interest in traces was initially expressed so SpecTcl should
#  be ok.
#
    
# The namspace below is used to allocate trace tokens
# and to hold the trace buffers.  A trace buffer is just a list
# of dicts.  The dicts contain the following information:
#   *  time - When the trace was received.
#   *  type - type of trace - gate, spectrum or parameter.
#   *  parameters - list of parameters passed to the trace proc.
#              Note that for gate traces, an additional initial list item
#              identifying the trace subtyppe is prepended (e.g.  add, delete, changed).
# Trace buffers themselves are held in an array indexed by client token.
# @note  Trace retention period enforcement is not handled by a periodic
#        procedure.  As the trace buffers don't increase when no traces
#        fire, instead the receipt of a trace performs any pruning of the
#        buffered traces.
#

namespace eval RestTrace {
    variable lastToken 0;
    variable traceBuffers
    array set traceBuffers [list]
    variable retentonPeriods
    array set retentionPeriods [list]
    
    # Any prior gate trace.
    
    variable priorGateAdd  ""
    variable priorGateDelete ""
    variable priorGateChange ""
}

#------------------------------------------------------------------------------
#  We put our utility methods in the name space:

##
# RestTrace::prune
#   Given the current time:
#     Iterate over the tracebuffers purging all traces older than the
#     retention period.
#
# @param time - the current timestamp.
#
proc RestTrace::prune {time} {

    #  Loop over all tokens (using retentionPeriods indices which _should_
    #  be the same as the indices in traceBuffers
    #
    foreach client [array names RestTrace::retentionPeriods] {
        set cutoff [expr {$time - $RestTrace::retentionPeriods($client)}]
        
        # Figure out the range of items to retain:
        
        for {set keep 0} {$keep < [llength $RestTrace::traceBuffers($client)]} {incr keep} {
            set item [lindex $RestTrace::traceBuffers($client) $keep]
            if {[dict get $item time] >= $cutoff} {
                break
            }
        }
        #  Keep is the first index or the bits of the trace buffer retained so:
        
        set RestTrace::traceBuffers($client) [lrange                         \
            $RestTrace::traceBuffers($client) $keep end                      \
        ]
        
    }
}
##
# RestTrace::allocateToken
#   Allocates a new client token:
#   - Figure out the token value.
#   - Create an new trace buffer
#   - Create a new client retention entry.
# @param retention  - how long the client wants traces retained
# @return integer - client token assigned.
#
proc RestTrace::allocateToken {retention} {
    set token [incr RestTrace::lastToken]
    set RestTrace::traceBuffers($token) [list]
    set RestTrace::retentionPeriods($token) $retention
    
    return $token
}
##
# RestTrace::bufferTrace
#
#  Given a trace dict:
#   - Add it to all trace buffers.
#   - Prune traces
#
# @param item - The trace dict to buffer.
#
proc RestTrace::bufferTrace {item} {
    set time [dict get $item time]
    
    foreach client [array names RestTrace::traceBuffers] {
        lappend RestTrace::traceBuffers($client) $item
    }
    
    RestTrace::prune $time
}

##
# versionGe
#   Given a version string determines if the current SpecTcl version
#   is at least that
#
# @param version - version string e.g. "5.5" - edit level is ignored.
# @return bool.
#
proc versionGe {version} {
    set splitVersion [split $version .-]
    set major [lindex $splitVersion 0]
    set minor [lindex $splitVersion 1]
    
    set actualVersion [version]
    set splitActual [split $actualVersion .-]
    set amajor [lindex $splitActual 0]
    set aminor [lindex $splitActual 1]
    
    if {$amajor > $major} {
        return 1
    }
    if {($amajor == $major) && ($aminor >= $minor)} {
        return 1
    }
    
    return 0
    
}
#-------------------------------------------------------------------------------
#

#   These are the trace procs.  They build the dict describing each
#   trace and then invoke bufferTrace
#

##
# RestTrace::parameterTrace
#   Trace for parameters.
# @param args - trace arguments.
#
proc RestTrace::parameterTrace {args} {
    set traceDict [dict create                                               \
        time [clock seconds] type parameter parameters $args                 \
    ]
    RestTrace::bufferTrace $traceDict
}
##
# RestTrace::spectrumTrace
#
#   Trace on spectra.
#
# @param args
#
proc RestTrace::spectrumTrace {args} {
    set traceDict [dict create                                             \
        time [clock seconds] type spectrum parameters $args                \
    ]
    RestTrace::bufferTrace $traceDict
}

##
# RestTrace::gateAddTrace
#   Trace invoked when a gate is added.
# @param args - gate trace arguments
# @note - if there's a prior gate add trace it will get invoked first.
#
proc RestTrace::gateAddTrace {args} {
    if {$RestTrace::priorGateAdd ne ""} {
        uplevel #0 $RestTrace::priorGateAdd $args;   # I think these get exploded
    }
    set traceDict [dict create                                                \
        time [clock seconds] type gate parameters [list add {*}$args]         \
    ]
    RestTrace::bufferTrace $traceDict
}
##
# RestTrace::gateDeleteTrace
#    Trace invoked when a gate is deleted.
# @param args - gate trace arguments.
# @note - if there's a prior gate delete trace it gets invoked as well.
#
proc RestTrace::gateDeleteTrace {args} {
    if {$RestTrace::priorGateDelete ne ""}  {
        uplevel #0 $RestTrace::priorGateDelete $args
    }
    set traceDict [dict create                                                \
        time [clock seconds] type gate parameters [list delete {*}$args]         \
    ]
    RestTrace::bufferTrace $traceDict
}
##
# RestTrace::gateChangeTrace
#
#   Gate changed trace
#
#  @param args gate trace arguments.
#
proc RestTrace::gateChangeTrace {args} {
    if {$RestTrace::priorGateChange ne ""} {
        uplevel #0 $RestTrace::priorGateChange $args
    }
    set traceDict [dict create                                                \
        time [clock seconds] type gate parameters [list changed {*}$args]         \
    ]
    RestTrace::bufferTrace $traceDict
}

##
# Handle binding traces:
#
proc RestTrace::binding {args} {
    
    set traceDict [dict create                                   \
        time [clock seconds] type binding parameters $args        \
    ]
    RestTrace::bufferTrace $traceDict
    
}

##
#  Establish the traces - Note that this happens before the SpecTcl commands
#  have been added (evidently). Therefore we dispatch it from the event loop.
#
after 100 {

    parameter -trace RestTrace::parameterTrace
    spectrum -trace  add [list RestTrace::spectrumTrace add]
    spectrum -trace delete [list RestTrace::spectrumTrace delete]
    set RestTrace::priorGateAdd [gate -trace add RestTrace::gateAddTrace]
    set RestTrace::priorGateDelete [gate -trace delete RestTrace::gateDeleteTrace]
    set RestTrace::priorGateChange [gate -trace change RestTrace::gateChangeTrace]
    
    # Sbind traces if available:
    
    if {[versionGe 5.5]} {
        sbind -trace [list RestTrace::binding add]
        unbind -trace [list RestTrace::binding remove]
    }
}


#-------------------------------------------------------------------------------
#  Server functions

Direct_Url /spectcl/trace SpecTcl_trace;    # We'll put these in the global ns.

##
# SpecTcl_trace/establish
#   Establish the interest in tracing things.
#
# @param retention - number of seconds (integer) maximum in _that_ client's
#                    trace buffer.
# @return - detail will be a token to be used by the client in future calls
#          to ../done ../fetch.
#
proc SpecTcl_trace/establish {retention} {
    set ::SpecTcl_trace/establish application/json
    
    set token [RestTrace::allocateToken $retention]
    return [SpecTcl::_returnObject OK $token];        # Tokens are integers.
}
##
# SpecTcl_trace/done
#   Clean up a client.
# @param token - token gotten from the /establish call.
#
proc SpecTcl_trace/done {token} {
    set ::SpecTcl_trace/done application/json
    
    array unset ::RestTrace::traceBuffers $token
    array unset ::RestTrace::retentionPeriods $token
    
    return [SpecTcl::_returnObject OK]
}
##
# SpecTcl_trace/fetch
#   Fetch the traces associated with a client - and clear the trace buffer.
#
# @param token - client token.
#
proc SpecTcl_trace/fetch {token} {
    set ::SpecTcl_trace/fetch application/json
    
    set result [dict create \
        parameter [list] spectrum [list] gate [list] binding [list] \
    ]
    set traces $::RestTrace::traceBuffers($token)
    
    set ::RestTrace::traceBuffers($token) [list]
    
    #  Process the traces..

    
    foreach trace $traces {
        set type [dict get $trace type]
        set args [dict get $trace parameters]
        dict lappend result $type [json::write string $args]
    }
    
    #  Convert the lists to JSON so we can treat the dict as an object.
    
    dict set result parameter [json::write array {*}[dict get $result parameter]]
    dict set result spectrum  [json::write array {*}[dict get $result spectrum]]
    dict set result gate      [json::write array {*}[dict get $result gate]]
    dict set result binding   [json::write array {*}[dict get $result binding]]
    
    
    return [SpecTcl::_returnObject OK                                     \
        [json::write object  {*}$result]                                  \
    ]
    
}



    

    
