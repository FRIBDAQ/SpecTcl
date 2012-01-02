#
#     http://www.gnu.org/licenses/gpl.txt
#
#    Author:
#             Ron Fox
#	     NSCL
#	     Michigan State University
#	     East Lansing, MI 48824-1321

package require Itcl

package provide restore 1.0

##
# Itcl singleton class that manages restoring state with observers.
# state restoration is really a source at global level.
# For the mcGUI Rewrite this is used to provide the tabaction classes/objects
# a hook to find the arrays their save observers may have placed in the files 
# to restore their visual state.
#
# Public methods provided are:
#   - getInstance  - Get the singleton instance
#   - addObserver  - Adds an observer that is called after the state restore.
#   - removeObserver - Remove an observer that has been added via addObserver.
#   - restore        - Restore state of the software.
#
#

itcl::class Restore {
    private common    instance [list]
    private variable  observers 

    #--------------------------------------------------------------------
    #
    # Public interface

    ##
    # Return the instance variable, creating the singleton if needed:
    #
    public proc getInstance {} {
	if {$instance == [list]} {
	    Restore ::#auto
	}
	return $instance
    }
    ##
    # In the singleton pattern, normally constructors are private, 
    # unfortunately, itcl does not honor private-ness of constructors hence the
    # intance check below

    private constructor {} {
	if {$instance ne [list]} {
	    error "Singleton-ness violated for autoSave"
	}
	array set observers [list]
	set instance $this
    }

    ##
    # Add an observer to the set of observers invoked on a restore.
    #
    # @param name - Name of the observer (must be unique).
    # @param script- Observers script.
    #
    public method addObserver {name script} {
	set observers($name) $script
    }
    ##
    # Remove an observer by name.  It is a no-op to remove an observer that
    # does not exist.
    # 
    # @param name - Name of observer to remove.
    #
    public method removeObserver name {
	if {[array names observers $name] eq $name} {
	    array unset observers $name
	}
    }
    ##
    # restore the program state from a file by sourceing the designated script at
    # global level and then invoking all defined observers at global level.
    #
    # @param name - filename to be sourced.
    #
    public method restore name {
	uplevel #0 source $name

	foreach observerName [array names observers] {
	    uplevel #0 $observers($observerName)
	}
    }

}
