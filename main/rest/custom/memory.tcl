#
#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2016.
#
#    You may use this software under the terms of the GNU public license
#    (GPL).  The terms of this license are described at:
#
#     http://www.gnu.org/licenses/gpl.txt
#
#     Author:
#      Scientific Software Team
#	     NSCL
#	     Michigan State University
#	     East Lansing, MI 48824-1321
#
#
set here [file dirname [info script]]
source [file join $here max.tcl];         # Make sure max is patched.

package require json::write

# Provides the API components at /spectcl/shmem
#
#
Direct_Url /spectcl/shmem  SpecTcl_Memory


# Send the shared memory key to the user
#
# Clients can use this method of the API to get the key of the shared
# memory and then attach to it. 
#
proc SpecTcl_Memory/key {} {
  set ::SpecTcl_Memory/key "application/json"

  set key [shmemkey]

  return [::SpecTcl::_returnObject OK [json::write string $key] ]
}

# Returns the size of the mapped shared memory segment
#
# The caller retrieves a json object containing the status and also the
# the size of the shared memory region in bytes.
#
proc SpecTcl_Memory/size {} {
  set ::SpecTcl_Memory/size "application/json"

  return [::SpecTcl::_returnObject OK [json::write string [shmemsize]] ]
}
