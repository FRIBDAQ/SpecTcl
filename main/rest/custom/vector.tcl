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
    set SpecTcl_Vector/lists application/json


    set vectors [treeparamvec -list $pattern]

    # turn this into a list of json objects:

    set result [list];    # Allows for no vectors:

    foreach v $vectors {
        dict unset v parameters
        lappend result [json::write object-strings {*}$v]
    }
    
    return [::SpecTcl::_returnObject "OK" [json::write array {*}$result]]
}