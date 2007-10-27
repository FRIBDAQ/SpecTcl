#  PeakStatistics.tcl
# 
#   This module contains entry points to caclulate peak statistics.
#   The statistics are required for the calibration module.  Peak statistics
#   are calculated and returned in parameter coordinates.
#
#   Public entries:
#      ComputeCentroid  - Computes the centroid of a region of a spectrum.
#   Private functions:
#       TranslateCoordinate - Translates a coordinate from one axis system to
#                             another.
#
#
namespace eval CalibrationGUI {

    # TranslateCoordinate
    #
    #   Translate a coordinate from one axis system to another 
    # Action: 
    #      sourcefraction <-- (sourcecoord - source.low)/(source.hi-source.low)
    #      result <-- sourcefraction*(target.hi-target.low) + target.low
    #      return result.
    # Parameters:
    #   SourceWindow [ list - in ]
    #      A low/high pair that represents an interval in the source coordinate 
    #      system. param: 
    #   TargetWindow [ list - in ]     
    #      A low/high pair that represents a window corresponding to SourceWindow 
    #      in the target coordinate system. param: 
    #   SourceValue [ real - in ]     
    #      The source coordinate value. 
    # Returns:
    #    The transformed coordinate.
    #
    proc  TranslateCoordinate {SourceWindow TargetWindow SourceValue} {

	#   Unpack the source and target windows:

	set SourceLow  [lindex $SourceWindow 0]
	set SourceHigh [lindex $SourceWindow 1]
	
	set TargetLow  [lindex $TargetWindow 0]
	set TargetHigh [lindex $TargetWindow 1]

	#  What fraction of the way into the source window is the source value:
	#
	set sourcefraction [expr ($SourceValue*1.0 - $SourceLow)/  \
			         ($SourceHigh - $SourceLow)]
	
	#  The transformation is that fraction into the target window:

	set result [expr $sourcefraction*($TargetHigh-$TargetLow) + $TargetLow]

	return $result
    }

    # ComputeCentroid:
    #
    #   Compute the centroid of a spectrum interval in parameter space. 
    # Action: 
    #      Look up spectrum
    #      Translate low/hi limits to spectrum coordinates.
    #      for each channel in [spectrumlow-spectrumhi]:    
    #         area += value of channel    
    #         weightedsum <-- channelnum*channelvalue
    #      spectrumcentroid <-- weightedsum/area
    #      parametercentroid <-- Translate coordinates of spectrum centroid 
    #                           to parameter space.
    #      return parametercentroid
    #
    # Parameters:
    #    Spectrum [ string - in ]     
    #       Name of the spectrum. 
    #    limits [ list - in ]     
    #       The limits within which to compute. 
    # Returns:
    #    The centroid.
    # Special Case:
    #    If there are no counts, the mid point of the area of interest is returned.
    #
    # Throws:
    #    An error if the spectrum is not a 1-d, or does not exist.
    #    An error if the integration limits don't fit in the spectrum.
    # Special cases:
    #    If there are no counts in the region, the integration returns the
    #    region midpoint.
    

    proc ComputeCentroid {Spectrum limits} {
	#
	#  Unpack the limits (these are in parameter coordinates:
	#
	set paramLow  [lindex $limits 0]
	set paramHigh [lindex $limits 1]

	#  Get the spectrum definition and figure out the transformation windows.
        #  Note that the spectrum channel window is just [0,(nchans-1)]
	#
	set SpectrumInfo [spectrum -list $Spectrum]
	set Type         [lindex $SpectrumInfo 2]
	if {$Type != 1} {
	    error "$Spectrum is not a 1-d spectrum in ComputeCentroid"
	}
	set AxisDefs    [lindex $SpectrumInfo 4]
	set XaxisDef    [lindex $AxisDefs     0];         # Really only one.
	set AxisLow     [lindex $XaxisDef     0]
	set AxisHigh    [lindex $XaxisDef     1]
	set Channels    [lindex $XaxisDef     2]
	set ChannelHi   [expr $Channels - 1]

	#  Transform paramLow -> spectrumLow and paramHigh -> spectrumHigh
	#  The limits of integration in spectrum channel coordinates.
	#

	set spectrumLow [TranslateCoordinate "$AxisLow $AxisHigh"   \
                                              "0        $ChannelHi"  \
			                      $paramLow]
	set spectrumHigh [TranslateCoordinate "$AxisLow $AxisHigh"     \
			                       "0        $ChannelHi"    \
 			                       $paramHigh]

	set spectrumLow   [expr int($spectrumLow)]
	set spectrumHigh  [expr int($spectrumHigh)]

	# Coordinates could be backwards:

	if {$spectrumLow > $spectrumHigh} {
	    set tmp          $spectrumLow
	    set spectrumLow  $spectrumHigh
	    set spectrumHigh $tmp
	}
	#  Ensure the limits are within the spectrum:
	
	if {($spectrumLow < 0) || ($spectrumHigh > $ChannelHi) } {
	    error "Integration limits ($spectrumLow, $spectrumHigh) are not both in the spectrum (0, $ChannelHi)."
	}

	#  Now compute the statistics in the spectrum coordinate system

	set Area        0.0
	set WeightedSum 0.0
	for {set c $spectrumLow} {$c <= $spectrumHigh} {incr c} {
	    set counts       [channel -get $Spectrum $c]
	    set Area         [expr $Area + $counts]
	    set WeightedSum  [expr $WeightedSum + $c*$counts]
	}
	if {$Area != 0.0} {
	    set centroid [expr $WeightedSum/$Area]
	} else {
	    set centroid [expr ($spectrumLow + $spectrumHigh)/2.0]
	}
	# Transform the centroid back to the parameter coordinate system:

	set ParameterCentroid [TranslateCoordinate  "0        $ChannelHi"    \
				                     "$AxisLow $AxisHigh"     \
				                     $centroid]

	return $ParameterCentroid
    }
}