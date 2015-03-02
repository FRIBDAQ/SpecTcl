##
#  Application direct URL for SpecTcl Spectrum definitions.
#
package require json::write

namespace eval ::SpecTcl {};	#  for local procs.


Direct_Url /spectcl/spectrum  SpecTcl_Spectrum


##
# List the spectra that match the specified pattern
# in Json format.  The return format is the usual object
# - status  - OK 
# - detail in this case an array of spectrum objects:
#   - name       - Name of the spectrum.
#   - type       - type of spectrum
#   - parameter - array of parameters 
#   - axes       - array of axis definitions.
#   - chantype   - Channel type.
#
# @param filter - glob pattern that filters the list of parameters.
#                 optional defaults to *
# @return - the json result.
# 
proc SpecTcl_Spectrum/list {{filter *}} {
    set ::SpecTcl_Spectrum/list "application/json"

    set spectra [spectrum -list $filter]

    set spectrumDefinitionArray [list]

    foreach spectrum $spectra {
	set name [lindex $spectrum 1]
	set type [lindex $spectrum 2]
	set params [lindex $spectrum 3]
	set axes  [lindex $spectrum 4]
	set chantype [lindex $spectrum  5]


	## The axes are an array of 
	#   low, high, bins objects
	#
	set axisArray [list]
	foreach axis $axes {
	    lappend axisArray [json::write object \
				   low [lindex $axis 0] \
				   high [lindex $axis 1] \
				   bins [lindex $axis 2]]
	}
	##
	# Parameters are an array of strings:
	#
	set parameterArray [::SpecTcl::_jsonStringArray $params]
	lappend spectrumDefinitionArray [json::write object \
					     name [json::write string $name] \
					     type [json::write string $type] \
					     parameters $parameterArray      \
					     axes [json::write array {*}$axisArray] \
					     chantype [json::write string $chantype]]
					 
					 
    }
    return [::SpecTcl::_returnObject OK [json::write array {*}$spectrumDefinitionArray] ]
}
##
#  Delete an existing spectrum.
#  @param name - name of the spectrum (mandatory).
#  @return - JSON encoding of the string. Possible statuses include:
#            "not found" - No such spectrum detail is spectrum name.
#            "missing parameter" Spectrum name not suppled detail "name"
#            "command failed" - spectrum -delete failed, detail is the error msg for the command.
#
#
proc SpecTcl_Spectrum/delete {{name ""}} {
    set ::SpecTcl_Spectrum/delete "application/json"

    # Name parameter must be supplied (assuming nobody makes a spectrum with 
    # a blank name

    if {$name eq ""} {
	return [::SpecTcl::_returnObject "missing parameter"  [json::write string name]]
    }
    # Spectrum must exist:

    if {[llength [spectrum -list $name]] == 0} {
	return [::SpecTcl::_returnObject "not found" [json::write string $name]]
    }

    if {[catch {spectrum -delete $name} msg]} {
	return [::SpecTcl::_returnObject "command failed"  [json::write string $msg]]
    }
    
    return [::SpecTcl::_returnObject]
    
}
##
# Create a new spectrum. All parameters are required except for chantype
# which defaults to 'long'.
#
# @param name       - Name of spectrum.
# @param type       - Spectrum type
# @param parameters - parameter list in tcl list format.
# @param axes       - Axis list in tcl list format.
# @param chantype   - Channel data type.
#
# @return the usual JSON object. Failures include:
#   - command failed    -  the spectrum creation command failed.
#   - missing parameter -  a mandatory parameter is missing.
#
proc SpecTcl_Spectrum/create {{name ""} {type ""} {parameters ""} {axes ""} {chantype long}} {
    set ::SpecTcl_Spectrum/create application/json

    # Check mandatory parameters:

    foreach mandatory [list name type parameters axes] {
	if {[set $mandatory] eq ""} {
	    return [::SpecTcl::_returnObject "missing parameter"  [json::write string $mandatory]]
	}
    }
    if {[catch {spectrum $name $type $parameters  $axes $chantype} msg]} {
	return [::SpecTcl::_returnObject "command failed" [json::write string $msg] ]
    }
    catch {sbind $name};		# Bind to displayer if needed.
    return [::SpecTcl::_returnObject]
} 
##
#  Get spectrum contents
#  
#   @param name - spectrum name
#   @param sample - stride between channel selections.
#  
#   @return JSON object on success the details are an array of 
#        - xchan - Xchannel number
#        - ychan - Y channel number (only if 2-d).
#        - value - channel value
#        - These are in raw channels not in real coordinates.
#  Failures include:
#    - not found  name is not a spectrum.
#    - missing parameter - name is not supplied.
#

