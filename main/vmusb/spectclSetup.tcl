package provide vmusbsetup 1.0
package require vmusbconstants
package require vmusbconfigfile 
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


#puts "In SpecTcl Setup"
#
#set here [file dirname [info script]]
#source [file join $here configFile.tcl]

#puts "configFile sourced"

configClear




#-----------------------------------------------------------------------------
# Creates a 1-d spectrum.
#

proc makeSpectrum {paramname channels} {
    set low 0
    set high [expr $channels-1]

    spectrum $paramname 1 $paramname  [list [list $low $high $channels]]
    treeparameter -create $paramname $low $high $channels ""; #  we don't know the units.
}
##
# createFreezeButton
#
#   When the first v1729 is seen in the stack, the freeze button is created:
#
proc createFreezeButton {} {
    if {![winfo exists .freeze]} {
	set freezeFadcSpectra 0
	checkbutton .freeze -text "Freeze fadc spectra"\
	    -variable freezeFadcSpectra \
	    -onvalue 1 -offvalue 0
	pack .freeze
	
    }
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
	    treeparameter -create $pname $low $hi $chans ""
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
    treeparameter -create $parameterName 0 15 16 ""

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
#---------------------------------------------------------------------------
#
#  Build channel maps for the V1729
#  In this case, the channels are really place holders for 
#  spectra.
# parameters:
#    param - Number of first available parametr.
#    name  - Name of the module we are managing.
# Returns:
#    next available parameter number.
#
proc buildV1729Map {param name} {
    global v1729postTriggers;	# Post trigger value for time reorder.
    global adcChannels;		# Has parameter names.

    # Make a parameter and 2048 2K spectra for each parameter.

    foreach paramName $adcChannels($name) {
	parameter $paramName $param
	for {set i 0} {$i < 2048} {incr i} { 
	    set spectrumName [format %04d.%s $i $paramName]
	    spectrum $spectrumName 1 $paramName {{0 2047 2048}}
	}
	incr param
    }
    paramMap $name $::typeV1729 $v1729postTriggers($name) $adcChannels($name)
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
#--------------------------------------------------------------------------
#
#  build spectra and channel maps for MADC32's the only hink here is that
#  if it's set the madcResolutions($name) array element overrides
#  channelCount($typeMADC32).
#
# Parameters:
#   param  - First free parameter.
#   name   - module name.
# Returns:
#   next available parameter.
#
proc buildMADC32Map {param name} {
    array set resolutions [list 2k 2048 4k 4096 4khires 4096 \
			       8k 8192 8khires 8192]
    set resolution $::channelCount($::typeMADC32); # default resolution.

    if {[array names ::madcResolutions $name] ne ""} {
	set resolution $resolutions($::madcResolutions($name))
    }
    return [makeParamsSpectraAndMap $param $name $::typeMADC32 $::adcChannels($name)  $resolution ]
}
#------------------------------------------------------------------
#
# Build a simple parameter/spectrum set and channel maps:
#
# Parameters:
#   param   - first free parameter.
#   name    - Name of module.
#   type    - Module type.
#   channels - Names of channels.
#   resolution - Spectrum channel count. 
#
proc makeParamsSpectraAndMap {param name type channels resolution} {
    set vsn        $::adcConfiguration($name)

    
    # Make the parameters and spectra:
    
    foreach parameter $channels {
	parameter $parameter $param
	incr param
	makeSpectrum $parameter $resolution
    }
    paramMap $name $type $vsn $channels
    return $param
    
}

#----------------------------------------------------------------------------
# build the channel maps, spectcl parameters and raw spectra from 
# the adcConfigurtion, readoutDeviceType and adcChannels information.
# This will all be driven by the adcChannels array.
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
	} elseif {$::readoutDeviceType($module) eq $::typeMADC32} {
	    set param [buildMADC32Map $param $module]
	} elseif {$::readoutDeviceType($module) eq $::typeV1729} {
	    set param [buildV1729Map $param $module]
	    createFreezeButton

	} else {
	    
	    set vsn        $::adcConfiguration($module)
	    set type       $::readoutDeviceType($module)
	    set resolution $::channelCount($type)
	    set channels   $::adcChannels($module)

	    set param [makeParamsSpectraAndMap $param $module $type $channels $resolution]

	    if 0 {
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

proc vmusbConfig filename {
    configRead $filename

    puts "Building channel maps"
    catch {buildChannelMaps 20} msg
    puts $msg

    puts "Building stack maps"
    
    catch {buildStackMaps} msg
    puts $msg

    puts "Binding spectra to Xamine"
    
    sbind -all
}


