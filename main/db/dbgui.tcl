#!/bin/sh
# -*- tcl -*-
# The next line is executed by /bin/sh, but not tcl \
exec tclsh "$0" ${1+"$@"}

#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2014.
#
#    You may use this software under the terms of the GNU public license
#    (GPL).  The terms of this license are described at:
#
#     http://www.gnu.org/licenses/gpl.txt
#
#    Authors:
#             Ron Fox
#             Giordano Cerriza
#	     NSCL
#	     Michigan State University
#	     East Lansing, MI 48824-1321


##
# @file dbGui.tcl
# @brief Grapical user interface for managing SpecTcl database configuration.
# @author Ron Fox <fox@nscl.msu.edu>
#

package require Tk
package require dbconfig
package require snit
package require img::png;        # for the icons.
package require dialogwrapper
package require SpecTclDB

namespace eval dbgui {
    set here [file dirname [info script]]
    variable version 1.2
    
    # Icons used in the treeview:
    
    variable iconcredits {Icons from Online Webfonts CC BY 3.0, Vitaly Gorbachev, Pixel perfect, and Flaticons via flaticon.com}

    variable folder [image create photo dbgui::folder -format png -file [file join $here folder.png]]
    variable openfolder [image create photo dbgui::openfolder -format png -file [file join $here openfolder.png]]
    variable spectrum   [image create photo dbgui::spectrum   -format png -file [file join $here spectrum.png]]
    variable configuration [image create photo dbgui::configuration -format png -file [file join $here configuration.png]]
    variable events [image create photo dbgui::events -format png -file [file join $here events.png]]
    variable database "";    #database instance command.
}
puts $dbgui::iconcredits;            # Also in help->about...
package provide dbgui $dbgui::version



##
# prompter for a string.
#    The code here produces a widget that can be wrapped by a DialogWrapper
#    to prompt for a string.  The proc evgui::promptString actually initiates
#    the modal dialog and returns the user's string.
#

##
# @class dbgui::StringPrompter 
#
#   Provides the dialog form for prompting.
#
# Options:
#   -text  The prompt to text.
#   -input The user input.
#
snit::widgetadaptor dbgui::StringPrompter {
    component string
    
    option -input
    
    delegate option -text to string
    
    constructor args {
        installhull using ttk::frame
        install string using message $win.msg -width 3i
        ttk::entry $win.input -textvariable [myvar options(-input)] 
        
        grid $win.msg -sticky nsew
        grid $win.input -sticky nsew
        
        $self configurelist $args
    }

}
##
# dbgui::promptString
#    Use the string prompter above wrapped in a dialog to prompt
#    for a string from the user.
#
# @param win - parent window for the dialog.
# @param prompt - prompt string to fill the message part of the formw with.
# @param initial - optional initial entry string.
# @return the user's input string.
# @retval "" - if the user cancelled out of the dialog.
#
proc dbgui::promptString {win prompt {initial {}}} {
    if {$win eq "."} {
        set win ""     ;                #unlikely but possible (e.g. in testing).
    } else {}
    toplevel $win.prompt
    set dlg [DialogWrapper $win.prompt.dlg]
    set formParent  [$dlg controlarea]
    set form [dbgui::StringPrompter $formParent.form -text $prompt -input $initial]
    $dlg configure -form $form
    
    grid $dlg -sticky nsew
    set button [$dlg modal]
    
    set result ""
    if {$button eq "Ok"} {
        set result [$form cget -input]
    }
    destroy $win.prompt
    
    return $result
}

##
# @class ListPrompter
#     Prompt for a selection from amongst a list.
#
#  OPTIONS:
#     -values - list of values in the list box.
#     -selectmode - passed through to list box to set selection mode.
# METHODS:
#    getSelection - Get the list of values selected.
#
# @note - this list box scrolls vertically and horizontally.
#
#
snit::widgetadaptor dbgui::ListPrompter {
    component listbox
    option -values -configuremethod _SetValues
    delegate option -selectmode to listbox
    
    constructor args {
        installhull using ttk::frame
        install listbox using listbox $win.lb -selectmode extended \
            -yscrollcommand [list $win.ysb set ]                   \
            -xscrollcommand [list $win.xsb set]
        
        # Scroll bars:
        
        ttk::scrollbar $win.ysb -orient vertical -command [list $listbox yview]
        ttk::scrollbar $win.xsb -orient horizontal -command [list $listbox xview]
        
        # Layout:
        
        grid $listbox $win.ysb -sticky nsew
        grid $win.xsb -sticky nsew
        
        # If the user keys a Control-a we should select all spectra.
        
        bind $listbox <Control-a> [list $listbox selection set 0 end]
        bind $listbox <Control-A> [list $listbox selection set 0 end]
        focus -force $listbox
        
        $self configurelist $args
    }
    #-----------------------------------------------------------------
    # public methods:
    
    ##
    # getSelection
    #   Gets the list of the _texts_ of the items in the selection.
    #
    # @return list - of strings.
    #
    method getSelection {} {
        set selectionIndices [$listbox curselection]
        set result [list]
        
        foreach index $selectionIndices {
            lappend result [$listbox get $index]
        }
        
        return $result
    }
    #-----------------------------------------------------------------
    # private methods:
    
    ##
    # _SetValues
    #   Given the list of values changed, repopulate the listbox:
    #
    # @param optname - option being configured.
    # @param value   - proposed new value.
    #
    method _SetValues {optname value} {
        $listbox delete 0 end
        $listbox insert end {*}$value
        
        set options($optname) $value
    }
}
##
#  promptList
#    Wraps a ListPrompter in a DialogWrapper, and returns the set of
#    selected items in the listbox.
#
# @param parent - parent widget
# @param items  - Items with which to populate the list box.
# @param mode   - Listbox selection mode (defaults to extended).
# @param 
#
# @return list of strings - items selected when Ok was clicked.
# @retval [list] no items selected or Cancel, Destroy done.
#
proc dbgui::promptList {parent items {mode extended}} {
    set top [toplevel $parent.prompt]
    set dlg [DialogWrapper $top.dialog]
    set formParent [$dlg controlarea]
    
    set form [dbgui::ListPrompter $formParent.form -values $items -selectmode $mode]
    $dlg configure -form $form
    grid $dlg -sticky nsew
    
    set button [$dlg modal]
    set result [list]
    if {$button eq "Ok"} {
        set result  [$form getSelection]
    }
    
    destroy $top
    return $result
}

