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
package require MetaDataEditor;   # Editor for metadata.


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
    delegate method addFit to wfplot;   # Expose the addFit method to the controller.

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
    

    component mdeditor
    delegate option -metadata to mdeditor
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

        install mdeditor using MetadataEditor $win.metadata 
        grid    $mdeditor -sticky nsew

        # bottom:

        ttk::button $win.commit -text {Commit Changes} -command [mymethod _commitRelay] \
            -state disabled

        grid $win.commit


       
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

        # now the metadata:

        set metadata [dict get $desc metadata]
        $mdeditor load $metadata
    
        #  Enable the commit button too:

        $win.commit configure -state normal
        
    }
    #  Private methods:

    
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
# METHODS:
#    addFit - adds a fit to the existing plot. 
#
# @note This stuff means that there is a defined order of operations
#   that must be followed:
#    1.  -samples must be set to create the plot.
#    2.  If desired, -name can be set to title the plot.
#    3.  0 or more callse to addFit can be made to superimpose
#       fit lines on the plot.
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

    # fitColors is the set of colors the fit lines cycle through.
    # fitColorIndex is the index of the next fit color in that list.

    variable fitColors [list               \
        red green blue magenta cyan yellow \
        "dark green" brown "hot pink"      \
    ]
    variable fitColorIndex 0

    #  Canvas dimensions so we can easily change them as we tweak.
    variable height 500
    variable width 800
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
    # Public methods:
    #

    ##
    # addFit name points
    #   Adds a fit line to the existing plot.  -samples must have
    #   been configured first.
    #   * The fit name is used to construct the series name as fit.fitname
    #   * The Series is added to the legend as "Fit: fitname"
    #
    #  @param name - name of the fit.
    #  @param points - fit points.
    #
    method addFit {name points} {
        if {$plotName ne ""} {
            # Make the x points:

            set xpts [_xpoints $points]
            set seriesName "fit.$name"
            set seriesTitle "Fit: $name"

            $plotName dataconfig $seriesName -type line -color [$self _nextColor]
            $plotName plotlist $seriesName $xpts $points [llength $points]
            $plotName legend $seriesName $seriesTitle

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
        set fitColorIndex 0;    # Reset the color index.
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
        $plotName legend trace waveform
        

        $plotName legendconfig -position top-right

        
    }
    #  _nextColor
    #    Return the next fit color in the fit color cycle:
    #
    method _nextColor {} {
        set result [lindex $fitColors $fitColorIndex]
        
        
        # Next fitColorIndex, Cycle if needed.
        incr fitColorIndex
        if {$fitColorIndex >= [llength $fitColors]} {
            set fitColorIndex 0
        }

        return $result
    }
    #  Utility proces:

    #  _ymax - compute the max of a plot given its points.
    #    we use max + 0.05*max

    proc _ymax series {
        set max [expr max([join $series ,])]
        set max [expr 1.05*$max]
        if {$max == 0} {
            set max 100.0
        }
        return $max
    }

    #  _xlabels - given an xaxis spec return a list of labels, every 100.

    proc _xlabels axis {
        return [_labels $axis 100]
    }
    # _ylabels - given an axis spec return labels every 1000

    proc _ylabels axis {
        return [_labels $axis 500]
    }
    # _labels - return labels for a given interval

    proc _labels {axis interval} {
        set max [lindex $axis 1]
        set result [list]
        if {$max == 0} {
            return [list 0.0 500.0]
        }
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
        
        set wfinfo [lindex [waveform getall $name] 0];    # dict of all:
        # Plot the trace:

        set points [dict get $wfinfo waveform]
       # set points [lindex $points 0] ;   # Could be several waveforms. each is name points 
        
        $options(-view) plot $points

        # Plot ach fit:

        foreach fit [dict get $wfinfo fits] {
            $options(-view) addFit [dict get $fit name] [dict get $fit points]
        }
    }
}