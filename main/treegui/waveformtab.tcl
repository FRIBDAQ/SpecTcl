##
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
#	    

# @file waveformtab.tcl
# @brief Provide a GUI tab to view waveforms.
# @note requires SpecTcl 7.0 or higher.

package provide SpecTclWaveforms 1.0

package require Tk;               # We are a GUI.
package require snit;             # for megawidgets.
package require Plotchart;        # FOr waveform plots.


##
#  WaveformWidget
#     This mega-widget contains the waveform widget. It can be installed in a
#     It has several compoents:
#   - A WaveformList widget which list the waveforms.
#   - A WaveformMetadataEditor widget which allows selected waveform metadata to b
#     edited.
#   - A WaveformPlotter widget which allows the selected waveform to be gotten and
#     plotted with Plotchart.
#
snit::widget WaveformWidget {

}


##
#  WaveFormList
#     This megawidget provides a list box and provides a double-click
#     callback with the selected waveform name.
#     The list box has vertical and horizontal scroll bars.
# OPTIONS:
#   -waveforms - names of the waveforms to list.
#   -selectcommand - Script to call if a waveform is double clicked.
#
snit::widget WaveformList {
    option -waveforms -default [list] -configuremethod _update
    option -selectcommand -default [list] 

    constructor args {
        #  Construct/layout the widget, process the configuration.

        
        ttk::scrollbar $win.vscroll -orient vertical -command [list $win.list yview]
        ttk::scrollbar $win.hscroll -orient horizontal -command [list $win.list xview]
        listbox $win.list -selectmode single \
            -xscrollcommand [list $win.hscroll set] -yscrollcommand [list $win.vscroll set]

        grid $win.list $win.vscroll -sticky nsew
        grid $win.hscroll -sticky nsew

        $self configurelist $args

        #  Add a bind for double-1 to dispatch to _selectRelay

        bind $win.list <Double-1> [mymethod _selectRelay]
    }
    # Methods:


    ##
    #  _update - invoked when the -waveforms list has been updated
    #  The list box is cleared and the waveforms are added one-by-one to the
    # @param opt  - option name (always -waveforms).
    # @param values - list of waveform names.
    #
    method _update {opt values} {
        # Can't fail so set the option:

        set options($opt) $values

        $win.list delete 0 end;              # clear the box.

    
        foreach name $values {
            $win.list insert end $name;     # add one-by-one to the list.
        }
    }
    ##
    # _selectRelay
    #    Invoked on a double click event in the list box. 
    #  If there is a non-empty -selectcommand option, we invoke the script
    #  passing it the name of the selected waveform.   The script is run
    # at the global level.
    method _selectRelay {} {
        set selection [$win.list curselection]
        set command $options(-selectcommand)

        # Require a script and a selection.
        if {[llength $selection] > 0 && $command ne ""} {
            set name [$win.list get $selection];   # There can be only one.
            uplevel 0 $command $name
        }
    }

}

##
#  WaveformMetadataEditor
#    This megawidget provides an editor for the metadata associated with a 
#    waveform.  Metadata are held in a ttk::treview table and consist of name
#    value pairs.  The editor also displays and allows you to edit the samples
#    value of the waveform even though that's not actually metadata.
#
# Layout:
#
#   +------------------------------------------------------+
#   | <name label>  <samples label> <samples value entry>  |
#   |                                                      |
#   |   +------------------------------------------+       |
#   |   |  Metadata in editable treeview           |       |
#   ...                 ...                                |
#   |   +------------------------------------------+
#   |           [Commit button]                            |
#   +------------------------------------------------------+
#
#  OPTIONS:
#     -command - called when the commit button is clicked
#     -samples (readonly) - number of samples.
#     -name    (readonly) - waveform name.
#     -metadata (readonly) -dict of curnent metadata keyed by name, 
# PUBLIC METHODS:
#    load   - loads the widget from the description of the waveform
#             (e.g. the output of a waveform list command for that name
#
#
snit::widget WaveformMetadataEditor {
    option -command -default [list]
    option -samples -default 0 -readonly 1
    option -name -default "                " -readonly 1
    option -metadata -default "" -readonly 1

    #
    # constructor
    #   Note that the commit button is initially disabled 
    #   the first 'load' operation will enable it.  This ensures
    #   the command script won't be calle dwith an  illegal configuration.
    # @param args - configuration options at construction time. 
    constructor args {
        $self configurelist $args

        # Top line of the mwidget.    
        frame $win.top
        ttk::label $win.top.name -textvariable [myvar options(-name)]
        ttk::label $win.top.samplelbl -text "Samples:"
        ttk::entry $win.top.samples -textvariable [myvar options(-samples)] \
            -validate focusout -validatecommand [mymethod _validSamples %s]
        
        grid $win.top.name $win.top.samplelbl $win.top.samples
        grid $win.top -sticky nsew

        # Middle:

        ttk::treeview $win.tree \
            -show headings -columns [list name value] -displaycolumns [list name value] \
            -selectmode browse
        $win.tree heading name -text name
        $win.tree heading value -text value
        grid $win.tree -sticky nsew

        # bottom:

        ttk::button $win.commit -text {Commit Changes} -command [mymethod _commitRelay] \
            -state disabled
        grid $win.commit


        # Establish the event handlers needed to edit metadata.

    }
    #  Public methods

    ##
    # load
    #   Loads data into the widget.
    #
    #  @param desc - description of the waveform as gotten from waveform list.  This is a dict with
    #  keys:
    #      - name - name of the waveform
    #      - samples - number of samples in the waveform.
    #      - metadata - itself a dict of name keys and value values.
    #
    method load desc {
        # Set the top stuff:

        set options(-name) [dict get $desc name]
        set options(-samples) [dict get $desc samples]

        # now the meatdata:

        set metadata [dict get $desc metadata]
        set existing [$win.tree children {}]
        $win.tree delete $existing;      # Clear the tree.

        dict for {key value} $metadata {
            $win.tree insert {} end -values [list $key $value]
        }
        

        # Enable the button:

        $win.commit configure -state normal
    }

}
