#
#  For testing outside of SpecTcl
# 

if {[info globals SpecTclHome] eq ""} {
    proc parammap args {
	puts "parammap $args"
    }
    
    proc spectrum args {
	puts "spectrum $args"
    }
}

#
#  This tcl file is used by SpecTcl to establish 
#  parameter maps for the digitizers that have been
#  declared in the system.
#

# We are going to operate as follows:
#  1. Interpret the daqconfig.tcl file really only looking for
#     the -modules lists on the events stack.
#  2. Expect arrays of the form parameters(digitizer) that have
#     the parameter names for the specific digitizer.
#  3. For each digitizer in the list, create/execute a parammap
#     Create spectra (4k raw channel spectra).
#


set daqconfig [file join ~ config daqconfig.tcl]

#
# Module type numbers:
#
set PH7xx   0
set AD811   1


# Process ph7xxx (phillips 7xxx series digitizers.

proc ph7xxx args {
    global channelCounts
    global moduleTypes

    set moduleName [lindex $args 1]
    set channelCounts($moduleName) 4096; # Number of channels in the spectrum.
    set moduleTypes($moduleName)   $::PH7xx
    set idindex [lsearch  -exact $args "-id"]
    if {$idindex != -1} {
	global moduleIds
	set moduleIds($moduleName) [lindex $args [incr idindex]]
    }
}

# Proces ortec ad811 digitizers.

proc ad811 args {
    global channelCounts
    global moduleTypes

    set moduleName [lindex $args 1]
    set channelCounts($moduleName) 4096
    set moduleTypes($moduleName)  $::AD811

    set idindex [lsearch  -exact $args "-id"]
    if {$idindex != -1} {
	global moduleIds
	set moduleIds($moduleName) [lindex $args [incr idindex]]
    }
}

#
#  There could be two stacks.. a scaler stack and an
#  events stack...though there's currently no support for scalers.
#  This means that we must build up the configuration for each stack.
#  This is done in the stackConfig global array.
#
#  Note that configuration argument pairs all start
#  at the 3'd (from 0) argument
#
proc stack args {
    global stackConfig
    set name   [lindex $args 1]
    set config [lrange $args 2 end]
    foreach {key value} $config {
	append stackConfig($name) " " $key " " [list $value]
    }
}

#
#   Procees the stackConfig array to get the module list.
#   for the event stack.
#
#  The strategy will be to throw the configuration of each module
#  into a local array which we then look at as follows:
#   If the -type is event we care about it.
#   look at the -modules value and return it.
#  Note that array set is cumulative. Hence the unset.
#
proc getModuleList {} {
    global stackConfig
    
    foreach stack [array names stackConfig] {
	array set config $stackConfig($stack)

	if {$config(-type) eq "event"} {
	    return $config(-modules)
	}
	unset config;             # For next time.
    }
    error "There is no event stack!!"
}

# Return the id of a module.  The id is a parameter that is 
# present in all modules and represents the value of a 16 bit marker
# that allows the decode software to recognize the module with
# less ambiguity than supported by most CAMAC modules.
#
proc getId name {
    global moduleIds

    if {[array names moduleIds $name] ne ""} {
	set id $moduleIds($name)
    } else {
	set id 0;		# Module ids default to 0 if not set.
    }
    return $id
}


#
#  Return the number of channels a spectrum of parameters of a module
#  should have.
#
proc getChannels name {
    global channelCounts
    return $channelCounts($name); # must exist else stack is corrupt.
}

#
#  Returns the type of a module

proc getType name {
    return $::moduleTypes($name)
}
#
#   Create the parameter maps and spectra
#   for the set of modules specified.
#   for each module we expect parameter(modulename) that is a list
#   of the parameters for that module.
#  
#  We do a parammap command for each of the modules.
#  The form of that command is:
#    parammap module-number module-id parameter-list
#
#
proc createMapAndSpectra modules {
    global parameters
    set moduleNumber 0
    foreach module $modules {
	set id       [getId       $module]
	set channels [getChannels $module]
	set type     [getType     $module]

	set axis [list 0 [expr $channels-1] $channels]
	set axisspec [list $axis]
	if {[array names parameters $module] ne ""} {
	    parammap -add $moduleNumber $type $id  $parameters($module)
	    foreach parameter $parameters($module) {
		spectrum $parameter 1 $parameter $axisspec
	    }
	}
	incr moduleNumber
    }
}


source $daqconfig

set modules [getModuleList]
createMapAndSpectra  $modules

