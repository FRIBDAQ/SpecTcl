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

if {[catch {configRead [file join ~ config  daqconfig.tcl]} msg]} {
    puts "Error in configuration file read: $msg"
}

puts "Configuration read"

set channelCount($typeCAEN)   4096
set channelCount($typeHYTEC)  8192
set channelCount($typeMADC32) 4096;	# Currently only 12 chans.
set channelCount($typeTDC1x90) 16384;   # for now this is the # of channels in a tdc spec
set channelCount($typeV977)    16;      # for a bit mask spec
set channelCount($typeMase)    8192;    # Spectrum channels for MASE.
set channelCount($typeHINP)    16384;	# Num channels in a default HINP spectrum.
set channelCount($typePSD)     8192;	# Num channels in a default PSD spectrum.

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


    # Create the parameter:

    set channels $::adcChannels($module)

    set parameterName [lindex $channels 0]
    parameter  $parameterName $param
    incr param

    # and it's mapping.

    paramMap $module $::readoutDeviceType($module) 0  $channels
    echo "Parammap done"


    # and the spectrum , a 16 bit bitmask spectrum.

    spectrum $parameterName b $parameterName {{0 15 16}}

    return $param
}
#----------------------------------------------------------------------------
# Build the channel maps and spectra for a MASE module.
# This module can have a large number of channels.. so we are not going
# to build a full parameter map.
# Global variable used:
#   maseCOBCount($name)   - Number of Cobs for this module.
#   maseCHBCounts($name)  - Number of CHB's for each COB.
#   adcChannels($name)    - The base name for the parameters we are going to make.
# For each CHB we generate the full complement of 32 parameters/raw spectra.
#   adcChannels($name)    - the basename for those parameters.  e.g. 
#                           if adcChannels($name) is 'george'
# we'll generate parameters/spectra of the form:
#    george.cob.chb.chan  
# Parameters:
#    param - The number of the first available parameter.
#    module- Nameo f the module being configured.
# Returns:
#    Number of the next usable parameter.
#
proc buildMaseMap {param module} {

    puts "In Mase"
    set basename $::adcChannels($module)
    set cobcount $::maseCOBCount($module)
    set chblist  $::maseCHBCounts($module)
    set channels $::channelCount($::typeMase)

    puts "$basename : $cobcount $chblist $channels"

    #  We're going to be a bit tricky;  The parameter map will be named by the
    # parameter _basename_ that will allow the unpacker to locate the parameter
    # and build itself an appropriate sparse set of tree parameters.
    # there won't actually be a parameter map in the parammap (empty parameter list).

    paramMap $basename $::readoutDeviceType($module) 0 [list]

    # now build the parameters/spectra

    for {set cob 0} {$cob < $cobcount} {incr cob} {
	set chbcount [lindex $chblist $cob]
	for {set chb 0} {$chb < $chbcount} {incr chb} {
	    for {set chan 0} {$chan < 32} {incr chan} {
		set parameterName [format $basename.%02d.%02d.%02d $cob $chb $chan]
		parameter $parameterName $param
		incr param
		makeSpectrum $parameterName $channels
	    }
	}
    }
    return $param
}
#
#   Hinp's unpacker takes a base name from 
#   adcChannels and takes the chip mapping from 
#   HINPChips
#
proc buildHINPMap {param module} {
    set basename $::adcChannels($module)
    set chipMap  $::HINPChips($module)
    set chanSize $::channelCount($::typeHINP)
    incr chanSize -1
    set channels $::channelCount($::typeHINP)

    paramMap $basename $::typeHINP 0 [list]
    foreach chip $chipMap {
	for {set i 0} {$i < 16} {incr i} {
	    set EParamName \
		[format "%s.e.%02d.%02d" $basename $chip $i]
	    set TParamName \
		[format "%s.t.%02d.%02d" $basename $chip $i]
	    parameter $EParamName $param
	    incr param
	    parameter $TParamName $param
	    incr param

	    spectrum $EParamName 1 $EParamName "{0 $chanSize  $channels}"
	    spectrum $TParamName 1 $TParamName "{0 $chanSize  $channels}"

	}
    }
    return $param
}

#
#   PSD's unpacker takes a base name from 
#   adcChannels and takes the chip mapping from 
#   PSDChips
#
proc buildPSDMap {param module} {
    set basename $::adcChannels($module)
    set chipMap  $::PSDChips($module)
    set chanSize $::channelCount($::typePSD)
    incr chanSize -1
    set channels $::channelCount($::typePSD)

    paramMap $basename $::typePSD 0 [list]
    foreach chip $chipMap {
	for {set i 0} {$i < 16} {incr i} {
	    set AParamName \
		[format "%s.a.%02d.%02d" $basename $chip $i]
	    set BParamName \
		[format "%s.b.%02d.%02d" $basename $chip $i]
	    set CParamName \
		[format "%s.c.%02d.%02d" $basename $chip $i]
	    set TParamName \
		[format "%s.t.%02d.%02d" $basename $chip $i]
	    parameter $AParamName $param
	    incr param
	    parameter $BParamName $param
	    incr param
	    parameter $CParamName $param
	    incr param
	    parameter $TParamName $param
	    incr param

	    spectrum $AParamName 1 $AParamName "{0 $chanSize  $channels}"
	    spectrum $BParamName 1 $BParamName "{0 $chanSize  $channels}"
	    spectrum $CParamName 1 $CParamName "{0 $chanSize  $channels}"
	    spectrum $TParamName 1 $TParamName "{0 $chanSize  $channels}"

	}
    }
    return $param
}
#----------------------------------------------------------------------------
#
#  Build channels and maps for a CAEN Dual range module.
#  the adc parameters give the base names for parameters .h and .l for
#  high and low ranges respectively.
# Parameters:
#   param   - First parameter number to use.
#   name    - Name of the module.
# Returns:
#   next unused parameter number.
#
proc buildCAENDualMap {param name} {
    set vsn        $::adcConfiguration($name)
    set resolution $::channelCount($::typeCAEN)
    set channels   $::adcChannels($name)
    
    set parameterList [list]



    foreach parameter $channels {
	parameter $parameter.h $param
	makeSpectrum $parameter.h $resolution
	lappend parameterList $parameter.h
	incr param

	parameter $parameter.l $param
	makeSpectrum $parameter.l $resolution
	lappend parameterList $parameter.l
	incr param
    }
    paramMap $name $::typeCAEN $vsn $parameterList

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
	} elseif {$::readoutDeviceType($module) eq $::typeMase} {
	    puts "MASE module"
	    set param [buildMaseMap $param $module]
	} elseif {$::readoutDeviceType($module) eq $::typeCAENDual} {
	    set param [buildCAENDualMap $param $module]
	} elseif {$::readoutDeviceType($module) eq $::typeHINP} {
	    set param [buildHINPMap $param $module]
	} elseif {$::readoutDeviceType($module) eq $::typePSD} {
	    set param [buildPSDMap $param $module]
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


