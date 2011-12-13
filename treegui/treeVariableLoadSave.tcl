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

package provide treeVariableLoadSave 1.0

##
#  Provides the snit megawiget that saves and restores
#  tree parameters.  The layout of the widget is as follows:
#  
#  -------------------------------------------------------------+
#  | [Load ]   [Save]    Current file:  /path/to/last/file      |
#  +------------------------------------------------------------+
#
# OPTIONS
#   -filename   - Full cannonicalized path to the most recent load/save file.
#   -loadcmd    - Script to process the load operation. %F is the path of the file
#                 selected by the user.  The last file is the default.
#   -savecmd    - Script to process the save operation. %F is the path of the file
#                 selected by the user.  The last file is the default.
#
snit::widget treeVariableLoadSave {
    hulltype ttk::frame

    option -filename -default [list]
    option -loadcmd  -default [list]
    option -savecmd  -default [list]

    # File types for the file box prompts.  We probably have two more
    # than actually needed.
    
    typevariable fileTypes {
	{{Tcl Scripts} {.tcl}  }
	{{Text files}  {.txt}  }
	{{Tree variable files} {.tv} }
	{{All Files}     * }
    }

    ##
    # Construct the widgets, lay them out and connect them to our
    # local action handlers.
    # @param args - option/value pairs.
    #
    constructor args {
	$self configurelist $args

	ttk::button $win.load -text "Load" -command [mymethod OnLoad]
	ttk::button $win.save -text "Save" -command [mymethod OnSave]
	ttk::label  $win.cfile -text " Current File: "
	ttk::label  $win.filename -textvariable ${selfns}::options(-filename) -width 60

	grid $win.load $win.save $win.cfile $win.filename
    }
    #-----------------------------------------------------------------------------
    #  Action handlers.

    ##
    #  Handle the Load button.  We prompt for a filename using 
    #  as a default directory the path and as a default name the name of the most recent
    #  file if it exists.  Otherwise the cwd is used and there is no default filename.
    # 
    method OnLoad {} {


	# Figure out where and which file was last and make that the default.

	set defaults [$self GetDefaultFile]

	
	# Prompt the user for the file.  If it's empty the user cancelled:

	set filename [tk_getOpenFile -defaultextension .tcl -filetypes $fileTypes \
			  -initialdir [lindex $defaults 0] \
			  -initialfile [lindex $defaults 1]  \
			  -parent $win \
			  -title "Select treevariable loadfile"]

	if {$filename ne ""} {
	    set options(-filename) $filename
	    $self Dispatch -loadcmd $filename
	}
    }
    ##
    # Handle the save button.  Prompt for the filename as for OnLoad and 
    # dispatch to the user's script.
    #
    method OnSave {} {
	# Figure out where and which file was last and make that the default.

	set defaults [$self GetDefaultFile]

	
	# Prompt the user for the file.  If it's empty the user cancelled:

	set filename [tk_getSaveFile -defaultextension .tcl -filetypes $fileTypes \
			  -initialdir [lindex $defaults 0] \
			  -initialfile [lindex $defaults 1]  \
			  -parent $win \
			  -title "Select treevariable loadfile"]

	if {$filename ne ""} {
	    set options(-filename) $filename
	    $self Dispatch -savecmd $filename
	}
    }
    #----------------------------------------------------------------------------
    #  Local private methods:

    ##
    # Return the current file and default file as a list.
    # @return list
    # @retval [list defaultdirectory defaultfilename]
    #
    method GetDefaultFile {} {
	set defaultFile $options(-filename)
	if {$defaultFile ne ""} {
	    set defaultDir  [file dirname $defaultFile]
	    set defaultFile [file tail $defaultFile]
	} else {
	    set defaultDir [pwd]
	    set defaultFile ""
	}
	return [list $defaultDir $defaultFile]
    }
    ##
    #  Dispatch one of the scripts:
    # @param option - selects which script is being dispatched.
    # @param filename - The filename which substitutes for any %F's.
    #
    method Dispatch {option filename} {
	::treeutility::dispatch $options($option) [list %F %W] [list $filename $win]
    }
}