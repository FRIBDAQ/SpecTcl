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

package provide spectrumManipulation 1.0

##
#  Provides a widget for manpipulating spectra.  This consists of two sets of widgets.
#  The top bar of widgets mostly manipulates spectra that have been created while
#  the bottom bar is used to define spectra that have been created.
#
#  Widget layout is:
#
#   +-------------------------------------------------------------------------------------+
#   | Spectrum Name                  [Create/replace] [Clear] [Delete]     Gate^ [Apply]  |
#   | [                    ]         [ ] Array        [] All  [Duplicate]  [   ] [Ungate] |
#   +-------------------------------------------------------------------------------------+
#   |  Parameter^     Low   High Bins   Units | Y Parameter  Low   High Bins  Units       |
#   | [           ]   [  ] [   ] [  ]   xxxx  | [        ]   [  ]  [  ] [  ]  xxxxx       |
#   +-------------------------------------------------------------------------------------+
#
#  This implies a widget construction of two frames that are gridded vertically.
#  In addition the top frame has an inner pair of frames. The left one contains the
#  spectrum related stuff and the right one the Gate related stuff.
#
# OPTIONS:
#  -clearcmd    - Script that is invoked when the Clear button is clicked on the top frame.
#                 %W Substitutes for the widget name.
#  -all         - The boolean state of the 'All' checkbutton.
#
# METHODS:
#
# 
snit::widget spectrumManipulation {
    hulltype ttk::frame

    option -clearcmd  -default [list]
    option -all       -default 0
    option -deletecmd -default [list]
    option -dupcmd    -default [list]
    option -ungatecmd -default [list]

    ##
    # Construct the widget and lay it out.  
    # We also connect the events, bindings etc. to scripts.
    # 
    # @args - The option name/values that configure this widget at construction time
    #
    constructor args {
	$self configurelist $args

	#
	# Set up the frame hierarchy.

	# Top frame:
	
	install topframe        using ttk::frame $win.top          -relief groove -borderwidth 2
	install spectrumops     using ttk::frame $win.top.spectra -relief groove -borderwidth 2
	install gateops         using ttk::frame $win.top.gates   -relief groove -borderwidth 2

	# Bottom frame:

	install bottomframe     using ttk::frame $win.bottom    -relief groove -borderwidth 2
	install xparameter      using ttk::frame $win.bottom.x -relief groove -borderwidth 2
	install yparameter      using ttk::frame $win.bottom.y -relief groove -borderwidth 2

	# Layout the top frame now:

	# Spectrum operations.
	
	ttk::button       $win.top.spectra.clear -text Clear -command [mymethod Dispatch -clearcmd]
	ttk::checkbutton  $win.top.spectra.all   -text All   -variable ${selfns}::options(-all)
	ttk::button       $win.top.spectra.delete -text Delete -command [mymethod Dispatch -deletecmd]
	ttk::button       $win.top.spectra.duplicate -text Duplicate -command [mymethod Dispatch -dupcmd]


	# Gate operations:

	ttk::button       $win.top.gates.ungate -text Ungate -command [mymethod Dispatch -ungatecmd]



	# Layout the widgets:

	grid $win.top.spectra.clear     -row 0 -column 2 -sticky nsew 
	grid $win.top.spectra.delete    -row 0 -column 3 -sticky nsew 
	
	grid $win.top.spectra.all       -row 1 -column 2 -sticky nsew 
	grid $win.top.spectra.duplicate -row 1 -column 3 -sticky nsew 

	grid $win.top.gates.ungate -sticky nsew 

	grid columnconfigure $win.top.spectra 0 -weight 2
	grid columnconfigure $win.top.spectra [list 1 2 3] -weight 1



	grid $win.top.spectra $win.top.gates -sticky nsew

	grid columnconfigure $win.top 0 -weight 5
	grid columnconfigure $win.top 1 -weight 1

	grid $win.top    -sticky nsew

	grid $win.bottom.x $win.bottom.y -sticky nsew
	grid columnconfigure $win.bottom all -weight 1
	grid $win.bottom      -sticky nsew

	grid columnconfigure $win all -weight 1
       
    }

    #----------------------------------------------------------------------------------------
    #  Event/bindings handlers.

    ##
    #  Dispatch an option script.  The subtitutions we provide are
    #  - %W - The widget ($self)
    # @param option - name of the option that contains the script to dispatch.
    #
    method Dispatch option {
	::treeutility::dispatch $options($option) [list %W] [list $self]
    }
}