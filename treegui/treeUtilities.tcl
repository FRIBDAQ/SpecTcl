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
#	    

# This file contains open functions in the 'treeutility' namespace that
# factor common code out of the various tab actions etc.
#

package provide treeUtilities 1.0

namespace eval ::treeutility {
}

##
#  Given a callback that generates names from a glob pattern
#  and a sample array element, returns the names of all array elements.
#  From the tree parameter point of view, an array element is
#  something of the regexp form prefix.\d+$ where 'prefix' can be just about anything.
#
#  @param sampleName - The sample array name.
#  @param generator  - Command that will generate a list of names from a glob pattern.
#
proc ::treeutility::listArrayElements {sampleName generator} {
    
    # So happens that the last element in the path looks like a file extension so
    # we can use [file rootname] to get the rest of it.
    set prefix [file rootname $sampleName]
    set prefixlen [llength [split $prefix .]]
    append prefix .
    
    set candidates [{*}$generator $prefix*]; # list only the stuff that starts out right.
    
    # For matching purposes we need to match elements that start with the prefix
    # and end with .digits.
    
    set result [list]
    foreach name $candidates {
	set end [join [lrange [split $name .] $prefixlen end]]
	
	if {[regexp -- {^\.([0-9])+$} .$end]} {
	    lappend result $name
	}
    }
    return  $result
}
##
#  Dispatch to a script with substitutions.
#  @param script - The script to dispatch.
#  @param substs - list of substitution patterns.  These are treated as regexps.
#  @param values - List of corresponding values to substitute for each match.
#
# @note - the script is run at the global level.
#
proc ::treeutility::dispatch {script substs values} {
    if {$script ne ""} {

	# Do the substitutions:

	foreach pattern $substs value $values {
	    regsub -all -- $pattern $script $value script
	}
	# Run the resulting script script:

	uplevel #0 $script
    }
}

##
# Return the file types used for configuration file dialogs:
# @return list
# @retval see the tk_getOpenfile/tk_getSaveFile documents to see the format of this.
#
proc ::treeutility::getFileTypes {} {
    return  {
	{{Tcl Scripts} {.tcl}  }
	{{Text files}  {.txt}  }
	{{Tree variable files} {.tv} }
	{{All Files}     * }
    }
}

##
# Perform a command with each element of a list appended to it
# Sort of like an STL for_each
# @param cmd - the command to run.
# @param list - The list oif parameters to apply.
#
proc ::treeutility::for_each {cmd list} {
    foreach element $list {
	{*}$cmd $element
    }
}