# Dialogs.tcl
#    Manages the graphical part of the calibration function.  Isolating into this
#    module separates gui from application.  I should be able to completely change
#    the appearance of the GUI by simply changing this module.
#    Public entries:
#      SelectParameterDialog    - Select the parameter to calibrate.
#      CalibrateParameterDialog - Setup the calibration fit.
#    Private entries:
#      MarshallCalibrationInfo  - Callback helper for CalibrateParameterDialog
#      BuildCalibrationBox      - Build the main section of the calibration fit
#                                 dialog box (for CalibrateParameterDialog)..
#    Private Variables:
#      RawPoints   - An array indexed by point number containing parameter
#                    coordinate centroids for each gate defined on the raw param.
#      CorrespondingPoints
#                  - An array indexed by point number containing (if defined)
#                    the value chosen to correspond to RawPOints(i).
#

namespace eval CalibrationGUI {
    # The following variables are private to this module:
    #

    variable CorrespondingPoints

    # Called to clear the correpsonding points array. Should be called before
    # destroying the toplevel.
    
    proc ClearPoints {} {
	variable CorrespondingPoints
	catch {unset CorrespondingPoints}
	set CorrespondingPoints(-1) unused;# -1 is invalid.
    }

    #
    #  SelectParameterDialog:
    #
    #    Displays the dialog that selects the parameter to calibrate. 
    # Action: 
    #    Create toplevel.Layout widgets as follows:
    #    Upper frame that contains a listbox and scrollbar to scroll the list box.
    #    Lower frame that contains the buttons OK, and Cancel.
    #    The OK button is attached to the callback with listbox widget and top 
    #    level as parameters.
    #    foreach parameter [parameter -list]    
    #      If parameter is not a calibrated parameter    
    #      Add parameter to list box.
    # Parameters:
    #     Callback [ procname - in ]
    #       The callback to invoke when the user has made their selection.   
    #       Note that the callback is only invoked when the user has clicked Ok, 
    #       not cancel. 
    # Returns:
    #    The top level widget.  The dialog is not intrinsically modal,
    #    The caller can use the top level to make it modal however.
    #
    proc SelectParameterDialog {Callback} {
	
	#  Create the top level, title it and block off the two frames:
	#    WorkArea   - the area the user interacts with.
	#    ActionArea - The area with the buttons:
	#
	set top [toplevel .selectparameter]
	wm title $top  "Select parameter to calibrate"

	set WorkArea    [frame $top.work]
	set ActionArea  [frame $top.action -relief groove -borderwidth 3]

	#  The work area has a list box and a scroll bar:
	#
	set parameterlist [listbox $WorkArea.list -selectmode single \
			      -yscrollcommand "$WorkArea.scroll set"]
	set scrollbar     [scrollbar $WorkArea.scroll                \
			       -command "$parameterlist yview"]

        # Fill the listbox with all parameters that are not calibrated params.

	set parameters [parameter -list]
	foreach parameter $parameters {
	    set name [lindex $parameter 0]
	    if {[calibparam -list $name] == ""} {    # Not calibrated
		$parameterlist insert end $name
	    }
	}

	# The action area has the two buttons.

	set cancel [button $ActionArea.cancel \
			-command "CalibrationGUI::ClearPoints; destroy $top" \
			-text "Cancel"   
		   ]
	set fit    [button $ActionArea.fit                             \
                              -command "$Callback $top $parameterlist" \
			      -text    "Fit..."
		    ]


	# Setup the geometry  of the whole thing..

	pack $parameterlist -side left
	pack $scrollbar     -side right -fill y

	pack $fit $cancel   -side left

	pack $WorkArea   -side top -fill x
	pack $ActionArea -side top -fill x

	#  Bind a double click in the list to the fit button.

	bind $parameterlist <Double-Button-1> "$fit invoke"
	
	return $top

    }
    #  CalibrateParameterDialog:
    #
    # Put up calibration dialog. 
    # Action: 
    #      Three frames:
    #           Top prompts for target parameter name and units.
    #           Middle is empty for now...
    #           bottom has the buttons 
    #              Create and Cancel.
    #               Cancel is hooked to self destruct.
    #               Create is hooked to MarshallCalibrationInfo which receives the
    #                      user callback, the top and middle widgets, 
    #                      and the number of possible calibration points. 
    #      BuildCalibrationBox is called fill in the middle frame
    #          It returns the number of fits.
    # Parameters:
    #     Parameter [ string - in ]
    #           Name of parameter to calibrate. 
    #     Callback [ procname - in ]
    #           Name of callback proc to call when the calibration fit has been set 
    #           up. 
    # Returns:
    #    The top level widget.  The dialog is not explicitly modal, the caller
    #    has the option to render the dialog modal, however since they get the
    #    top level widget.
    #
    proc CalibrateParameterDialog {Parameter Callback} {
	set top  [toplevel .calibrate]
	wm  title $top "Calibrate the parameter"

	#   Create the three frames.  The work area is made up of the top and 
	#   middle frames. The action area is the bottom frame only.

	set topframe       [frame $top.top]
	set middleframe    [frame $top.middle]
	set bottomframe    [frame $top.bottom -relief groove -borderwidth 3]

	#  The widgets in the top frame consist of an entry for the name of
	#  the new calibrated parameter, and another for the units.
	#  These are also labelled:

	label $topframe.plabel -text "New Parameter Name: "
	entry $topframe.target
	append targetname $Parameter _ calibrated
	$topframe.target insert end $targetname;    #  Default target.
	label $topframe.ulabel -text "Units: "
	entry $topframe.units

	# The top frame packing is easily done now:

	pack $topframe.plabel \
	     $topframe.target \
	     $topframe.ulabel \
 	     $topframe.units  -side left

	# Filling and packing the middle frame is delegated to BuildCalibration
	# box:

	set NumPoints [BuildCalibrationBox $middleframe $Parameter]
	if {$NumPoints < 2} {
	    tk_dialog .error "Insufficient points" \
		"There must be at least 2 slices on the parameter $Parameter to create a calibrated parameter" \
		error 0 Dismiss
	    destroy $top
	    return
	}

	#  The only thing left for us to do is setup the bottom frame:

	set perform [button $bottomframe.create                                   \
			 -text "Create"                                           \
			 -command "CalibrationGUI::MarshallCalibrationInfo        \
                                                           $topframe              \
                                                           $middleframe           \
                                                           $NumPoints $Callback"  
			 ]
        set cancel [button $bottomframe.cancel -text "Cancel" -command "destroy $top"]

	pack $perform $cancel -side left
	pack $topframe $middleframe $bottomframe -side top -fill x

	return $top
    }
    # MarshallCalibrationInfo:
    #
    #   Prepare for and call the user callback to create the calibrated parameter. 
    # Action: 
    #    TargetName <-- value of top.targetname entry.
    #    Units      <-- value of top.units entry.
    #    for each line of widgets in middle:
    #       if the data point line should be used:
    #           get the raw and fitted value as a 2 element list.
    #           append this list to the data point list.
    #    Compute top level widget by splitting the widgetname at .'s 
    #              and taking the first list element.
    #    Invoke the user callback with the 
    #             top level widget, name, units and data points as the list. 
    # Parameters:
    #     TopFrame [ widget - in ]     
    #             Top frame widge $TopFrame.target is target parameter name, 
    #             while $TopFrame.units contains the units string.
    #     MiddleFrame [ widget - in ]
    #             Middle frame widget of the calibration dialog. 
    #             This contains n lines.    
    #                  For line i:    $MiddleFrame.frame$i.useme$i is the checkbox 
    #                                 that determines if that point is part of the fit
    #                                 Its associated variable will be useme_$i
    #                                 $MiddleFrame.frame$i.rawdata$i is the value of the 
    #                                 raw data point.    
    #                                 CorrespondingPoints($i) is the calibration value.
    #     nPositions [ int - in ]     
    #            Number of position gates that are used. (number of potential data 
    #            points). 
    #     UserCallback [ procname - in ]
    #            Name of the user callback  
    #
    proc MarshallCalibrationInfo {TopFrame MiddleFrame nPositions UserCallback} {
	variable CorrespondingPoints

	#  First extract the target name and units.  The target name
	#  is not allowed to be blank.  We'll pop up an error dialog and
	#  return if it is:

	set TargetName [$TopFrame.target get]
	set Units      [$TopFrame.units get]
	if {$TargetName == ""} {
	    tk_dialog .guierror "Missing target"          \
		"You must supply a target parameter name" \
		error 0 Dismiss
	    return
	}
	#  Now collect the information from the middle frame:

	set CalibrationList ""
	for {set item 0} {$item < $nPositions} {incr item} {
	    global useme_$item
	    set used [set useme_$item]
	    if {$used} {
		set raw [$MiddleFrame.rawdata$item cget -text]
		set calibrated "undefined"
		if {[array names CorrespondingPoints $item] == $item} {
		    set calibrated $CorrespondingPoints($item)
		}
		#  Only use this if there's a calibrated point:

		if {$calibrated != "undefined"} {
		    lappend CalibrationList "$raw $calibrated"
		}
	    }
	}

	# Figure out the top level widget:

	set WidgetPath [split $TopFrame "."];      #   .a.b.c ->  {} a b c
	set TopLevel   .[lindex $WidgetPath 1];    #   -> .a 


	# Now we're ready to call the user callback:

	$UserCallback $TopLevel $TargetName $Units $CalibrationList
	
    }
    #  BuildCalibrationBox:
    #
    #    Build the middle frame of the calibration box dialog. 
    # Action: 
    #    Locate a spectrum for the parameter
    #    if no spectrum     
    #      return 0
    #    Create list of slices on parameter
    #    for each slice
    #     centroid <-- PeakStatistics::ComputeCentroid spectrrum name low hi    
    #     create and populate strip frame for the data point.
    #   return NumberOfSlicesFound 
    #
    # Parameters:
    #
    proc BuildCalibrationBox {Frame RawParameter} {
	global CorrespondingPoints

	ClearPoints

	#  I need a spectrum in order to be able to integrate.

	set SpectrumName [FindSpectrum $RawParameter]
	if {$SpectrumName == ""} {
	    return 0
	}
   
	set SliceList    [FindSlices $RawParameter]
	set DataPoint    0
	set CentroidList ""
	foreach slice $SliceList {
	    set Description [lindex $slice 3];#  The slice description.
	    set limits    [lindex $Description 1]
	    set limits    "{$limits}"
	    set name      [lindex $slice 0]
	    if {![catch "::CalibrationGUI::ComputeCentroid $SpectrumName $limits" \
		     centroid]} {
		lappend CentroidList "$name $centroid"
		incr DataPoint
	    } else {
	    }
	}
	if {$DataPoint == 0} {		# No point in doing any more work.
	    return 0
	}
	#  These guys get laid out in a grid.

	set DataPoint 0
	set CentroidList [lsort -real -unique -increasing -index 1 $CentroidList]
	foreach item $CentroidList {
	    set Name     [lindex $item 0]
	    set centroid [format "%.2f" [lindex $item 1]]

	    checkbutton $Frame.useme$DataPoint -variable useme_$DataPoint \
		-text $Name
	    label       $Frame.rawdata$DataPoint   -text $centroid
	    MakeValueMenu $Frame.calibrated$DataPoint \
		::CalibrationGUI::CorrespondingPoints($DataPoint)

	    #  Pack the items into their frames from top to bottom
	    #  with appropriate anchors to get them to line up:

	    grid $Frame.useme$DataPoint -sticky w -padx 10 \
		-column 0 -row [expr $DataPoint ]
	    grid $Frame.rawdata$DataPoint   -sticky e -padx 10 \
		-column 1 -row [expr $DataPoint]
	    grid $Frame.calibrated$DataPoint  -sticky w -padx 10 \
		-column 2 -row [expr $DataPoint]

	    incr DataPoint

	}

	return $DataPoint;		# Number of data points displayed.
	
    }
    #
    #  FindSpectrum:
    #     Locate the 'best' 1-d spectrum that has the specified parameter.
    #     The 'best' spectrum is the one that covers the larges range of the
    #     parameter, with ties broken by the maximum number of channels.
    #
    # Action:
    #     Range    <-- 0
    #     Channels <-- 0
    #     Found    <-- ""
    #     EnumerateSpectra
    #     foreach spectrum
    #        if Spectrum is 1d and is on the specified parameter
    #          if spectrum range > Range   OR:
    #             spectrum range == Range and spectrum channels > Channels
    #             Found = spectrum name
    #             Range = Spectrum Range
    #             Channels = SpectrumChannels.
    #             
    #     return Found.
    # Parameters:
    #    parameter   (string in)
    #       Name of the parameter we're looking for.
    #           
    proc FindSpectrum {parameter} {
	set Range    0
	set Channels 0
	set Found    ""
	set Spectra [spectrum -list]
	foreach spectrum $Spectra {

	    # Decode name, type, parameters from spectrum description.

	    set name       [lindex $spectrum 1]
	    set type       [lindex $spectrum 2]
	    set parameters [lindex $spectrum 3]
	    if {($type == "1") && ($parameters == $parameter)} {

		# Break apart the axis definition.

		set axes [lindex $spectrum 4]
		set axis [lindex $axes     0]
		set low  [lindex $axis     0]
		set high [lindex $axis     1]
		set num  [lindex $axis     2]
		set r    [expr abs($high - $low)]
		if {($r > $Range)   ||
		    (($r == $Range) && ($num > $Channels))} {
		    set Range    $r
		    set Channels $num
		    set Found    $name
		}
	    }
	}
	return $Found
    }
    # FindSlices:
    #    Get the definitions of all the slices that are set on the specified parameter.
    # Action:
    #    list <-- ""
    #    foreach gate 
    #       If gate is a slice and set on the parameter
    #          append slice to list
    #    return list.
    #
    # Parameters:
    #    name (string [in]):
    #      Name of the parameter we are looking for.
    #
    proc FindSlices {name} {
	set list ""
	foreach gate [gate -list] {
	    set type        [lindex $gate 2]
	    set description [lindex $gate 3]
	    if {($type == "s") && ([lindex $description 0] == $name) } {
		lappend list $gate
	    }
	}
	return $list
    }
    # MakeValueMenu
    #    Creates the menu of calibrated values that is given for each data point.
    #
    # Action:
    #    Create menubutton.
    #    Create menu
    #    foreach datum in CalibrationData
    #       add command for datum to menu with command to  set variable <- value
    # Parameters:
    #    menuname  (string [in]):
    #       widget path to use for the menubutton.  Note that the menu will be 
    #       called $menuname.choices
    #    var  (string [in]):
    #       Name of the variable to get the value of the menu.
    #
    proc MakeValueMenu {menuname var} {
	variable CalibrationData
   
       
	set $var "undefined"
	menubutton  $menuname -textvariable $var -relief raised -borderwidth 2
	set choices [menu $menuname.choices -tearoff 0]
	$menuname configure -menu $choices
	$choices add command -label "undefined" -command "set $var undefined"
	foreach datum $CalibrationData {
	    $choices add command -label   $datum \
		                 -command "set $var $datum"
	}
    }
	

}