#
#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2005.
#
#    You may use this software under the terms of the GNU public license
#    (GPL).  The terms of this license are described at:
#
#     http://www.gnu.org/licenses/gpl.txt
#
#     Author:
#             Ron Fox
#            NSCL
#            Michigan State University
#            East Lansing, MI 48824-1321
#
package provide editGammaSummary 1.0
package require snit
package require browser
package require guiutilities

#
#   This widget provides the spectrum type specific part of the spectrum
#   editor for gamma summary spectra.  Gamma summary spectra are like
#   summary spectra, where each x coordinate column is actually a gamma spectrum
#  (compare with summary spectra where each column is just a 1-d spectrum).
#
#  This spectrum editor is kind of unique in that it can also define the spectrum
#  in terms of existing spectra.
#
# APPEARANCE:
#
# +---------------------------------------------------------------------+
# | +--------------------------+              +---------------------+   |
# | |  Browser with gamma      |              | Parameter list with |   |
# | | spectra, parameters and  |  +-------+   | separators for each |   |
# | | gates                    |  | Next  |   | x channel that look |   |
# | | (no details)             |  |channel|   | like:               |   |
# | |                          |  +-------+   | --- Channel n  ---  |   |
# | +--------------------------+              +---------------------+   |
# |                                          Low  High Channels Units   |
# |                                         [   ] [   ] [    ] [    ]   |
# +---------------------------------------------------------------------+
#
#  BINDINGS:
#     double clicking a gamma spectrum will add all of its parameters
#       to the end of the parameter list box.
#     double clicking a parameter will add it to the end of the parameter
#       list box.
#     double clicking a gate will set the spectrum's initial gate
#       (handled in the containing GUI).
#     double clicking a parameter in the list box will:
#        Remove it from the list box.
#        If that empties out the channel, that channel is destroyed
#        and the remaining ones renumbered.  Note, however there  will always
#        be a channel 0 separator.
#     double clicking a separator removes it and all of the parameters in that
#        channel.
#     Clicking the 'Next Channel' button creates a new separator for the
#        next channel in the summary spectrum.
#
# OTHER:
#   The low high channels, units boxes:
#    Users can type into them.
#    If parameters have been selected, and all boxes are empty,
#    they are loaded from the information in the first of the parameters
#    if that is a tree parameter (else left blank).
#
# OPTIONS:
#   -browser   - provides a browser to put in the left side.  This is actually
#                a required option, though that is not checked... errors will
#                result if not supplied, however.
# 'PUBLIC' METHODS:
#   reinit     - Reinitializes the widget/data associated with the widget
#                to an empty state.
#   load name  - Loads the editor with the gamma summary spectrum described
#                by 'name'.
#   getHelpTopic - Returns a help topic that describes how to use the GUI
#                  (used by the framing GUI to know what help to display when
#                  the 'Help' button it displays is clicked.
#   getParameters - Provides the parameters as a list of parameter lists.
#   getAxes       - Provides the low,high,channels,units as a  list.
#                   Empty if not defined.
#
snit::widget editGammaSummary {
    option -browser {}
    
    # Quick access widget variables:
    
    variable parameterListBox;            # Parameter list box widget.
    variable lowEntry;                    # Low limit entry widget.
    variable hiEntry;                     # High limit entry widget.
    variable channelEntry;                # Entry widget for axis channel count
    variable unitEntry;                   # Units entry widget.
    
    # Data required:
    
    variable parameters;                  # Array of parameter lists.
    variable separatorCoords;             # Array of separator y coordinates.
    variable currentChannel 0
    
    #
    #  Constructor - builds the GUI and lays it out.
    #  After that the remainder is event driven.
    constructor args {
        $self configurelist $args
        
        # Configure the browser and update it as we will be changing
        # significantly what it can display.
        
        set browser $options(-browser)
        $browser configure   -spectrumscript [mymethod selectSpectrum]
        $browser configure   -parameterscript [mymethod selectParameter]
        $browser configure   -filterspectra   [mymethod gsSpectraOnly]
        $browser configure   -restrict   [list spectra parameters gates]
        $browser configure   -detail 0
        $browser update

        # top level frame:
        
        set topframe [frame $win.topframe]
        
        #  The next frame contains the Next Channel button, the list box
        #  and its scrollbar.
        #
        set rightframe [frame $topframe.rightframe -relief flat]
        
        label  $rightframe.parlabel -text {Parameter Lists}
        button $rightframe.nextchan -text {Next Channel} \
                                    -command [mymethod nextChannel]
        set list [frame $rightframe.list]
        set parameterListBox \
            [listbox $list.parameters \
                        -yscrollcommand [list $list.scroll set] \
                        -height 20]
        scrollbar $list.scroll     -orient vertical    \
                                            -command [list $parameterListBox yview]
        
        # Grid the widgets in the rightframe:
        
        grid $parameterListBox $list.scroll -sticky nsew
        
        grid     x                $rightframe.parlabel
        grid $rightframe.nextchan $list
        
        
        #  The next frame contains the axis definition widgets and their labels:
        #  brframe - bottom right frame.
        
        set brframe [frame $topframe.brframe -borderwidth 3 -relief groove]
        label $brframe.lowlbl   -text Low
        label $brframe.hilbl    -text High
        label $brframe.chanlbl  -text Bins
        label $brframe.unitlbl  -text Units
        
        set lowEntry [entry $brframe.low      -width 5]
        set hiEntry  [entry $brframe.high     -width 5]
        set channelEntry [entry $brframe.bins     -width 5]
        set unitEntry [entry $brframe.units    -width 5]
        
        #  grid the axis specification widgets:
        
        grid $brframe.lowlbl $brframe.hilbl $brframe.chanlbl $brframe.unitlbl
        grid $brframe.low    $brframe.high  $brframe.bins    $brframe.units
        
        
        # Layout the frames and the browser in the GUI:
        
        grid $rightframe -sticky nsew
        grid $brframe    -sticky e

        pack $browser $topframe -side left -fill y -expand 1
        
        
        # Additional event bindings:
        
        bind $parameterListBox <Double-1>  [mymethod removeParam %x %y]
        
        $self reinit        
    }
    
    # reinit
    #   Initialize the GUI state and the internal data state as if nothing
    #   had ever been entered:
    #
    method reinit  {} {
        #
        # Get rid of internal data structures:
        #
        foreach element [array names parameters] {
            unset parameters($element)
        }
        set currentChannel -1
 
        #
        # Empty the list box except for --- Channel 0 ---:
        #
        $parameterListBox delete 0 end
        $self nextChannel
        
        #
        #  Empty out the axis specification entries:
        #
        setEntry $lowEntry {}
        setEntry $hiEntry {}
        setEntry $channelEntry {}
        setEntry $unitEntry {}
        
        
    }
    
    
    # gsSpectraOnly description
    #
    #     Filter that ensures that the spectrum chooser part of the
    #     browser only displays gamma spectra, that is type g1.
    #
    # Parameters:
    #    description - Spectrum description for a candidate spectrum
    # Returns:
    #   0   - Spectrum should not be in browser.
    #   1   - Spectrum should be in browser.
    #
    method gsSpectraOnly description {
        set type [lindex $description 2]
        if {$type eq "g1"} {
            return 1
        } else {
            return 0
        }
    }
    #  getHelpTopic
    #   Returns the topic that supplies context sensitive help for the editor.
    # Returns:
    #   "gammasummary"
    #
    method getHelpTopic {} {
        return "gammasummary"
    }
    
    # nextChannel
    #
    #  Advances to specifying the next channel
    #  This:
    #   - increments currentChannel
    #   - inserts a channel marker for the current channel at the bottom of the
    #     parameter list box.
    #   - creates an empty list of parameters for the new column.
    #
    #
    method nextChannel {} {
        incr currentChannel
        set parameters($currentChannel) [list]
        $parameterListBox insert end "--- Channel $currentChannel ---"
        
        # Store the separator index in the separatorCoords array:
        
        set index [$parameterListBox index end]
        incr index -1
        set separatorCoords($currentChannel) $index
        
        
    }
    # selectParameter
    #   Called when the user double clicks a parameter.
    #   the parameter is added to the listbox and the current channels list.
    #   If the axis specifications are empty, they are set from this as well:
    # Parameters:
    #   path   - the browser path to the element.
    method selectParameter path {
        set name [::pathToName $path]
        
        # If axis info is available for the parameter pull it out
        # and conditionally set the axis entries:
        

            
	$self setAxisIfNotSet $name
    
        
        #  Save the parameter and update the listbox:
        
        lappend parameters($currentChannel) $name
        $parameterListBox insert end $name
    }
    
    # setAxisIfNotSet
    #
    #   If lists boxes are emtpy in the axis spec,
    #   they are filled from the corresponding items
    #   in the method parameters:
    # Parameters:
    #   param  - name of the parameter to use for setting.
    #            this will only be a no-op if the
    #            parameter is a tree param.
    #
    method setAxisIfNotSet parameter {

        set info [treeparameter -list $parameter]

        if {[llength $info] != 0} {
            set info [lindex $info 0]
            set low [lindex $info 2]
            set hi  [lindex $info 3]
            set bins [lindex $info 1]
            set units [lindex $info 5]
	}
        if {[$lowEntry get] eq ""} {
            setEntry $lowEntry $low
        }
        if {[$hiEntry get] eq ""} {
            setEntry $hiEntry $hi
        }
        if {[$channelEntry get] eq ""} {
            setEntry $channelEntry $bins
        }
        if {[$unitEntry get] eq ""} {
            setEntry $unitEntry $units
        }
        
        
    }
    
    #  removeParam x y
    #
    #  Remove the item from the parameter list box that has just been double
    #  clicked.  Then regenerate the list box:
    #
    # Parameter:
    #    x,y     - The coordinates of the pointer relative to the widget
    #              when the double click occured.
    #
    method removeParam {x y} {
        set coords @$x,$y
        set itemIndex [$parameterListBox index $coords]
        
        # we need to figure out where we are relative to the channel as well
        # as which channel, so that we can delete from the appropriate
        # channel list:
        
        for {set i $currentChannel} {$i >= 0} {incr i -1} {
            if {$itemIndex >= $separatorCoords($i)} {
                break
            }
        }
        #
        #   Can't delete separators:
        #
        if {$itemIndex == $separatorCoords($i)} {
            return
        }
        # Figure out which item it is in that parameter list.
        
        set chanArrayIndex [expr $itemIndex - $separatorCoords($i) -1]
        set parameters($i) [lreplace $parameters($i) $chanArrayIndex $chanArrayIndex]
        
        $self regenerateListBox
        
        
    }
    
    #
    #  Regenerate the list box after a paramter has been removed.
    #  the parameter removal may result in channel deletion.
    #
    method regenerateListBox {} {
        $parameterListBox delete 0 end
        set newCurrentChannel -1
        
        # First make a new parameters array that does not have any
        # empty slots:
        
        for {set i 0} {$i <= $currentChannel} {incr i} {
            if {[llength $parameters($i)] != 0} {
                incr newCurrentChannel
                set  newParameters($newCurrentChannel) $parameters($i)
            }
        }
        # Kill off the old parameter and separator coords arrays:
        
        foreach element [array names parameters] {
            unset parameters($element)
            unset separatorCoords($element)
        }
        
        # Now build up the new information
        
        set currentChannel -1
        
        # If there are no channels with data, just make the first empty one
        # and we're done:
        
        if {$newCurrentChannel == -1} {
            $self nextChannel
        } else {
            #  Build up the new listBox, parameters,and separatorCoords array:
            
            for {set i 0} {$i <= $newCurrentChannel} {incr i} {
                $self nextChannel
                set parameters($i) $newParameters($i)
                foreach parameter $parameters($i) {
                    $parameterListBox insert end $parameter
                }
            }
        }
    }
    #
    #   Lets the user select a spectrum.  
    #   Only gamma 1-d spectra are displayed.  When the user selects
    #   a gamma spectra, all of its parameters are added to the current
    #   channel list.
    #
    #  Parameters:
    #    path     - Tree path to the item double clicked.
    # 
    method selectSpectrum path {
	set name [::pathToName $path];	#  Spectrum name.
	set list [spectrum -list $name]

	set def  [lindex $list 0]
	set params [lindex $def 3]

	foreach param $params {
	    lappend parameters($currentChannel) $param
	    $parameterListBox insert end $param
	    $self setAxisIfNotSet $param

	}

    }    

    # Returns the parameters from the spectrum definition.
    # this is a list of parameter lists.  Each sublist defining the parameters
    # in a vertical stripe.
    #
    method getParameters {} {
	set result [list]
	for {set i 0} {$i <= $currentChannel} {incr i} {
	    lappend result $parameters($i)
	}
       	return $result
    }
    # Returns the axis definition for hte
    # spectum. 
    #  Since the user can type these in, be sure they are all of the correct type:
    #
    method getAxes {} {
	set low  [$lowEntry    get]
	set hi   [$hiEntry      get]
	set bins [$channelEntry get]

	if {[string is double -strict $low] &&
	    [string is double -strict $hi]  &&
	    [string is integer -strict $bins]} {
	    return [list [list $low $hi $bins]]
	} else {
	    return [list]
	}
    }
    #
    #  Load an existing spectrum into the editor.
    # Parameters:
    #    name   - name of the spectrum to load.  Note that
    #             The caller has ensured this is a gamma summary spectrum.
    #
    method load name {
	$self reinit
	set info [lindex [spectrum -list $name] 0]
	
	set pars       [lindex $info 3]
	set axis       [lindex [lindex $info 4] 0]

	# Fill the parameters array, and then ask the listbox to update itself.

	
	foreach column $pars {
	    set parameters($currentChannel) $column
	    set separatorCoords($currentChannel) 0;#  for regen listbox.
	    incr currentChannel
	}
	incr currentChannel -1;		# it incremented too far.
	
	$self regenerateListBox

	# Now the axis specs.. The units come fromt he first parameter,
	# but everything else from the axis specs:

	set p [lindex [lindex $pars 0] 0]
	$self setAxisIfNotSet $p

	set low [lindex $axis 0]
	set hi  [lindex $axis 1]
	set bins [lindex $axis 2]

	setEntry $lowEntry $low
	setEntry $hiEntry $hi
	setEntry $channelEntry $bins

    }

    
}