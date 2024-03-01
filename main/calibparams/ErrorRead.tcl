#
#   This file contains the ErrorRead package.
#
#    ErrorRead provides:
#     SourceFile - sourcing a TCL file with error processing
#
# This package lives in the namespace ErrorRead

package provide ErrorRead 1.0
namespace eval ErrorRead {
    # Description:	sources sources commands from a file until either eof, or 
    #                   a delimeter is seen.  If an error occurs a user error 
    #                   handler is called.
    # Parameters:	
    #    fd          - TCL file descriptor open on the output file.
    #    errorproc   - proc called on error.  The proc is passed the command abd 
    #                  the result string from the catch that caught the error.
    #    enddelim    - End delimeter
    # Restrictions:
    #    Commands must fit on a single source line of the file.
    #
    proc SourceFile {fd errorproc enddelim} {
	while {![eof $fd]} {
	    set line [gets $fd]
	    if {$line == $enddelim} {
		return
	    }
	    puts "Evaluating $line"
	    if {[catch {eval $line} errormsg]} {  # Error fired.

		set status [$errorproc "$line"  "$errormsg"]

		if {$status == 0} {	# Abort with error....
		    error "$line $errormsg"
		}
		if {$status == 1} {	# Retry without handler.
		    if {[catch {eval $line} errormsg]} {
			error "Unable to process: $line :  $errormsg"
		    }
		}
		# Control falls here if continue without retry.
	    };				# end if initial try fails.
	};				# End file not eof.
    };					# SourceFile

    namespace export SourceFile
}