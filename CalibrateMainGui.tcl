#
#  CalibrateGuiMain.tcl
#  
#    This is the driver of the calibration software.
#    public entry is:
#        CalibrateGUI - this can be attached to a button to get the
#                       calibration ball rolling.
#
#    private functions:
#        FitSelected              - Callback from SelectParameterDialog, start the fit.
#        CreateCalibratedParameter- Create the calibrated parameter selected.
#        ValidateFitname          - Determines if the fit named is acceptable.
#        ValidateTargetName       - Determines if the parameter name is acceptable.
#        ValidateDataPoints       - Determines if all data points are acceptable.
#        CreateFit                - Creates the calibration fit.
#        Calibrate                - Does the final calibrated parameter creation.
#
package provide CalibrationGUI 1.0


#   Get my script directory and read the other files that supply namespace members.

set FileList {GlobalData.tcl CalibrationPoints.tcl Dialogs.tcl PeakStatistics.tcl}
set me       [info script]
set dir      [file dirname $me]
foreach File $FileList  {
    source $dir/$File
}

namespace eval CalibrationGUI {

    variable CalibrationPointsRead 0
    variable RawParameter



    #  CalibrateGUI:
    #
    #     Entry point to the calibration system.  This function is usually bound to 
    #     some gui button. 
    #  Action: 
    #     - If the calibration points have not yet been read:    
    #          invoke CalibrationPoints::ReadCalibrationPoints    
    #          Set flag indicating calibratino points were read.
    #     - Invoke the SelectParameterDialog giving the function FitSelected as    
    #        the callback. 
    #
    proc CalibrationGUI {} {
	variable CalibrationPointsRead
	
	if {!$CalibrationPointsRead} {
	    ReadCalibrationPoints
	    set CalibrationPointsRead 1
	}

	SelectParameterDialog CalibrationGUI::FitSelected
    }
    # FitSelected:
    #
    #      This callback is invoked from the SelectParameterDialog function of 
    #      Dialogs.tcl.   
    #   Action: 
    #      - Extracts the parameter name from this listbox 
    #      - Destroys the dialog (dismissing it).
    #      -  Invokes Dialog::CalibrateParameterDialog function in Dialogs passing
    #         CreateCalibratedParameter as the callback. 
    #
    # Parameters:
    #   TopLevel [ Widget - in ]
    #        The top level widget of the dialog. 
    #   ListBox [ ListboxWidget - in ]
    #        The list box widget that contains the parameter the user selected to be calibrated. 
    #
    proc FitSelected {TopLevel ListBox} {
	variable RawParameter 

	# Figure out which parameter was selected:

	set selindex [$ListBox curselection]
	if {$selindex == ""} {
	    tk_dialog .error "No selection" \
		"You must select a parameter to calibrate or cancel" \
		info 0 Dismiss
	    return
	}
	set Parameter [$ListBox get $selindex]
	set RawParameter $Parameter

	# Destroy the dialog that called us:

	destroy $TopLevel

	# And go to the next dialog in the process.

	CalibrateParameterDialog $Parameter CalibrationGUI::CreateCalibratedParameter
    }
    # CreateCalibratedParameter:
    #
    #    Invoked by Dialog::CalibrateParameterDialog to create the actual fit.    
    # Parameters:
    #     TopLevel [ Widget - in ]     
    #            The top level widget of the dialog that called us 
    #            (used to dismiss it) 
    #     TargetParameterName [ String - in ]
    #            Name of the target parameter the user selected.  This will also
    #            be used as the fit name.
    #     Units [ String - in ]
    #           The Units for the calibrated parameter (blank means there are none).
    #     Points [ List - in ]
    #           A list of calibration points.  Each element of the list is a two 
    #           element sublist containing a raw parameter value and an associated 
    #           calibrated value. 
    #
    proc CreateCalibratedParameter {TopLevel TargetParameterName Units Points} {
	variable RawParameter

	if {![ValidateFitName $TargetParameterName]} {
	    return;			# Let the user try again or cancel.
	}
	if {![ValidateTargetName $TargetParameterName]} {
	    return;			# Let the user try again or cancel.
	}
	if {![ValidateDataPoints $Points]} {
	    return;			# Let the user try again or cancel.
	}

	if {[catch "CreateFit $TargetParameterName {$Points}" msg]} {
	    tk_dialog .fiterror "Fit failed"                          \
		"Unable to perform the fit of $TargetParameterName: $msg" \
		warning 0 Dismiss
	    return;			# User can try again or cancel.
	}
	if {[catch "CalibrateParameter $TargetParameterName $RawParameter {$Units} \
                   $TargetParameterName" msg] } {
	    tk_dialog .caliberror "Calibration failed" \
		"Unabled to perform calibration of $TargetParameterName: $msg" \
		warning 0 Dismiss
	    return
	}
	#  Everything worked so::

	destroy $TopLevel
    }
    # ValidateFitName:
    #
    #     Validates the name of a fit. 
    # Action: 
    #      If the fit passed in exists:   
    #          Dialog: Fit exists, do you want to replace?   
    #          If yes:      
    #            delete existing fit.      
    #            return true
    #          else:
    #            return false.
    #       else:
    #           return true 
    #
    # Parameters:
    #      FitName [ string - in ]  
    #           The name of the fit to check.
    #
    proc ValidateFitName {FitName} {
	set fit [calibrationfit -list $FitName]
	if {$fit != ""} {
	    set reply [tk_dialog .replacefit "Replace Fit?"              \
			   "Fit $FitName already exists: $fit Do you want to replace it?"  \
			   questhead 1 Yes No]
	    puts "Got $reply"
	    if {$reply == 0} {
		calibrationfit -delete $FitName
		return 1
	    } else {
		return 0
	    }
	} else {
	    return 1;			# Fit is nonexistent and therefore ok.
	}
    }
    # ValidateTargetName:
    #    
    #      Determines if the calibration target parameter name is valid. 
    #  Action: 
    #      If TargetName Exists as CalibratedParameter:
    #         Dialog:  TargetName is already a calibrated parameter replace?   
    #         If yes:
    #            delete calibrated parameter TargetName
    #            return true  
    #         else:
    #            return false
    #      else:
    #         If TargetName Exists as a raw Parameter:
    #            Dialog:   TargetName already exists as a raw parameter replace?
    #            If yes:
    #               delete raw parameter TargetName 
    #               return true 
    #            else:
    #               return false   
    #         else:
    #            return true       
    # Parameters:
    #      TargetName [ string - in ]  
    #         Name of the parameter to validate.
    #
    proc ValidateTargetName {TargetName} {
	set calibdescription [calibparam -list $TargetName]
	if {$calibdescription != ""} {
	    set answer [tk_dialog .calibexists "Calibrated Parameter exists"   \
			    "A calibrated parameter named $TargetName already exists: $calibdescription do you want to replace it?" \
			    questhead   1 Yes No]
	    if {$answer == 0} {
		calibparam -delete $TargetName
		return 1
	    } else {
		return 0
	    }
	} else {			# No such calibrated parameter...
	    set fail [catch "parameter -list $TargetName"]
	    if {!$fail} {
		set answer [tk_dialog .rawexists "Raw parameter exists" \
				"$TargetName exists as a raw parameter.  Replace?" \
				questhead 1 Yes No]
		if {$answer == 0} {
		    parameter -delete $TargetName
		    return 1
		} else {
		    return 0
		}    
	    } else {
		return 1
	    }
	}
    }
    # ValiateDataPoints:
    #
    #    Determines if there are sufficient data points and if the mapping of raw 
    #    data points to calibrated data points has any duplication (which is illegal).
    # Action: 
    #      If list size(DataPoints) < 2:    
    #         Dialog: Not enough data points.    
    #         return false
    #      Foreach DataPoint:
    #         If DataPoint.calibrated already seen:
    #               Dialog: Data point duplication (DataPoint.calibrated)
    #               return false
    #     return true
    #
    proc ValidateDataPoints {Points} {
	if {[llength $Points] < 2} {
	    tk_dialog .toofewpts "Too few points" \
		"At least 2 data points are required to perform a fit"   \
		error 0 Dismiss
	    return 0
	}
	foreach point $Points {
	    set raw         [lindex $point 0]
	    set calibrated  [lindex $point 1]
	    if { ([array names Seen $calibrated] == $calibrated) } {
		tk_dialog .duplicatept "Duplicate point" \
		    "Two or more data points have the same calibrated value" \
		    error 0 Dismiss
		return 0
	    } else {
		set Seen($calibrated) $raw
	    }
	}
	return 1
    }
    # CreateFit:
    #
    #     Create a fit. 
    # Action: 
    #     - Create fit FitName as a linear fit
    #     - Foreach point in DataPoints    
    #           Add the point to FitName
    #     - Perform the fit.   
    # Parameters:
    #     FitName [ string - in ] 
    #        The name of the fit to create. 
    #     DataPoints [ List - in ]     
    #        List of data points.  Each list element is a 2 element sublist 
    #        that contains
    #               raw           - A raw parameter value    
    #               calibrated    - The desired calibrated value corresponding to raw.  
    # 
    proc CreateFit {FitName DataPoints} {
	calibrationfit -create linear $FitName
	foreach point $DataPoints {
	    calibrationfit -add $FitName $point
	}
	calibrationfit -perform $FitName
    }
    # Calibrate:
    #
    #   Creates the desired calibrated parameter 
    # Action: - 
    #    Find first unused parameter id
    #    Create parameter 
    #
    # Parameters:
    #    TargetName [ string - in ] 
    #        Name of the target parameters.
    #    RawName [ string - in ]  
    #        Name of the raw parameter name.
    #    Units [ string - in ]  
    #        Units tring.
    #    FitName [ string - in ]  
    #        Fit associated with the calibrated parameter.
    #
    proc CalibrateParameter {TargetName RawName Units FitName} {
	set paramno 0
	while {1} {
	    incr paramno
	    if {[catch "parameter -list -id $paramno"]} {
		break
	    }
	}
	calibparam -create $TargetName $paramno $RawName $FitName $Units
    }
    
}