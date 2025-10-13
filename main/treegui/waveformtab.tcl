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
#  OPTIONS:
#     -names        - waveform names.
#     -selectscript - Script to handle the listing selecting a waveform.  Receives the waveform name as a param
#     -waveform  - Set the current waveform in the md editor. 
#     -wfupdatescript - Update a waveform definition. Receives the waveform definition dict as a parameter.
#     -plotupdatescript - Update the plot. Receives the waveform name as a parameter.
# Methods:
#    plot  plot a waveform.
#
snit::widget WaveformWidget {
    component listing
    component mdeditor
    component wfplot

    option -selectscript
    option -waveform -configuremethod _setWaveform
    option -wfupdatescript
    option -plotupdatescript

    delegate option -names to listing as -waveforms

    constructor args {
        install listing using WaveformList $win.list -selectcommand [mymethod _selectWaveform]
        install mdeditor using WaveformEditor $win.editor -command [mymethod _updateWfDef]
        install wfplot using WaveformDisplay $win.plot -command [mymethod _updatePlot]

        grid $listing $mdeditor -sticky s
        grid $wfplot -columnspan 2

        $self configurelist $args
    }

    # Public methods:

    ##
    # plot
    #
    #   Given data from waveform list for a single waveform, plot it.
    #
    # @param data - dtaa is at least a two element list containing the name of the
    #     waveform and the data points.  At least because for MPI, there will actually
    #     be a trace from each worker.
    #
    method plot data {
        set name [lindex $data 0]
        set trace [lindex $data 1]
        $wfplot configure -name $name -samples $trace
    }
    # Private methods

    ##
    #  _setWaveform
    #    Called by the client to:
    # * Load a waveform definition into the editor.
    # * set the name in the waveform plot.
    # param opt - name of the optin, always -waveform.
    # @param value - should be a waveform definition from waveform list.
    #
    method _setWaveform {opt value} {
        set name [dict get $value name]
        $mdeditor load $value
        $plot configure -name $name

        set options($opt) $value

       
    }
    ##
    #  _selectWaveform
    #     Called when a waveform was selected from the listing widget.
    #    If options(-selectscript) is not empty it is called with the
    #   selected waveform name as the parameter.
    #
    # @param name - waveform name that was selecte.
    #
    method _selectWaveform name {
        set script $options(-selectscript)
        if {$script ne ""} {
            uplevel 0 $script $name
        }
    }
    ##
    #  _updateWfDef
    #    Called when the waveform metadata editor has had commit clicked.
    # If options(-wfupdatescript) is defined, it is called with the new
    # waveform definition dict.
    #
    method _updateWfDef {} {
        set script $options(-wfupdatescript)
        if {$script ne ""} {
            set name [$mdeditor cget -name]
            set samples [$mdeditor cget -samples]
            set metadata [$mdeditor cget -metadata]

            set definition [dict create name $name samples $samples metadata $metadata]
            uplevel 0 $script {$definition};   # since uplevel unwraps a level of list.
        }
    }
    ##
    # _updatePlot
    #    Clicked if the waveform plot wants an update.
    # Preconditions:
    #   There must be a selected waveform name in the metadata editor.
    #   There must be a -plotupdatescript
    #
    #  If these preconditions are met, the plotupdate script is called at level 0.
    #  with the selected name as the parameter.  It is expected that it will get the
    #  waveform samples and invoke plot.
    #
    method _updatePlot {} {
        set name [$mdeditor cget -name]
        set script $options(-plotupdatescript)
        if {($name ne "") && ($script ne "")} {
            uplevel 0 $script $name
        }
    }
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
#   |  <metadata name> <value>  [new button]               |
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
snit::widget WaveformEditor {
    option -command -default [list]
    option -samples -default 0 -readonly 1
    option -name -default "" -readonly 1
    option -metadata -default "" -readonly 1 -cgetmethod _getmetadata

    #
    # constructor
    #   Note that the commit button is initially disabled 
    #   the first 'load' operation will enable it.  This ensures
    #   the command script won't be calle dwith an  illegal configuration.
    # @param args - configuration options at construction time. 
    constructor args {
        $self configurelist $args

        # Top line of the mwidget.    
        frame $win.top -relief groove
        ttk::label $win.top.name -textvariable [myvar options(-name)] -width -10
        ttk::label $win.top.samplelbl -text "Samples:"
        ttk::entry $win.top.samples -textvariable [myvar options(-samples)] \
            -validate focusout -validatecommand [mymethod _validSamples %s]
        
        grid $win.top.name $win.top.samplelbl $win.top.samples
        grid $win.top -sticky nsew

        # Metadata editing:

        set md [labelframe $win.metadata -relief groove -text metadata]
        ttk::label $md.name -width -10;   #name is loaded here.
        ttk::entry $md.value
        ttk::button $md.commit -text "Modify" -state disabled -command [mymethod _updateMetadata]
        ttk::button $md.new -text "New..." -command [mymethod _newMetadata] -state disabled

        grid $md.name $md.value $md.commit
        grid x $md.new
        grid $md -sticky nsew

        # Metadata view.
        ttk::scrollbar $win.treescroll -orient vertical -command [list $win.tree yview]
        ttk::treeview $win.tree \
            -show headings -columns [list name value] -displaycolumns [list name value] \
            -selectmode browse -yscrollcommand [list $win.treescroll set]
        $win.tree heading name -text name
        $win.tree heading value -text value
        grid $win.tree $win.treescroll -sticky nsew

        # bottom:

        ttk::button $win.commit -text {Commit Changes} -command [mymethod _commitRelay] \
            -state disabled
        grid $win.commit


        # Establish the event handlers needed to edit metadata.

        bind $win.tree <<TreeviewSelect>> [mymethod _loadMetaEditor]
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
        

        # Enable the buttons:

        $win.commit configure -state normal
        $win.metadata.new configure -state normal
    }
    #  Private methods:

    ##
    # _getmetadata
    #   Marshall the metadata from the tree:
    #
    #  @param opt -name of the option (always -metdata)
    #  @return dict keyed by metadata name and with metadata values
    
    method _getmetadata opt {
        set result [dict create]
        foreach child [$win.tree children {}] {
            set key_value [$win.tree item $child -values]
            dict append result [lindex $key_value 0] [lindex $key_value 1]
        }

        return $result
    }
    ##
    # _validSamples
    #    Called when the entry loses focus If the entry contents are not a valid
    #  integer, the value is restored and a pop-up tells the user about the problem.
    #
    # @param old - original value(?)
    #
    method _validSamples old {
        
        if {[catch {snit::integer validate $options(-samples)}] || $options(-samples) <= 0} {
            set options(-samples) $old
            tk_messageBox -parent $win.top.samples -type ok -icon error \
                -message "Samples values must be an integer > 0"
            return 0
        }
        return 1
    }
    ##
    # _commitRelay
    #    If the # of samples is valid, call the commit script which can do what it wants.
    #
    method _commitRelay {} {
        set command $options(-command)
        if {$command ne ""} {
            # Require valid samples:

            if {[$self _validSamples $options(-samples)]} {
                uplevel 0 $command
            }
        }
    }
    ##
    # _loadMetaEditor
    #   Load the metadata editor with the currently selected metadata
    #   The metadata commit button is enabled so that once editing is 
    #   done the new value can be updated in the tree view.
    #
    method _loadMetaEditor {} {
        set selected [$win.tree selection]
        if {[llength $selected] == 0} {
            return ; # there's no selection actually.
        }
        set selectData [$win.tree item $selected -values]
        $win.metadata.name configure -text [lindex $selectData 0]
        $win.metadata.value delete 0 end
        $win.metadata.value insert 0 [lindex $selectData 1]
        $win.metadata.commit configure -state normal
    }
    ##
    # _updateMetadata
    #   The metadata Modiy button was clicked.  Pull the data from the
    #   metadata label and entry and update the table.
    #
    method _updateMetadata {} {
        set name [$win.metadata.name cget -text]
        set value [$win.metadata.value get]
        
        # Find the metadata item by name in the tree and update it.

        set item [$self _findMetadata $name]
        if {$item ne ""} {
            $win.tree item $item -values [list $name $value]
        }
    }
    ##
    #   _newMetadata
    #    Prompt for a new metadata item.  If one is added and has both name and value not empty,
    #    it is appended to the tree.  This uses the MetaDataPrompter dialog below.
    #
    method _newMetadata {} {
        MetadataPrompter $win.prompt
        set md [$win.prompt get]
        destroy $win.prompt
        if {$md eq ""} return;            # Cancdled.
        set name [lindex $md 0]
        set value [lindex $md 1]

        if {$name eq "" || $value eq ""} {
            tk_messageBox -parent $win -icon error -type ok \
                -message {Meta data must have both a name and a value; neither can be blank}
        } else {
            # Don't allow duplicates
            if {[$self _findMetadata $name] ne ""} {
                tk_messageBox -parent $win -icon error -type ok \
                    -message "$name is an existing metadata name, duplicates are not allowed"
            } else {
                $win.tree insert {} end -values [list $name $value]
            }
        }
    }
    #-- utility methods:

    #  _findMetadata - find a metadata item given its name:
    # Returnns an empty string if not found.

    method _findMetadata {name} {
        foreach item [$win.tree children {} ] {
            if {$name eq [lindex [$win.tree item $item -values] 0]} {
                return $item
            }
        }
        #  Not found.
        return ""
    }
    
}

##
#  MetadataPrompter
#    This is a modal dialog that prompts a user for a new bit of metadata.
#
#  Layout:
#   +--------------------------------+
#   | Name: [    ] Value [    ]      |
#   +--------------------------------+
#   | [Ok]      [Cancel]             |
#   +--------------------------------+
#
# Usage:
#\verbatim
#    MetadataPrompter .somepath
#    .somepath get
#\endverbatim
#
#  the get returns a two element list of name, value
#  The list is empty if cancel was clicked.
#
snit::widget MetadataPrompter {
    hulltype toplevel

    # The buttons set this to Ok for the ok button Cancel for the cancel button.
    # If the dialog is destroyed via its window controls, that's an implied cancel so...
    variable action Cancel;    
    variable  hiddenFrame
    constructor args {
        #  there are no args so go directly to layout.

        # The top part has the prompt stuff wrapped in a frame.

        set value [ttk::frame $win.value]
        ttk::label $value.namelbl -text "Name: " -relief groove
        ttk::entry $value.name
        ttk::label $value.valuelbl -text "Value: "
        ttk::entry $value.value

        grid $value.namelbl $value.name $value.valuelbl $value.value
        grid $value -sticky nsew

        # The button (action) part has the buttons:

        set action [ttk::frame $win.action]
        ttk::button $action.ok -text Ok -command [mymethod _onOk]
        ttk::button $action.cancel -text Cancel -command [mymethod _onCancel]
        grid $action.ok $action.cancel
        grid $action

    }
    ##
    # get
    #    crate a hidden frame, set modal and focus then wait for the
    #    hidden frame to be destroyed.
    # @return Returns the dialog 'value'.
    # 
    # @note The caller must destroy the dialog.
    #
    method get {} {
        set hiddenFrame [frame $win.hidden]
        focus $win
        tkwait window $hiddenFrame
        #  If the action variable does not exist we were destroyed by window controls:

        if {[catch {set action}]} {
            return [list]
        }

        # One of the buttons was clicked _or_ we were destroyed:

        if {$action eq "Cancel"} {
            return [list]
        } else {
            return [list [$win.value.name get] [$win.value.value get]]
        }
    }

    #  private methods (button handler)

    method _onOk {} {
        set action Ok
        destroy $hiddenFrame
    }
    method _onCancel {} {
        set action Cancel
        destroy $hiddenFrame
    }

}

##
#  WaveformDisplay
#   Provides a plotchart display for a waveform.
#
# OPTIONS:
#   -name   - name of the waveform.
#   -samples - Waveform samples - setting updates the plot.
#   -command - Script to execute when an update is required/requested.
#
# In this initial version only buttons update the display. Future version may
# provide for timed update requests.
#
# In this version the Y axis goes from 0-max(-samples) + 5%
#
# THe x axis goes from 0 - [llength -samples].
# 
# Layout:
#   +-------------------------------------+
#   |   plot area                         |
#   |      [Update]                       |
#   +-------------------------------------+
snit::widget WaveformDisplay {
    option -name -default ""  -configuremethod _updateTitle
    option -samples -default [list] -configuremethod _updatePlot
    option -command -default [list]

    variable plotName "";         # Name of the plot chart plot.

    #  Canvas dimensions so we can easily change them as we tweak.
    variable height 500
    variable width 700
    constructor args {
        #  We need to create the UI before doing the configure list
        #  In cas the -samples  ares set.

        canvas $win.plot -width $width -height $height
        grid $win.plot -sticky nsew
        ttk::button $win.update -text Refresh -command [mymethod _relayUpdate]
        grid $win.update
        
        $self configurelist $args
    }

    destructor {
        if {$plotName ne ""} {
            destroy $plotName;    # Kill any hanging plot.
        }
    }
    # Private methods:

    ##
    #  _relayUpdate
    #    Called when the plot wants an update.
    # 
    method _relayUpdate {} {
        set script $options(-command)
        if {$script ne ""} {
            uplevel 0 $script
        }
    }

    ##
    # _updateTitle
    #   Update the title string.  If there's a current plot,
    #   The title is set.
    # @param opt - option name, always -name.
    # @param value - new value for the option.
    #
    method _updateTitle {opt value} {
        set options($opt) $value

        if {$plotName ne ""} {
            # There's a plot:

            $plotName title $value top
        }
    }
    ##
    # _updatePlot
    #    If there's a plott it must be destroyed.  This involves destroying the canvas
    #    and the plot and recreating it.
    #  The axis limits for x/y must be computed
    #  The X/Y labels must be computed and the X series as well.
    #
    method _updatePlot {opt value} {
        set options($opt) $value
        destroy $win.plot
        if {$plotName ne ""} {
            destroy $plotName
            set plotName ""
        }
        # Figure out our axis ranges and labels.

        set xaxis [list 0 [llength $options($opt)] ""]
        set yaxis [list 0 [_ymax $options($opt)] ""]
        set xlabels [_xlabels $xaxis ]
        set ylabels [_ylabels $yaxis ]
        # Generate the canvas and plot:
        canvas $win.plot -width $width -height $height
        grid $win.plot -row 0 -column 0
        set plotName [Plotchart::createXYPlot $win.plot $xaxis $yaxis \
            -xlabels $xlabels -ylabels $ylabels          \
        ]
        $plotName title $options(-name) top;   #  Title (if ther's none it'll be blank).

        #  Generate the x series:

        set xpts [_xpoints $options($opt)]

        $plotName plotlist trace $xpts $options($opt) [llength $options($opt)]
        $plotName dataconfig trace -type line

    }

    #  Utility proces:

    #  _ymax - compute the max of a plot given its points.
    #    we use max + 0.05*max

    proc _ymax series {
        set max [expr max([join $series ,])]
        set max [expr 1.05*$max]
        return $max
    }

    #  _xlabels - given an xaxis spec return a list of labels, every 100.

    proc _xlabels axis {
        return [_labels $axis 100]
    }
    # _ylabels - given an axis spec return labels every 1000

    proc _ylabels axis {
        return [_labels $axis 1000]
    }
    # _labels - return labels for a given interval

    proc _labels {axis interval} {
        set max [lindex $axis 1]
        set result [list]

        for {set i 0} {$i <= $max} {incr i $interval} {
            lappend result $i
        }
        return $result
    }

    # _xpoints - produce the x pointes of the trace:

    proc _xpoints yvalues {
        set max  [expr {[llength $yvalues] - 1}]
        return [_labels [list 0 $max] 1]
    }
}

##
#  WaveformController
#     The controller for the waveform MVC triad.  The model is SpecTcl
# itself and the command set that provides access to the waveforms.
#
# OPTIONS
#    -view - the widget that contains the vew.  When it's confi9gured, we hook
#           into its script callbacks and alsso load the waveform names
#           into the view.
#
snit::type WaveformController {
    option -view -configuremethod _loadWaveforms
    

    constructor {args} {
        $self configurelist $args;    #might call _loadWaveforms.
    }
    #  Option processing:

    ##
    # _loadWaveforms
    #   Process setting the -view option.   The names of the waveforms are
    #  loaded into the view's -names option.
    #
    # @param option - name of the option (-view always).
    # @param value  - View widget command.
    #
    method _loadWaveforms {option value} {
        set options(-view) $value

        #Load the names into the view:

        set listing [waveform list];    # waveform dicts.
        set names [list]
        foreach def $listing {
            lappend names [dict get $def name]
        }
        $value configure -names $names
        
        # Next hook our callback methods into the script callbacks for the view:

        $value configure -selectscript [mymethod _selectWaveform] \
            -wfupdatescript [mymethod _wfredefine] -plotupdatescript [mymethod _plot]

    }
    #   View callback handlers:

    ##
    #  _selectWaveform
    #    Called when the user selected a waveform from the selection list
    #  We load it into the metadata editor part of the view:
    #
    # @param name - name of the selected waveform.
    #
    method _selectWaveform name {
        
        set def [waveform list $name]
        if {[llength $def] > 0} {;      # It's a list maybe deleted?
            set def [lindex $def 0]
            
            #There's a view since it called us:
            $options(-view) configure -waveform $def
        }
    } 
    ##
    # _wfredefine
    #    Called when the metadata editor part of the view has asked us to commit
    #   changes to the waveform definition.
    #
    #  @param def - Dict that is the new waveform definition.
    #
    method _wfredefine def {
        set name [dict get $def name]
        set samples [dict get $def samples]
        set metadata [dict get $def metadata]

        waveform resize $name $samples
        waveform metadata set $name {*}$metadata
    }
    ##
    # _plot
    #    Called when the user wants to update the plot of a waveform.
    #    We just need to get the waveform data and send it on to the view's 
    #   plot method
    #
    # @param name - name of the waveform.
    #
    method _plot name {
        set points [waveform get $name]
        set points [lindex $points 0] ;   # Could be several waveforms.

        $options(-view) plot $points
    }
}