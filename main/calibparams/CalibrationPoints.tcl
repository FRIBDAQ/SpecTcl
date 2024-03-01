#
#   CalibrationPoints.tcl - This file contains scripts that 
#   handle the initial read of the calibration data files.
#   Two files are read in, a system wide calibration file with a prefix
#   relative name in SystemCalibrationPath
#   A second in a user calibratiohn path that is home directory relative.
#
#   Entry points in this module:
#      public:
#           ReadCalbrationPoints - reads the two files, preprocesses
#                                  the data and stores the points in 
#                                  CalibratedData.
#      private:
#           ReadCalibrationFile  - Reads a single calibration point data file.
#
namespace eval CalibrationGUI {
    #
    # ReadCalibrationPoints: 
    #
    # Reads both of the calibration point files.
    # Action: - ReadCalibrationPointFile SystemCalibrationPath
    #         - ReadCalibrationPointFile UserCalibrationPath
    #         - Insert Calibration point lists into an array indexed by point.
    #         - Foreach element of the array 
    #               append the element to the calibration    
    #               point list (note this removes duplicates).
    #         - Sort resulting list
    #         - CalibrationData <-- resulting list. 
    # Parameters:
    #    NONE
    # Implicit inputs:
    #   CalibrationGUI::SystemCalibrationPath   
    #   CalibrationGUI::UserCalibrationPath
    #   ::SpecTclHome
    # Implicit Outputs:
    #   CalibrationGUI:: CalibrationData

    proc ReadCalibrationPoints {} {
	variable SystemCalibrationPath
	variable UserCalibrationPath
	variable CalibrationData

	#  The System Calibration path is relative to SpecTclHome

	global SpecTclHome
	set    SystemFile $SpecTclHome/$SystemCalibrationPath
	set    SystemCalibrationList [ReadCalibrationPointsFile $SystemFile]

	#  The user Calibration path is relative to ~ which is usable
	#  in Tcl filenames:

	set    UserFile  ~/$UserCalibrationPath
	set    UserCalibrationList [ReadCalibrationPointsFile $UserFile]
	
	#  The trick below eliminates duplicate points:

	foreach point $SystemCalibrationList {
	    set Calibrations($point) $point
	}
	foreach point $UserCalibrationList {
	    set Calibrations($point) $point
	}
	#  Now take each of the points and append it to the final calibration
	#  list.
	foreach point [lsort -real [array names Calibrations]] {
	    lappend CalibrationData $point
	}
    }
    # 
    #  ReadCalibrationPointsFile:
    #
    # Reads a single calibration point file and returns a list of the 
    # values in the file.
    # Parameters:
    #   Path [ string - in ]     
    #          Full path to the file to read (must be a valid target of tcl open). 
    # Returns:
    #    The set of calibration points in the file.  If the file cannot be opened,
    #    an empty list is returned.
    #
    proc ReadCalibrationPointsFile {Path} {

	set ReturnValue "";		# Could be empty even if readable.

	# Ensure file readability:

	if {! [file readable $Path] } {
	    return ""
	} else {
	    set fd [open $Path r]
	    while {![eof $fd]} {
	        gets $fd data
		if {[scan $data %f point] == 1} {
		    lappend ReturnValue $point
		}
	    }
	    close $fd
	}
	return $ReturnValue
    }
    
}
