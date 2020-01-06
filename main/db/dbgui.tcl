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


namespace eval dbgui {
    set here [file dirname [info script]]
    variable version 1.0
    
    # Icons used in the treeview:
    
    variable iconcredits {Icons from Vitaly Gorbachev, Pixel perfect via flaticon.com}

    variable folder [image create photo dbgui::folder -format png -file [file join $here folder.png]]
    variable openfolder [image create photo dbgui::openfolder -format png -file [file join $here openfolder.png]]
    variable spectrum   [image create photo dbgui::spectrum   -format png -file [file join $here spectrum.png]]
    
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
#  @class menubar
#   Creates the menubar for the application and provides callbacks for
#   specific items.  The menubar is set as the menubar for the appropriate
#   top-level widget.
#
#    Menus:
#       File:  New... open...
#       Help:  About
#
#  Callback options:
#     *  -oncreate - create a new database the file is passed as a parameter to the
#                  script.
#     *  -onopen   - Open an existing database file.  The file is passed as a parameter
#                  to the script.
#
#  Normally, on successful completions, these callbacks should pass a new
#  database command to the treeview widget, and the filename to the statusbar
#  widget.
#
snit::type dbgui::menubar {
    option -oncreate
    option -onopen
    
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
        
        menu $window.file -tearoff 0
        $window.file add command -label New... -command [mymethod _OnNew]
        $window.file add command -label Open... -command [mymethod _OnOpen]
        $window add cascade -label "File" -menu $window.file 
        
        
        menu $window.help -tearoff 0
        $window.help add command -label About... -command [mymethod _OnAbout]
        $window add cascade -label "Help" -menu $window.help
        
        #  Figure out our top level and configure it so our menu is the
        #  menubar.
        
        set top [$self _TopLevel ]
        $top config -menu $window
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
        puts _TopLevel
        set path [split $window .]
        if {[llength $path] == 1} {
            return .
        } else {
            return .[lindex $path 0]
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
            eval $options(-oncreate) [list $filename] 
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
            eval $options(-onopen) [list $filename] 
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
}
