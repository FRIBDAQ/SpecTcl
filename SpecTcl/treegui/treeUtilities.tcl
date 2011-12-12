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