##
# Prompter prior to playback provides options to:
#  -  Reload the configuration associated with the run.
#  -  Clear the spectra.
#
# OPTIONS:
#    -reload - reload the configuration state.
#    -clear  - Clear the spectra state.
#
snit::widgetadaptor dbgui::PlaybackOptions {
    option -reload -default 1
    option -clear  -default 1
    
    constructor args {
        installhull using ttk::frame
        
        ttk::checkbutton $win.reload -onvalue 1 -offvalue 0 \
            -variable [myvar options(-reload)] -text {Reload configuration?}
        ttk::checkbutton $win.clearspec -onvalue 1 -offvalue 0 \
            -variable [myvar options(-clear)] -text {Clear all spectra?}
        
        grid $win.reload    -sticky w
        grid $win.clearspec -sticky w
        
        $self configurelist $args
        
    }
}
##
# Prompt for the playback options:
#   @param parent the parent of the toplevel the prompter will be constructed
#                  into.
#   @param reload - optional parameter 1/0 reload the configuration from the
#                    prior to playback (default 0).
#   @param clear  - Optional parameter 1/0 clear all spectra prior to playback
#                   (default to 1).
# @return dict  With keys:
#      -   reload  value of reload flag.
#      -   clear   value of clear flag.
#
proc dbgui::promptPlaybackOptions {parent {reload 0} {clear 1} } {
    set top [toplevel $parent.playbackopts    ]
    set dlg [DialogWrapper $top.dlg]
    set formParent [$dlg controlarea]
    
    set form [dbgui::PlaybackOptions $formParent.form -reload $reload -clear $clear]
    $dlg configure -form $form
    pack $dlg -fill both -expand 1 -anchor w
    set result [$dlg modal]
    set values [list]
    if {$result eq "Ok"} {
        set values [dict create reload [$dlg cget -reload] clear [$dlg cget -clear]]
    }
    destroy $top
    return $values
}
#------------------------------------------------------------------------------
###   The GUI is intended to live in a top-level widget as it will want
#   to create a menubar.
#
#   The GUI consists of three components:
#    *   A menubar supports connecting to databases as well as the creation
#        of some new databases.  Some less often used functions are also
#        provided through the menu-bar.
#    *  A treeview shows the contents of the 'current' database.  Each
#       save-set is a top-level folder while below them are folders for
#       configuration and spectra.  Context menus can do restorations from those
#       and their children (parameters, spectra, gates, applications for the config
#       individual spectra for the spectra folder).
#    *  A status bar shows the open file, and the selected dataset if one is
#       selected.
#

##
#  @class dbgui::menubar
#   Creates the menubar for the application and provides callbacks for
#   specific items.  The menubar is set as the menubar for the appropriate
#   top-level widget.
#
#    Menus:
#       File:  New... open...
#       Save:  Configuration, Spectrum.
#       Recording Enable/Disable, Autosave...
#       Help:  About
#
#  Callback options:
#     *  -oncreate - create a new database the file is passed as a parameter to the
#                  script.
#     *  -onopen   - Open an existing database file.  The file is passed as a parameter
#                  to the script.
#     *  -onconfigsave - Save configuration.
#     *  -onspecsave   - Save spectrum.
#     * -onenablerecord - Enable recording.
#     * -ondisablerecord - Disable event recording.
#     * -onautosave      - requested autosave list update.
#  Normally, on successful completions, these callbacks should pass a new
#  database command to the treeview widget, and the filename to the statusbar
#  widget.
#
#  Methods:
#    enableSave   - enable the save method.
#    disableSave  - disable the save method.
#
snit::type dbgui::menubar {
    option -oncreate
    option -onopen
    option -onconfigsave
    option -onspecsave
    option -onenablerecord
    option -ondisablerecord
    option -onautosave
    
    
    delegate method * to hull
    delegate option * to hull
    
    variable window
    variable recordingState 0
    ##
    # constructor
    #  install a menu as the hull.
    #  Create the submenus.
    #  Attach them to  appropriate cascade buttons.
    #
    #  @note some fancy footwork is needed because the
    #        widgetadaptor does not seem to work properly if the
    #        hull is made from a menu.
    
    constructor  args {
        $self configurelist $args
        
        set path [split $self :]
        set window [lrange $path end end]
        set window [menu ${window}w]
        puts "Window: $window"
        
        menu $window.file -tearoff 0
        $window.file add command -label New... -command [mymethod _OnNew]
        $window.file add command -label Open... -command [mymethod _OnOpen]
        $window add cascade -label "File" -menu $window.file 
        
        menu $window.save -tearoff 0
        $window.save add command -label Configuration... -command [mymethod _OnCfgSave]
        $window.save add command -label Spectrum...      -command [mymethod _OnSpecSave]
        $window add cascade -label Save -menu $window.save
        
        menu $window.record -tearoff 0
        $window.record add checkbutton -label {Enable Recording} -offvalue 0 \
            -onvalue 1  -command [mymethod _OnToggleRecording] \
            -state disabled -variable [myvar recordingState]
        $window.record add command -label {Autosave spectra...} \
            -command [mymethod _SelectAutoSave] -state disabled
        $window add cascade -label "Recording" -menu $window.record
        
        menu $window.help -tearoff 0
        $window.help add command -label About... -command [mymethod _OnAbout]
        $window add cascade -label "Help" -menu $window.help
        
        #  Figure out our top level and configure it so our menu is the
        #  menubar.
        
        set top [$self _TopLevel ]
        puts "My Toplevel is $top"
        $top config -menu $window
    }
    ##
    # abortRecording
    #   Turn off the recording checkbox without calling any callbacks.
    #   This is done because the user, or I decided not to record data after all.
    #
    method abortRecording {} {
        set recordingState 0;             # checkbutton follows this.
    }
    ##
    # actuallyRecording
    #   If the user decided not to stop recording after all we need this hook to
    #   keep the user interface honest.
    #
    method actuallyRecording {} {
        set recordingState 1
    }
    ##
    # enableRecordingControls
    #
    #   Enables the recording checkbuttons.
    #
    method enableRecordingControls {} {
        $window.record entryconfigure 0 -state normal
    }
    
    ##
    # disableRecordingControls
    #
    method disableRecordingControls {} {
        $window.record entryconfigure 0 -state disabled
    }
    method enableAutosave {} {
        $window.record entryconfigure 1 -state normal
    }
    method disableAutosave {} {
        $window.record entryconfigure 1 -state disabled
    }
    
    ##
    # enableSave
    #    Enable the menu entries in the save menu:
    #
    method enableSave {} {
        for {set i 0} {$i < 2} {incr i} {
            $window.save entryconfigure $i -state normal
        }
    }
    ##
    # disableSave
    #   Disable the menu entries in the save menu:
    #
    method disableSave {} {
        for {set i 0} {$i < 2} {incr i} {
            $window.save entryconfigure $i -state disabled
        }
    }
    
    #--------------------------------------------------------------------
    # private methods:
    
    
    ##
    # _TopLevel -
    #   Figure out our top-level.  This is unfortunately necessary because
    #   [windowfo toplevel .m] where .m is a menu, returns .m rather than e.g. .
    #
    #  We assume that one of the followindowg cases holds:
    #  *  Our widget name is of the form .thing - in which case . is the toplevel.
    #  * Our widget name is of the form .top...thing in which case .top is the toplevel.
    #
    #
    method _TopLevel {} {
        set path [split $window .]
        puts $path
        if {[llength $path] == 1} {
            return .
        } else {
            return .[lindex $path 1]
        }
    }
    
    ##
    # _OnNew
    #   Responds to the File->New menu entry:
    #   Prompts for a new filename, and calls the user's code script if registered.
    #   If the user has not registered a script an error dialog is thrown.
    #
    method _OnNew {} {
        set filename [tk_getSaveFile -title {Select file}                    \
            -filetypes [list                                                 \
                [list Database .db]                                         \
                [list Config   .cfg]                                        \
                [list "All files" * ]                                       \
            ]
        ]
        if {$filename eq  ""} return;               # No file chosen.
        if {$options(-oncreate) ne ""} {
            uplevel #0 $options(-oncreate) [list $filename] 
        } else {
            tk_messageBox -icon error -type ok -title {No -oncreate script} \
                -message "To use the dbgui::menubar menu requires you specify a -oncreate script"
        }
    }
    ##
    # _OnOpen
    #   Called in response to a File->Open click.
    #   Let the user choose a file. Pass that file on to the  -onopen script.
    #   If the user has not registered a script an error dialog is tossed.
    #
    method _OnOpen {} {
        set filename [tk_getOpenFile -title {Select file}                    \
            -filetypes [list                                                 \
                [list Database .db]                                         \
                [list Config   .cfg]                                        \
                [list "All files" * ]                                       \
            ]
        ]
        if {$filename eq  ""} return;               # No file chosen.
        if {$options(-onopen) ne ""} {
            uplevel #0 $options(-onopen) [list $filename] 
        } else {
            tk_messageBox -icon error -type ok -title {No -onopen script} \
                -message "To use the dbgui::menubar menu requires you specify a -onopen script"
        }        
    }
    ##
    # _OnAbout
    #   Called in response to the Help->About message. Outputs a message describing
    #   this all.
    #
    #
    method _OnAbout {} {
        set myVersion [package versions dbgui]
        set dbVersion [package versions dbconfig]
        
        set message "
    SpecTcl database Configuration GUI:\n
    GUI Version: $dbgui::version\n
    Database API version: $dbVersion
    $dbgui::iconcredits
    NSCL  2020
"
        tk_messageBox -icon info -type ok -title "Help About" -message $message
    }
    ##
    # _OnCfgSave
    #    Called when the menu entry to save a configuration is called.
    #    The -onconfigsave script is called if it exists.  NO-OP if not.
    #
    method _OnCfgSave {} {
        set script $options(-onconfigsave)
        if {$script ne ""} {
            uplevel #0 $script
        }
    }
    ##
    # _OnSpecSave
    #   Called when the Save->Spectrum menu element is clicked.
    #   The -onspecsave script is called.
    #
    method _OnSpecSave {} {
        set script $options(-onspecsave)
        if {$script ne ""} {
            uplevel #0 $script
        }
    }
    ##
    # _OnToggleRecording
    #   Called when the state of the record checkbox changes.
    #   We get the current state of that box and decide whether to call
    #   the script in -onenablerecording or -ondisablerecording.
    #
    method _OnToggleRecording {} {
        if {$recordingState} {
            set option -onenablerecord
        } else {
            set option -ondisablerecord
        }
        set script $options($option)
        
        if {$script ne ""} {
            uplevel #0 $script
        }
    }
    ##
    # _SelectAutoSave
    #   If there is an -onautosave script, it is called now.
    #
    method _SelectAutoSave {} {
        set script $options(-onautosave)
        if {$script ne ""} {
            uplevel #0 $script
        }
    }
}
#----------------------------------------------------------------------------
#
#  dbgui::dbview
#    Provides a view of a configuration database.  The window shows a simple
#    hierarchy of

