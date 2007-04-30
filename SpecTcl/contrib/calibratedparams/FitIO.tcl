#
#   This file contains the package FitIO
#   FitIO exports the following procs:
#
#    WriteFits   - Writes the currently defined set of fits to file.
#    SourceFits  - Sources the currently defined set of fits to file.
#    
#  This package lives in the namespace FitIO.
#
package provide FitIO      1.0
package require ErrorRead
namespace eval FitIO {

    # Description:	Writes the set of known fits to file.  Each fit is written as a 
    #               set of fit commands required to regenerate the fit in its current 
    #               state. 
    # Parameters
    #   fd - TCL File descriptor open on a writable file.
    #
    #  Note that in all output, the fit name is protected with "'s so that
    #  if the user is perverse enough to have a fit name with embedded whitespace
    #  it will still source correctly.
    #
    proc WriteFits {fd} {
	puts "WriteFits $fd"
	set timestamp [clock seconds]
	puts $fd "\# Fit definitions written at [clock format $timestamp]"
	
	# Iterate through the fits:

	foreach aFit  [fit -list] {
	    puts "Working on fit $aFit"
	    set name  [lindex $aFit 0]
	    set type  [lindex $aFit 1]
	    set state [lindex $aFit 2]
	    set points [lindex $aFit 3]

	    # Write the fit creational:

	    puts $fd "fit -create $type \"$name\""
	    
	    # If there are points, insert them:
	    
	    if {[llength $points]} {
		puts $fd "fit -add \"$name\"  $points"
	    }
	    # If the fit is performed, perform it:

	    if {$state == "performed"} {
		puts $fd "fit -perform \"$name\""
	    }
	}

	puts $fd "\#ENDFITS"
    }
    # Description	Reads the set of fits from file.
    # Parameters	
    #   fd           - File descriptor open on the fit file.
    #   ErrorRoutine - proc called on errors. See ErrorRead::SourceFile for
    #                  more information as we'll be calling that.  Note that
    #                  this proc must be resolvable in the ErrorRead namespace
    #                  this may require the caller to qualify it.
    #
    proc SourceFits {fd ErrorRoutine} {
	puts "In SourceFits"
	ErrorRead::SourceFile $fd $ErrorRoutine "\#ENDFITS"
    }


    namespace export WriteFits SourceFits
}