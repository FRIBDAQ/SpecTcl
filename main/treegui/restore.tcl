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
#   - addPreObserver - Adds an observer called prior to state restor.
#   - removePreobserver - Removes a pre-observer.
#   - addObserver  - Adds an observer that is called after the state restore.
#   - removeObserver - Remove an observer that has been added via addObserver.
#   - restore        - Restore state of the software.
#
#

itcl::class Restore {
    private common    instance [list]
    private variable  observers 
    private variable  preObservers
    
    #-----------------------------------------------------------------
    #
    # Private utilities


    ##
    # Invoke a set of observers.
    # 
    # @param observerList the result of an [array get] on an observers array.
    #                     this is a list of the form index1 value1 index2 value2...
    #
    private method invokeObservers observerList {

	foreach [list name value] $observerList {
	    uplevel #0 $value
	}
    }

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
	    error "Singleton-ness violated for restore"
	}
	array set observers [list]
	array set preObservers [list]
	set instance $this
    }
    ## Add an observer invoked prior to state restore.  If state is saved
    # in global variables this gives an observing object a chance to clear
    # that state first.
    #
    # @param name - name identifying the observer.  This is in a namespace
    #               distinct from the (post)observer namespace.
    # @param script - Script to run when the observer is triggered.
    #
    public method addPreObserver {name script} {
	set preObservers($name) $script
    }
    ##
    # Remove a pre restore observer
    # 
    # @param name - Name of the observer to delete.
    #               This is a no-op if no observer by that name was established.]
    #
    public method removePreObserver name {
	if {[array names preObservers $name] eq $name} {
	    array unset preObservers $name
	}
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

	invokeObservers [array get preObservers]

	uplevel #0 source $name

	invokeObservers [array get  observers]

    }

}
