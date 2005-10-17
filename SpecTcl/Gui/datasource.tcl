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
package provide datasource 1.0
package require Iwidgets
package require guiutilities
#
#  Namespace to hold some of the configuration entries.
#
namespace eval datasource {
    variable daqroot         /usr/opt/daq;   # Where the DAQ software is installed.
    variable lasthost        localhost;  # Most recent online host.
    variable lasteventfile   {}
    variable lastpipecommand {}
    variable lastpipeargs    {}
    variable lastrunlist     {}
    variable runlistFiles    {}
    variable warnedFilters   0
    variable lastFilterFile {}
}

# attachOnline
#      Attach to an online data source.
#      datasource::daqroot is assumed to hold the installation
#      root of an nscldaq installation and the bin subdir of that
#      will have spectcldaq which is a pipe data source to the online system.
#      We need to pop up a dialog to request the node from which we take data.
#
proc attachOnline {} {
    hostprompt .hostprompt -host $::datasource::lasthost
    .hostprompt modal
    if {[winfo exists .hostprompt]} {
        set host [.hostprompt cget -host]
        if {$host != ""} {
            set $::datasource::lasthost $host
            set url [format "tcp://%s:2602/" $host]
            catch stop;                         # In case analysis is active.
            attach -pipe [file join $::datasource::daqroot bin spectcldaq]  $url
            start
        }
        destroy .hostprompt
    }
}
# attachFile
#       Prompts for an event file to attach to and does the deed.
#
proc attachFile {} {
    set file [tk_getOpenFile  -defaultextension .evt                                   \
                              -initialfile [file tail $::datasource::lasteventfile]    \
                              -initialdir  [file dirname $::datasource::lasteventfile] \
                              -filetypes [list [list "Event files" .evt]]]

    if {$file != ""} {
        if {[file readable $file]} {
            catch stop
            attach -file $file
            start
            set ::datasource::lasteventfile $file
        } else {
            tk_messageBox -icon error  -title {Can't read file} \
                          -message "Could not read the file $file permission problem or file does not exist"
        }
    }
}
# attachPipe
#      Attaches to an arbitrary pipe data source.  The
#      datasource program is gotten from a file chooser, but the
#      parameters are provided via an entry widget.
#
proc attachPipe {} {
    attachpipe .attachpipe -initialdir [file dirname $::datasource::lastpipecommand] \
                           -initialfile  $::datasource::lastpipecommand \
                           -initialargs $::datasource::lastpipeargs
    .attachpipe modal

    if {[winfo exists .attachpipe]} {
        set command    [.attachpipe cget -command]
        set arguments  [.attachpipe cget -arguments]
        if {$command != "" } {
            if {[file executable $command]} {
                catch stop
                if {![catch {eval attach -pipe $command $arguments} msg] } {
                    start
                    set ::datasource::lastpipecommand $command
                    set ::datasource::lastpipeargs    $arguments
                } else {
                    tk_messageBox -icon error -title "Can't attach" \
                        -message "The `attach -pipe $command $arguments' failed because: $msg"
                }
            } else {
                tk_messageBox -icon error -title "Can't execute file" \
                    -message {You do not have execute access to $command}
            }
        }
        destroy .attachpipe
    }
}
# attachRunList
#        Attaches to a runlist.  A runlist is a file that contains
#        paths of event files.  Each file is analyzed in turn.
#        A run is considered anlayzed when there is a transition
#        from active -> inactive on analysis (this is determined
#        by tracing RunState.
proc attachRunList {} {
    set runlist [tk_getOpenFile -defaultextension .clu                                  \
                              -initialfile [file tail $::datasource::lastrunlist]    \
                              -initialdir  [file dirname $::datasource::lastrunlist] \
                              -filetypes [list [list "Cluster files" .clu]]]
    if {$runlist != ""} {
        if {$datasource::runlistFiles != ""} {
            set answer [tk_messageBox -icon question -title {stop current runlist}       \
                                      -type okcancel  \
                                      -message {A cluster file is currently being processed do you want to continue  or cancel this new runlist request}]
            if {$answer == "cancel"} {
                return
            } else {
                catch {trace remove variable ::RunState write nextFileInRunList}
            }
        }
        if {[catch {open $runlist r} msg]} {
            tk_messageBox -icon error -title "Can not open file"    \
                -message "Could not open $runlist because: $msg"
            return
        }
        set files   [read $msg]
        set datasource::runlistFiles [split $files "\n"]
        catch stop
        trace add variable ::RunState write nextFileInRunlist
        set ::RunState 0;             # Start the next file...
    }

}
#  attachFilter
#       Attaches to a filter file.  This is much the same as attaching a
#       normal file but we must specify format=filter.
#
proc attachFilter {} {

    # If we have not already done so make sure the user knows what they are doing.

    if {!$::datasource::warnedFilters} {
        set answer [tk_messageBox -type okcancel -icon warning -title {Filter Warning} \
                    -message {SpecTcl must usually be tailored to read filter files by removing the
event processors that unpack raw parameters and substituting the filter event processors for them.
This is described in the online SpecTcl documentation.  If this SpecTcl has not been tailored in
that way, you should cancel this operation}]
        set ::datasource::warnedFilters 1
        if {$answer == "cancel"} return
    }
    #  Get the filter file.

    set filename [tk_getOpenFile -defaultextension .flt                                   \
                                -initialdir [file dirname $::datasource::lastFilterFile]  \
                                -initialfile [file tail $::datasource::lastFilterFile]    \
                                -title {Select filter file}                               \
                                -filetypes [list [list {Filter Files} .flt]]]
    if {$filename != ""} {
        if {[file readable $filename]} {
            catch stop
            if {[catch {attach -file -format filter $filename} failure]} {
                tk_messageBox -icon error -title {Attach Failed} \
                              -message "Failed to attach to $filename because: $failure"
                return
            }
            catch start
        } else {
            tk_messageBox -icon error -title {Can't read file} \
                -message "File $filename cannot be read ensure it exists and you have read access."
        }
    }
}

# nextFileInRunlist name index op
#       Trace added to the RunState variable.  If the run state
#       has changed to 0 (inactive) we assume the previous run ended
#       and lift the next run file from the cluster file.. and start
#       analyzing it.  If the run file cannot be analyzed (e.g. doesn't exist),
#       The user can continue or cancel the runlist.
# Parameters:
#    name    - Name of the traced variable (::RunState).
#    index   - Array index (empty).
#    op      - operation (w).
# Implicit inputs:
#       varname has already been given the new value.
proc nextFileInRunlist {varname index op} {
    global $varname
    #  ensure analysis halted.
    #
    while {[set $varname] == 0} {
        #  If we finished the last one...stop the music.
        if {[llength $::datasource::runlistFiles] == 0} {
            trace remove variable $varname write nextFileInRunlist
            return
        }
        #  Extract the next run file from the list:

        set file [lindex $::datasource::runlistFiles 0]
        set ::datasource::runlistFiles [lrange $::datasource::runlistFiles 1 end]
        if {$file != ""} {
            catch stop
            if {![catch {attach -file $file} msg]} {
                catch start
            } else {
                set answer [tk_messageBox -icon error -title {attach failed} \
                            -type okcancel                                   \
                            -message "Failed to attach file $file because: $msg Ok to continue processing or cancel?"]
                if {$answer == "cancel"} {
                    set ::datasource::runfile {}
                    trace remove variable $varname write nextFileInRunlist
                    return
                }
            }
        }

    }
}

#
#  Prompter for a host for the attachonline.
#
snit::widget hostprompt {
    hulltype toplevel
    option -host           {}
    option -okcommand      {}
    option -cancelcommand  {}

    variable hidden {}
    constructor args {
        label $win.hostlabel -text Host:
        entry $win.host

        button $win.ok     -text Ok     -command [mymethod onOk]
        button $win.cancel -text Cancel -command [mymethod onCancel]
        button $win.help   -text Help   -command [list spectclGuiDisplayHelpTopic  hostPrompt]


        grid $win.hostlabel $win.host
        grid $win.ok        $win.cancel  $win.help

        $self configurelist $args
    }
    # modal
    #     Turns this into a modal dialog.
    #
    method modal {} {
        if {$hidden == ""} {
            set hidden [frame $win.hidden]
            wm deiconify $win
            focus $win
            grab  $win
            tkwait window $hidden
        }
    }
    # onOk
    #    Called on a click of the ok button. -okscript is invoked if defined
    #    and if we are modal, the hidden window is destroyed.
    #
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
    #    Called on a click of the cancel button.  The entry is blanked
    #    -cancelscript is called if defined and, if we are modal,
    #    the hidden frame is destroyed.
    #
    method onCancel {} {
        setEntry $win.host {}
        set script $options(-cancelcommand)
        if {$script != ""} {
            eval $script
        }

        if {$hidden != ""} {
            destroy $hidden
            set hidden {}
        }
    }
    # configure -host value
    #      Called to configure a host name
    #      into the widget.
    #
    onconfigure -host value {
        setEntry $win.host $value
    }
    # cget -host
    #      Called to retrieve the hostname from the widget.
    oncget -host {
        return [$win.host get]
    }
}
#  attachpipe:
#      A dialog widget that prompts for a pipe command to attach to and a
#      set of arguments to pass it.
#
#
snit::widget attachpipe {
    hulltype toplevel

    option -initialdir    {}
    option -initialfile   {}
    option -initialargs   {}
    option -command       {}
    option -arguments     {}
    option -okcommand     {}
    option -cancelcommand {}

    variable hidden       {}

    constructor args {
        $self configurelist $args

        ::iwidgets::fileselectionbox $win.fsb -directory $options(-initialdir)    \
                                    -filesearchcommand [mymethod filterFiles]    \
                                    -selectioncommand  [mymethod onOk]
        setEntry $win.fsb.selection $options(-initialfile)

        label $win.argslabel -text {Parameters: }
        entry $win.args      -width 32
        setEntry $win.args $options(-initialargs)


        button $win.ok     -text Ok     -command [mymethod onOk]
        button $win.cancel -text Cancel -command [mymethod onCancel]
        button $win.help   -text Help   -command [list spectclGuiDisplayHelpTopic attachPipe]

        grid $win.fsb           -
        grid $win.argslabel  $win.args
        grid $win.ok         $win.cancel  $win.help

    }
    # modal
    #     Turns the  box into a modal dialog.  This is done by
    #     creating the usual hidden frame, grabbing and waiting for
    #     the hidden frame to be destroyed.
    method modal {} {
        if {$hidden == ""} {
            set hidden [frame $win.hidden]
            wm deiconify $win
            focus $win
            grab  $win
            tkwait window $hidden
        }
    }
    # filterFiles dir mask
    #     Determines which set of files to show in the file part
    #     of the file chooser.  We only show executable files.
    # Parameters:
    #  dir  - The dir to check
    #  mask - wildcard mask that narrows the set of files.
    # Returns:
    #   List of files that match mask in dir that are regular and executable.
    #
    method filterFiles {dir mask} {
        set files [glob [file join $dir $mask]]
        set result [list]
        foreach file $files {
            if {[file isfile $file] && [file executable $file]} {
                lappend result [file tail $file]
            }
        }
        return $result
    }
    # onOk
    #     Accepts the dialog results;  Calls -okcommand if defined.
    #     if modal, destroys the hidden widget.
    #
    method onOk {} {
        set script $options(-okcommand)
        if {$script != ""} {
            eval $script
        }
        if {($hidden != "") && [winfo exists $hidden]} {
            destroy $hidden
            set hidden {}
        }
    }
    # onCancel
    #      Cancels the dialog.  The file is set to empty.
    #      If -cancelcommand is defined, it is called.
    #      If the dialog is modal, the hidden window is destroyed.
    #
    method onCancel {} {
        setEntry $win.fsb.selection {}

        set script $options(-cancelcommand)
        if {$script != ""} {
            eval $script
        }
        if {($hidden != "") && [winfo exists $hidden]} {
            destroy $hidden
            set hidden {}
        }
    }
    oncget -command {
        return [$win.fsb get]
    }
    oncget -arguments {
        return [$win.args get]
    }
}
