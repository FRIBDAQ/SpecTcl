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

package require Itcl
package require guistate


package provide autosave 1.0

##
#  This Itcl class provides a singleton that is in charge
#  of doing auto saves to failsafe.tcl in the cwd.
#  The singleton provides the following methods:
#
#  - getInstance     -static that returns (constructing as needed) the instance
#  - enableFailsafe  - Turns on failsafe saves.
#  - disableFailsafe - Turns off failsafe saves.
#  - failsafeSave    - If failsafe is enabled, saves the state of the system.
#
itcl::class autoSave {
    private common instance [list]
    private variable enabled 0

    #----------------------------------------------------------------
    #
    # Public interface:

    ##
    # retrieve (create if needed) the singleton instance
    #
    # @return Name of the one instance of this class.
    public  proc  getInstance {} {
	if {$instance == [list]} {
	    autoSave ::#auto
	}
	return $instance
    }
    ## 
    # turn on failsafe saving:
    #
    public method enableFailsafe {} {
	set enabled 1
    }
    ##
    # Turn off failsafe saving:
    #
    public method disableFailsafe {} {
	set enabled 0
    }
    ##
    # Save if failsafe is enabled:
    #
    public method failsafeSave {} {
	if {$enabled} {
	    set fd [open failsafe.tcl w]
	    set prior $::guistate::writeDeletes
	    set $::guistate::writeDeletes 0
	    writeAll $fd
	    close $fd
	    set ::guistate::WriteDeletes $prior
	}
    }
    ##
    # In the singleton pattern, the construtor is labeled private
    # so that it can only be invoked, if needed by the getInstance
    # common method.
    # Unfortunately itcl does not honor private restrictions on constructors.
    # hence the check for instance below.
    #
    private constructor {} {
	if {$instance ne [list]} {
	    error "Singleton-ness violated for autoSave"
	}
	set enabled 1
	set instance $this


    }

}