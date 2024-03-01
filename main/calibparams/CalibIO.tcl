#
#  This file contains the CalibIO package and namespace. The namespace exports
#  the following procs:
#
#    WriteCalibs   - Writes the set of calibrations to file.
#    SourceCalibs  - Reads the set of calibrations from a file.
#
#
package   provide CalibIO      1.0
package   require ErrorRead

namespace eval    CalibIO {

    # Description:	proc that iterates through calibrations and writes them
    #                   to file.
    # Parameters
    #    fd   - File descriptor.
    #  All names are quoted in case the user has entered some pathologicals.
    #
    proc WriteCalibs {fd} {
	set timestamp [clock seconds] 
	puts $fd "\# Calibration definitions written at [clock format $timestamp]"

	# Iterate over the calibrated parameter definitions.
	
	foreach calibration [calibparam -list] {
	    set targetname [lindex $calibration 0]
	    set targetid   [lindex $calibration 1]
	    set rawname    [lindex $calibration 2]
	    set fitname    [lindex $calibration 3]
	    set units      [lindex $calibration 4]
	    #
	    #  If the fit was deleted, the var below will be (deleted).
	    #
	    set LastElementOfFitname [lindex $fitname end]
	    #
	    #   Only write if all parameter elements are well defined.
	    #
	    if {($targetname != "-deleted-")         &&         \
		($rawname    != "-deleted-")         &&         \
		($LastElementOfFitname != "(deleted)") } {
		puts $fd \
  "calibparam -create \"$targetname\" $targetid \"$rawname\" \"$fitname\" \"$units\""
	    }
	}
	puts $fd "\#ENDCALIBPARAMS"
    }
    # Description:	proc that iterate reads calibrations from a file.
    # Parameters:	
    #     fd        - File descriptor.
    #     ErrorProc - proc called on error (see ErrorRead::SourceFile for more
    #                 information about errorproc.

    proc SourceCalibs {fd ErrorProc} {
	ErrorRead::SourceFile $fd $ErrorProc "\#ENDCALIBPARAMS"
    }

    
    namespace export WriteCalibs SourceCalibs
}
