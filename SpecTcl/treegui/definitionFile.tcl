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

package provide definitionFileWidget 1.0

##
#  Provides an interface for loading definition files.  The layout of this widget is:
#
#   +--------------------------------------+
#   |        Definition file:              |
#   |       <current def file label>       |
#   |  [Load]       [Save]                 |
#   |   [] Cumulate  [] Failsafe           |
#   +--------------------------------------+
#
# OPTIONS:
#   -filename     - The contents of the <current def file label> field 
#   -accumulate   - The state of the Cumulate checkbutton.
#   -makefailsafe - The state of the Failsafe checkbutton.
#   -loadcmd      - Script that is called when the Load button is clicked.
#   -savecmd      - Script that is called whenthe Save button is clicked.
#
# SUBSTITUTIONS:
#   Callbacks support the following susbitutions:
# 
#   - %W  - The widget name.
#   - %N  - The filename chosen by the user.
# 
snit::widget definitionFileWidget {
    hulltype ttk::frame

    option -filename     -default Unknown -configuremethod SetFilename
    option -accumulate   -default 0
    option -makefailsafe -default 1
    option -loadcmd      -default [list]
    option -savecmd      -default [list]


    ##
    # Construct the widget.
    # @args - option name/values for the initial configuration.
    #
    constructor args {

	# Create the component widgets...

	ttk::label $win.fnamelabel -text {Definition file:} -justify center
	ttk::label $win.filename   -text Unknown -width 20 -justify center

	ttk::button $win.load -text Load -command [mymethod DispatchLoad]
	ttk::button $win.save -text Save -command [mymethod DispatchSave]
	
	ttk::checkbutton $win.accumulate -text Cumulate \
	    -onvalue 1 -offvalue 0                      \
	    -variable ${selfns}::options(-accumulate)

	ttk::checkbutton $win.failsafe   -text Failsafe \
	    -onvalue 1 -offvalue 0                      \
	    -variable ${selfns}::options(-makefailsafe)

	# Lay them out on the frame:

	grid $win.fnamelabel -columnspan 2 
	grid $win.filename   -columnspan 2 
	grid $win.load $win.save
	grid $win.accumulate $win.failsafe

	grid columnconfigure $win all -weight 1

	# configure the widget components:

	$self configurelist $args

    }
    #-----------------------------------------------------------------------
    #  Configuration management.
    
    ##
    # Configure -filename   If the filename is longer than the width of the'
    # widget, the last width-3 characters are put in the widget preceded by ellipsis.
    # @param option - the option to modify (-filename)
    # @param value  - The new value of the option.

    method SetFilename {option value} {
	set options($option) $value

	set maxWidth [$win.filename cget -width]

	if {[string length $value] > $maxWidth} {
	    set maxWidth [expr {$maxWidth - 3}]
	    set value ...[string range $value end-$maxWidth end]
	}
	$win.filename configure -text $value
    }
    #----------------------------------------------------------------------
    # Action handlers for the user interface.

    ##
    # Handle the Save button.  This dispatches to the -savecmd after first
    # prompting for a filename.  If no filename is given no callback is 
    # performed.  If no script is registered nothing happens as well.
    # We configure ourself with a new -filename if the callback runs as well.
    #
    method DispatchSave {} {
	set script $options(-savecmd)

	if {$script ne ""} {
	    set filename [tk_getSaveFile \
			      -defaultextension .tcl  \
			      -filetypes [::treeutility::getFileTypes] \
			      -initialfile [$self GetDefaultFile]   \
			      -initialdir  [$self GetDefaultDir]    \
			      -parent      $win \
			      -title    "Select Save filename"]
	    if {$filename ne ""} {
		::treeutility::dispatch $script [list %W %N] [list $win $filename]
		$self configure -filename $filename
	    }
	}
    }

    ## 
    # Handle the Load button.  This dispatches to the  -loadcmd after first
    # prompting for a filename.  If no filename is given or if there is no
    # script no callback is performed. We configure ourselves with a new -filename
    # if the callback runs.
    #
    method DispatchLoad {} {
	set script $options(-loadcmd)

	if {$script ne ""} {
	    set filename [tk_getOpenFile \
			      -defaultextension .tcl  \
			      -filetypes [::treeutility::getFileTypes] \
			      -initialfile [$self GetDefaultFile]   \
			      -initialdir  [$self GetDefaultDir]    \
			      -parent      $win \
			      -title    "Select Save filename"]
	    if {$filename ne ""} {
		::treeutility::dispatch $script [list %W %N] [list $win $filename]
		$self configure -filename $filename
	    }
	}    
    }
    #------------------------------------------------------------------------
    #  Private utilities:

    ##
    # Return the correct default file.  This is either the tail of the -filename
    # option value or "" if the -filename is 'Unknown'
    #
    method GetDefaultFile {} {
	set filename $options(-filename)

	if {$filename eq "Unknown"} {
	    set default ""
	} else {
	    set default [file tail $filename]
	}
	return $default
    }
    ##
    # Return the correct default directory.  This is either the dirname of -filename or
    # [pwd] if that is 'Unknown'.
    #
    method GetDefaultDir {} {
	set filename $options(-filename)

	if {$filename eq "Unknown"} {
	    set default [pwd]
	} else {
	    set default [file dirname $filename]
	}
	return $default
    }

}