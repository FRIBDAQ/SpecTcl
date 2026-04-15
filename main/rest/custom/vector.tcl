##
# Application direcdt UFL for SpecTcl vector valued parameter operations.
#
package require json::write

Direct_Url /spectcl/vector SpecTcl_Vector

##
# List the vectors that are defined.
#  @param pattern (defaults to *) filters by vector name
#
proc SpecTcl_Vector/list {{pattern *}} {
    set SpecTcl_Vector/list application/json


    set vectors [treeparamvec -list $pattern]

    # turn this into a list of json objects:

    set result [list];    # Allows for no vectors:

    foreach v $vectors {
        dict unset v parameters
        lappend result [json::write object-strings {*}$v]
    }
    
    return [::SpecTcl::_returnObject "OK" [json::write array {*}$result]]
}
##
# setlow
#   Set the low value of a vector
# @param name - name of the vector required.
# @param low  - new low value
# Result has an empty detail.
#
proc SpecTcl_Vector/setlow {{name ""} {low ""}}  {
    set SpecTcl_Vector/setlow application/json

    if {$name eq "" || $low  eq ""} {
        return [::SpecTcl::_returnObject "missing parameter" \
            [json::write string "need both 'name' and 'low' query parameters"]]
    }
    set status [catch {
        treeparamvec -setlow $name $low
    } msg ]

    if {$status} {
        # failed
        return [::SpecTcl::_returnObject "bad status from treeparamvec command" \
            [json::write string $msg]] 
    }

    return [::SpecTcl::_returnObject]
}
##
# sethigh
#   set the high limit value of a vector.
#
# @param name - name of the vector.
# @param high - new high value
# On success, result detail is empty.
#
proc SpecTcl_Vector/sethigh {{name ""} {high ""}} {
    set SpecTcl_Vector/sethigh application/json

    if {$name eq "" || $high  eq ""} {
        return [::SpecTcl::_returnObject "missing parameter" \
            [json::write string "need both 'name' and 'high' query parameters"]]
    }

    set status [catch {
        treeparamvec -sethigh $name $high
    } msg]

    if {$status} {
        # failed
        return [::SpecTcl::_returnObject "bad status from treeparamvec command" \
            [json::write string $msg]] 
    }

    return [::SpecTcl::_returnObject]
}

##
# setbins
#    set the binning of a vector
#
# @param name - vector name.
# @param bins - number of bins desired.
# on success the detauk is empty.
#
proc SpecTcl_Vector/setbins {{name ""} {bins ""}} {
    set  SpecTcl_Vector/setbins application/json

    if {$name eq "" || $bins  eq ""} {
        return [::SpecTcl::_returnObject "missing parameter" \
            [json::write string "need both 'name' and 'bins' query parameters"]]
    }

    set status [catch {
        treeparamvec -setbins $name $bins
    } msg]

    if {$status} {
        # failed
        return [::SpecTcl::_returnObject "bad status from treeparamvec command" \
            [json::write string $msg]] 
    }

    return [::SpecTcl::_returnObject]
}

## 
# setunits
#   Set a new units value for a vector
#
# @param name - name of the vector.
# @param units - new units.
# on success, the detail is nonexistent.
#
proc SpecTcl_Vector/setunits {{name ""} {units ""}} {
    if {$name eq "" || $units  eq ""} {
        return [::SpecTcl::_returnObject "missing parameter" \
            [json::write string "need both 'name' and 'units' query parameters"]]
    }

    set status [catch {
        treeparamvec -setunits $name $units
    } msg]

    if {$status} {
        # failed
        return [::SpecTcl::_returnObject "bad status from treeparamvec command" \
            [json::write string $msg]] 
    }

    return [::SpecTcl::_returnObject]
}