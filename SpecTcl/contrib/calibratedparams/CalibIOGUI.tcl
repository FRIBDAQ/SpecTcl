#
#   This file implements the CalibIOGUI package.
#   this package exports the following procs:
#
#     WriteConfiguration - prompts for a file and writes calibrations and fits to it.
#     ReadConfiguration  - Prompts for a file and reads calibrations and fits from it.
#
#
package provide CalibIOGUI     1.0
package require FitIO
package require CalibIO
package require Prompt

namespace eval  CalibIOGUI {
    variable    OverwriteAll   0;	# If true overwrite remaining without prompt.

    #  Prompts for continue or fail with a tk_dialog:
    #
    proc ContinueOrFail {msg} {
	set reply [tk_diaolg .continuorfail "Continue or Fail?" $msg \
		       error 0 Continue Abort]
	if {$reply == 0} {
	    return Continue
	} else {
	    return Abort
	}
    }

    # Description	Callback used with SourceFits to handle errors from that
    #                   procedure.  The only sort of error we handle with any grace
    #                   is the attempt to duplicate a fit.
    # Parameters
    #    command - Failing command
    #    message    - Error Message
    proc FitError {command message} {
	variable OverwriteAll
	puts "In Fit Error $command $message"

	set first       [lindex $message 0]
	set last        [lindex $message end]
	
	# Check for this being a fit exists:
	#
	if {($first == "Fit:") && ($last == "exists")} {
	    if {[lindex $command 1] == "-create"} {
		set fitname [lindex $command 3]
	    } else {
		set fitname [lindex $command 2]
	    }
	    if {$OverwriteAll} {	# Unconditional ovewrite/retry.
		fit -delete $fitname
		return 1
	    } else {			# Need to prompt for action:
		set question \
		    "Fit $fitname already exists select from the actions below" 
		set action [Prompt::Prompt $question  \
                                          0           \
				{{Overwrite 1} {"Overwrite All" 2} {Abort 3}}]

		if {$action == 3} {	# abort..
		    return 0
		}
		if {$action == 2} {	# Overwrite all...
		    set OverwriteAll 1
		}
		#  Overwrite or overwrite all, action is the same:
		
		fit -delete $fitname;	# Kill the fit
		return 1;		# Retry the command.
			
	    }
	} else {				# Unexpected error type.. abort:
	    return 0
	}   
    }
    # Description:	Error proc for calibration reads.
    # Parameters:	
    #     command   - Command that failed
    #     message   - Reason it failed.
    #
    proc CalibError {command message} {
	puts "Calib error $command $message"
	variable OverwriteAll

	set WordNum 0
	set target    [lindex $command 2]
	set targetid  [lindex $command 3]
	set rawname   [lindex $command 4]
	set fitname   [lindex $command 5]

	foreach word $message {		# List -> array.
	    set words($WordNum) $word
	    incr WordNum
	}
	incr WordNum -1;		# WordNum now the last index.

	#  Fit not defined?
	
	if { ($words(0) == "The") && ($words(2) == $fitname) && \
	     ($words($WordNum) == "not") } {
	    if {[ContinueOrFail "Error executing : $command : $message"] == "Continue"} {
		puts "returning 2"
		return 2
	    } else {
		puts "Returning 0"
		return 0
	    }
	}

	# Raw parameter not defined?

	if {($words(0) == "The")   && ($words(1) == "Raw")  && \
	    ($words(2) == "Parameter") && ($words(3) == $rawname) && \
	    ($words($WordNum) == "not") } {
	    if {[ContinueOrFail "Error executing : $command $message" ] == "Continue"} {
		puts "Returning 2."
		return 2
	    } else {
		puts "Returning 0"
		return 0
	    }
	}
	# Look at duplication of the target parameter name or id:

	set prompt 0;			# This is optimistic...
	if {($words(1) == "calibrated") && ($words(3) == $target) && \
		($words($WordNum) == "exists.") } {
	    set prompt 1
	    set message "Attempting to redefinine existing calibrated parameter $target"
	    set killcmd "calibparam -delete $target"
	}

	if {($words(0) == "A") && ($words(1) == "parameter") && ($words(2) == "named") \
	    && ($words(3) == $target) && ($words($WordNum) == "exists.")} {

	    set prompt 1;		# Duplicate name..
	    set message "Attempting to redefine parameter $target"
	    set killcmd "parameter -delete $target"
	    puts "Killcmd set: $killcmd"
	}
	if {($words(0) == "A") && ($words(1) == "parameter") &&     \
            ($words(2) == "with") &&                                \
	    ($words(3) == "the") && ($words(4) == "id:") &&          \
	     ($words(5) == $targetid) &&                            \
	     ($words($WordNum) == "exists") } {
	    set prompt 1;		# Duplicate id...
	    set message "Attempting to redefine the parameter id $targetid"
	    set killcmd "parameter -delete -id $targetid"
	}

	if {!$prompt} {			# Unrecognized problem... abort.
	    puts "Returning 0"

	    return 0
	}
	#  If overwite all already set we know what to do:

	if {$OverwriteAll} {
	    puts "Returning 1 $killcmd"
	    eval $killcmd;		# Kill the entity however...
	    return 1;			# Retry the command.
	} else {			# Need to prompt.
	    set WhatToDo [Prompt::Prompt $message 0 \
	      {{"Keep old" 0} {Overwrite 1} {"Overwrite all" 2} {Abort 3}}]
	    if {$WhatToDo == 3} {
		puts "returining 0"
		return 0;		# Abort.
	    }
	    if {$WhatToDo == 0} {
		puts "returning 2"
		return 2;		# Keep the old one...
	    }
	    if {$WhatToDo == 2} {	# Overwrite all:
		set OverwriteAll 1
	    }
	    #  Ovewrite all or just overwrite...

	    eval $killcmd
	    return 1
	}
	puts "Invalid flow of control in CalibError"
    }
    # Description:	Prompts for a configuration file.  If the file exists and is
    #                   readable, the fits and calibrations are read from it.
    # Parameters:	-none-
    #
    proc ReadConfiguration {} {
	variable OverwriteAll

	set Filename [tk_getOpenFile                             \
		      -defaultextension .tcl                     \
		      -filetypes     { {{TCL Scripts} {.tcl} }   \
				       {{Tk Scripts} {.tk} }     \
				       {{All Files} * }}         \
 		      -parent .                                  \
		      -title "Select script to read"]
	# Ensure the file exists:

	if {$Filename == ""}  return
	if {![file exists $Filename]} {
	    Prompt::Error "No such file $Filename"
	    return
	}
	#  Ensure the file is readable:

	if {![file readable $Filename]} {
	    Prompt::Error "$Filename is not readable"
	    return
	}
	#  Try to open the file (should work but we'll catch anyway).

	if {[catch {open $Filename r} msg]} {
	    Prompt::Error "Open failed for $Filename : $msg"
	    return
	}
	set fd $msg

	# Read the fits:

	set OverwriteAll 0;		# Rest the overwrite all ok flag...


	if {[catch {FitIO::SourceFits $fd CalibIOGUI::FitError} msg]} {
	    Prompt::Error "Failed to read fits: $msg"
	    return
	}

#	if {[catch {FitIO::SourceFits $fd CalibIOGUI::FitError} msg]} {
#	    Prompt::Error "Failed to read fits: $msg"
#	    return
#	}

	# Read the calibrations:
	set OverwriteAll 0;		# Reset the overwrite all ok flag...

	if {[catch {CalibIO::SourceCalibs $fd CalibIOGUI::CalibError} msg]} {
	    Prompt::Error "Failed to read calibrations: $msg"
	    return
	}

	close $fd

    }
    # Description	Prompts for and writes a file containing the fit definitions 
    #                   and the calibration definitions.
    # Parameters	-none-
    #
    proc WriteConfiguration {} {
	set Filename [tk_getSaveFile                             \
			  -defaultextension .tcl                 \
			  -filetypes { {{TCL Scripts} {.tcl} }   \
				       {{Tk Scripts} {.tk} }     \
				       {{All Files} * }}         \
			  -parent .                              \
			  -title "Select script to read"]
	if {$Filename != ""} {		# Can cancel in which case name is blank.
	    if {[catch {open $Filename w} msg]} {
		Prompt::Error $msg
		return
	    }
	} else {
	    return
	}
	set fd $msg
	# Write the fits:

	if {[catch {FitIO::WriteFits $fd} msg]} {
	    Prompt::Error $msg
	    return
	}
	# Write the parameters:

	if {[catch {CalibIO::WriteCalibs $fd} msg]} {
	    Prompt::Error $msg
	}
	close $fd
    }
    namespace export WriteConfiguration
    namespace export ReadConfiguration
}