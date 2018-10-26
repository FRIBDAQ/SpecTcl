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

package provide spectrumMaskWidget 1.0

##
# Provides a GUI that allows users to select the specific subset of spetra
# they are interested in via a Glob pattern.
# Format of the widget is:
#  
#  +-------------------------------------------------------------+
#  | [Update spectrum list] Spectrum Mask>: <mask entry> [Clear] |
#  +-------------------------------------------------------------+
#
# OPTIONS:
#   -mask       - set/get the mask entry value.
#   -updatecmd  - Script to invoke when the mask has been updated.
#
# SUBSTITUTIONS:
#    %M  - The current mask value.
#    %W  - The widget name.
#
# AUTONOMOUS ACTIONS:
#    If the Clear button is clicked, the mask entry is modified to *
#    which implies that -updatecmd is invoked.
#
# NOTE:
#   Key bindings are used to drive the callback as otherwise validation (the other option) is
#   a prevalidation making it just too hard to figure out the current value of entry.
#

snit::widget spectrumMaskWidget {
    hulltype ttk::frame

    option -mask      -configuremethod SetMask -cgetmethod GetMask
    option -updatecmd -default [list]

    ##
    # Create/layout the widget components and setup the events/bindings.
    # @param args -option/values for the initial configuration.
    #
    constructor args {

	ttk::button $win.update  -text {Update Spectrum List} -command [mymethod Dispatch -updatecmd]
	ttk::label  $win.masklbl -text { Spectrum Mask: }
	ttk::entry  $win.mask    -width 20
	ttk::button $win.clear   -text Clear -command [mymethod SetMask -mask *]

	bind $win.mask <KeyRelease> [mymethod Dispatch -updatecmd]

    $self configure -mask *;   # Initial mask must be set this way now.
	$self configurelist $args; # Done now so that -mask configuration triggers the update.

	grid $win.update $win.masklbl $win.mask $win.clear
	grid columnconfigure $win all -weight 1


    }
    #-----------------------------------------------------------------------------------
    #  Configuration management.

    ##
    # Handle the -mask configuration.  This:
    #  - Saves the mask in options(-mask)
    #  - Rewrites the contents of the mask entry
    #  - Dispatches to the -updatecmd  handler script.
    #
    # @param option - option name (should be -mask).
    # @param value  - New value of the -mask option.
    #
    method SetMask {option value} {
        $win.mask delete 0 end
        $win.mask insert end $value

    
        $self Dispatch -updatecmd; # Mask has changed so let the client know about it.
    }
    ##
    #  GetMask
    #    Handle mask getting.   This addresses daqdev/SpecTcl#369
    #   By making the returned mask * if the entry is empty.
    #
    # @param optname - name of the option to fetch (ignored).
    # @return string - The mask to use.
    #
    method GetMask optname {
        set mask [string trim [$win.mask get]]
        
        if {$mask eq ""} {
            set mask "*"
        }
        return $mask
    }

    #-----------------------------------------------------------------------------------
    # Action handlers.

    ## 
    # Dispatch to a user script substitutions supported include %W (the widget) %M (mask value)
    # @param option - option containing the script to which we dispatch.
    #
    method Dispatch option {
	treeutility::dispatch $options($option) [list %W %M] [list $win [$win cget -mask]]
    }

}