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
package provide spectrum 1.0
package require snit
package require browser
package require edit1d
package require edit2d
package require edit2dmulti
package require editmulti
package require editstrip
package require editGammaDeluxe
package require guiutilities
package require Iwidgets
package require guihelp

#  spectrumGui implements a dialog that can edit
#  spectrum definitions.  It uses a technique
#  similar to that of the gate editor to
#  provide type specific editors for each spectrum type.
#  A slightly different wrinkle, however is that this dialog
#  has a common slot for a gate that can be initially applied
#  to the spectrum.    Thus at this level we create the
#  browser, populating it with gates and spectra and
#  providing the browser widget id to the spectrum
#  specific editor so that it can lay it out in its own
#  editor as it sees fit.
#
#   The overall layout of the dialog is:
#
#    +-------------------------------------------------------------+
#    |   {Spectrum Type}   Spectrum Name: [             ]          |
#    |   Gate: <...........>           Array? ( )                  |
#    +-------------------------------------------------------------+
#    |       area for spectrum type specific editors...            |
#    +-------------------------------------------------------------+
#    |  [OK]   [Accept]  [Cancel]                                  |
#    +-------------------------------------------------------------+
#
#  Options supported are:
#      Callbacks:
#       -cancelcommand    -  Called to cancel spectrum creation.
#       -acceptcommand    -  Called to create a new spectrum, while
#                            retaining this dialog.
#       -okcommand        - Called to create a new spectrum and dismiss
#                           this dialog.
#
snit::widget spectrumGui {
    hulltype toplevel

    option -cancelcommand {}
    option -acceptcommand {}
    option -okcommand     {}

    variable emptyString {<............>}
    variable spectrumType
    variable spectrumTypeNames

    variable helpTopic genericSpectrumBuilder

    constructor args {
        $self configurelist $args

        set spectrumType $emptyString
        array set spectrumTypeNames [list 1 1-d 2 2-d g1 {gamma 1-d} g2 {gamma 2-d} gd {Gamma 2-d x/y} s Summary b bitmask S {Strip Chart} m2 {2d Sum Spectrum}]


        # Top common contents frame:

        frame $win.common -relief groove -borderwidth 3
        menubutton $win.common.type -menu $win.common.type.menu -text {Spectrum Type} \
                                    -relief raised -borderwidth 3
        set typemenu [menu $win.common.type.menu -tearoff 0]
        $typemenu add command -label 1-d           -command [mymethod start1dEditor 1]
        $typemenu add command -label 2-d           -command [mymethod start2dEditor]
        $typemenu add separator
        $typemenu add command -label {gamma 1-d}   -command [mymethod startMultiparameterSpectrumEditor g1]
        $typemenu add command -label {gamma 2-d}   -command [mymethod startMultiparameterSpectrumEditor g2]
	$typemenu add command -label {gamma 2-d x/y} -command [mymethod startGamma2dDeluxeEditor]
	$typemenu add command -label {2-d Sum}     -command [mymethod start2dSumEditor]
        $typemenu add separator
        $typemenu add command -label {Summary}     -command [mymethod startMultiparameterSpectrumEditor s]
        $typemenu add command -label {bitmask}     -command [mymethod start1dEditor b]
        $typemenu add command -label {Strip Chart} -command [mymethod startStripchartEditor]

        label $win.common.namelabel -text {Spectrum Name: }
        entry $win.common.name


        label $win.common.gatelabel -text {Gate: }
        label $win.common.gate      -text $emptyString
        checkbutton $win.common.specarray -text {Array? }

        grid $win.common.type $win.common.namelabel $win.common.name
        grid $win.common.gatelabel $win.common.gate $win.common.specarray


        # Frame for the spectrum specific editor:

        frame $win.editor
        label $win.editor.contents -text {Select a spectrum type from the menu above}
        pack  $win.editor.contents

        # Buttons at the bottom of the gui.


        frame  $win.action        -relief groove -borderwidth 3
        button $win.action.ok     -text Ok      -state disabled   -command [mymethod dispatchOption -okcommand]
        button $win.action.accept -text Accept  -state disabled   -command [mymethod dispatchOption -acceptcommand]
        button $win.action.cancel -text Cancel                    -command [mymethod dispatchOption -cancelcommand]
        button $win.action.help   -text Help                      -command [mymethod onHelp]
        pack $win.action.ok $win.action.accept $win.action.cancel $win.action.help -side left


        pack $win.common -side top -fill x -expand 1
        pack $win.editor -side top -fill x -expand 1
        pack $win.action -side top -fill x -expand 1

        # Event bindings:

        bind $win.common.gate <Double-1> [list $win.common.gate configure -text $emptyString]

    }
    # reinit
    #     Reinit the current editor to the 'empty state'.
    #
    method reinit {} {
        $win.editor.contents reinit
        $win.common.gate configure -text $emptyString
        $win.common.name delete -0 end
    }
    # load name
    #     Load the spectrum editor with the specified spectrum.
    #
    method load name {
        $win.common.name configure -text $name

        set info [spectrum -list $name]
        set info [lindex $info 0]

        set type   [lindex $info 2]

        set gateinfo [apply -list $name]
        set gateinfo [lindex $gateinfo 0]
        set gate     [lindex $gateinfo 1]
        set gatename [lindex $gate     0]

        ::setEntry $win.common.name $name

        if {$gatename == "-TRUE-"} {
            set gatename $emptyString
        }
        $win.common.gate configure -text $gatename

        # Start up the type dependent editor and load it too.

        switch -exact -- $type {
            1 -
            b {
                $self start1dEditor $type
                $win.editor.contents load $name
            }
            2 {
                $self start2dEditor
                $win.editor.contents load $name
            }
	    gd {
		$self startGamma2dDeluxeEditor
	        $win.editor.contents load $name
	    }
            g1 -
            g2 -
            s {
                $self startMultiparameterSpectrumEditor $type
                $win.editor.contents load $name
            }
            S {
                $self startStripchartEditor
                $win.editor.contents load $name
            }
	    m2 {
		$self start2dSumEditor
		$win.editor.contents load $name
	    }
            default {
            }
        }

    }
    # createBrowser
    #     Create the browser appropriately configured for us.
    #     the editor we hand this too may require additional configuration.
    #     We will update on return from creating the editor to allow it
    #     to apply any necessary filtering.
    #
    method createBrowser {} {
        if {[winfo exists $win.editor.browser]} {
            destroy $win.editor.browser
        }
        browser $win.editor.browser -gatescript [mymethod selectGate] -restrict {parameters gates} \
                -showcolumns [list type low high bins units] -width 5in
        return $win.editor.browser
    }
    # start1dEditor stype
    #      Start a 1-d spectrum editor for spectrum stype
    #      (currently limited to 1 and b).
    # Parameters:
    #    stype   - The type of spectrum to create.
    #
    method start1dEditor {stype} {
        $self setSpectrumType $stype

        set browser [$self createBrowser]
        destroy $win.editor.contents

        edit1d $win.editor.contents -browser $browser

        pack $win.editor.contents -fill x -expand 1
        $browser update
        set helpTopic [$win.editor.contents getHelpTopic]

    }
    # start2dEditor
    #     Start a 2-d spectrum editor for types
    #     2.
    #
    method start2dEditor {} {
        $self setSpectrumType 2

        set browser [$self createBrowser]
        destroy $win.editor.contents

        edit2d $win.editor.contents -browser $browser

        pack $win.editor.contents -fill x -expand 1
        $browser update
        set helpTopic [$win.editor.contents getHelpTopic]

    }
    # startGamma2dDeluxeEditor
    #     Starts the editor for 2d gamma deluxe spectra.  These are gamma
    #     spectra that have parameters bound to specific axes.
    #
    method startGamma2dDeluxeEditor {} {
	$self setSpectrumType gd

	set browser [$self createBrowser]
	destroy $win.editor.contents

	editGammaDeluxe $win.editor.contents -browser $browser

	pack $win.editor.contents -fill x -expand 1

	$browser update
	set helpTopic [$win.editor.contents getHelpTopic]
    }
    # startMultiparameterSpectrumEditor stype
    #     Starts a spectrum creating editor for
    #     spectra that allow an unbounded number of parameters.
    #     this includes at present:  g1, g2, s.
    #     One parameterization need is the number of axis descriptions
    #     that the user will be allowed to specify.
    #     For g1, s this is 1, for g2 this is 2.
    #
    method startMultiparameterSpectrumEditor stype {
        $self setSpectrumType $stype

        set browser [$self createBrowser]
        destroy $win.editor.contents

        if {$stype == "g2"} {
            set axiscount 2
        } else {
            set axiscount 1
        }
        editmulti $win.editor.contents -browser $browser -axes $axiscount

        pack $win.editor.contents -fill x -expand 1
        $browser update
        set helpTopic [$win.editor.contents getHelpTopic]

    }
    #  Start2dSumEditor
    #    Starts a spectrumeditor for the 2-d sum spectra.
    #
    method start2dSumEditor {} {
	$self setSpectrumType m2
	destroy $win.editor.contents

	# Can't create the browser using createBrowser 'cause we want spectra too
	# and -restrict is not dynamic.
	
	destroy $win.editor.browser
        browser $win.editor.browser -gatescript [mymethod selectGate] \
	    -restrict {parameters gates spectra} \
	    -showcolumns [list type low high bins units] -width 5in
	edit2dMulti $win.editor.contents -browser $win.editor.browser

	pack $win.editor.browser $win.editor.contents -fill x -expand 1 -side left
	$win.editor.browser update

	set helpTopic [$win.editor.contents getHelpTopic]

    }

    # startStripchartEditor
    #     Starts the stripchart spectrum editor.
    #     This allows users to create stripchart spectra
    #     from inside the GUI.
    #
    #
    method startStripchartEditor {} {
        $self setSpectrumType S
        destroy $win.editor.contents

        set browser [$self createBrowser]
        editstrip $win.editor.contents -browser $browser
        pack $win.editor.contents -fill x -expand 1

        $browser update
        set helpTopic [$win.editor.contents getHelpTopic]
    }
    # getName
    #    Get the specturm name.. could be nil
    #
    method getName {} {
        set name [$win.common.name get]
        if {$name == $emptyString} {
            return [list]
        } else {
            return $name
        }
    }
    # getType
    #    Get the spectrum type.  In theory this
    #    cannot be empty, since the user is not allowed
    #    to activate ok or accept until a spectrum type has
    #    been chosen.
    #
    method getType {} {
        return $spectrumType
    }
    # getGate
    #     Return the current gate name.
    #      This can be empty if the user has not
    #      yet chosen a gate.
    #
    method getGate {} {
        set gateName [$win.common.gate cget -text]
        if {$gateName != $emptyString} {
            return $gateName
        } else {
            return [list]
        }
    }
    # getParameters (delegated)
    #      Requests the editor to return the list of parameters
    #      chosen by the user. Could be empty on a premature
    #      ok/accept.
    #
    method getParameters {} {
        return [$win.editor.contents getParameters]
    }
    # getAxes (delegated)
    #     Requests that the editor return the list of axis
    #     descriptions chosen by the user.  This can be empty
    #     if the user is prematurely clicking ok.
    method getAxes {} {
        return [$win.editor.contents getAxes]
    }
    # isArray
    #     Determines if the user is requesting an array of
    #     spectra:
    # Returns:
    #   1   - array is being requested.
    #   0   - array is not being requested.
    #
    method isArray {} {
        set name [$win.common.specarray cget -variable]
        global $name
        return [set $name]
    }
    # dispatchOption reason
    #     Dispatches to an external script stored in an options
    #     element
    # Parameters:
    #    reason  - an option e.g. -okcommand
    #
    method dispatchOption {reason} {
        if {$options($reason) != ""} {
            if {[catch {eval $options($reason)} errorMessage]} {
		# do nothing as the the thrower is supposed to provide an error dialog.
            }
        }
    }
    # onHelp
    #      Display the help topic.
    #      It is up to the rest of this class to keep the correct help topic in
    #      the helpTopic variable.
    #
    method onHelp {} {
        spectclGuiDisplayHelpTopic $helpTopic
    }
    # selectGate name
    #      Responds to double clicks on a gatename.  This loads
    #      the name of thet gate into the gate label.
    # Parameters:
    #   name   - full path of the gate (e.g. Gate....).
    #
    method selectGate {name} {
        set name [::pathToName $name]
        $win.common.gate configure -text $name
    }
    # setSpectrumType stype
    #      Peforms common code associated with changing the type
    #      of spectrum being created.  Sets the menu button's
    #      label appropriately, and enbles the ok/cancel buttons.
    #      Also stores the gate type away for when the gate is actually
    #      produced.
    #
    # Parameters:
    #    stype  - the SpecTcl gate type code.
    #
    method setSpectrumType {stype} {
        set typeName $spectrumTypeNames($stype)
        set spectrumType $stype

        $win.common.type configure -text $typeName
        $win.action.ok      configure -state normal
        $win.action.accept  configure -state normal
    }

}
# saveSpectrumDialog
#      This dialog allows the user to save a bunch of spectra.
#      The dialog is divided into a file selector from iwidgets
#      and a spectrum selector using the browser and a list box.
#  Layout:
#  Options:
#      - All those for ::iwidgets::fileselectionbox
#      -spectra  - The set of spectra in the spectrum selection box.
#      -okcommand
#      -cancelcommand
#  methods:
#      get,filter    - delegated to the fileselectionbox.
#
#
#
snit::widget saveSpectrumDialog {
    hulltype toplevel

    delegate method get    to filebox
    delegate method filter to filebox
    delegate option *      to filebox

    option -spectra
    option -okcommand
    option -cancelcommand

    variable hidden {}

    constructor args {
        install filebox using ::iwidgets::fileselectionbox $win.fbox -mask *.spec

        label   $win.speclabel -text {Spectra to Write}
        listbox $win.spectra     -yscrollcommand [list $win.scrollbar set]
        scrollbar $win.scrollbar -orient vertical -command [list $win.spectra yview]

        browser $win.b  -restrict spectra -detail 0 -showcolumns {type}    \
                        -filterspectra  [mymethod spectrumFilter]           \
                        -spectrumscript [mymethod addSpectrum] -width 3in -treewidth 1.8in

        frame $win.command          -borderwidth 3 -relief groove
        button $win.command.ok      -text Ok     -command [mymethod onOk]
        button $win.command.cancel  -text Cancel -command [mymethod onCancel]
        button $win.command.help    -text Help   -command [list spectclGuiDisplayHelpTopic savespectrum]

        grid $win.fbox                -                       -
        grid $win.b                   x                       x
        grid   ^                      $win.speclabel          x              -sticky s
        grid   ^                      $win.spectra            $win.scrollbar -sticky ns

        pack $win.command.ok $win.command.cancel $win.command.help -side left
        grid $win.command                -                     -   -sticky ew


        bind $win.spectra <Double-1> [mymethod removeSpectrum %x %y]

        $self configurelist $args
    }
    # modal
    #    Make the dialog modal.
    #
    method modal {} {
        if {$hidden == ""} {
            set hidden [frame $win.hidden]
            wm deiconify $win
            focus $win
            grab $win
            tkwait window $hidden
        }
    }

    # onOk
    #       Called when the Ok Button is clicked.
    #
    method onOk {} {
        set script $options(-okcommand)
        if {$script != ""} {
            eval $script
        }
        if {$hidden != ""} {
            destroy $hidden
            set hidden {}
        }

    }
    # onCancel
    #     Called when the cancel button is clicked.
    #
    method onCancel {} {
        $self configure -spectra {}
        setEntry $win.fbox.selection {}

         set script $options(-cancelcommand)
        if {$script != ""} {
            eval $script
        }
        if {$hidden != ""} {
            destroy $hidden
            set hidden {}
        }

    }
    # spectrumFilter
    #         Works with the browser to ensure that only the unselected
    #         spectra appear in the browser.
    # Parameters:
    #    descr   - The spectrum description for a spectrum to filter.
    method spectrumFilter descr {
        set spectrum [lindex $descr 1]
        set selected [$self cget -spectra]
        if {[lsearch -exact $selected $spectrum] ==  -1} {
            return 1
        } else {
            return 0
        }
    }
    # addSpectrum path
    #        Called on a double-click of a spectrum.
    #        Adds the spectrum to the list box.
    # Parameters:
    #    path  - The full path in the browser to the spectrum.
    #
    method addSpectrum path {
        set name [pathToName $path]
        $win.spectra insert end $name
        $win.b update
    }
    # removeSpectrum x y
    #          Removes the spectrum closest to the pointer from the list of selected
    #          spectra.
    # Parameters:
    #    x,y   - Widget relative coordinates of the pointer.
    method removeSpectrum {x y} {
        set index [$win.spectra index @$x,$y]
        $win.spectra delete $index
        $win.b update
    }
    # getFilter
    #      Returns the current value of the filebox filter string.
    #
    method getFilter {} {
        return [$win.fbox.filter get]
    }
    # configure -spectra list
    #     Sets the current set of spectra selected to the
    #     list supplied.
    #
    onconfigure -spectra list {
        $win.spectra delete 0 end
        foreach spectrum $list {
            $win.spectra insert end $spectrum
        }
        $win.b update
    }
    # cget -spectra
    #       Returns the set of spectra in the selected set.
    #
    oncget      -spectra {
        return [$win.spectra get 0 end]
    }
}
# readSpectrumDialog
#       This megawidget prompts for a file and the necessary switches
#       to read as many spectra as exist from file.
#       The switches accepted by the sread command are represented
#       by checkboxes labelled:   "snapshot", "replace if exists", and "bind to display"
#
# options
#     All supported by the iwidgets dialogbox
#     -snapshot       bool true if want snapshots on by default.
#     -replace        bool true if want replace on by default.
#     -bind           bool true if want bind on by default.
#     -okcommand
#     -cancelcommand
#
#   In the absence of these switches; -snapshot is true, -replace false, and -bind true.
#
snit::widget readSpectrumDialog {
    hulltype toplevel

    option -snapshot  1
    option -replace   0
    option -bind      1

    option -okcommand     {}
    option -cancelcommand {}

    delegate option  *      to filebox
    delegate method  get    to filebox
    delegate method  filter to filebox

    variable hidden {}

    constructor args {
        install filebox using ::iwidgets::fileselectionbox $win.fbox -mask *.spec

        checkbutton $win.snapshot -text {Snapshot}
        checkbutton $win.replace  -text {Replace existing spectra}
        checkbutton $win.bind     -text {Bind to display}

        frame  $win.command        -borderwidth 3 -relief groove
        button $win.command.ok     -text Ok      -command [mymethod onOk]
        button $win.command.cancel -text Cancel  -command [mymethod onCancel]
        button $win.command.help   -text Help    -command [list spectclGuiDisplayHelpTopic readspectrum]

        grid $win.fbox               -                -
        grid $win.snapshot           $win.replace     $win.bind
        pack $win.command.ok $win.command.cancel $win.command.help -side left
        grid $win.command            -                - -sticky ew

        # set default states of the checkboxes

        $self configure -snapshot 1 -replace 0 -bind 1

        $self configurelist $args
    }
    # modal
    #     Make the dialog modal.
    #
    method modal {} {
        if {$hidden == ""} {
            set hidden [frame $win.hidden]
            wm deiconify $win
            focus $win
            grab $win
            tkwait window $hidden
        }
    }
    # onOk
    #     Called for the ok button.
    #
    method onOk {} {
        set script $options(-okcommand)
        if {$script != ""} {
            eval $script
        }

        if {$hidden != ""} {
            destroy $hidden
            set hidden ""
        }
    }
    # onCancel
    #    called for the cancel button.
    #
    method onCancel {} {
        $win.fbox.selection delete 0 end

        set script $options(-cancelcommand)
        if {$script != ""} {
            eval $script
        }

        if {$hidden != ""} {
            destroy $hidden
            set hidden ""
        }
    }

    # configure -snapshot
    #           Set the value of the snapshot checkbutton.
    onconfigure -snapshot value {
        SetCheckButton $win.snapshot $value
    }
    # configure -replace
    #         set value of the replace checkbutton.
    onconfigure -replace value {
        SetCheckButton $win.replace $value
    }
    # configure -bind
    #      sEt the value of the bind checkbutton.
    #
    onconfigure -bind  value {
        SetCheckButton $win.bind $value
    }
    #    Get value of the snapshot checkbutton.
    #
    oncget -snapshot {
        return [GetCheckButton $win.snapshot]
    }
    #   Get value of the replace checkbutton.

    oncget -replace {
        return [GetCheckButton $win.replace]
    }
    #   Get value of the bind checkbutton.
    #
    oncget -bind     {
        return [GetCheckButton $win.bind]
    }


    proc SetCheckButton {widget value} {
        if {$value} {
            $widget select
        } else {
            $widget deselect
        }
    }
    proc GetCheckButton widget {
        set varname [$widget cget -variable]
        global $varname
        return [set $varname]
    }
}
# addSpectrum widget
#     Called to add a spectrum to the system.
#     If the spectrum already exists, the user is prompted
#     for overwrite, and if accepted, the old spectrum is
#     deleted and recreated.
#     - If there is a gate it is applied to the spectrum.
#     - The spectrum is bound to the displayer.
proc addSpectrum widget {
    set name [$widget getName]
    set type [$widget getType]
    set parameters [$widget getParameters]
    set axes  [$widget getAxes]
    set gate [$widget getGate]
    set array [$widget isArray]


    #  Ensure the definition is complete:

    if {($name == "")       || ($type == "") ||
        ($parameters == "") || ($axes == "")} {
        tk_messageBox -icon error -title {incomplete spectrum} \
           -message {The spectrum has not been completely described.  Please correctly fill in the dialog}
        error incomplete
    }

    # If an array is being asked for, the spectrum must be a 1-d only:
    #
    if {$array && ($type != "1")} {
        tk_messageBox -title {Cannot use array} -icon error \
            -message {Only 1-d spectra can be built in arrays. Uncheck the array checkbutton and try again}
        error {only  single}
    }
    #  Duplicate spectrum?

    if {!$array} {

	if {$type eq "m2"} {
	    set multiparams $parameters
	    set parameters [list]
	    foreach pair $multiparams {
		set x [lindex $pair 0]
		set y [lindex $pair 1]

		# If there are incomplete coordinate pairs complain and exit
		if {($x eq "") || ($y eq "")}  {
		    tk_messageBox -icon error -title {Unbalanced parameters} \
			-message {2-d sum spectra must have the same number of x and y parameters}
		    error incomplete
		}
		#  Unwrap the pair into the flat paramter list.
		lappend parameters $x
		lappend parameters $y
	    }
	}

        set info [spectrum -list $name]
        if {$info != ""} {
            set keep [tk_dialog .duplicate {Spectrum Exists} \
                        "$name is already a spectrum.  Do you want to replace it?" \
                         questhead 1 Ok Cancel]
            if {$keep} {
                error duplicate
            }
            spectrum -delete $name
        }
        # Make the new spectrum.
	
	# Special case code for m2 spectra.. which have parameters in some
	# funny way:


        set stat [catch {spectrum $name $type $parameters $axes} msg]
	if {$stat} {
	    tk_messageBox -icon error -title {failed to make}  \
		-message "Could not create spectrum $name : $msg"
	    error spectrumerror
	}
        set stat [catch {sbind $name} msg]
	if {$stat} {
	    tk_messageBox -icon error -title {failed to bind} \
		-message "Could not bind $name :  $msg"
	    error bindfailure
	}

        # If there's a gate apply it.

        if {$gate != ""} {
            apply $gate $name
        }
    } else {
        # Can only be one parameter:

        set parpath [split $parameters .]
        set last    [lindex $parpath end]
        if {[scan $last "%d" index] != 1} {
            tk_messageBox -title notArray -icon error \
                -message "$parameters is not an array element, cannot make an array of spectra"
            error notArray
        }
        set parbase [join [lrange $parpath 0 end-1] .]
        set parameters [parameter -list $parbase.*]
        foreach par $parameters {
            set pname [lindex $par 0]
            set path [split $pname .]
            set index [lindex $path end]

            #  Make the spectrum.

            set stat [catch {spectrum $name.$index $type $pname $axes} msg]
	    if {$stat} {
		tk_messageBox -icon error -title {failed to make}  \
		    -message "Could not create spectrum $name : $msg"
		error spectrumerror
	    }
            if {$gate != ""} {
                apply $gate $name
            }
	    set stat [catch {sbind $name.$index} msg]
	    if {$stat} {
		tk_messageBox -icon error -title {failed to bind} \
		    -message "Could not bind $name :  $msg"
		error bindfailure
	    }
	    

        }
    }
    .gui.b update
    $widget reinit
    failsafeWrite
}

