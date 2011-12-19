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


package provide spectrumType 1.0


##
#  This class provides the spectum type selector.
#  This contains the following widgets laid out as follows:
#
#  +------------------------------------------------------+
#  |  () 1D    () Stripchart        | () Word (16 bits)   |
#  |  () 2D    () Bitmask           | () Long (32 bits)   |
#  +------------------------------------------------------+
#
#  Note that the original provided additional spectrum types
#  that we don't because the they really can't be represented by the
#  mechanics provided.  The original GUI also  provided byte data types
#  we don't bother with either.
#
# OPTIONS:
#   -spectrumtype   - SpecTcl spectrum type contained by the radio button.
#                     NOTE if is possible for this value to be 'invalid'
#                     for the radio buttons but valid for SpecTcl.
#   -datatype       - Value of data type radio button.  In addition to the
#                     SpecTcl types of word and long, this could possibly
#                     have the value of 'byte' 
# 
# @note currently all types of spectra support all data types, so there's
#       no need to couple the data types to the spectrum types.  Note as well
#       that SpecTcl supports a Byte (8 bits) data type but as I'd be surprised
#       if anybody uses it it's going to be omitted here.
#
  
snit::widget spectrumType {
    hulltype ttk::frame

    option -spectrumtype -default 1   -configuremethod Dispatch
    option -datatype    -default long
    option -command      -default [list] 

    ##
    # Construct the widget.  No behavior is really required.
    #
    # @param args - the option name value pairs that make up the initial configuration.
    #
    constructor args {
	$self configurelist $args

	ttk::frame $win.spectypes -relief groove -borderwidth 3
	ttk::frame $win.datatypes -relief groove -borderwidth 3

	#  The radio button set for spectrum types

	ttk::label $win.spectypes.typelabel -text "Spectrum Type"
	foreach type [list 1 2 S b] label [list 1D 2D Stripchart Bitmask] {
	    ttk::radiobutton $win.spectypes.t$type -text $label \
		-variable ${selfns}::options(-spectrumtype) -value $type \
		-command [mymethod dispatch]
	}
	# The radio button set for data types:

	ttk::label $win.datatypes.datatypelabel -text "Data Type"
	foreach type [list word long] label [list "Word (16 bits)" "Long (32 bits)"] {
	    ttk::radiobutton $win.datatypes.d$type -text $label -value $type \
		-variable ${selfns}::options(-datatype)
	}

	# Layout the widgets and frame:

	grid $win.spectypes.typelabel -columnspan 2
	grid $win.spectypes.t1 $win.spectypes.tS -sticky nsw -padx 15 -pady 5
	grid $win.spectypes.t2 $win.spectypes.tb -sticky nsw -padx 15 -pady 5

	grid $win.datatypes.datatypelabel 
	grid $win.datatypes.dword -sticky nsw -pady 5 -padx 15
	grid $win.datatypes.dlong -sticky nsw -pady 5 -padx 15

	grid $win.spectypes -row 0 -column 0 -sticky nsw
	grid $win.datatypes -row 0 -column 1 -sticky nse
	
    }
    #----------------------------------------------------------------------------------
    #  Configuration mangement

    ##
    # Dispatch to a user script if the spectrum type changed.
    # @param option -the configuration option changing.
    # @param value  - new value.
    #
    method Dispatch {option value} {
	set options($option) $value

	$self dispatch
    }

    #-------------------------------------------------------------------------
    # Event handlers:

    ##
    # Dispatch the -command script:
    #
    method dispatch {} {
	::treeutility::dispatch $options(-command) [list] [list]
    }	
}