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
# @file   SpecTclUtils.tcl
# @brief  Package providing SpecTcl utilities in Tcl.
# @author Ron Fox <fox@nscl.msu.edu>
#

package provide SpecTclUtils 1.0
package require SpecTclRESTClient

#  Packages that will be available in the NSCLDAQ env:

if {[array names env DAQTCLLIBS] ne ""} {
    lappend auto_path $env(DAQTCLLIBS)
    package require portAllocator
}

namespace eval Xamine {
    variable restClient  [list]     # Rest client.
    variable haveDAQ
    if {[info command portAllocator] ne ""} {
        set haveDAQ 1
    } else {
        set haveDAQ 0
    }
}

##
# Xamine::initRestClient
#   Initialize access to the REST client
#
# @param host - host in which the client lives.
# @param port - Port or service (service only if NSCLDAQ).
# @param user - User running SpecTcl (only needed for port as a service)
# @note On success, Xamine::restClient is set to the SpecTclRest client object
#       that will be used to communicate with SpecTcl.
#
proc Xamine::initRestClient {host port {user ""}} {
    
    # If the port is a service and we can look it up do so:
    
    if {![string is integer -strict $port]} {
        if {!$Xamine::haveDAQ} {
            error "Service lookup is only supported in the NSCLDAQ environment"
        }
        set pman [portAllocator %AUTO% -hostname $host]
        set svrport [$pman findServer $port $user]
        if {$svrport eq ""} {
            error "There's no service $port advertised by the user '$user'"
        }
        set port $svrport
    }
    # Now the port is an integer so we can create the rest interface:
    
    set Xamine::restClient [SpecTclRestClient %AUTO% -host $host -port $port]
    
    #  Figure out if this really works by doing a harmless operation:
    #  Note there's no assurance that SpecTcl won't later exit.
    
    set status [catch {
        $Xamine::restClient version
    } msg]
    if {$status} {
        $Xamine::restClient destroy
        error "Cannot fetch the SpecTcl version, make sure it is running $msg"
    }
}
##
# Xamine::getLocalMemory
#    Return information about the Xamine shared memory when SpecTcl is run
#    in the local host.  This can be used to setup the shared memory environment
#    variables needed by Xamine when we start it.
#
# @return two element list with key and size.
#
# @note initRestClient must have successfully run.
#
proc Xamine::getLocalMemory { } {
    set key [$Xamine::restClient shmemkey]
    set size [$Xamine::restClient shmemsize]

    return [list $key $size]
}
    
