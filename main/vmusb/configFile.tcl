#
#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2005.
#
#    You may use this software under the terms of the GNU public license
#    (GPL).  The terms of this license are described at:
#
#     http://www.gnu.org/licenses/gpl.txt
#
#     Author:
#             Ron Fox
#	     NSCL
#	     Michigan State University
#	     East Lansing, MI 48824-1321
#

#  This file provides software that will decode a configuration file.
#  the only thing we are interested in are the adc lines.  From them
#  we are interested in the names, types, order and geographical slots.
#  The following global arrays will be created arrays are indexed by device
#  name:
#
#    adcConfiguration - Contains virtual slot numbers for devices that have them
#                       -1 for those that don't.
#    readoutDeviceType - Contains the device type code for each device.
#                        See below for device types.
#    stackOrder        - Indexed by stack name.  The order of the modules
#                       in the stack.  If a chain is in the list,
#                       its name is replaced by the order of the modules
#                       in the chain.
#    stackNumber       - Indexed by stack name. Contains the stack number.
#                       The stack number is:
#                       0  - Un numbered nim1 triggered stack.
#                       1  - Un numbered scaler triggered stack.
#                       n  -  value of -stack  config parameter.
#
#  Global variables that are used but not important to the outside world:
#      chainOrder      - For CAENchain, the order in which the
#                        devices appear, indexed by chain.
#
#

package provide vmusbconfigfile 1.0
package require vmusbconstants


#  We create as well spectra for each single parameter, and corresponding
#  pairs of n's.


# we will be maintaining a global array
# array will be indexed by adc name.  Each array will contain the
# geo address assigned to the adc.

array set adcConfiguration [list]; # empty array.

