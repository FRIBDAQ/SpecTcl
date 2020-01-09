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

namespace eval dbgui {
    set here [file dirname [info script]]
    variable version 1.0
    
    # Icons used in the treeview:
    
    variable iconcredits {Icons from Vitaly Gorbachev, Pixel perfect via flaticon.com}

    variable folder [image create photo dbgui::folder -format png -file [file join $here folder.png]]
    variable openfolder [image create photo dbgui::openfolder -format png -file [file join $here openfolder.png]]
    variable spectrum   [image create photo dbgui::spectrum   -format png -file [file join $here spectrum.png]]
    variable configuration [image create photo dbgui::configuration -format png -file [file join $here configuration.png]]
}
puts $dbgui::iconcredits;            # Also in help->about...
package provide dbgui $dbgui::version

#------------------------------------------------------------------------------
##
#   The GUI is intended to live in a top-level widget as it will want
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
#  @class dbgui::menubar
#   Creates the menubar for the application and provides callbacks for
#   specific items.  The menubar is set as the menubar for the appropriate
#   top-level widget.
#
#    Menus:
#       File:  New... open...
#       Save:  Configuration, Spectrum.
#       Help:  About
#
#  Callback options:
#     *  -oncreate - create a new database the file is passed as a parameter to the
#                  script.
#     *  -onopen   - Open an existing database file.  The file is passed as a parameter
#                  to the script.
#     *  -onconfigsave - Save configuration.
#     *  -onspecsave   - Save spectrum.
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
    
    
    delegate method * to hull
    delegate option * to hull
    
    variable window
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
        set script $option(-specsave)
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
#    -oncofigrestore - Script run when the UI asks for a configuration to be restored.
#    -onspecsave       - Script run when the UI asks for a spectrum to be saved.
#    -onspecrestore    - Script run when the UI asks for a spectrum to be restored.
#    -promptspectrum - Script run to prompt for a spectrum name
#                      If not supplied a simple dialog with a text entry is used.#
#  Methods:
#    All ttk::treeview methods.
#    setDatabaseCommand - set a new database command.
#                         regenerates the entire treeview.
#    getCurrentConfig   - Returns the name of the currently selected configuration
#                         (empty string if none).
#    getCurrentSpectrum - Returns name of currently selected spectrum
#    addConfiguration   - Inform the interface there's a new configuration.
#    addSpectrum        - Informthe interface there's a new spectrum in a configuration
#
snit::widgetadaptor dbgui::dbview {
    delegate method * to hull
    
    option -onconfigsave -default [list]
    option -onconfigrestore -default [list]
    option -onspecsave -default [list]
    option -onspecrestore -default [list]
    option -promptspectrum -default [list]
    
    delegate option * to hull
    
    variable dbcommand [list]
    variable configContextMenu
    variable spectrumContextMenu
    
    
    constructor args {
        installhull using ttk::treeview
        
        $win config -columns [list  label] -displaycolumns #all
        $win heading 0 -text description
        $win column  0 -stretch 1
        
        # Capture open and close events so that we can open and close the
        # folders when they happen on top levels:
        
        bind $win <<TreeviewOpen>> [mymethod _OnOpen]
        bind $win  <<TreeviewClose>> [mymethod _OnClose]
        
        #  Create the context (right click) menus and bind
        #  the right clicks on the configuration and spectrum tags to
        #  pop up their context menus.
        
        set configContextMenu [$self _CreateConfigContextMenu]
        set spectrumContextMenu [$self _CreateSpectrumContextMenu]

        $win tag bind configuration <ButtonPress-3> \
             [mymethod _PostMenu $configContextMenu  %X %Y %x %y]


        $win tag bind spectrum      <ButtonPress-3> \
            [mymethod _PostMenu $spectrumContextMenu %X %Y %x %y]
        
        bind $win <Key-Escape> "
            $configContextMenu unpost
            $spectrumContextMenu unpost
        "
    }
    #----------------------------------------------------------------
    # Public METHODS:
    #
    
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
        set selection [$win selection]
        
        if {$selection ne ""} {
            while {[$win parent $selection] ne ""} {
                set selection [$win parent $selection]
            }
            return [$win item $selection -text]
        }
        return ""
    }
    ##
    # getCurrentSpectrum
    #    If a spectrum is selected, returns its name.
    #
    # @return string
    # @retval - no spectrum is selected (includes the case that nothing is selected).
    #
    method getCurrentSpectrum {} {
        set selection [$win selection]
        
        if {$selection ne ""} {
            set tags [$win item $selection -tag]
            if {"spectrum" in $tags} {
                return [$win set $selection 0]
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
                set tstamp [dict get $config time]
                $self _AddConfiguration $cfgname $tstamp
            }
        }
    }
    
    
    #----------------------------------------------------------------
    #  Private methods:
    
    ##
    # _Clear
    #    Removes all entries from the display:
    #
    method _Clear {} {
        set configs [$win children {}];         # Top levesl are configurations.
        if {[llength $configs] > 0} {
            $win delete $configs
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
                [dict get $config name] [dict get $config time]
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
        set configid [$win insert {} end  -text $configName                  \
            -image $dbgui::folder -tags configuration                         \
        ]
        $win set $configid 0 $description
        
        
        #  Now load the sub-elements.  We have a configuration and
        #  0 or more spectrum items.
        
        $win insert $configid end -image $dbgui::configuration -tags configuration
        
        foreach spectrum [dbconfig::listSavedSpectra $dbcommand $configName] {
            set sid [$win insert $configid end -image $dbgui::spectrum   \
                -tags spectrum                                            \
            ]
            $win set $sid 0 $spectrum
        }
        
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
        set element [$win identify item $wx $wy]

        $win selection set $element
        $menu  post $x $y
    }
    ##
    # _OnOpen
    #    If the current entry's image is a folder, make it an openfolder
    #
    method _OnOpen {} {
        set item [$win focus]
        if {$item ne "{}"} {
            set image [$win item $item -image]
            if {$image eq $dbgui::folder} {
                $win item $item -image $dbgui::openfolder
            }
        }
    }
    ##
    # _OnClose
    #   If the entry's image is an open folder turn it into a closed one.
    #
    method _OnClose {} {
        set item [$win focus]
        if {$item ne "{}"} {
            set image [$win item $item -image]
            if {$image eq $dbgui::openfolder} {
                $win item $item -image $dbgui::folder
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
    #         child of $win so that we can ensure that the path is unique.
    #
    method _CreateConfigContextMenu {} {
        set result [menu $win.configcontextmenu -tearoff 0 \
            -postcommand [list tk_menuSetFocus $win.configcontextmenu]     \
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
    #  @return menu widget path.  Will be created as a child of $win.
    method _CreateSpectrumContextMenu {} {
        set result [menu $win.spectrumcontextmenu -tearoff 0        \
            -postcommand [list tk_menuSetFocus $win.spectrumcontextmenu]
        ]
        
        $result add command -label Resave \
            -command [mymethod _OnResaveCurrentSpectrum]
        $result add command -label Save... -command [mymethod _OnSaveSpectrum]
        $result add command -label {SaveConfiguration...} \
            -command [mymethod _OnConfigSave]
        $result add command -label {Load Configuration} \
            -command [mymethod _OnConfigRestore]
        $result add separator
        $result add command -label {Load} -command [mymethod _OnSpectrumRestore]

        bind $result <Key-Escape> [list $result unpost]

                
        return $result
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
        set configName [dbgui::promptString $win {Configuration Name}]
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
    ##
    # _PromptSpectruim
    #    Asks the user for the name of a spectrum.
    #   - If -promtpspectrum has a script that's used to prompt.  The return
    #     value is the spectrum name.  The database and current configuration
    #     and spectrum name if one is active are passed to the user.
    #   - If no -promptspectrum is defined, then the user is propmted with a string
    #    prompter.
    #
    # @return string - spectrum name "" if none should be chosen.
    method _PromptSpectrum {} {
        set config [$self getCurrentConfig]
        set spec   [$self getCurrentSpectrum] ;   # could be empty/
        
        set script $options(-promptspectrum)
        if {$script ne ""} {
            set script [lappend $dbcommand $config $spec]
            return [uplevel #0 $script]
        } else {
            return [dbgui::promptString $win {Spectrum name:} $spec]
        }
    }
    ##
    # _SaveSpectrum
    #   Contains the common code to invoke -onspecsave;
    #   database command, configuration name and spectrum name are added to
    #   the script in -onspecsave and that script is run.
    #
    # @param config  - name of the selected configuration.
    # @param name    - List of names of spectra to save.
    #
    method _SaveSpectrum {config name} {
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
        $self _SaveSpectrum [$self getCurrentConfig] [$self getCurrentSpectrum]
    }
    ##
    # _OnSaveSpectrum
    #   Called to save an arbitrary spectrum:
    #     Prompt for the spectrum name.
    #     Invoke _SaveSpectrum if a non blank spectrum was returned.
    #
    method _OnSaveSpectrum {} {
        set spec [$self _PromptSpectrum]
        if {$spec ne ""} {
            set cfg [$self getCurrentConfig]
            $self _SaveSpectrum $cfg $spec
        }
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
#  @note - if this is a problem, could use a toplevel as a hull :-)
snit::widgetadaptor dbgui::dbgui {
    component menubar
    component view
    component statusbar
    
    option -database -configuremethod _SetDatabase
    option -onloadconfig
    option -onrestoreconfig
    option -onloadspectrum
    option -onrestorespectrum
    option -spectrumlister
    
    variable afterid -1
    
    constructor args {
        installhull using ttk::frame
        install menubar using dbgui::menubar $win.menu
        install view    using dbgui::dbview  $win.db
        install statusbar using dbgui::StatusLine $win.sl
        
        grid $view -sticky nsew
        grid $statusbar -sticky nsew
        
        $self configurelist $args
        $self _UpdateStatusBar 1000
        
        #  Setup menu actions:
        
        $menubar configure -onopen [list $self configure -database]
        $menubar configure -oncreate [mymethod _OnCreateDatabase]
        $menubar configure -onconfigsave [mymethod _OnSaveConfig]
    }
    destructor {
        if {$afterid != -1} {
            after cancel $afterid
        }
    }
    
    ############################################################################
    # Private methods
    
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
        if {[info commands dbgui::database] ne ""} {
            dbgui::database close
        }

        sqlite3 ::dbgui::database $optval
        
        
        $view setDatabaseCommand dbgui::database
        $statusbar configure -database [file normalize $optval]
        
        set options($optname) $optval
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
        sqlite3 ::dbgui::newdatabase $path
                dbconfig::makeSchema ::dbgui::newdatabase
                ::dbgui::newdatabase close
                $self configure -database $path;    #Takes care of the rest.
    }
    
    ##
    # _OnSaveConfig
    #
    #   The Menu has requested a configuration save.
    #
    method _OnSaveConfig  {} {
        set name [dbgui::promptString $win "Name of new configuration (must be unique)"]
        if {$name ne ""} {
            if {[info command ::dbgui::database] eq ""} {
                    tk_messageBox -type ok -icon error -title "No database" \
                        -message {ERROR: program bug - no database has been established yet}
            } elseif {[$self _CfgExists $name]} {
                tk_messageBox -type ok -icon error -title {Duplicate config name} \
                    -message "There already is a configuration named $name not going to overwrite it"
                
            } else {
            
                dbconfig::saveConfig dbgui::database $name
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
        set configs [dbconfig::listConfigs ::dbgui::database]
        foreach config $configs {
            if {$name eq [dict get $config name]} {
                return 1
            }
        }
        return 0
    }
    
}
