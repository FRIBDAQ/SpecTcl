# 
#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2005.
#
#   You may use this software under the terms of the GNU public license
#    (GPL).  The terms of this license are described at:
#
#     http://www.gnu.org/licenses/gpl.txt
#
#     Author:
#             Ron Fox
#	     NSCL
#	     Michigan State University
#	     East Lansing, MI 48824-1321
#

package provide preferences 1.0
package require snit
package require guiutilities
package require guihelp



#
# Editor dialog for preferences.
# The dialog is laid out as shown below:
#
#  +---------------------------------------------+
#  | 2-d axis defaults:                          |
#  | X axis:  [         ]                        |
#  | Y axis:  [         ]  [] Same as X          |
#  +---------------------------------------------+
#  | NSCL DAQ:                                   |
#  | NSCL DAQ root: [           ]  [browse...]   |
#  +---------------------------------------------+
#  | [Ok]   [Cancel] [Help]                      |
#  +---------------------------------------------+
#
# OPTIONS:
#     -xdefault   - Value in the X axis entry.
#     -ydefault   - Value in the Y axis entry.
#     -daqroot    - Value of location of nscldaq root.
#
#     -okscript   - Script to execute on click of ok.
#     -cancelscript - Script to execute on click of cancel.
#     -helpscript   - Script to execute on click of help.

#
#    
snit::widget prefsEditor {
    hulltype toplevel
    option   -xdefault
    option   -ydefault
    option   -daqroot

    option   -okscript
    option   -cancelscript
    option   -helpscript

    variable same 0
    variable hidden ""


    constructor args {
	label $win.axistitle  -text {2-d Axis defaults}
	label $win.xaxistitle -text {X axis:}
	entry $win.xaxis      -width 8 -textvariable ${selfns}::options(-xdefault) \
	                      -vcmd [mymethod xkey %P] -validate all
	label $win.yaxistitle -text {Y axis:}
	entry $win.yaxis       -width 8 -textvariable ${selfns}::options(-ydefault)
	checkbutton $win.same  -text {Same as X} \
	                       -variable ${selfns}::same   \
	                       -command [mymethod setSameAxes]

	frame $win.daqparams  -relief groove -borderwidth 3
	label $win.daqparams.title -text {NSCLDAQ Parameters}
	label $win.daqparams.rootlbl -text {NSCL DAQ Root: }
	entry $win.daqparams.root    -textvariable ${selfns}::options(-daqroot) \
	                             -width 15
	button $win.daqparams.browse -text Browse... -command [mymethod browseRoot]

	frame $win.action -relief groove -borderwidth 3
	button $win.action.ok     -text Ok -command       [mymethod dispatch -okscript]
	button $win.action.cancel -text Cancel -command   [mymethod dispatch -cancelscript]
	button $win.action.help   -text Help   -command   [mymethod dispatch -helpscript]

	# Layout the stuff:

	grid $win.axistitle        -             -
	grid $win.xaxistitle      $win.xaxis     x
	grid $win.yaxistitle      $win.yaxis     $win.same

	grid $win.daqparams.title       -                       -
	grid $win.daqparams.rootlbl $win.daqparams.root   $win.daqparams.browse
	grid $win.daqparams        -columnspan 3 -sticky ew

	pack $win.action.ok $win.action.cancel $win.action.help -side left
	grid $win.action -columnspan 3 -sticky ew

	$self configurelist $args

    }

    # browseRoot
    #   Browses for the root directory of the NSCLDAQ software.
    #
    method browseRoot {} {
	set options(-daqroot) [tk_chooseDirectory \
				   -title {Choose NSCLDAQ root directory}]

    }


    # dispatch opt
    #   Dispatches control to the appropriate action script.
    #
    method dispatch opt {
	set script $options($opt)
	if {$script ne ""} {
	    eval $script
	}
    }

    # setSameAxes ... invoked by the same checkbutton.
    #                Mostly the variable same does what we need,
    #                but if this is a transition to same=1, load
    #                the y axis entry with the x axis value and
    #                disable the y .
    #
    method setSameAxes {} {
	if {$same} {
	    setEntry $win.yaxis [$win.xaxis get]
	    $win.yaxis configure -state disabled
	} else {
	    $win.yaxis configure -state normal
	}
    }
    # xkey final
    #    Got a keystroke in the xaxis entry.  If same is set,
    #    Transfer the current value to the yaxis..
    #
    method xkey final {
	if {$same} {
	    $win.yaxis configure -state normal
	    setEntry $win.yaxis $final
	    $win.yaxis configure -state disabled
	}
	return 1
    }
    
}


namespace eval preferences {
    variable SpecTclDefaults ".SpecTclDefaults"
    namespace export editPrefs readPrefs savePrefs
}

proc preferences::ok {} {
    set ::GuiPrefs::preferences(defaultXChannels) [.prefs cget -xdefault]
    set ::GuiPrefs::preferences(defaultYChannels) [.prefs cget -ydefault]
    set ::GuiPrefs::preferences(defaultDaqRoot)   [.prefs cget -daqroot]
    destroy .prefs
}
proc preferences::cancel {} {
    destroy .prefs
}
proc  preferences::help {} {
    spectclGuiDisplayHelpTopic prefs
}

proc preferences::editPrefs {} {
    prefsEditor .prefs  -xdefault $::GuiPrefs::preferences(defaultXChannels) \
	                -ydefault $::GuiPrefs::preferences(defaultYChannels) \
	-okscript     preferences::ok                                            \
	-cancelscript preferences::cancel                                   \
        -helpscript   preferences::help

    focus .prefs
    grab  .prefs
    tkwait window .prefs
    
}
proc preferences::readPrefs {} {
    if {[file readable [file join ~ $::preferences::SpecTclDefaults]]} {
	namespace eval ::GuiPrefs {
	    source [file join ~ $::preferences::SpecTclDefaults]
	}
    }
}
proc preferences::savePrefs {} {
    set fd [open [file join ~ $::preferences::SpecTclDefaults] w]
    puts $fd "# SpecTcl gui preferences written [clock format [clock seconds]]"
    foreach pref [array names GuiPrefs::preferences] {
	puts $fd "set preferences($pref) $GuiPrefs::preferences($pref)"
    }
    close $fd
}