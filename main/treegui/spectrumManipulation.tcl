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
package require treemenuWidget
package require spectrumAxis

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
# TODO:
#   Add the rest of the option docs.
#
# METHODS:
#
# 
snit::widget spectrumManipulation {
    hulltype ttk::frame

    option -clearcmd       -default [list]
    option -all            -default 0
    option -deletecmd      -default [list]
    option -dupcmd         -default [list]
    option -ungatecmd      -default [list]
    option -gates          -default [list] -configuremethod NewGates
    option -gateselectcmd  -default [list]
    option -gate           -default [list]
    option -applycmd       -default [list]

    option -spectrumname   -default [list]
    option -parameters     -default [list] -configuremethod SetParameters
    option -createcmd      -default [list]
    option -array          -default 0
   

    # Delegations for the axes:

    #    X:

    delegate option -xparameter to xaxis as -parameter
    delegate option -xlow       to xaxis as -low
    delegate option -xhi        to xaxis as -high
    delegate option -xunits     to xaxis as -units
    delegate option -xbins      to xaxis as -bins
    delegate option -xparamselected to xaxis as -command
    delegate option -xchanged   to xaxis as -changed
    
    #    Y:

    delegate option -yparameter to yaxis as -parameter
    delegate option -ylow       to yaxis as -low
    delegate option -yhi        to yaxis as -high
    delegate option -yunits     to yaxis as -units
    delegate option -ybins      to yaxis as -bins
    delegate option -yparamselected to yaxis as -command 
    delegate option -ystate     to yaxis as -state
    delegate option -ychanged   to yaxis as -changed

    delegate option -arraystate to array as -state


    ##
    # Construct the widget and lay it out.  
    # We also connect the events, bindings etc. to scripts.
    # 
    # @args - The option name/values that configure this widget at construction time
    #
    constructor args {

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

	# Stock the menus now.

	$self configurelist $args


	# Spectrum operations.

	ttk::label       $win.top.spectra.label  -text {SpectrumName}
	ttk::entry       $win.top.spectra.name   -textvariable ${selfns}::options(-spectrumname)

	ttk::button      $win.top.spectra.create -text Create/Replace -command [mymethod Dispatch -createcmd]
	install array using ttk::checkbutton $win.top.spectra.array  -text Array -onvalue 1 -offvalue 0 \
	    -variable ${selfns}::options(-array)
	
	ttk::button       $win.top.spectra.clear -text Clear -command [mymethod Dispatch -clearcmd]
	ttk::checkbutton  $win.top.spectra.all   -text All   -variable ${selfns}::options(-all)
	ttk::button       $win.top.spectra.delete -text Delete -command [mymethod Dispatch -deletecmd]
	ttk::button       $win.top.spectra.duplicate -text Duplicate -command [mymethod Dispatch -dupcmd]



	# Gate operations:

	ttk::menubutton   $win.top.gates.gatesel -text Gate -menu $win.top.gates.gatesel.gates
	treeMenu          $win.top.gates.gatesel.gates -command [mymethod MenuDispatch -gateselectcmd %L %N]
	ttk::button       $win.top.gates.apply   -text Apply  -command [mymethod Dispatch -applycmd]
	ttk::entry        $win.top.gates.gateentry -width 12  -textvariable ${selfns}::options(-gate)
	ttk::button       $win.top.gates.ungate -text Ungate  -command [mymethod Dispatch -ungatecmd]

	# The two axis widgets:


	install xaxis using spectrumAxis     $win.bottom.x.axis
	install yaxis using spectrumAxis     $win.bottom.y.axis


	# Layout the widgets:


	grid $win.top.spectra.label     -row 0 -column 0 -sticky nsew
	grid $win.top.spectra.name      -row 1 -column 0 -sticky nsew

	grid $win.top.spectra.create    -row 0 -column 1 -sticky w
	grid $win.top.spectra.array     -row 1 -column 1 -sticky w

	grid $win.top.spectra.clear     -row 0 -column 2 -sticky nsew 
	grid $win.top.spectra.delete    -row 0 -column 3 -sticky nsew 
	
	grid $win.top.spectra.all       -row 1 -column 2 -sticky nsew 
	grid $win.top.spectra.duplicate -row 1 -column 3 -sticky nsew 

	grid $win.top.gates.gatesel   -row 0 -column 0  -sticky nsw
	grid  $win.top.gates.apply    -row 0 -column 1  -sticky nse
	grid $win.top.gates.gateentry -row 1 -column 0  -sticky nsw
	grid $win.top.gates.ungate    -row 1 -column 1  -sticky nse

	grid columnconfigure $win.top.spectra 0 -weight 3
	grid columnconfigure $win.top.spectra [list 1 2 3] -weight 1



	grid $win.top.spectra $win.top.gates -sticky nsew

	grid columnconfigure $win.top 0 -weight 9
	grid columnconfigure $win.top 1 -weight 1

	grid $win.top    -sticky nsew

	grid $win.bottom.x.axis 
	grid $win.bottom.y.axis

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
    ## 
    # Dispatch a menu selection from a tree menu.
    # This differs from Dispatch in that %L %N are also substituted for.
    # @param option - the option to dispatch.
    # @param label  - Menu label clicked.
    # @param path   - Full menu path clicked.
    #
    method MenuDispatch {option label path} {

	# The extra [list] commands below allow for paths and labels with spaces and other
	# special characters.

	::treeutility::dispatch $options($option) [list %W %L %N] [list $self [list $label] [list $path]]
    }

    #-------------------------------------------------------------------------------------
    #  Configuration management

    ##
    #  Configuration of the -gates option changed...destroy and recreate the tree menu.
    #  @param option - name of the option that was modified.
    #  @param value  - list of the gates to display.
    #
    method NewGates {option value} {
	set options($option) $value

	destroy  $win.top.gates.gatesel.gates

	treeMenu $win.top.gates.gatesel.gates -items $value -command [mymethod MenuDispatch -gateselectcmd %L %N]
    }
    ##
    # Configure the parameters into the two parameter pull downs.
    # @param option - name of option (-parameters)
    # @param value  - New value of the option.
    #
    method SetParameters {option value} {
	set options($option) $value
	

	$xaxis configure -parameters $value
	$yaxis configure -parameters $value
    }
}