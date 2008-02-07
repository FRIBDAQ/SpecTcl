# This file contains the Tk implementation of the Prompt package.
# The intent is that if I ever need to do prompting from tcl scripts
# rather than Tk scripts, I can write a call compatible TclPrompt.tcl
# I could then even determine at run time if the script was in Tcl or Tk
# and source in the appropriate package version.
# I can't charge TUNL, however for stuff they don't need so I won't implement
# TclPrompt..yet.
#
# Prompt lives in the Prompt Namespace.

package    provide Prompt     1.0
namespace   eval Prompt {
    # Description:	Generic prompting for TCL/Tk programs. Uses a Tk-dialog to 
    #                   output a user defined message and prompt for one of 
    #                   several possible replies.
    # Parameters:	
    #      question         - The user output that should be   
    #      answerdefault  - Default answer index
    #      Answerlist - List of 2 element sublists giving prompt/value.
    #
    proc Prompt {question answerdefault AnswerList} {
	#
	# Build up the tk_dialog command ans the AnswerValues
	# array that lets us lookup the answer value given
	# the button selected.
	#
	set command "tk_dialog .tkprompt Prompt \"$question\" questhead"
	append command " $answerdefault"


	foreach answer $AnswerList {
	    set prompt [lindex $answer 0]
	    set value  [lindex $answer 1]
	    lappend command $prompt;	# Add the button to the dialog.
	    lappend valuelist $value
	}
	set result [eval $command]
	return [lindex $valuelist $result]
    }
    #   Put up a dialog to ack an error:

    proc Error {msg} {
	tk_dialog .tkprompt "Error!" $msg error 0 Dismiss
    }

    #  No exports to prevent conflicts hopefully.
}