proc addSpectrumDestroyWidget widget {
    addSpectrum $widget
    destroy $widget
}

# createSpectrum
#      Create a new spectrum.
#      This uses the spectrumGui class to request and edit a new spectrum.
#
proc createSpectrum {} {
    if {[winfo exists .newspectrum]} {
        return
    }
    spectrumGui .newspectrum   -okcommand     [list addSpectrumDestroyWidget .newspectrum] \
                               -acceptcommand [list addSpectrum .newspectrum]              \
                               -cancelcommand [list destroy .newspectrum]
}
# editSpectrum path
#      Edits an existing spectrum.  The
#      spectrum editor is brought up and loaded with the
#      specific spectrum.
# Parameters:
#   path   - The full path to the spectrum in the browser.
#
proc editSpectrum path {
    set name [::pathToName $path]

    createSpectrum

    .newspectrum load $name

}
# deleteSpectrum name
#     Asks the user if they want to delete a spectrum.
#     if so...does it.
#
proc deleteSpectrum  name {
    set nokill [tk_dialog .delspec "Delete" \
        "Are you sure you want to delete the spectrum $name?" \
        questhead  1 Yes No]
    if {!$nokill} {
        spectrum -delete $name
        failsafeWrite
    }
    .gui.b update
}
# writeSpectrum name
#     Writes the indicated spectrum to file after prompting for
#     a filename.
# Parameters:
#  name  - Name of the spectrum to write.
#
proc writeSpectrum name {
    set fname [tk_getSaveFile -defaultextension .spec             \
                              -title {Choose OutputFile}          \
                              -filetypes [list                    \
                                           [list "Spectrum Files" .spec] \
                                           [list "Text Files"     .txt]]]
    if {$fname != ""} {
        swrite -format ascii $fname $name
    }
}
# selectSpectrumListbox
#           This is a megawidget that can be used
#           to select a list of spectra.
# Options:
#    -spectrumlist    - Set/get the spectrum list.
# Methods:
#    get              - like cget -spectrumlist
#
# Layout:
#    +---------------------------------------------+
#    |  +---------------------+   +--------------+ |
#    |  | browser confined to |   |  selected    | |
#    |  | spectra only        |   | spectra.     | |
#    |  +---------------------+   +--------------+ |
#    +---------------------------------------------+