#-----------------------------------------------------------------
#
#   clear the configuration.
#   The catch is required in case the config does not (yet) exist.
#
proc configClear {} {
    global adcConfiguration
    array set adcConfiguration [list]

}
#------------------------------------------------------------------
#  Read a configuration file.  This is just a source.
#
proc configRead filename {
    puts "Sourceing the config file:"
    if {[catch {uplevel #0 source $filename} msg]} {
	puts "Configuration file failed $msg"
    }
    puts "Sourced"
}


#-------------------------------------------------------------------
# adcCreate
#   Creates a new adc; Creates and adc with the default geo value (0).
#   presumably, the  adcConfig will override this value later.
#
proc adcCreate tail {
    puts "adcCreate $tail"
    global adcConfiguration

    set name [lindex $tail 0]
    set adcConfiguration($name) 0
    set ::readoutDeviceType($name) $::typeCAEN
}
#-------------------------------------------------------------------
# adcConfig
#   Configures an adc. For now we are only interested in the
#   -geo switch which will have our virtual slot number.
#
proc adcConfig tail {
    global adcConfiguration

    set name [lindex $tail 0]
    set config [lrange $tail 1 end]
    foreach {key value} $config {
	if {$key eq "-geo"} {
	    set ::adcConfiguration($name) $value
	}
    }
}



#----------------------------------------------------------------
#
#  Configures a v1729 FADC.  We're just looking for the
#  -chanmask parameter which we'll put in this module's
#  v1729channelMasks variable.
#  
proc v1729config tail {
    global v1729channelMasks
    global v1729postTriggers
    
    set name [lindex $tail 0]
    set options [lrange $tail 1 end]
    puts "Tail: $options"
    foreach {optname optval} $options {
	if {$optname eq "-posttrigger"} {
	    set ::v1729postTriggers($name) $optval
	}
    }
}

#-----------------------------------------------------------------
#
#  Create a v1729 and then process the configuration parameters.
#
proc v1729create tail {
    global readoutDeviceType
    global v1729channelMasks

    set name [lindex $tail 0]

    set ::adcConfiguration($name)   -1
    set ::readoutDeviceType($name) $::typeV1729
    set ::v1729postTriggers($name) 64
    v1729config $tail
}
#---------------------------------------------------------------
# Support the vmusb dummy module type.  We support it
# unless it's scalers are being read:
#
proc vmusb args {
    # Name of the modules is always after the subcommand:

    set name [lindex $args 1]
    set ::adcConfiguration($name) -1
    set ::readoutDeviceType($name) $::typeVMUSB
    
}

#------------------------------------------------------------------
#
# v1729 - processes the V1729 command.  This requires additional
#         data. Specifically we need to know the channel enables
#         mask which will go into the global atrray
#         v1729channelMasks(name).  This is initialized the first time
#         a module is seen to 0xf which enables all 4 channels.
#         but can be modified by the -chnmask configuration switch.
#
proc v1729a args {
    set subcommand [lindex $args 0]
    set tail       [lrange $args 1 end]

    if {$subcommand eq "create"} {
	v1729create $tail
    }
    if {$subcommand eq "config"} {
	v1729config $tail
    }

}

#------------------------------------------------------------------
#
# v977 - Processes the v977 command, dispatches to the create/config
#        commands.  
#
proc v977 args {
    set subcommand [lindex $args 0]
    set tail       [lrange $args 1 end]
    set name       [lindex $tail 0]
    
    if {$subcommand eq "create"} {
	adcCreate $tail
    }
    if {$subcommand eq "config"} {
	adcConfig $tail
    }
    set ::readoutDeviceType($name) $::typeV977

}
#------------------------------------------------------------------
# hinp   Process the hinp command which configures the HINP XLM
#        firmware/chip front end devices.
#
proc hinp args {
    set subcommand [lindex $args 0]
    set tail       [lrange $args 1 end]

    if {$subcommand eq "create"} {
	set name [lindex $tail 0]
	set ::adcConfiguration($name) 0; # Change this to the -id later.
	set ::readoutDeviceType($name) $::typeHINP
    }
}

#------------------------------------------------------------------
# PSD   Process the psd command which configures the PSD XLM
#        firmware/chip front end devices.
#
proc psd args {
    set subcommand [lindex $args 0]
    set tail       [lrange $args 1 end]

    if {$subcommand eq "create"} {
	set name [lindex $tail 0]
	set ::adcConfiguration($name) 0; # Change this to the -id later.
	set ::readoutDeviceType($name) $::typePSD
    }
}
#-------------------------------------------------------------------
#
#  caenv965  Process the caenv956 command which handles CAEN
#            dual range qdc's.  This is just the same as the
#            adc command but the type is different.
#            In fact we use the adcConfig proc to do our
#            configuration magic.
#
proc caenv965 args {
    set subcommand [lindex $args 0]
    set tail       [lrange $args 1 end]
    
    if {$subcommand eq "create"} {
	set name [lindex $tail 0]
	set ::adcConfiguration($name)  0
	set ::readoutDeviceType($name) $::typeCAENDual
    
    }
    if {$subcommand eq "config"} {
	adcConfig $tail
    }
}
#-------------------------------------------------------------------
# adc - processes the adc command dispatches to the create/config
#       commands.  All other subcommands are no-ops.
#
proc adc args {
    puts "adc $args"
    set subcommand [lindex $args 0]
    set tail       [lrange $args 1 end]
    puts "Subcommand $subcommand tail: $tail"
    if {$subcommand eq "create"} {
	puts "Create"
	adcCreate $tail
    }    
    if {$subcommand eq "config"} {
	puts "config"
	adcConfig $tail
    }
    puts "done"
}

proc caenv792 args {
    adc {*}$args
}


#----------------------------------------------------------------
#  The hytec command processes creation and configuration commands
#  for the  hytec N2530 ADC. 
#
proc hytec args {
    set subcommand [lindex $args 0]
    set name       [lindex $args 1]
    
    set ::readoutDeviceType($name) $::typeHYTEC
#    set ::adcConfiguration($name)  -1;	# There is no geo associated with this adc.

    # The hytec readout uses a marker word to simulate a vsn/geo.
    # the config param for this is "-id" followed by the value of
    # the marker.

    set idindex [lsearch -exact $args "-id"]
    if {$idindex != -1} {
	incr idindex
	set ::adcConfiguration($name) [lindex $args $idindex]
    }
}
#--------------------------------------------------------------
#
#  The mase command processes the creation and configuration 
#  commands for the mase command.
#  This device has no geographical address.
#
proc mase args {
    set subcommand [lindex $args 0]
    set name       [lindex $args 1]

    set ::readoutDeviceType($name)  $::typeMase
    set ::adcConfiguration($name)   -1;	# no geo.

    # at this point we are not interested in the set of configuration 
    # options that may have been set.
}

#---------------------------------------------------------------
# The madc command processes the creation and configuration of
# Mesytec 32 channel ADC modulees.
#
proc madc args {
    set subcommand [lindex $args 0]
    set name       [lindex $args 1]

    set ::readoutDeviceType($name) $::typeMADC32

    # The config or create subcommand have the 
    # -id config which sets the 'vsn' for this module.

    if {($subcommand eq "create") || ($subcommand eq "config")} {
	set ididx [lsearch -exact $args "-id"]
	if {$ididx != -1} {
	    incr ididx
	    set ::adcConfiguration($name) [lindex $args $ididx]
	}
	# Save the -resolution parameter in madcResolutions(name)
	
	set residx [lsearch -exact $args "-resolution"]
	if {$residx != -1} {
	    incr residx
	    set ::madcResolutions($name) [lindex $args $residx]
	}
    }
}
#---------------------------------------------------------------
# The mtdc command processes the creation and configuration of
# Mesytec 32 channel TDC modules.
#
proc mtdc args {
    set subcommand [lindex $args 0]
    set name       [lindex $args 1]

    set ::readoutDeviceType($name) $::typeMTDC32

    # The config or create subcommand have the
    # -id config which sets the 'vsn' for this module.

    if {($subcommand eq "create") || ($subcommand eq "config")} {
	set ididx [lsearch -exact $args "-id"]
	if {$ididx != -1} {
	    incr ididx
	    set ::adcConfiguration($name) [lindex $args $ididx]
	}
	# Save the -resolution parameter in mtdcResolutions(name)

	set residx [lsearch -exact $args "-resolution"]
	if {$residx != -1} {
	    incr residx
	    set ::mtdcResolutions($name) [lindex $args $residx]
	}
    }
}
#---------------------------------------------------------------
#
#  The tdc1x90 command processes the creation and
#  configuration of CAEN V1x90 modules.
#  In addition to the standard processing,
#  we will look for the following:
#   -refchannel - the channel number that contains the
#                 digitized gate (default 0), 
#   -depth      - The number of hits to retain for each channel.
#                  (default 16)
#   -channelcount - The number of channels the TDC has.
#                   This could be 16, 32, 64, or 128
#                   and determines the width of the channel field
#                   and data fields in the TDC data words.
#   -vsn             The virtual slot numbger.
#   -window         Width of the trigger matching window (in ticks).
#   -offset         Offset from the trigger to star tof window (in ticks)
#   -edgeresolution Resolution of LSB (enumerated as nnnps).
#
#  These get saved for SpecTcl as follows:
#   
#   CAENV1x90(name) as a list [list $refchannel $depth $channelcount]
#   adcConfiguration(name) the virtual slot.
#   V1x90Widnwos(name) a list containing the width, offset and resolution
#                      in floating point ns.
#
proc tdc1x90 args {
    set subcommand [lindex $args 0]
    set name       [lindex $args 1]

    # Default the -depth/-refchannel if needed:

    if {[array names ::CAENV1x90 $name] eq ""} {
	set ::CAENV1x90($name) [list -1 16 128]
    }

    # Default the trigger window and offset for now

    if {[array names ::V1x90Windows $name] eq ""} {
	puts "Making v1x90windows for $name"
	set ::V1x90Windows($name) [list 1000.0 -1000.0 0.1]
	puts "$::V1x90Windows($name)"
    }
    
    set ::readoutDeviceType($name) $::typeTDC1x90

    #  Both the create and the config commands can configure:

    if {($subcommand eq "create") || ($subcommand eq "config")} {
	set refidx [lsearch -exact $args "-refchannel"]
	set depthidx [lsearch -exact $args "-depth"]
	set chansidx [lsearch -exact $args "-channelcount"]

	set winidx   [lsearch -exact $args "-window"]
	set offidx   [lsearch -exact $args "-offset"]
	set residx   [lsearch -exact $args "-edgeresolution"]

	set slotidx  [lsearch -exact $args "-vsn"]

	if {$refidx != -1} {
	    incr refidx
	    set refchan [lindex $args $refidx]
	    set ::CAENV1x90($name) [lreplace $::CAENV1x90($name) 0 0 $refchan]
	} 

	if {$depthidx != -1} {
	    incr depthidx
	    set  depth [lindex $args $depthidx]
	    set ::CAENV1x90($name) [lreplace $::CAENV1x90($name) 1 1 $depth]
	}
	if {$chansidx != -1} {
	    incr chansidx
	    set  chans [lindex $args $chansidx]
	    set ::CAENV1x90($name) [lreplace $::CAENV1x90($name) 2 2 $chans]
	}

	#  The timing information; all is in nsec.

	if {$winidx != -1} {
	    incr winidx
	    set windowticks [lindex $args $winidx]
	    set windowns    [expr $windowticks * 25.0]
	    set ::V1x90Windows($name) \
		[lreplace $::V1x90Windows($name) 0 0 $windowns]
	}

	if {$offidx != -1} {
	    incr offidx
	    set offsetTicks [lindex $args $offidx]
	    set offsetNs    [expr $offsetTicks * 25.0]
	    puts "replacing v1x90windows $name element1"
	    set ::V1x90Windows($name) \
		[lreplace $::V1x90Windows($name) 1 1 $offsetNs]
	}

	if {$residx != -1} {
	    incr residx
	    set restext [lindex $args $residx]
	    scan $restext "%fps" resps
	    set resns [expr $resps/1000.0]
	    set ::V1x90Windows($name) \
		[lreplace $::V1x90Windows($name) 2 2 $resns]
	}
	if {$slotidx != -1} {
	    incr slotidx
	    set ::adcConfiguration($name) [lindex $args $slotidx]
	}
    }

    
}


#---------------------------------------------------------------
#  We need to use this command to fill in the chainOrder array of 
#  the order of modules in the chain.  This allows the stack command
#  to accurately figure out the module order in the presence of
#  chains that aggregate several V785 readout 
#
#  - Extract the chain name.
#  - If the args contains -modules extract the next list item
#    and use it as the value for the chain's chainOrder entry.
#
proc caenchain args {
    set name     [lindex $args 1]
    set modIndex [lsearch -exact $args "-modules"]
    if {$modIndex != -1} {
	incr modIndex
	set ::chainOrder($name) [lindex $args $modIndex]
    }
}
#---------------------------------------------------------------
#
#  For each of the stacks, the stack command must create a
#  stackNumber and stackOrder entry.
#
proc stack args {
    set stackname [lindex $args 1]
    
    # Hunt for the modules config and fill in stackOrder if so.
    # Chains are dealt with here too.
    
    set moduleIndex [lsearch -exact $args "-modules"]

    if {$moduleIndex != -1} {
	incr moduleIndex
	set moduleOrder [lindex $args $moduleIndex]
	foreach name $moduleOrder {
	    if {[array names ::chainOrder $name] ne ""} {
		foreach item $::chainOrder($name) {
		    lappend ::stackOrder($stackname) $item
		}
	    } else {
		lappend ::stackOrder($stackname) $name
	    }
	}
    }

   # Figure out the stack number. Simplifying assumption is that
    # users of the nim1, and scaler trigger will not specify a -stack
    # entry or if they do, it will be the correct stack number.

    set trigIndex [lsearch -exact $args "-trigger"]
    if {$trigIndex != -1} {
	incr trigIndex
	set trigger [lindex $args $trigIndex]
	if {$trigger eq "nim1"} {
	    set ::stackNumber($stackname) 0
	} elseif {$trigger eq "scaler"} {
	    set ::stackNumber($stackname) 1
	}
    }  
    set stackIndex [lsearch -exact $args "-stack"]
    if {$stackIndex != -1} {
	incr stackIndex
	set ::stackNumber($stackname) [lindex $args $stackIndex]
    }    
}


# 
# The configuration file can have serveral commands we don't need to
# deal with in SpecTcl...
# these are defined as procs that do nothing:
#
proc sis3820 args {
}
proc v830 args {
}

proc v1495sc args {
}


proc marker args {
}
proc sis3804 args {
}

proc addtcldriver name {
    proc $name args {}
}