#\verbatim
#    Save set name (date) (folder icon).
#      +---> Configuration  (settings icon)
#      +---> Spectra (folder icon)
#         +----> List of saved spectra. (spectrum icon).
#  Options:
#    All ttk::treeview options.
#    -onconfigsave   - Script run when the UI asks a configuration to be saved.
#    -onconfigrestore - Script run when the UI asks for a configuration to be restored.
#    -onspecsave       - Script run when the UI asks for a spectrum to be saved.
#    -onspecrestore    - Script run when the UI asks for a spectrum to be restored.
#    -promptspectrum - Script run to prompt for a spectrum name
#                      If not supplied a simple dialog with a text entry is used.#
#    -onplayrun      - Script run to playback a run.
#    -onstopplayback - script to run to stop playback.
#
#  Methods:
#    All ttk::treeview methods.
#    setDatabaseCommand - set a new database command.
#                         regenerates the entire treeview.
#    getCurrentConfig   - Returns the name of the currently selected configuration
#                         (empty string if none).
#    getRunInConfig     - Returns the number of the run in the configuration.
#    getCurrentSpectrum - Returns name of currently selected spectrum
#    addConfiguration   - Inform the interface there's a new configuration.
#    addSpectrum        - Informthe interface there's a new spectrum in a configuration
#    playing            - Indicates a config/run is being played back.
#    notPlaying         - Indicates a config/run is no longer being played.
#
# NOTE:
#    If playing, the runMenu play entry is disabled and the specified run is given
#    the playing tag which  sets a distinct background color.
#    notplaying will revert to the normal background color and enable the play
#    menubuttton.
#
snit::widgetadaptor dbgui::dbview {

    component tree
    
    
    option -onconfigsave -default [list]
    option -onconfigrestore -default [list]
    option -onspecsave -default [list]
    option -onspecrestore -default [list]
    option -onplayrun   -default  [list]
    option -onstopplay  -default [list]
    option -onstopplayback -default [list]
    
    option -state -default normal -configuremethod _SetState

    
    delegate option * to tree
    delegate method * to tree
    
    variable dbcommand [list]
    variable configContextMenu
    variable spectrumContextMenu
    variable runContextMenu
    
    constructor args {
        installhull using ttk::frame
        install tree using ttk::treeview $win.tree \
            -yscrollcommand [list $win.scroll set]
        ttk::scrollbar $win.scroll -orient vertical -command [list $tree yview]
        
        grid $tree $win.scroll -sticky nsew
        grid columnconfigure $win 0 -weight 1
        grid rowconfigure    $win 0 -weight 1
        
        $tree config -columns [list  label] -displaycolumns #all
        $tree heading 0 -text description
        $tree column  0 -stretch 1
        
        # Capture open and close events so that we can open and close the
        # folders when they happen on top levels:
        
        bind $tree <<TreeviewOpen>> [mymethod _OnOpen]
        bind $tree  <<TreeviewClose>> [mymethod _OnClose]
        
        #  Create the context (right click) menus and bind
        #  the right clicks on the configuration and spectrum tags to
        #  pop up their context menus.
        
        set configContextMenu [$self _CreateConfigContextMenu]
        set spectrumContextMenu [$self _CreateSpectrumContextMenu]
        set runContextMenu    [$self _CreateRunContextMenu]

        $tree tag bind configuration <ButtonPress-3> \
             [mymethod _PostMenu $configContextMenu  %X %Y %x %y]


        $tree tag bind spectrum      <ButtonPress-3> \
            [mymethod _PostMenu $spectrumContextMenu %X %Y %x %y]
        
        $tree tag bind events <ButtonPress-3> \
            [mymethod _PostMenu $runContextMenu %X %Y %x %y]
        
        $tree tag configure playing -background green
        
        # Unpost any menu that's active:
        
        bind $tree <Key-Escape> "
            $configContextMenu unpost
            $spectrumContextMenu unpost
            $runContextMenu unpost
        "
    }
    #----------------------------------------------------------------
    # Public METHODS:
    #
    ##
    # playing
    #   Specifies that a config/run are being played back.
    #   - Adds the playing tag to the run element of the tree indicating
    #     the playback is in progress.
    #   - Disables the Runcontextmenu 'Play' command.
    # @param config - the configuration
    # @param run    - the run in that configuration.
    #
    method playing {config run} {
        $runContextMenu entryconfigure 0 -state disabled
        $runContextMenu entryconfigure 1 -state normal
        set item [$self _FindRun $config $run]
        if {$item ne ""} {
            $tree tag add playing $item
        }
    }
    ##
    # notPlaying
    #   Specifies that a configu/run are  no longer being played back.
    #   removes the tag and re-enables the play button.
    # @param config - the configuration
    # @param run    - the run in that configuration.
    #
    method notPlaying {config run } {
        $runContextMenu entryconfigure 0 -state normal
        $runContextMenu entryconfigure 1 -state disabled
        set item [$self _FindRun $config $run]
        if {$item ne ""} {
            $tree tag remove playing $item
        }
    }
    
    ##
    # getCurrentConfig
    #    If there is a selection, returns the name of the configuration it's in.
    #    -  Find the parent of the selection whose parent is {}. Could be the
    #       selected item.
    #    - Return the -text of that entry.
    # @note  This code assumes all top level items are configurations.
    #        If that is not the csae, we need to modify this.
    # @return string - name of the configuration the selection is in.
    # @retval ""     -   There is not currently selected configuration.
    #
    method getCurrentConfig {} {
        set selection [$tree selection]
        
        if {$selection ne ""} {
            while {[$tree parent $selection] ne ""} {
                set selection [$tree parent $selection]
            }
            return [$tree item $selection -text]
        }
        return ""
    }
    ##
    # getRunInConfig
    #   Given the name of a configuration, returns the run number of the event
    #   data stored in that configuration.
    #
    # @param config - the configuration name.
    # @return int - run numbger.
    # @retval empty string - if there is no run data in the config.
    # @note - while the database supports more than one run per saveset,
    #         the GUI does not.  Furthermore the SpecTcl even saving
    #         code creates a new saveset for each run.
    #
    method getRunInConfig config {
        if {[catch {dbconfig::openSaveSet $dbcmd $config} saveset]} {
            return "";              # not even a matching save set.
        }
        #  Get list of dicts of runs in saveset:
        
        set runs [dbconfig::getRunInfo $saveset]
        $saveset destroy
        
        # Figure out the run number of the first (and only)
        # run:
        
        if {[llength $runs] == 0} {
            return "";               # no runs.
        }
        set run [lindex $runs 0]
        return [dict get $run number]
    }
    
    
    ##
    # getCurrentSpectrum
    #    If a spectrum is selected, returns its name.
    #
    # @return string
    # @retval - no spectrum is selected (includes the case that nothing is selected).
    #
    method getCurrentSpectrum {} {
        set selection [$tree selection]
        
        if {$selection ne ""} {
            set tags [$tree item $selection -tag]
            if {"spectrum" in $tags} {
                return [$tree set $selection 0]
            }
        }
        return ""
    }
    ##
    # setDatabaseCommand
    #   Sets a new database command -
    #   clears the widget,
    #   loads the widget from the new database.
    #
    #  @paramnewcmd
    #     New database command.
    method setDatabaseCommand {newcmd} {
        $self _Clear
        set dbcommand $newcmd
        $self _Load
    }
    ##
    # addConfiguration
    #    Add a new configuration to the UI.  The configuration is assumed
    #    to already be in the database...though we're defensive about that.
    #    The configuration is assumed _not_ to be in the GUI yet and we're not
    #    defensive about that.
    # @param name  - name of the new configuration
    #
    method addConfiguration name {
        set configs [dbconfig::listConfigs $dbcommand]
        foreach config $configs {
            set cfgname [dict get $config name]
            if {$cfgname eq $name} {
                set tstamp [dict get $config timestamp]
                $self _AddConfiguration $cfgname $tstamp
            }
        }
    }
    ##
    # addSpectrum
    #   Add a new spectrum to a configuration item in the GUI.
    # 
    # @param config - name of the configuration.
    # @param sname  - name of the spectrum to add.
    # @note if the spectrum is already in the configuration it is not added.
    # @note if the configuration does not exist this is also a noop.
    # @note the configuration folder  is not opened automatically.
    # @note this only impacts the GUI. It does not change the
    #       database in any way.  This should be called either
    #       after saving a spectrum or as part of scanning the
    #       database to build the gui.
    #
    method addSpectrum {config sname} {
        set configItem [$self _GetConfigElement $config]
        if {$configItem ne ""} {
            set spectra [$self _GetSpectrumChildren $configItem]
            if {$sname ni $spectra} {
                set sid [$tree insert $configItem end -image $dbgui::spectrum   \
                    -tags spectrum                                            \
                ]
                $tree set $sid 0 $sname
            }
        }
    }
    
    #----------------------------------------------------------------
    #  Private methods:
    
    ##
    # _SetState
    #   Configure method for the -state option.
    #   While Tk supports normal, disabled, active, we only care about
    #   disbaled and other.  disabled turns off the context menus while
    #   other enables them:
    #
    method _SetState {optname val} {
        set options($optname) $val;    # We're lax about the options allowed.
        
        if {$val eq "disabled"} {
            $tree tag bind configuration <ButtonPress-3> \
                  [list]
     
     
             $tree tag bind spectrum      <ButtonPress-3> \
                 [list]
             
             $tree tag bind events <ButtonPress-3> \
                [list]
        } else {
            $tree tag bind configuration <ButtonPress-3> \
                 [mymethod _PostMenu $configContextMenu  %X %Y %x %y]


            $tree tag bind spectrum      <ButtonPress-3> \
                [mymethod _PostMenu $spectrumContextMenu %X %Y %x %y]
        
            $tree tag bind events <ButtonPress-3> \
                [mymethod _PostMenu $runContextMenu %X %Y %x %y]
        }
        
    }
    
    ##
    # _FindItem
    #   Given an item find a child whose -text has the requested value:
    #
    # @param parent - parent to search.
    # @param value  - Value to match with the -text attribute of the item.
    # @return item  - Item id of the found item or "" if not found.
    #
    method _FindItem {parent item} {
        set children [$tree children $parent]
        foreach child $children {
            if {$item eq [$tree item $child -text]} {
                return $child
            }
        }
        return ""
    }
    
    ##
    # _FindRun
    #    Locate the item associated with the specified run number.
    #
    # @param config - enclosing configuration name.
    # @param run    - Run Number within the configuration
    # @return item  - Item name of the found run item or {} if none found.
    #
    method _FindRun {config run} {
        set item [$self _FindItem {} $config];    # Find the configuration
        if {$item ne ""} {
            return [$self _FindItem $item $run]
        }
        return ""
    }
    ##
    # _Clear
    #    Removes all entries from the display:
    #
    method _Clear {} {
        set configs [$tree children {}];         # Top levesl are configurations.
        if {[llength $configs] > 0} {
            $tree delete $configs
        }
    }
    ##
    # _Load
    #    loads informtion about the databse into the window.
    #
    #   Top levels are folders named by config the description gives the save date.
    #   Each config has a configuration icon and a spectra folder.
    #   The spectra folder has names of the spectrum contents saved in that config.
    #
    #  @note dbcommand  - contains the database commanbd.
    #
    method _Load {} {
        set configs [dbconfig::listConfigs $dbcommand]
        foreach config $configs {
            $self _AddConfiguration \
                [dict get $config name] [dict get $config timestamp]
        }
    }
    ##
    # _AddConfiguration
    #   Add a new configuration to the treeview.
    #
    # @param configName - name of the configuration
    # @param timestamp  - [clock seconds] when the configuraiton was saved.
    #
    method _AddConfiguration {configName timestamp} {
        set description "Saved [clock format $timestamp]"
        lappend values ""
        lappend values $description
        set configid [$tree insert {} end  -text $configName                  \
            -image $dbgui::folder -tags configuration                         \
        ]
        $tree set $configid 0 $description

        
        #  Now load the sub-elements.  We have a configuration and
        #  0 or more spectrum items.
        
        $tree insert $configid end -image $dbgui::configuration -tags configuration
        
        set saveset [dbconfig::openSaveSet $dbcommand $configName]

        foreach spectrum [dbconfig::listSavedSpectra $saveset] {
            set sid [$tree insert $configid end -image $dbgui::spectrum   \
                -tags spectrum                                            \
            ]
            $tree set $sid 0 $spectrum
        }
        #  If there's run data associated with this configuration
        #  get the information and add a run element.
        
        if {[dbconfig::hasRun $saveset]} {
            set info [dbconfig::getRunInfo $saveset]
            set info [lindex $info 0]
            set rid [$tree insert $configid end -image dbgui::events \
                -tags events \
                -text [dict get $info number]]
            $tree set $rid 0 [dict get $info title]
            
        }
        $saveset destroy
        
    }
    ##
    # _PostMenu
    #   Selects the item under the pointer and posts a context menu there.
    #
    # @param menu - widget containing the menu to post.
    # @param x,y  - Screen coordinates where menu should appear.
    # @param wx,wy - Pointer coords within the window.
    #
    method _PostMenu {menu x y wx wy} {
        set element [$tree identify item $wx $wy]

        $tree selection set $element
        $menu  post $x $y
    }
    ##
    # _OnOpen
    #    If the current entry's image is a folder, make it an openfolder
    #
    method _OnOpen {} {
        set item [$tree focus]
        if {$item ne "{}"} {
            set image [$tree item $item -image]
            if {$image eq $dbgui::folder} {
                $tree item $item -image $dbgui::openfolder
            }
        }
    }
    ##
    # _OnClose
    #   If the entry's image is an open folder turn it into a closed one.
    #
    method _OnClose {} {
        set item [$tree focus]
        if {$item ne "{}"} {
            set image [$tree item $item -image]
            if {$image eq $dbgui::openfolder} {
                $tree item $item -image $dbgui::folder
            }
        }
        
    }
    ##
    # _CreateConfigContextMenu
    #    Creates the context menu for configuration items:
    #    -   Save... - requests a configuration name and calls -onconfigsave
    #    -   Load    - Gets the current configuration and calls -onconfigrestore
    #
    # @return - the menu widget path produced.
    # @note - While menus are considered toplevels, we're going to make this a
    #         child of $tree so that we can ensure that the path is unique.
    #
    method _CreateConfigContextMenu {} {
        set result [menu $tree.configcontextmenu -tearoff 0 \
            -postcommand [list tk_menuSetFocus $tree.configcontextmenu]     \
        ]
        $result add command -label Save... -command [mymethod _OnConfigSave]
        $result add command -label {Save Spectrum...} \
            -command [mymethod _OnSaveSpectrum]
        $result add separator
        $result add command -label Load    -command [mymethod _OnConfigRestore]

        # Keypress escape should unpost the menu if its posted.
        
        bind $result <Key-Escape> [list $result unpost]
        
        return $result
    }
    ##
    # _CreateSpectrumContextMenu
    #
    #    Creates the contextmenu for spectrum items.
    #
    #   -   Save    - calls _OnResaveCurrentSpectrum.
    #   -   Save... - calls _OnSaveSpectrum.
    #   -   Load    - Calls _OnRestoreSpectrum.
    #
    #  @return menu widget path.  Will be created as a child of $tree.
    method _CreateSpectrumContextMenu {} {
        set result [menu $tree.spectrumcontextmenu -tearoff 0        \
            -postcommand [list tk_menuSetFocus $tree.spectrumcontextmenu]
        ]
        
        $result add command -label Resave \
            -command [mymethod _OnResaveCurrentSpectrum]
        $result add command -label Save... -command [mymethod _OnSaveSpectrum]
        $result add command -label {Load} -command [mymethod _OnSpectrumRestore]
        $result add separator
        $result add command -label {SaveConfiguration...} \
            -command [mymethod _OnConfigSave]
        $result add command -label {Load Configuration} \
            -command [mymethod _OnConfigRestore]
        


        bind $result <Key-Escape> [list $result unpost]

                
        return $result
    }
    ##
    # _CreateRunContextMenu
    #
    #   The context menu associated with a run:
    # @return menu widget path.
    #
    method _CreateRunContextMenu {} {
        set result [menu $tree.runcontextmenu -tearoff 0 \
            -postcommand [list tk_menuSetFocus $tree.spectrumcontextmenu] \
        ]
        
        $result add command -label Play... \
            -command [mymethod _OnPlayback]
        $result add command -label Stop -command [mymethod _OnStopPlayback] -state disabled
        bind $result <Key-Escape> [list $result unpost]
        
        return $result
    }
    ##
    # _OnStopPlayback
    #    Called to request a stop to playing event data.
    #
    #   -onstopplayback is called if not empty.
    #
    method _OnStopPlayback {} {
        set script $options(-onstopplayback)
        uplevel #0 $script
    }
    ##
    # _OnPlayback
    #   Called to play back run data in the database.
    #   If the -onplayrun script is non null, it is called with the following
    #   parameters appended:
    #    - Configuration name.
    #    - Run number.
    #    - Database command.
    #   The code establishing the script is assumed to know the file the database
    #   command belongs to.
    #   
    #
    method _OnPlayback {} {
        set script $options(-onplayrun)
        if {$script ne ""} {
            set config [$self getCurrentConfig]
            if {$config ne ""} {
                set run    [$self getRunInConfig $config]
                if {$run ne ""} {
                    uplevel #0 $script $config $run
                }
            }
        }
    }
    ##
    # _OnConfigSave
    #   Called when the user wants a configuration file saved.
    #   Pop up a name prompter to get the configuration file saver.
    #   Ensure the configuration does not exist.  Once we have a good file,
    #   invoke the -onconfigsave script passing the database and configuration
    #   name as parameters.
    #
    method _OnConfigSave {} {
        set configName [dbgui::promptString $tree {Configuration Name}]
        if {$configName ne ""} {
            set configs [dbconfig::listConfigs $dbcommand]
            foreach config $configs {
                if {$configName eq [dict get $config name]} {
                    tk_messageBox -icon error -type ok -title "Configuration exists" \
                        -message "Configuration '$configName' already exists."
                    return
                }
            }
            #  If we got here we can dispatch:
            
            set script $options(-onconfigsave)
            if {$script ne ""} {
                lappend script $dbcommand $configName
                uplevel #0 $script
            }
        }
    }
    ##
    # _OnConfigRestore
    #    Called when the user wants to restore a configuration:
    #    The current configuration is determined.
    #    The -onconfigrestore script is invoked passing the database command
    #    and the configuration name as parameters.
    #
    method _OnConfigRestore {} {
        set configName [$self getCurrentConfig]
        set script $options(-onconfigrestore)
        if {$script ne ""} {
            lappend script $dbcommand $configName
            uplevel #0 $script
        }
    }

    # _SaveSpectrum
    #   Contains the common code to invoke -onspecsave;
    #   database command, configuration name and spectrum name are added to
    #   the script in -onspecsave and that script is run.
    #
    # @param config  - name of the selected configuration.
    # @note the script is expected to prompt for the spectra to save.
    #
    method _SaveSpectrum {config {name {}}} {
        set script $options(-onspecsave)
        if {$script ne ""} {
            lappend script $dbcommand $config $name
            uplevel #0 $script
        }
    }
    ##
    # _OnResaveCurrentSpectrum
    #    Call _SaveSpectrum with the current configuration and spectrum.
    #
    method _OnResaveCurrentSpectrum {} {
        $self _SaveSpectrum  [$self getCurrentConfig] [$self getCurrentSpectrum]
    }
    ##
    # _OnSaveSpectrum
    #   Called to save an arbitrary spectrum:
    #     Prompt for the spectrum name.
    #     Invoke _SaveSpectrum if a non blank spectrum was returned.
    #
    method _OnSaveSpectrum {} {
        set cfg [$self getCurrentConfig]
        $self _SaveSpectrum $cfg
    }
    ##
    # _OnSpectrumRestore
    #    Figure out the current spectrum and configuration and
    #    call -onspecrestore.
    #
    method _OnSpectrumRestore {} {
        set config [$self getCurrentConfig]
        set spec  [$self getCurrentSpectrum]
        
        set script $options(-onspecrestore)
        if {$script ne ""} {
            lappend script $dbcommand $config $spec
            uplevel #0 $script
        }
    }
    ##
    # _GetConfigElement
    #   Return the element id of a specific configuration folder.
    #   configuration folders are the children of {} their -text
    #   is the name of the configuration.
    #
    # @param name - name of the configuration to get.
    # @return string -element id of the configuration.
    # @retval "" - no such configuration folder.
    #
    method _GetConfigElement  name {
        set configs [$tree children {}]
        foreach config $configs {
            if {$name eq [$tree item $config -text]} {
                return $config
            }
        }
        # No matches:
        
        return ""
    }
    ##
    # _GetSpectrumChildren
    #   @param config element id of a configuration.
    #   @return list of names - list of spectrum names in the
    #                           configuration.
    #   Spectrum names are children with the spectrum -tag.
    #   Names are the column 0 values.
    #
    method _GetSpectrumChildren config {
        set children [$tree children $config]
        set result [list]
        
        foreach child $children {
            if {"spectrum" in [$tree item $child -tags]} {
                lappend result [$tree set $child 0]
            }   
        }
        
        return $result
    }
}
##
#  @class StatusLine
#    The status line of the database gui shows several items:
#    - Current database file name.
#    - Current configuration, if any.
#    - Current Spectrum, if any.
#    - What the heck -- let's put in the current date/time.
#
#  Options
#    -database - databsae filename.
#    -configuration - current configuration.
#    -spectrum      - current spectrum.
#
snit::widgetadaptor dbgui::StatusLine {
    option -database      -default ""
    option -configuration -default ""
    option -spectrum      -default ""
    
    variable afterid -1
    
    constructor args {
        installhull using ttk::frame
        ttk::label $win.dbt -text {Database: }
        ttk::label $win.db -textvariable [myvar options(-database)] -width 32
        ttk::label $win.cfgt -text {Current config: }
        ttk::label $win.config -textvariable [myvar options(-configuration)] -width 16
        ttk::label $win.spt -text {Current Spectrum: }
        ttk::label $win.spectrum -textvariable [myvar options(-spectrum)] -width 16
        
        # The dummy text string is just to ensure the label is wide enough.
        
        ttk::label $win.dt -text "Wed Jan 08 09:05:33 EST 2020"
        
        grid $win.dbt $win.db $win.cfgt $win.config $win.spt $win.spectrum $win.dt -sticky nsew
        $self configurelist $args
        
        $self _Clock $win.dt
    }
    destructor {
        if {$afterid != -1} {
            after cancel $afterid
        }
    }
    
    method _Clock widget {
        $widget configure -text [clock format [clock seconds]]
        set afterid [after 1000 [mymethod _Clock $widget]]
    }
}
##
# @class dbgui::dbgui
#    The full database GUI consist of a menubar, the database view and
#    the status line all stacked vertically filling space.
#    note that in order for the menubar to work without conflict, this should all
#    be put into an empty toplevel, or at least one that's known not to have a
#    menubar.
#
# OPTIONS:
#    -database       -     set the database.
#    -spectrumlister - provide script to list spectra.
#    -onconfigchange - Provide a script to handle changes in the configuration.\
# METHODS:
#   There are no public methods.
#
#  @note - if this is a problem, could use a toplevel as a hull :-)
#
snit::widgetadaptor dbgui::dbgui {
    component menubar
    component view
    component statusbar
    
    option -database -configuremethod _SetDatabase
    option -spectrumlister
    option -onconfigchange -default [list]
    
    variable afterid -1
    variable recording 0
    variable playing   0
    
    constructor args {
        installhull using ttk::frame
        install menubar using dbgui::menubar $win.menu
        install view    using dbgui::dbview  $win.db -selectmode browse
        install statusbar using dbgui::StatusLine $win.sl
        
        
        grid $view -sticky nsew
        grid $statusbar -sticky nsew
        
        grid rowconfigure $win  0 -weight 1
        grid columnconfigure $win 0 -weight 1
        
        $self configurelist $args
        $self _UpdateStatusBar 1000
        
        #  Setup menu actions:
        
        $menubar configure -onopen [list $self configure -database]
        $menubar configure -oncreate [mymethod _OnCreateDatabase]
        $menubar configure -onconfigsave [mymethod _OnSaveConfig ignoreme]
        $menubar configure -onspecsave [mymethod _OnSaveSpectrum]
        $menubar configure -onautosave [mymethod _SetAutoSaveSpectra]
        $menubar configure -onenablerecord [mymethod _StartRecording]
        $menubar configure -ondisablerecord [mymethod _StopRecording]
        
        #  Util there's a configuration the save menu must be disabled.
        
        $menubar disableSave
        
        # Setup response to popup menus in the view:
        
        $view configure -onconfigsave [mymethod _OnSaveConfig]
        $view configure -onspecsave  [mymethod _OnSaveSpectrumToConfig]
        $view configure -onconfigrestore [mymethod _OnRestoreConfig]
        $view configure -onspecrestore [mymethod _OnLoadSpectrum]
        $view configure -onplayrun     [mymethod _OnPlay]
        $view configure -onstopplayback [mymethod _OnStopPlay]
    }
    destructor {
        if {$afterid != -1} {
            after cancel $afterid
        }
        catch {$menubar destroy}  ;  # In case it's not there yet.
    }
    
    ############################################################################
    # Private methods
    
    ##
    # _StartRecording
    #   Enables recording data:
    #   - Warn that we can't join a run in progress to record.
    #   - If we are playing back stop  it.
    #   - enable recording.
    #   
    method _StartRecording {} {
        set answer [tk_messageBox                                        \
            -title "Recording notes" -icon question -type yesno        \
            -message {If you are in the middle of playing back a run from the database, playback will be stopped
Furthermore, you cannot join a run in progress.  If a run is in progress, you must either
stop analyzing it (offline) first or end it (online) before this operation is safe.
Do you want to continue?}                                                      \
        ]
        if {$answer eq "no"} {
            $menubar abortRecording
            return
        }
        if {$playing} {
            $self _OnStopPlay;     # Turns off playback from the database.
        }
        daqdb enable
        $view configure -state disabled;#   Most ops fail with locked db.
        set recording 1
        
    }
    ##
    # _StopRecording
    #    Disables event recording:
    #    Note this is not recommended in the middle of a run:
    method _StopRecording {} {
        
        set answer [tk_messageBox                                        \
            -title "Stop recording?" -icon question -type yesno        \
            -message {If you are in the middle of recording a run, this is not
recommended as the end run data may not be recorded properly.  Do you still want
to stop event recording at this time?}                                 \
                    ]
        if {$answer eq "no"} {
            $menubar actuallyRecording
        }
        
        daqdb disable
        set recording 0
        $view configure -state enabled
        
        # Note a new (bunch of) configurations was/were made.  For now just refresh
        
        $self configure -database [$self cget -database]
    }
    
    ##
    # _OnPlay
    #   Respond to the request to play a run.
    #
    # @param config - the enclosing configuration name.
    # @param run    - run number
    # @note the caller ensures these are truely there in the database, or we don't
    #        get called.
    #
    method _OnPlay {config run} {
        puts "Playback Run $run in '$config'"
        
        # To playback data we need to disable recording if that's on:
        
         if {$recording} {
            set reply [tk_messageBox -title {Stop Recording? } -icon question \
                -type yesno \
                ]
            -message {Recording is in progress.  To playback a run we need to stop that is that ok?
(If a run is actively being analyzed this is not recommended).}

            if {$reply eq "yes"} {
                daqdb disable;            # Turn off current recording.
                set recording 0
            } else {
                $menubar abortRecording
                return;                   # Cancel playback request.
            }
        }           
        # Prompt here for do you want to restore that configuration first?
        # clear spectra?
        
        set pbOptions [dbgui::promptPlaybackOptions $win]
        if {$pbOptions eq ""} return
        if {[dict get $pbOptions reload]} {
            [dbconfig::restoreConfig $dbgui::database $config] destroy
            sbind -all
        }
        if {[dict get $pbOptions clear]} {
            clear -all
        }

        $menubar disableRecordingControls;    # Cannot record when playing back.
        
        $view playing $config $run
        daqdb play $run
        $view notPlaying $config $run
        $menubar enableRecordingControls
        set playing 1
    }
    ##
    # stop playback of a playback in progress.
    #
    method _OnStopPlay {} {
        daqdb stop
        set playing 0
    }
    
    ##
    # _UpdateStatusBar
    #    Updates the status bar fields we can update and
    #    reschedules
    #
    # @param ms  - Number of milliseconds for next update.
    #
    method _UpdateStatusBar ms {
        $statusbar configure -configuration [$view getCurrentConfig]
        $statusbar configure -spectrum      [$view getCurrentSpectrum]
        
        set afterid [after $ms [mymethod _UpdateStatusBar $ms]]
    }
    
    ##
    # _SetDatabase
    #   Set a new value for the database to peek at.
    #   If the dbgui::database command exists, close it.
    #   sqlite3 opens the database using the command dbgui::database
    #   Sets the statusbar filename.
    #   Sets the view's database command
    #
    # @param optname - name of the option being set.
    # @param optval  - filename.
    #
    method _SetDatabase {optname optval} {
        if {[info commands $dbgui::database] ne ""} {
            $dbgui::database destroy
        }
        set dbgui::database [dbconfig::connect $optval]
        
        daqdb open $optval

        
        
        $view setDatabaseCommand $dbgui::database
        $statusbar configure -database [file normalize $optval]
        
        set options($optname) $optval
        
        $menubar enableSave
        $menubar enableRecordingControls
        $menubar enableAutosave
    }
    ##
    # _OnNewDatabase
    #    Called in response to the menubar's New...selection:
    #    -  Create the database.
    #    -  Create the schema into the database.
    #    -  configure ourself to use this new database.
    #
    # @param path - path to the new database.
    # @note any existing file by this name will be deleted.
    #       the File->New... code has already verified that the user is
    #       ok with this.
    #
    method _OnCreateDatabase path {
        if {[file exists $path]} {
            if {![file writable $path]} {
                tk_messageBox -type ok -icon error \
                    -title "Cannot delete existing"  \
                    -message "Unable to delete existing file [file normalize $path] before creating a new database"
                return
            } else {
                file delete $path
                
            }
        }
        dbconfig::makeSchema  $path
        $self configure -database $path;    #Takes care of the rest.
        
    }
    
    ##
    # _OnSaveConfig
    #
    #   The Menubar has requested a configuration save.
    #   The gui has a context menu which autonomouly prompts.
    #
    # @param database - from the context menu the database (we're going to use
    #                   the database we have).
    #@param name      - optional configuration name. We prompt if not
    #                   it's not provided.
    #
    method _OnSaveConfig  {database {name {}}} {
        if {$name eq ""} {
            set name [dbgui::promptString $win "Name of new configuration (must be unique)"]
        }
        if {$name ne ""} {
            if {[info command $::dbgui::database] eq ""} {
                    tk_messageBox -type ok -icon error -title "No database" \
                        -message {ERROR: program bug - no database has been established yet}
            } elseif {[$self _CfgExists $name]} {
                tk_messageBox -type ok -icon error -title {Duplicate config name} \
                    -message "There already is a configuration named $name not going to overwrite it"
                
            } else {
                #  Save config and destroy the command it returns.
                [dbconfig::saveConfig $dbgui::database $name] destroy
                $view addConfiguration $name
            }
        }
    }
    ##
    # _CfgExists
    #    @param name -name of a configuration.
    #    @return bool - true if the configuration exists.
    #
    method _CfgExists name {
        set configs [dbconfig::listConfigs $::dbgui::database]
        foreach config $configs {
            if {$name eq [dict get $config name]} {
                return 1
            }
        }
        return 0
    }
    ##
    # _OnSaveSpectrum
    #   Process the menubar's Save->Spectrum:
    #
    #   Preconditions:
    #     * There must be  acurrent cnfiguration to save to.
    #     * The user must have established a -spectrumlister script
    #       that will list the names of all spectra that can be saved.
    #
    method _OnSaveSpectrum {} {
        if {[info command $::dbgui::database] eq ""} {
            tk_messageBox -type ok -icon error -title "No database" \
                -message "No database open"
            return
        }
        
        set config  [$view getCurrentConfig]
        if {$config eq ""} {
            tk_messageBox -type ok -icon error -title "No configuration" \
                -message {First click on a configuration in which to save the spectra}
            return
        }
        $self _OnSaveSpectrumToConfig {} $config
    }
    ##
    # _OnSaveSpectrumToConfig
    #    Saves a spectrum to a given configuration.  This is needed because
    #    the context menu  from the view passes the configuration name
    #    while the Save->Spectrum .. menu entry can't because it knows nothing
    #    of the view.
    #
    # @param config - configuration to which the spectrum is saved.
    #
    method _OnSaveSpectrumToConfig {ignore config {spectrum {}}} {
        if {$spectrum eq ""} {
            set spectra [$self _GetSpectrumList]
            set toSave [$self _GetSaveList $spectra]
        } else {
            set toSave $spectrum
        }
        set saveset [dbconfig::openSaveSet $dbgui::database $config]
        foreach name $toSave {
            dbconfig::saveSpectrum $saveset $name
            $view addSpectrum $config $name
        }
        $saveset destroy

    }
    ##
    # _GetSpectrumList
    #    - ask the -spectrumlister to give us a list of the spectra:
    #
    #  @return list of strings - the spectrum names.
    #  @note - if there's no lister just return an empty list.
    method _GetSpectrumList {} {
        set result [list]
        set script $options(-spectrumlister)
        if {$script ne ""} {
            set result [uplevel #0 $script]
        }
        return $result
    }
    ##
    # _SetAutoSaveSpectra
    #   Sets the autosaved spectrum list for recording.
    #
    method _SetAutoSaveSpectra {} {

        set list [$self _GetSaveList  [$self _GetSpectrumList]]
        daqdb autosave $list
    }
    
    ##
    # _GetSaveList
    #   Get the list of items to save given a list to choose from
    #
    # @param list - list of items.
    #
    method _GetSaveList list {
        return  [dbgui::promptList $win $list]
        
    }
    ##
    # _OnRestoreConfig
    #    Processes context menu config restoration.
    #F
    # @param db - database command.
    # @param configname - name of the save set to restore.
    #
    method _OnRestoreConfig {db configname} {
        dbconfig::restoreConfig $db $configname
        sbind -all ;   # Make displayer aware of spectra.
        
        set script $options(-onconfigchange)
        if {$script ne ""} {
            uplevel #0 $script
        }
    }
    ##
    # _OnLoadSpectrum
    #   Load a spectrum.
    #
    # @param db - database command.
    # @param config - configuration name.
    # @param spec   - spectrum name.
    #
    method _OnLoadSpectrum {db config spec} {
        set saveset [dbconfig::openSaveSet $db $config]
        dbconfig::restoreSpectrum  $saveset $spec
        $saveset destroy
    }
}
##
# dbgui::updateTreeGui
#    Asks all components of the Tree gui to update themselves after a
#    new configuration was read:
#
proc dbgui::updateTreeGui {} {
    puts "Updating tree gui"
 
    # It's remotely possible the user has not loaded the treegui
        
    if {[winfo exists .treegui]} {
        .treegui.notebook.spectra updateSpectrumList
        .treegui.notebook.gates updateGates
        FolderGui::updateBrowser
    }
}
##
#  Lister for spectra
#
proc dbgui::listSpectra {} {
    set result [list]
    foreach spectrum [spectrum -list] {
        lappend result [lindex $spectrum 1]
        
    }
    return $result
}
##
# start
#
#   Start the whole gui OFF:
#
proc dbgui::start {} {
    toplevel .dbgui
    dbgui::dbgui .dbgui.gui -spectrumlister [list dbgui::listSpectra]
    pack .dbgui.gui -fill both -expand 1
    
    #   Link with treeGUI:
    
    .dbgui.gui configure -onconfigchange dbgui::updateTreeGui
}
#---------------------------------------------------------------------------
# Entry point:

dbgui::start