#
#   GlobalData.tcl - provides the global data for the Calibration Gui pacakge.
#
#   See below for more details:

namespace eval CalibrationGUI {
    #
    #   SystemCalibrationPath contains the installation relative path to the
    #   file containing system calibration points. 

    variable SystemCalibrationPath "etc/CalibrationPoints.dat"

    #   UserCalibrationPath contains the home directory relative path to the
    #   file that contains user calibration data. If this file exists,
    #   it is read in to supplement any calibration point data that may 
    #   have been read in from the SystemCalibrationPath:
    #
    variable UserCalibrationPath ".SpecTcl/CalibrationPoints.dat"

    #    CalibrationData will be filled in to contain an ordered list of
    #    calibration points.  The list should only contain unique elements.
    #   

    variable CalibrationData ""


}