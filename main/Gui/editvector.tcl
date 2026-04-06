package provide editvector 1.0
package require snit


##
#  we're going to take a little different approach
# as in my opinion, after the fact, editparameter et fils
# couple too tightly to the fact they are editing parameters.
#
#

### 
#   vectoreditor 
#
#  Provides the widget with the view.  It's shaped like a 
#  parameter editor  but has these options:
#
#  OPTIONS:
#
#
#  -low - low value.
#  -high - high value.
#  -bins - bins recommended for axes.
#  -units - units of measure.
#  -name  - name of the vector.
#
# Note that whenever -low, -high, -bins or -units are
# configured, their values are cached for restoration.
#
# METHOD: 
#   Modal - makes this a modal dialog.
#
#   
snit::widget editvector {
    hulltype toplevel
    #  Script options
    

    option -low -default 0     -configuremethod _configvalue
    option -high -default 100  -configuremethod _configvalue 
    option -bins -default 100  -configuremethod _configvalue
    option -units -default ""  -configuremethod _configvalue
    option -name -default ""

    variable reversion -array [list \
        -low 0 -high 100 -bins 100 -units "" \
    ]

    variable action "";         # what caused me to exit modality.

    #  Constructor..
    #  
    constructor args {
        # Row of labels:
        ttk::label $win.namelbl -text Name
        ttk::label $win.lowlbl  -text Low
        ttk::label $win.hilbl   -text High
        ttk::label $win.binlbl  -text Bins
        ttk::label $win.unitslbl -text Units

        #  Now the values:

        ttk::label $win.name -textvariable [myvar options(-name)]
        ttk::entry $win.low  -textvariable [myvar options(-low)]
        ttk::entry $win.high -textvariable [myvar options(-high)]
        ttk::entry $win.bins -textvariable [myvar options(-bins)]
        ttk::entry $win.units -textvariable [myvar options(-units)]

        #  Now the action frame:

        set action [ttk::frame $win.actions -borderwidth 3 -relief groove]
        ttk::button $action.ok -text Ok -command [list set [myvar action] Ok]
        ttk::button $action.accept -text Accept -command [list set [myvar action] Accept]
        ttk::button $action.revert -text Revert -command [mymethod _onRevert] 
        ttk::button $action.cancel -text Cancel -command [list set [myvar action] Cancel]

        # Lay this all out:

        grid $win.namelbl $win.lowlbl $win.hilbl $win.binlbl $win.unitslbl
        grid $win.name $win.low $win.high $win.bins $win.units

        grid $action.ok $action.accept $action.revert $action.cancel
        grid $action -columnspan 5 -sticky ew
    


        $self configurelist $args;   #Update from the configuration parameters.
    }
    ##
    # _configvalue 
    #    Called when configuring a new value for a metadata option.
    #  We save in the reversion array and set the option which modifies the UI.
    method _configvalue {opt value} {
        set reversion($opt) $value;                # New revert target.
        set options($opt) $value
    }
    ##
    # _onRevert
    #    Revert the GUI to the reversion array.
    # 
    method _onRevert {} {
        foreach opt [array names reversion] {
            set options($opt) $reversion($opt)
        }
    }
    

    #------------------------- public methods.

    ##
    # modal
    #    makes the dialog modal.
    #   This will return the reason for leaving modal which can be one of 
    #   Ok - ok button clicked.
    #   Accept - accept button clicked.
    #   Cancel - Cancel button clicked.
    #   destroyed     - Widget was destroyed.
    #
    method modal {} {
        bind $win <Destroy> [list set [myvar action] destroyed]

        # Make sure we get visible

        while {![winfo viewable $win]} {
            update
        }

        grab set $win

        vwait [myvar action];    # Wait for an action or destrution.

        catch [grab release $win]; # Would fail if $win was destroyed.

        if {[info exists action]} {
            return $action
        } else {
            return destroyed
        }
    }

}