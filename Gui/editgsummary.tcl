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
    
    #---------------------------------------------------------------------------
    #   Stubs
    #---------------------------------------------------------------------------
    
    method selectSpectrum args {}
    method selectParameter args {}
    
    method nextChannel args {}
    
    method reinit args {}
    method load args {}
    method getHelpTopic args {return ""}
    method getParameters args {return [list]}
    method getAxes args {return [list]}
    
}