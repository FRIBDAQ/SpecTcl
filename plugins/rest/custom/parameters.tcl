##
#  Application direct URL for SpecTcl parameter handling
#
package require json::write





Direct_Url /spectcl/parameter  SpecTcl_Parameter

##
#  List the spectcl parameters in Json format
#
#  Parameters are returned as a JSON array.  
#  Each element is an object containing:
#  - name - name of the parameter.
#  - id   - Parameter id.
#  If, in addition, the parameter is a tree parameter,
#  The following attrributes will also be present:
#  - bins - default bin count.
#  - low  - Suggested low limit
#  - high - Suggested high limit.
#  - units- Units. of measure.
#
# Post/query parameters:
#  - filter - glob pattern that parameter names must match
# 
proc SpecTcl_Parameter/list {{filter *}} {
    set ::SpecTcl_Parameter/list "application/json"; # output mimetype is json.
    
    set rawParameters [parameter -list $filter]
    set json {}
    set jsonArray [list]
    foreach parameter $rawParameters {
	set name [lindex $parameter 0]
	set id   [lindex $parameter 1]
	set itemList [list name [json::write  string $name] id $id]

	# If there's a corresponding tree paramter add
	# its items to the itemList:

	set treeparam [treeparameter -list $name]
	if {[llength $treeparam] > 0} {
	    set treeparam [lindex $treeparam 0]
	    lappend itemList bins [lindex $treeparam 1]
	    lappend itemList low  [lindex $treeparam 2]
	    lappend itemList hi   [lindex $treeparam 3]
	    lappend itemList units [json::write string [lindex $treeparam 5]]
	}

	# Encode the JSON object.

	lappend jsonArray [json::write object  {*}$itemList]
	
    }
    return [deflateResponse [::SpecTcl::_returnObject "OK" [json::write array  {*}$jsonArray]]]

}
##
#  Edit a tree parameter.  Note that all parameters other than name are
#  optional.  A blank name is the same as  a parameter not found:
#  
#   @param name - name of the parameter to edit.
#                 must be an existing tree parameter else NotFound is returned.
#   @param bins - Suggested bin count for new spectra.
#   @param low  - Parameter low limit
#   @param high - Suggested high limit.
#   @param units - Units.
#
#   @return 'web page' returned.

proc SpecTcl_Parameter/edit {{name ""} {bins ""} {low ""} {high ""} {units ""}} {
 
    set ::SpecTcl_Parameter/edit "application/json"


    # Name must be valid


    set parameter [treeparameter -list $name]
    if {[llength $parameter] == 0} {

	return [::SpecTcl::_returnObject "not found"  [json::write string $name]]
    }
    #
    # We're going to get the tree parameter info about the tree parameter
    # and see what changed to decide which treeparameter modifiction ops to do.
    #
    set parameter [lindex $parameter 0]
    set oldbins      [lindex $parameter 1]
    set oldlow       [lindex $parameter 2]
    set oldhigh      [lindex $parameter 3]
    set oldunits     [lindex $parameter 5]
    
    set newbins $oldbins
    set newlow  $oldlow
    set newhigh $oldhigh
    set newunits $oldunits

    # Update the new items for each non empty parameter:

    foreach item [list bins low high units] newitem [list newbins newlow newhigh newunits] {
	if {[set $item] ne ""} {
	    set $newitem [set $item]
	}
    }
    ##
    # Deal with changes unfortunately, /low/high are not separable
    #
    if {($oldlow != $newlow) || ($oldhigh != $newhigh)} {
	if {[catch {treeparameter -setlimits $name $newlow $newhigh} msg]} {
	    return [::SpecTcl::_returnObject "command failed"  [json::write string $msg]]
	}
    }
    #  These can be mechanically done:
    #
    foreach newitem {newbins newunits} olditem {oldbins oldunits} subcommand {-setbins -setunit} {
	if {[set $newitem] != [set $olditem]} {
	    if {[catch {treeparameter $subcommand $name [set $newitem]} msg]} {
		return [::SpecTcl::_returnObject "command failed" [json::write string $msg]]
	    }

	}
    }
    return [::SpecTcl::_returnObject]

}

##
# Promote a parameter that is not a tree parameter to a 
# tree parameter... all parameters are mandatory except units
#
#
# @param name - name of the parameter if no such parameter exists NOTFOUND
# @param low  - Parameter low limit
# @param high - Parameter high limits
# @param bins - suggested binning.
# @param units - Parameter units (can be empty).
#
proc SpecTcl_Parameter/promote {{name ""} {low ""} {high ""} {bins ""} {units ""}} {
    set ::SpecTcl_Parameter/promote "application/json"

    #
    # Check for error conditions
    # First error returns a failure status
    #

    # Mandatory parameters:
 
    foreach mandatory [list name low high bins] {
	if {[set $mandatory] eq ""} {
	    return [::SpecTcl::_returnObject "missing parameter"  [json::write string $mandatory]]
	}
    }
    # The parameter must exist:

    set parameter [parameter -list $name]
    if {[llength $parameter]  == 0} {
	return [::SpecTcl::_returnObject "not found"  [json::write string $name]]
    }
    # The parameter must not already be a tree parameter:

    set tree [treeparameter -list $name]
    if {[llength $tree] != 0} {
	return [::SpecTcl::_returnObject "already treeparameter"  [json::write string $name]]
    }
    
    # Create the tree parameter:

    if {[catch {treeparameter -create $name $low $high $bins $units} msg]} {
	return [::SpecTcl::_returnObject "command failed"  [json::write string $msg]]
    }
    return [SpecTcl::_returnObject]
}