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
# @file  ServicePort.tcl
# @brief Provides support for to make the HTTPDPort variable managed.
# @author Ron Fox <fox@nscl.msu.edu>
#

##
#  Provides a package that can be pulled into SpecTclRC.tcl to set the
#  HTTPDPort from a variable from a port that was allocated by the port manager.
#  Including this package therefore requires that:
#  1. The SpecTcl TclLibs directory tree is in the package path.
#  2. The NSCLDAQ TclLibs directory tree is in the package path to alow us
#     to pull in the port manager support packages.
#
#  Use as follows:
#  \verbatim
#     package require DAQService
#     set HTTPDPort [SpecTcl::getServicePort serviceName]
#

package provide DAQService 1.0
package require portAllocator

namespace eval SpecTcl {
    proc getServicePort {name} {
        set mgr [::portAllocator  %AUTO% ]
        set result [$mgr allocatePort $name]
        $mgr destroy
        return $result
    }
}