snit::widget selectSpectrumListbox {
    option -spectrumlist
    constructor args {
        label     $win.sellabel -text {Selected Spectra}
        listbox   $win.spectra   -yscrollcommand [list $win.paramscroll set]
        scrollbar $win.paramscroll -orient vertical -command [list $win.spectra yview]

        browser $win.b     -restrict spectra       \
                           -showcolumns   [list type low high units] \
                           -filterspectra [mymethod spectrumFilter] \
                           -spectrumscript [mymethod selectSpectrum]

        grid $win.b     $win.sellabel            - -sticky s
        grid   ^        $win.spectra $win.paramscroll -sticky ns

        bind $win.spectra <Double-1> [mymethod removeSpectrum %x %y]
        $self configurelist $args
    }
    # get
    #     Return the contents of the spectrum list box.
    #
    method get {} {
        return [$win.spectra get 0 end]
    }
    # spectrumFilter descrip
    #       Determines which spectra go in the browser.
    #
    method spectrumFilter description {
        set name [lindex $description 1]
        set selected [$self get]
        if {[lsearch -exact $selected $name] == -1} {
            return 1
        } else {
            return 0
        }
    }
    #  selectSpectrum
    #          Adds a spectrum from the browser to the selected list.
    #
    method selectSpectrum path {
        set name [pathToName $path]
        $win.spectra insert end $name
        $win.b update
    }
    # removeSpectrum   x y
    #         Remove the spectrum from the selected listbox that
    #         is under the pointer (x/y)
    #
    method removeSpectrum  {x y} {
        set index [$win.spectra index @$x,$y]
        $win.spectra delete $index
        $win.b update
    }
    #    Set the spectrum list box
    #
    onconfigure -spectrumlist list {
        $win.spectra delete 0 end
        foreadch spectrum $list {
            $win.spectra insert end $list
        }
        $win.b update
    }
    #  Get the spectrum list box contents.
    #
    oncget      -spectrumlist {
        return [$self get]
    }
}
# selectSpectrumList
#           This is a dialog megawidget that allows the use
#           to select a list of spectra.
#
# Options:
#    -okcommand     - Script to respond to ok button.
#    -cancelcommand - Script to respond to cancel button.
#    -spectrumlist  - Delegated to the selectSpectrumListbox component
#                     sets the contents of the spectrum list.
# Widget commands:
#    get            - Delegated to the selectSpectrumListbox
#                     retrieves the list of selected spectra
#    modal          - make the dialog modal.
#
#   Layout:
#         +-----------------------------+
#         | +-----------------------+   |
#         | | selectSpectrumListBox |   |
#         | +-----------------------+   |
#         |  <Ok>   <Cancel>            |
#         +-----------------------------+
#
#  See above for the structure of a selectSpectrumListBox.
#
snit::widget selectSpectrumList {
    hulltype toplevel

    option -okcommand
    option -cancelcommand

    delegate option -spectrumlist to spectrumlistbox
    delegate method get           to spectrumlistbox

    variable hidden {}

    constructor args {
        install spectrumlistbox using selectSpectrumListbox $win.listbox
        button $win.ok      -command [mymethod onOk]     -text Ok
        button $win.cancel  -command [mymethod onCancel] -text Cancel
        button $win.help    -command [list spectclGuiDisplayHelpTopic selectspectrumlist] -text Help

        grid $win.listbox       -
        grid $win.ok           $win.cancel $win.help 

        $self configurelist $args
        set hidden {}
    }
    # onOk
    #      Called when the Ok button is clicked.
    #      If the user supplied an -okcommand script it
    #      is invoked.   If the dialog is modal
    #      hidden is killed.
    #
    method onOk {} {
        # do this first in case the script kills us.

        if {$hidden != ""} {
            destroy $hidden
            set hidden {}
        }
        set script $options(-okcommand)
        if {$script != ""} {
            eval $script
        }

    }
    # onCancel
    #      Responds to a click on the cancel button.
    #      if the user supplied an -cancelcommand script
    #      it is invoked.
    #      If the dialog is modal, hidden is killed.
    #
    method onCancel {} {
        # do this first in case the script kills us.

        if {$hidden != ""} {
            destroy $hidden
            set hidden {}
        }

        set script $options(-cancelcommand)
        if {$script != ""} {
            eval $script
        }


    }
    #  modal
    #         Turn the dialog into a modal dialog.
    #
    method modal {} {
        if {![winfo exists $hidden]} {
            set hidden [frame $win.hidden]
            wm deiconify $win
            focus $win
            grab $win
            tkwait window $hidden

        }
    }
}
# deleteListOfSpectra  widget
#             Called when the deletesSpectrumList dialog's ok was clicked.
#             we confirm this is what the user wants and if so, delete the
#             spectra and destroy the dialog.  Note that if the spectrum
#             list is empty we just destroy the dialog.
# Parameters:
#    widget  - The spectrum list prompting dialog
#
proc deleteListOfSpectra widget {
    set spectrumList [$widget get]

    #  Empty spectrum list.

    if {[llength $spectrumList] == 0} {
        destroy $widget
        return
    }
    # Prompt for confirmation:

    set answer [tk_messageBox -icon question -title {Delete spectra} -type okcancel -message "\
You have selected the following spectra for deletion: [join $spectrumList {, }].  \
if you are sure you want to delete these spectra, click Ok, otherwise click Cancel to return \
to the spectrum selection dialog."]

    if {$answer == "ok"} {
        foreach spectrum $spectrumList {
            spectrum -delete $spectrum
        }
        failsafeWrite
        .gui.b update
        destroy $widget
    }

}

