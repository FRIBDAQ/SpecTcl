#
#   Setup SpecTcl's unpacking.
#    This script sets up SpecTcl's auto unpack system for data
#    from the VM-USB.
#    The script processes the user's daqconfig file
#    using the configFile script.
#
#  That script is going to create the following global arrays that we will use:
#    adcConfiguration - indexed by digitizer name, this contains the 
#                       virtual slot numbers of digitizers that have them and -1 for those that
#                       don't.
#    readoutDeviceType- indexed by digitizer name, this contains the device type (a small integer)
#    stackOrder       - indexed by stack name, this contains the order in which modules
#                       are read in a stack.
#    stackNumber      - indexed by stack name, this contains the number of that stack.
#
#  The users's configuration file is supposed to have created the array:
#    adcChannels      - The adcChannels array is supposed to be indexed by digitizer
#                       name and provide SpecTcl parameter names for each channel of the
#                       digitizer.
#                       
# As a bonus we set up raw parameter spectra for each parameter that is defined.
#
# 
# TODO:  Make TDC1x90
# parameters/spectra appropriately given the depth for each
#        TDC channel

#
#  For testing outside of SpecTcl
# 

if {[info globals SpecTclHome] eq ""} {
    proc paramMap args {
	puts "paramMap $args"
    }
    
    proc spectrum args {
	puts "spectrum $args"
    }
    proc parameter args {
	puts "parameter $args"
    }
    proc stackMap args {
	puts "stackMap $args"
    }
    proc sbind args  {
	puts "sbind $args"
    }
}


puts "In SpecTcl Setup"

set here [file dirname [info script]]
source [file join $here configFile.tcl]

puts "configFile sourced"

configClear

puts "Configuration cleared"

if {[catch {configRead [file join $here daqconfig.tcl]} msg]} {
    puts "Error in configuration file read: $msg"
}

puts "Configuration read"

set channelCount($typeCAEN)   4096
set channelCount($typeHYTEC)  8192
set channelCount($typeMADC32) 4096;	# Currently only 12 chans.
set channelCount($typeTDC1x90) 16384;   # for now this is the # of channels in a tdc spec
set channelCount($typeV977)    16;      # for a bit mask spec

#-----------------------------------------------------------------------------
# Creates a 1-d spectrum.
#

proc makeSpectrum {paramname channels} {
    set low 0
    set high [expr $channels-1]

    spectrum $paramname 1 $paramname  [list [list $low $high $channels]]
}


#----------------------------------------------------------------------------
#
#   The V1x90 maps are a bit special.
#   Each 'parameter' is really a parameter array.
#
#  Two additional lists are used to describe how to set up the module
#
#   CAENV1x90(name) is a three element list consisting of
#                   in order : reference channel number, 
#                              depth (number of elements in each parameter array)
#                              channelcount - Number of channels in the TDC.
#   V1x90Windows(name) is a three element list that describes the module timing
#                      all values are floating point ns.  These are used
#                      to define the limits and channel counts in each spectrum.
#                      The list elements are in order:
#                            The window width
#                            The window offset relative to the trigger.
#                            The resolution of each channel.
#
#                      the low limit of the spectrum is the offset.
#                      the high limit of the spectrum is offset+width-1
#                      The number of channels is width/resolution
#
#  Parameters:
#       baseparam  - number of the first parameter we can create.
#       name       - Name of the module we're processing
#  Returns:
#       next free parameter number.
#                     
proc buildV1x90Maps {baseparam name} {
    puts "BuildV1x90Maps"
    set tdcInfo    $::CAENV1x90($name)
    set tdcRes     $::V1x90Windows($name)
    set vsn        $::adcConfiguration($name)
    set parameters $::adcChannels($name)

    # Figure out spectrum low/hi and channel count:

    set width  [lindex $tdcRes 0]
    set offset [lindex $tdcRes 1]
    set res    [lindex $tdcRes 2]

    puts "List : $tdcRes"

    set low $offset
    set hi  [expr $offset + $width - 1.0]
    set chans [expr int($width/$res)]
    puts "Spectrum: $low $hi $chans"

    # Pull out the stuff we need from the CAENV1x90 list:

    set depth [lindex $tdcInfo 1]

    #  Now loop over the parameters, making the arrays
    #  and spectra.  Parameter arrays are tree parameters in the decoder
    #  so they have names like basename.%d  That's good for depth 1-9.
    #

    foreach basename $parameters  {
	for {set i 0} {$i < $depth} {incr i} {
	    set pname [format "$basename.%d" $i]
	    parameter $pname $baseparam
	    incr baseparam
	    spectrum $pname 1 $pname "{[list $low $hi $chans]}"
	}
	
    }
    paramMap $name $::readoutDeviceType($name) $vsn [list]
    return $baseparam
}
#---------------------------------------------------------------------------
# Build the channel maps and spectrum for a V977
# We're going to build a parameter map with only one parameter
# and a bit map spectrum for the device.
# Parameters:
#    param   - Number of the first usable parameter.
#    name    - module name
# Returns:
#    Number of next usable parameter.
#
proc buildv977Map {param module} {

    puts {IN buildv977map}

    # Create the parameter:

    set channels $::adcChannels($module)

    set parameterName [lindex $channels 0]
    puts "Setting parameter $param -> $parameterName"
    parameter  $parameterName $param
    incr param

    # and it's mapping.

    echo "Param map $module $::readoutDeviceType($module) 0 $channels"
    paramMap $module $::readoutDeviceType($module) 0  $channels
    echo "Parammap done"


    # and the spectrum , a 16 bit bitmask spectrum.

    spectrum $parameterName b $parameterName {{0 15 16}}

    return $param
}
#----------------------------------------------------------------------------
# Build the channel maps, spectcl parameters and raw spectra from 
# the adcConfigurtion, readoutDeviceType and adcChannels information.
# This will all be driven by the adcCahnnels array.
#
# Parameters:
#   param  - the number of the first parameter.

proc buildChannelMaps param {
    puts "Building channel maps"
    foreach module [array names ::adcChannels] {
	puts "Processing $module"
	if {$::readoutDeviceType($module) eq $::typeTDC1x90} { 
	    puts "V1x90 $module"
	    set param [buildV1x90Maps $param $module]

	} elseif {$::readoutDeviceType($module) eq $::typeV977} {

	    puts "V977 $module"
	    set param [buildv977Map $param $module]

	    #  Give SpecTcl the parameter map for the module:
	} else {
	    set vsn        $::adcConfiguration($module)
	    set type       $::readoutDeviceType($module)
	    set resolution $::channelCount($type)
	    set channels   $::adcChannels($module)
	    
	    # Make the parameters and spectra:
	    
	    foreach parameter $channels {
		parameter $parameter $param
		incr param
		makeSpectrum $parameter $resolution
	    }
	    paramMap $module $type $vsn $channels
	}
    }
}


#--------------------------------------------------------------------------
# Build the stack order maps.  These define, for each stack,
# the order in which the modules read by that stack appear.
# The assumption is that stack 1 is a scaler stack always if used at all.
#
proc buildStackMaps {} {
    puts "In buildStackMaps!!"

    foreach stack [array names ::stackNumber] {
	
	set stackno $::stackNumber($stack)
	if {$stackno != 1} {
	    puts "Stackmap for $stack : $stackno list: $::stackOrder($stack)"
	    stackMap $stackno  $::stackOrder($stack)
	}
    }
}

#--------------------------------------------------------------------------
#
#  Setup SpecTcl
#


puts "Building channel maps"
buildChannelMaps 20
puts "Building stack maps"

buildStackMaps
puts "Binding spectra to Xamine"

sbind -all