proc SpecTcl_Spectrum/contents {{name ""} {sample 1}} {
    set ::SpecTcl_Spectrum/contents application/json

    if {$name eq ""} {
	return [::SpecTcl::_returnObject "missing parameter" [json::write string name]]
    }

    set def [spectrum -list $name]

    if {[llength $def] == 0} {
	return [::SpecTcl::_returnObject "not found"] [json::write string $name]]
    }
    # With the exception of sumary spectra, the axes tell me both the dimension
    # and the bins.. for summary spectrum the y bins are the number of parameters in the def.

    set def    [lindex $def 0]
    set name   [lindex $def 1]
    set type   [lindex $def 2]
    set params [lindex $def 3]
    set axes   [lindex $def 4]

    
    if {([llength $axes] == 2) || ($type eq "s")} {
	set dims 2

	# If summary  spectrum make a fake second axis:

	if {$type eq "s"} {
	    lappend axes [list 0 0 [llength $params]]
	}
    } else {
	set dims 1
    }


    # Return the appropriate guy:
    
    return [::SpecTcl::_getSpectrum$dims $name $axes $sample]
    
}
##
# Clear a set of spectra.
#
# @param pattern - glob pattern of spectra to clear  Defaults to *
#
proc SpecTcl_Spectrum/zero {{pattern *}} {
    set ::SpecTcl_Spectrum/zero application/json
    clear $pattern
    return [::SpecTcl::_returnObject]
}			   
				
# -----------------------------------------
#
#  Private procs

#
#  Return the contents of a 1-d spectrum as JSON with good status
#   @param name - spectrum name.
#   @param axes - axis specifications.
#   @param sample - stride between channel selections.
#
#  @return - array of non zero channels, see SpecTcl_Spectrum/contents.
#
proc ::SpecTcl::_getSpectrum1 {name axes sample} {
    set channels [lindex [lindex $axes 0] 2]
    set nonZeroChannels [list]
    for {set c 0} {$c < $channels} {incr c $sample} {
	set value [channel -get $name $c]
	if {$value != 0} {
	    lappend nonZeroChannels [json::write object \
				  xchan $c \
				  value $value]
	}

    }
    return [::SpecTcl::_returnObject OK [json::write array {*}$nonZeroChannels]]
}


#
# Return the contents of a 2-d spectrum as JSON with good status
#
# @param name -spectrum name
# @param axes  spectrum axes
# @param sample - stride between channel selections.
#
#  See SpecTcl_Spectrurm/contents
#
proc ::SpecTcl::_getSpectrum2 {name axes sample} {
    set xchans [lindex [lindex $axes 0 ] 2]
    set ychans [lindex [lindex $axes 1] 2]

    set nonZeroChannels [list]
    
    #  How we do this depends on the SpecTcl version.
    #  If the version command is implemented we have an scontents
    #  command which will speed up the channel fetch loop:
    
    if {[info command version] eq ""} {
        for {set y 0} {$y < $ychans} {incr y $sample} {
            for {set x 0} {$x < $xchans} {incr x $sample} {
                set value [channel -get $name [list $x $y]]
                if {$value != 0} {
                    lappend nonZeroChannels [json::write object \
                                                 xchan $x \
                                                 ychan $y  \
                                                 value $value]
                }
            }
        }
        return [::SpecTcl::_returnObject OK  [json::write array {*}$nonZeroChannels]]
    } else {
        # Note that we also have inflate/deflate:
        package require compress
        
        set data [scontents $name]
        foreach channel $data {
            lappend nonZeroChannels [json::write object        \
                                     xchan [lindex $channel 0] \
                                     ychan [lindex $channel 1] \
                                     value [lindex $channel 2] \
            ]                       
        }
        set json [::SpecTcl::_returnObject OK  [json::write array {*}$nonZeroChannels]]
        set jsonGzip [deflate $json]
        
        #  Force content encoding -> gzip.
        #
        set sock [Httpd_CurrentSocket]
        Httpd_AddHeaders $sock Content-Encoding deflate
        return $jsonGzip
    
    }
        
}