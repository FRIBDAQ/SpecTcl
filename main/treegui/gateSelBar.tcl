#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2009.
#
#    You may use this software under the terms of the GNU public license
#    (GPL).  The terms of this license are described at:
#
#     http://www.gnu.org/licenses/gpl.txt
#
#    Author:
#             Ron Fox
#	     NSCL
#	     Michigan State University
#	     East Lansing, MI 48824-1321



package require Tk
package require snit
package require treeUtilities

package provide gateSelBar 1.0

##
# provides a snit megawidget that makes up the bottom bar of the gates tab.
# The visual appearance of this is:
#
#  +----------------------------------------------------------------+
#  | [  Update Gate List]  Gate mask <               > [Clear]      | 
#  +----------------------------------------------------------------+
#
#  OPTIONS
#     -updatecmd  - Script to run when the "Update Gate List" button is clicked.
#     -maskcmd    - Script to run when the gate mask changes.
#     -mask       - Current value of the mask.
#
#  Note that if the Clear button is clicked it will set the Gate mask to
#  * autonomously and invoke the -maskcmd script.
#
#  Substitutions:
#     %M  - provides the current value of the Mask entry for both
#           command scripts.
#
snit::widget gateSelBar {
    hulltype ttk::frame

    option -updatecmd [list]
    option -maskcmd   [list]
    option -mask -readonly true -cgetmethod GetMask


    ##
    # Build the widget components, lay them out and attach appropriate
    # callbacks.
    # @args - option name/value pairs.
    #
    constructor args {
	$self configurelist $args

	#  Create the widgets...where possible ttk widgets are used.
	#
	ttk::button $win.update  -text "Update Gate List" -command [mymethod dispatch -updatecmd]
	ttk::label  $win.masklbl -text "Gate Mask"
	ttk::entry  $win.mask    -width 32 
	ttk::button $win.clear   -text Clear -command [mymethod clearMask]

	# All key releases in the mask dispatch to -maskcmd.

	bind $win.mask <KeyRelease> [mymethod dispatch -maskcmd]

	# Layout the widget geometry.

	grid $win.update $win.masklbl $win.mask $win.clear  -sticky ew


	# Set initial entry value:

	$win.mask insert 0 *
    }
    #------------------------------------------------------------------------
    # Configuration management.

    ##
    # Retrieve the current value of the mask
    #
    # @param option - the option to retrieve ..must be -mask
    #
    method GetMask option {
	return [$win.mask get]
    }
    #--------------------------------------------------------------------------
    #  Event handlers:

    ##
    # Generic dispatch.  This handles the dispatch of the -updatecmd and -maskcmd
    # operations.
    # @param option The name of the option that holds the script we are dispatching.
    #
    method dispatch option {
	::treeutility::dispatch $options($option) %M [list [list [$win.mask get]]]
    }
    ##
    # Called when the clear button is clicked.  Clears the mask back to "*" and
    # dispatches the -maskcmd script.
    #

    method clearMask {} {
	$win.mask delete 0 end
	$win.mask insert 0 *

	$self dispatch -maskcmd
    }
}