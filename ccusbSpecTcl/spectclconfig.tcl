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
#   The strategy is to parse the chunks we need out of daqconfig.tcl
#   and combine that with the parameter information ~/config/daqconfig.tcl
#
# Specific global arrays that get built are:
#
#   parameters    - (from daqconfig.tcl) - indexed by the name of a module
#                    provides names for each of the module channels.
#   channelCounts - Indexed by the module name, represents the number of channels
#                   each digitizer provides (channels in a raw spectrum).
#   moduleTypes   - Array indexed by module name provides the type of module
#                   (see Module type numbers below).
#   moduleIds     - Array indexed by module name provides the id tag that 
#                   prefixes the data from the module.  This is
#                   picked out of the -id configuration parameter for the module.
#                   by convention all modules provide id = 0 if not configured.
# 


set daqconfig [file join ~ config daqconfig.tcl]

#
# Module type numbers, extend this table as needed.
#
set moduleTypeCodes(ph7xxx)  0
set moduleTypeCodes(ad811)   1
set moduleTypeCodes(lrs2249) 2
set moduleTypeCodes(lrs2228) 3
set moduleTypeCodes(c1205)   4

# Resolutions for each module type:

set moduleChannels(ph7xxx)  4096
set moduleChannels(ad811)   4096
set moduleChannels(lrs2249) 2048
set moduleChannels(lrs2228) 4096
set moduleChannels(c1205)   4096


#  Processes a module.. this will be front ended by a simple 
# function for each module type.
#
#  Parameters:
#    command -  The command that invoked us.
#    args    -  the rest of the arguments.
proc module {command tail} {
    set args $tail
    set moduleType                  $command
    set moduleName                  [lindex $args 1]
    set ::channelCounts($moduleName)  $::moduleChannels($moduleType)
    set ::moduleTypes($moduleName)    $::moduleTypeCodes($moduleType)

    set idindex [lsearch -exact $args "-id"]
    if {$idindex != -1} {
	set ::moduleIds($moduleName)  [lindex $args [incr idindex]]
    }
}



# Process ph7xxx (phillips 7xxx series digitizers.

proc ph7xxx args {
    module ph7xxx $args
}

# Proces ortec ad811 digitizers.

proc ad811 args {
    module ad811 $args

}

#  LRS 2249:

proc lrs2249 args {
    module lrs2249 $args
}

#  LRS 2228

proc lrs2228 args {
    module lrs2228 $args
}
# Caen 1205 QDC

proc c1205 args {
    module c1205 $args
}


#  For now scalers are ignored.

proc lrs2551 args {
}
proc c257 args {
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

#  The CANE C1205 module requires special handling because it provides 3
#  potential channels of data for each of the input channels.  This is because
#  it provides 3 ranges of value per channel.   We are going to take the parameters
#  passed in as a base name from which a 3 element 'treearray' will be built.
#  I say 'treearray' because we're not going to actually create tree parameters here,
#  just the parameters.
#  Each parameter will also have a 1d spectrum associated with it.,
#  @param id          - The module id programmed via the -id configuration option.
#  @param number      - The module number used in the mapparam command.
#  @param type        - The module type.
#  @param parametesr  - list of parameter base name.
#
proc mapC1205Channels {id number type parameters} {
    #
    # Produce the parameter list.  Note that if a parameter is '' it produces
    # three un-created parameters.

    if {$parameters ne ""} {
	set expandedParameters [list]
	foreach parameter $parameters {
	    if {$parameter eq ""} {
		lappend expandedParameters ""
		lappend expandedParameters ""
		lappend expandedParameters ""
	    } else {
		foreach suffix {.low .mid .high} {
		    lappend expandedParameters $parameter$suffix
		}
	    }
	}
	# Create the parameter map, which also creates the parameter:

	parammap -add $number $type $id $expandedParameters

	# Now create a spectrum for each non blank parameter:
	
	foreach parameter $expandedParameters {
	    if {$parameter ne ""} {
		spectrum $parameter 1 $parameter {{0 4095 4096}}
	    }
	}
    }

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

	#  The 1205 module is special because it creates 3 parameters
	#  for each of the 16 channels... looking like a tree array per channel.
	#
	if ($type eq ::$moduleTypeCodes(c1205) {
	    mapC1205Channels $id $moduleNumber $type $parameters($module)
	} else {

	    if {[array names parameters $module] ne ""} {
		parammap -add $moduleNumber $type $id  $parameters($module)
		foreach parameter $parameters($module) {
		    spectrum $parameter 1 $parameter $axisspec
		}
	    }
	}
	incr moduleNumber
    }
}


source $daqconfig

set modules [getModuleList]
createMapAndSpectra  $modules

