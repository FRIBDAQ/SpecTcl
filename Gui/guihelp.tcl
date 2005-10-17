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

package provide guihelp 1.0
package require Iwidgets

# This package provide the SpecTclGUI help subsystem.
#

namespace eval guihelp {
    variable helpdir [file join [file dirname [info script]] help]
}

#---------------------General utilities -------------------------

# findInPath path program
#      Locate a program in a path.
# Parameters:
#   path    - A directory path set in the same format as a unix
#             PATH (colon separated directories).
#   program - A program name e.g. netscape
# Returns:
#    Full path specification of the program or empty
#    if there is none.
#
proc findInPath {path program} {
    global tcl_platform
    if {$tcl_platform(platform) == "windows"} {
        set pathsep ";"
    } else {
        set pathsep ":"
    }
    set dirlist [split $path $pathsep]


    foreach dir $dirlist {
        set fullpath [file join $dir $program]
        if {[file executable $fullpath]} {

            return $fullpath
        }
    }
    return [list]
}
# findBrowser
#     Locates a web browser.  The browser is located as follows:
#     If the BROWSER env name exists and translates to an executable
#     that's returned otherwise we look in the path for browsers as
#     follows:
#     firefox, mozilla, netscape
# Returns:
#    A full path to a browser to run or {} if there is no browser.
#
proc findBrowser {} {
    global env
    if {[array names env BROWSER] != ""} {

        return $env(BROWSER)
    }
    if {[array names env PATH] != ""} {
        set path $env(PATH)

        foreach browser {firefox mozilla netscape} {
            set fullpath [findInPath $path $browser]

            if {$fullpath != ""} {
                return $fullpath
            }
        }
    }
    return [list]
}
# getGuiHelpDirectory
#    Return the location of the help information.
#
proc getGuiHelpDirectory {} {
    return $::guihelp::helpdir
}
#------------------------ Dialogs -------------------------------

#aboutMeDialog
#    Displays a help about...dialog for me.
#
proc aboutMeDialog {} {
    set message {This is the SpecTcl Gui version }
    append message [package version SpecTclGui]

    tk_messageBox -icon info -message $message -title About...
}

# spectclProgramHelp
#    Display the SpecTcl program help in the user's browser
#    The browser is fired off in the background.   If a browser
#    cannot be found, a message indicating this is produced.
#
proc spectclProgramHelp {} {
    global SpecTclHome
    set url "file://"
    set url [file join $SpecTclHome doc index.htm]

    set browser [findBrowser]
    if {$browser == ""} {
        tk_messageBox -icon warning -title noBrowser \
            -message {I cannot find a web browser in which to display the help.
 set the BROWSER environment variable to point to your favorite
 browser and restart SpecTcl to enable help}
    } else {
        exec $browser $url &
    }
}
# spectclGuiDisplayHelpTopic topic
#         Displays the specified help topic.  If necessary, a help browser
#         is created and configured.
#
# Parameters:
#    topic  - The topic to display (the file without the .html).
#
proc spectclGuiDisplayHelpTopic topic {
    if {![winfo exists .helpdialog]} {
        set helpdir [getGuiHelpDirectory]
        ::iwidgets::hyperhelp .helpdialog -helpdir $helpdir
        .helpdialog configure -topics [list Intro Menus Browser Spectra Parameters Gates statusLine Variables Vocabulary]
    }

    .helpdialog showtopic $topic
    .helpdialog activate

    return .helpdialog
}
#
# spectclGuiHelpTopics
#     Displays a help topic index.  Returns the
#     widget in which the help browser was built.
#     If the browser already exists it is destroyed
#     first.
#
proc spectclGuiHelpTopics {} {
    return [spectclGuiDisplayHelpTopic Intro]
}