# selectAndDeleteSpectra
#        Selects a set of spectra to delete.
#
proc selectAndDeleteSpectra {} {
    if {[winfo exists .deletespectrumdialog]} return
    selectSpectrumList .deletespectrumdialog -okcommand [list deleteListOfSpectra .deletespectrumdialog] \
                                     -cancelcommand [list destroy .deletespectrumdialog]
}


# saveSeveralSpectra
#       This proc prompts for a filename and set of spectra
#       and saves all the spectra into the single file.
#
proc saveSeveralSpectra {} {
    saveSpectrumDialog .savemany
    .savemany modal

    if {[winfo exists .savemany]} {
        set spectra [.savemany cget -spectra]
        set file    [.savemany get]

        if {([llength $spectra] != 0) && ($file !="")} {
            if {[file dirname $file] == "."} {
                set filter [.savemany getFilter]
                set dir    [file dirname $filter]
                set file   [file join $dir $file]
            }
            if {[catch {eval swrite -format ascii [list $file] $spectra} msg]} {
                tk_messageBox -icon error -title "Failed!" \
                    -message "Could not write [join $spectra {, }] to $file : $msg"
            }
        }

        destroy .savemany
    }
}

# readSpectrumFile
#       This proc prompts for a filename and the switch values
#       to use to read all the spectra from a single input file
#       into SpecTcl.
#
proc readSpectrumFile {} {
    readSpectrumDialog .readmany
    .readmany modal

    if {[winfo exists .readmany]} {
        set file [.readmany get]
            #  set the switches:

        if {[.readmany cget -snapshot]} {
            lappend switches "-snapshot"
        } else {
            lappend switches "-nosnapshot"
        }

        if {[.readmany cget -replace]} {
            lappend switches "-replace"
        } else {
            lappend switches -noreplace
        }

        if {[.readmany cget -bind]} {
            lappend switches "-bind"
        } else {
            lappend switches "-nobind"
        }

        if {$file != ""} {
            if {[file dirname $file] == "."} {
                set filter [.savemany getFilter]
                set dir    [file dirname $filter]
                set file   [file join $dir $file]
            }
            if {[catch {open $file r} msg]} {
                tk_messageBox -icon error -title {Open failed} \
                    -message "Could not open file: $file : $msg"
            } else {
                set fd $msg

                while {![eof $fd]} {
                    catch {eval sread -format ascii $switches $fd}
                }
            }
        }
        failsafeWrite
        .gui.b update
        destroy .readmany
    }
}
