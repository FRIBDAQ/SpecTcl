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
package require snit
package require Iwidgets
package require guiutilities

package require daqdefs

package require segmentedrun

#
#  Namespace to hold some of the configuration entries.
#
namespace eval datasource {
    variable daqroot         [list $::daqdefs::daqroot /usr/opt/daq/current /usr/opt/daq/8.1 /usr/opt/daq/8.0  /usr/opt/daq];   # Where the DAQ software is installed.
    variable lasthost        localhost;  # Most recent online host.
    variable lastformat      ring10
    variable lasteventfile   {}
    variable lastpipecommand {}
    variable lastpipeargs    {}
    variable lastrunlist     {}
    variable runlistFiles    {}
    variable warnedFilters   0
    variable lastFilterFile {}
    variable actualSpecTclDaq {}
    variable lastring      $::tcl_platform(user)
    variable clustersize   8192
}


##
# defaultFormat
#
#   Determines the default format to use for a data source.
#   In  most cases, this is just ::datasource::lastformat, however
#   since this can come from the preferences file and that may just have an
#   an unqualified 'ring'  'ring' gets mapped to 'ring10'....which is stored
#   back into ::datasource::lastformat
#
# @return string - default data format.
#
proc defaultFormat {} {
    
    if {$::datasource::lastformat eq "ring"} {
        set ::datasource::lastformat ring10
    }
    return $::datasource::lastformat
}

# datasource::findSpecTclDaq
#    Locate the atual spectcldaq file.  We try for it in the
#    nscldaq roots described by daqroot, and if we can't find
#    it there, we ask the user to browse for it.
#
proc datasource::findSpecTclDaq {} {
    if {$datasource::actualSpecTclDaq ne ""} {
	return;				# already found.
    }
    if {[array names GuiPrefs::preferences defaultDaqRoot] ne ""} {
	set candidate \
	    [file join $GuiPrefs::preferences(defaultDaqRoot) bin spectcldaq]
	if {[file executable $candidate]} {
	    set datasource::actualSpecTclDaq $candidate
	    return
	}
	
    }

    foreach root $datasource::daqroot {
	set candidate [file join $root bin spectcldaq]
	if {[file executable $candidate]} {
	    set datasource::actualSpecTclDaq $candidate
	    return
	}
    }
    #  Didn't find one so prompt the user with tk_getOpenFile:

    set datasource::actualSpecTclDaq \
	[tk_getOpenFile -title {Locate spectcldaq for me please}]
    if {[file executable $datasource::actualSpecTclDaq]} {
	return
    }
    set datasource::actualSpecTclDaq [list]
    tk_messageBox -icon info -title {No spectcldaq}   \
	-message \
	{Can't locate spectcldaq, and you could not help, cancelling the attach}


}
##
# attachDataSource
#   Perform the actual attach.  This factors out the following actions:
#   *  Shutting down the previous data source (if active)
#   *  Providing the value to the -format switch from  a parameter.
#   *  Performing the actual attach command.
#   *  If necessary/possible performing a ringformat command to hint at what
#      format data might be in for ring buffer data sources.
#   *  starts analysis.
#
# @param format     - Data format e.g. spectrodaq, ring10, ring11...
# @param size       - -size parameter.
# @param sourcetype - An attach source type switch (e.g. -pipe or -file.)
# @param sourcespec - Source specification.  Note that this parameter
#                     will be {*}'d when passed to attach so it can be a multiword
#                     command for -pipe data sources.
#
proc attachDataSource {format size sourcetype sourcespec} {
    catch stop
    
    # Collapse the format for ring* -> ring
    
    if {[string match ring* $format]} {
        set dataFormat ring
    } else {
        set dataFormat $format
    }
    
    #  Attach the data source.
    
    set command [list \
        attach -size $size -format $dataFormat $sourcetype {*}$sourcespec
    ]
    {*}$command
    
    #  Figure out if ringformat must be used:
    
    if {[string match ring* $format]} {
        if {$format eq "ring10"} {
            ringformat 10.0
        } elseif {$format eq "ring11"} {
            ringformat 11.0
        }
        # Any other format CRingBufferDecoder will have to do its best to
        # sort things out.
    }
    #  start analysis:
    
    start
}

#
# attachOnline
#      Attach to an online data source.
#      datasource::daqroot is assumed to hold the installation
#      root of an nscldaq installation and the bin subdir of that
#      will have spectcldaq which is a pipe data source to the online system.
#      We need to pop up a dialog to request the node from which we take data.
#
proc attachOnline {} {
    hostprompt .hostprompt -host $::datasource::lasthost \
	-format [defaultFormat]                          \
	-buffersize $::GuiPrefs::preferences(defaultBuffersize)  \
	-ringname   $::datasource::lastring
    .hostprompt modal
    if {[winfo exists .hostprompt]} {
        set host [.hostprompt cget -host]

        if {$host != ""} {            
	    set format [.hostprompt cget -format]
	    set additionalInfo {}

	    if {[string match ring* $format]} {
		set additionalInfo [.hostprompt cget -ringname]
                set ::datasource::lastring $additionalInfo
            }
            set size [.hostprompt cget -buffersize]
    
            set ::datasource::lasthost     $host
	    set ::datasource::lastformat   $format            
	    set ::GuiPrefs::preferences(defaultBuffersize) $size
            
	    set helper [.hostprompt onlinehelper $host $additionalInfo]

            attachDataSource $format $size -pipe $helper
        }
        destroy .hostprompt
    }
}
# attachFile
#       Prompts for an event file to attach to and does the deed.
#
proc attachFile {} {

    attachfile .prompt                                               \
	-defaultextension .evt                                       \
	-initialfile  $::datasource::lasteventfile                   \
	-initialdir   [file dirname $::datasource::lasteventfile]   \
	-buffersize   $::GuiPrefs::preferences(defaultBuffersize)   \
	-format [defaultFormat]
    .prompt modal
    set file [.prompt cget -initialfile]
    set size [.prompt cget -buffersize]
    set format [.prompt cget -format]
    destroy .prompt
	
    if {$file != ""} {
        if {[file readable $file]} {
           set isRing 0
	    set ::GuiPrefs::preferences(defaultBuffersize) $size
            set ::datasource::lasteventfile $file
	    set ::datasource::lastformat $format

            # If the file is part of a segmented run, we ask the user
	    # if they'd like us to play back the entire run:

	    set segments [listRunFiles $file]
	    if {[llength $segments] == 1} {
		attachDataSource $format $size -file $file
	    } else {
		if {[doSegmentedRun $segments]} {
		    attachDataSource $format $size -pipe [segmentPlaybackCommand $segments]
		} else {
		    attachDataSource $format $size -file $file;	# just do the selected file.
		}
	    }
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
    .attachpipe configure \
	                   -buffersize $::GuiPrefs::preferences(defaultBuffersize)
    .attachpipe modal

    if {[winfo exists .attachpipe]} {
        set command    [.attachpipe cget -command]
        set arguments  [.attachpipe cget -arguments]

        if {$command != "" } {
            if {[file executable $command]} {

		set size [.attachpipe cget -buffersize]
                set format [.attachpipe cget -format]
                set ::GuiPrefs::preferences(defaultBuffersize) $size
                set ::datasource::lastpipecommand $command
                set ::datasource::lastpipeargs    $arguments
                attachDataSource $format $size -pipe "$command $arguments"
            } else {
                tk_messageBox -icon error -title "Can't execute file" \
                    -message {You do not have execute access to the pipe source program file: $command }
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
    
    # Prompt for the filename, buffering and the format of the input files
    # At this time, all cluster files must have the same format!
    
    attachfile .clusterchooser \
        -defaultextension .clu -initialfile $::datasource::lastrunlist \
        -initialdir [file dirname $::datasource::lastrunlist]   \
        -buffersize $::GuiPrefs::preferences(defaultBuffersize) \
        -format [defaultFormat]
    
    .clusterchooser modal
    
    # Fish the stuff out of the dialog:
    
    set runlist [.clusterchooser cget -initialfile]
    set size   [.clusterchooser cget -buffersize]
    set format [.clusterchooser cget -format]
    
    destroy .clusterchooser
    
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
        set datasource::lastrunlist $runlist
        set datasource::lastformat  $format
        set datasource::clustersize $size
        
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
            set status [catch \
                {attachDataSource $::datasource::lastformat $::datasource::clustersize \
                -file $file} msg]
    
            if {$status} {
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
#  Detach from data sources.. well not really, this is actuall stop
#  and attach -file /dev/null.
#
proc detach {} {
    catch stop
    attach -file /dev/null
}

#
#  This radio box can be used within the various prompters to select the
#  format of the data source.  For now this is restricted to three hard coded
#  choices:
#   nscl  -  NSCL standard buffers with event sizes < 32K and buffersize < 32K
#   jumbo -  NSCL standard buffers with events/buffers sizes potentially > 32k
#   ring10 -  Data from NSCLDAQ 10.x
#   ring   - Data from NSCLDAQ 11.x
#

#  
#  OPTIONS:
#    -format   - get set the radio button value which can be nscl, jumbo, ring
#  Methods:
#    helptext  - Get help text that describes the formats.
#    pipehelper- Given a node, provides the helper to be used for a pipe data
#                source for the current format.  The directory is assumed to be
#                provided by the caller, or in the path.
#                For ring buffers, the assumption is that the 
#                ring for the current user is desired.
#
#

snit::widget formatChooser {
    variable formats {nscl jumbo ring10 ring11};     # Valid options
    option   -format ring10;        # Default option.
    option   -command [list];	  # Command to call if format changes.

    constructor args {

	set col 0
	foreach format $formats {
	    radiobutton $win.$format -variable [myvar options(-format)] \
		-value $format -text $format -command [mymethod dispatch]
	    grid $win.$format -sticky w -row 0 -column $col
	    incr col
	}
	$self configurelist $args

	set initialFormat $options(-format)
	after 1 [list $win.$initialFormat select]; # Not sure why this must be scheduled(?)
    }
    destructor {

    }
	    
    #
    #  Return help text for the formats
    #
    method helptext {} {
	append result "Format   Meaning\n"
	append result "nscl     nscl buffers/events < 32K words (< 10.0)\n" 
	append result "jumbo    nscl buffers/events >= 32KWords (< 10.0)\n"
	append result "ring10   Ring buffer data from nscldaq 10.x\n"
        append result "ring11   Ring buffer data from nscldaq 11.x+\n"

	return $result
    }
    #
    #  Method to return the correct helper command for online attaches.
    #  The assumption is that for each format there's a method
    #  by that name.. The assumption is also that the 
    #  caller will figure out the directory part of the path.
    #
    method onlinehelper {host {other {}}} {
	return [$self $options(-format) $host $other]
    }

    #-----------------------------------------------------------------
    # 
    # Private methods
    #
    
    # Dispatch the -command switch.  The reciever will get the new
    # format value

    method dispatch {} {
	set command $options(-command)
	if {$command ne ""} {
	    uplevel #0 $command  $options(-format)
	}
    }
    
    # Construct the helper program line for spectrodaq online
    
    method spectrodaq {host {ignored {}}} {
	#
	# Really dirty:

	set url tcp://$host:2602
	datasource::findSpecTclDaq
	return "$datasource::actualSpecTclDaq $url"
    }

    # Construct the helper program for nscl format 

    method nscl {host {ignored {}}} {
	return [$self spectrodaq $host]
    }
    # Construct the helper program for jumbo format:

    method jumbo {host {ignored {}}} {
	return [$self spectrodaq $host]
    }

    # Construct the helper program for ring buffers.
    # We are going to assum the current user is the
    # owner of the target ring.
    #
    # The following strategy is used to locate ringselector:
    # - Look for it in the default daqroot in the preferences dialog.
    # - Look for it in /usr/opt/daq/current/bin : if present use that one.
    # - Look for it in /usr/opt/daq/1*.* sorted descdending and take the first one.
    #
    method ring {host {ringname ""}} {
	if {$ringname eq ""} {
	    set ringname $::tcl_platform(user)
	}
	set url "tcp://$host/$ringname"
	set selection "--sample=PHYSICS_EVENT"

	set firstDirs [list]

	if {[array names GuiPrefs::preferences defaultDaqRoot] eq "defaultDaqRoot"} {
	    if {$GuiPrefs::preferences(defaultDaqRoot) ne ""} {
		lappend firstDirs [file tail $GuiPrefs::preferences(defaultDaqRoot)]
	    }
	}
	lappend firstDirs current

	
	set ringselector ""
	lappend dirs {*}[glob -directory /usr/opt/daq 1*.*]; # These are full dirnames.
	foreach dir $dirs {
	    lappend dirtails [file tail $dir]
	}
	set dirtail [lsort -decreasing  $dirtails]; # sorted by decreasing version...
	set dirtails [list {*}$firstDirs {*}$dirtail]
	foreach dir $dirtails {
	    set ringselector [file join /usr/opt/daq $dir bin ringselector]
	    if {[file executable $ringselector]} {
		break
	    }
	}
    
	if {$ringselector eq ""} {
	    tk_messageBox -title "No Ringdaq" -icon error \
		-message {Unable to find an installation of NSCL ringdaq - contact your sysadmin.}

	    return "";		# ensure stuff fails.
	} else {
	    # see if we can use the --non-blocking switch:

	    set ringselector [file normalize $ringselector]
	    set rootdir [file join [file dirname $ringselector] ..]
	    if {[$self hasNonBlocking $rootdir]} {
		append ringselector " --non-blocking"
	    }
	    return "$ringselector --source=$url $selection"

	}
    }
    ##
    # hasNonBlocking
    #    If the VERSION file indicates the version is > 11.0-rc14
    #    ringselector has the --non-blocking flag.
    #
    # @param rootdir - Top level directory of the daq install.
    #
    method hasNonBlocking rootdir {
	set fd [open [file join $rootdir VERSION]]
	set versionString [gets $fd]
	close $fd

	set version [lindex $versionString 1]
	set splitVersion [split $version '-']
	set majorMinor [lindex $splitVersion 1]

	if {$majorMinor > 11.0} {
	    return 1
	}
	# The edit level  is a bit tricky.  It can be of the form:
	# rcn where n is a counting number and rc means this is a release
	# candidate or mmm a three digit number with leading zeroes that is
	# the edit level of a production release.  production releases
	# are always considered later than release candidates.
	#

	set editLevel [lindex $splitVersion 2]
      
	if {[string range $editLevel 0 1] eq "rc"} {
	    set isRc 1
	    set level [string range $editLevel 2 end]
	} else {
	    set isRc 0
	    set level $editLevel
	}
	# we only need to worry about tie breakders (11.0 versions < rc14):

	if {$majorMinor eq "11.0"} {
	    if {$isRc && ($level < 14)} { # rc's less than 14 can't
		return 0
	    } else {		# anything else can.
		return 1
	    }
	}

	if {($majorMinor == 11.0) && ($rc >= 14)} {
	    return 1
	}
	return 0

    }
    ##
    # ring10, ring11 - are all ring sources
    #
    method ring10 {host {ringname ""}} {
        return [$self ring $host $ringname]
    }
    method ring11 {host {ringname ""}} {
        return [$self ring $host $ringname]
    }

}

#
#  Prompter for a host for the attachonline.
#
snit::widget hostprompt {
    hulltype toplevel
    option -host           {}
    option -ringname       -default $::tcl_platform(user) -configuremethod ringChanged \
	-cgetmethod getRing
    option -okcommand      {}
    option -cancelcommand  {}
    option -buffersize     8192

    delegate option -format to format

    delegate method onlinehelper to format
    

    variable hidden {}
    constructor args {
        label $win.hostlabel -text Host:
        entry $win.host
	label $win.ringlabel -text {Ring:}
	entry $win.ring

	spinbox $win.buffersize -values {512 1024 2048 4096 8192 16384 32768 65536}
	label   $win.buflabel   -text {Buffer size in bytes: }
	$win.buffersize set $options(-buffersize)

	label         $win.fmtlabel -text {Data format}
	install format using formatChooser $win.fmt -command [mymethod formatChanged] -format ring10
        button $win.ok     -text Ok     -command [mymethod onOk]
        button $win.cancel -text Cancel -command [mymethod onCancel]
        button $win.help   -text Help   -command [list spectclGuiDisplayHelpTopic  hostPrompt]
	
        grid $win.hostlabel $win.host
	grid $win.ringlabel $win.ring
	grid $win.buflabel  $win.buffersize
	grid $win.fmtlabel
	grid $win.fmt
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
    # configure -buffersize n
    #    SEt the buffersize.
    #
    onconfigure -buffersize value {
	set options(-buffersize) $value
	$win.buffersize set $value
    }

    # cget -host
    #      Called to retrieve the hostname from the widget.
    oncget -host {
        return [$win.host get]
    }
    # cget -buffersize
    #       Return the sizeof the buffer.
    oncget -buffersize {
	return [$win.buffersize get]
    }
    ##
    # the ring name changed...update the entry:
    #
    method ringChanged {option value} {
	$win.ring delete 0 end
	$win.ring insert end $value
	set options($option) $value
    }
    ##
    # get the ring from the entry:
    #
    method getRing {option} {
	return [$win.ring get]
    }
    ## 
    # The format changed.  If the format is 'ring' we can 
    # enable the ring label/entry...otherwise disable them
    #
    method formatChanged newFormat {
	if {[string match ring* $newFormat]} {
	    $win.ringlabel configure -state normal
	    $win.ring      configure -state normal
	} else {
	    $win.ringlabel configure -state disable
	    $win.ring      configure -state disable
	}
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
    option -buffersize    8192
    
    delegate option -format to format

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


	label   $win.sizelbl -text {Buffer Size: }
	spinbox $win.size    -values {512 1024 2048 4096 8192 16384 32768 65536}
	$win.size set $options(-buffersize)

        button $win.ok     -text Ok     -command [mymethod onOk]
        button $win.cancel -text Cancel -command [mymethod onCancel]
        button $win.help   -text Help   -command [list spectclGuiDisplayHelpTopic attachPipe]
        
        install format using formatChooser $win.format -format ring10

        grid $win.fsb           -
        grid $win.argslabel  $win.args
	grid $win.sizelbl    $win.size
        grid $win.format  -columnspan 2
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
        return [lsort -increasing $result]
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
    onconfigure -buffersize value {
	$win.size set $value
	set options(-buffersize) $value
    }
    oncget -buffersize {
	return [$win.size get]
    }
    oncget -command {
        return [$win.fsb get]
    }
    oncget -arguments {
        return [$win.args get]
    }
}
#
#   Snit widget to prompt for a file and buffersize for an event data source.
#   The top part of this widget is an iwdigets fileselectionbox.
#   The middle part a buffersize selector.
#   The bottom part the an Ok/Cancel button pair.
#   Options:
#     -defaultextension  (.evt)    - Default file extension.
#     -initialfile        {}       - Initially selected file.
#     -initialdir         .        - Directory initially being searched.
#     -buffersize         8129     - Buffersize.
#     -command            {}       - Script invoked when ok is clicked.
#     -cancel             {}       - script invoked when cancel is clicked.
snit::widget attachfile {
    hulltype toplevel
    option -defaultextension  {.evt}
    option -initialfile       {}
    option -initialdir        {.}
    option -buffersize        8192
    option -command           {}
    option -cancel            {}

    delegate option -format to  format

    variable hidden {}

    constructor args {
	# the file selector:

	iwidgets::fileselectionbox $win.fsb -directory $options(-initialdir) \
                                            -mask *$options(-defaultextension) 
	$self setSelectedFile $options(-initialfile)

	label $win.sizelabel -text {Buffer size: }
	spinbox $win.size    -values {512 1024 2048 4096 8192 16384 32768 65536}
	$win.size set $options(-buffersize)

	label $win.formatlabel -text {Format:}
	install format using formatChooser $win.format

	button $win.ok     -text Ok     -command [mymethod onOk]
	button $win.cancel -text Cancel -command [mymethod onCancel]

	grid $win.fsb           -
	grid $win.sizelabel $win.size
	grid $win.format
	grid $win.ok        $win.cancel
	                                    
	$self configurelist $args
    }
    # modal:
    #   Turns this into a modal dialog.
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
    # endModal
    #    Ends the modalness of the window.
    #
    method endModal {} {
	if {$hidden != ""} {
	    destroy $hidden
	    set hidden {}
	    grab release $win
	}
    }
    # setSelectedFile name
    #    Sets the name of the selected file.
    #
    method setSelectedFile name {
	$win.fsb.selection clear
	$win.fsb.selection insert end $name
    }
    #  onOk  
    #    Invoke the -command script and end modality.
    #
    method onOk {} {
	set script $options(-command)
	if {$script != ""} {
	    eval $script
	}
	$self endModal
    }
    # onCancel
    #    Invoke the -cancel script and end modality.
    #    Set the value to "" though.
    method onCancel {} {
	set script $options(-cancel)
	if {$script != ""} {
	    eval $script
	}
	$self configure -initialfile {}
	$self endModal
    }
    # configure -defaultextension
    #    Set a new default extension and refilter the box.
    #
    onconfigure -defaultextension {ext} {
	set currentMask [$win.fsb cget -mask]
	set currentFile [file rootname $currentMask]
	set currentMask $currentFile$ext
	set options(-defaultextension) $currentMask
	$win.fsb configure -mask $currentMask
	$win.fsb filter
    }
    # configure -initialfile 
    #     Set the selecte file.  This should be a full path.
    #
    onconfigure -initialfile name {
	set options(-initialfile) $name
	$self setSelectedFile $name
    }
    # configure -initialdir
    #    Set the search directory.
    onconfigure -initialdir name {
	$win.fsb configure -directory $name
	$win.fsb filter
	set options(-initialdir) $name
    }
    # configure -buffersize
    #   Set the buffersize spinbox value.
    #
    onconfigure -buffersize value {
	$win.size set $value
	set options(-buffersize) $value
    }
    #  cget -defaultextension
    #    Returns the extension part of the mask.
    #
    oncget -defaultextension {
	return [file extension [$win.fsb cget -mask]]
    }
    # cget -initialfile 
    #    Return the currently selected file.
    #
    oncget -initialfile {
	return [$win.fsb get]
    }
    # cget -bufferszie
    #      Get the selected buffersize.
    #
    oncget -buffersize {
	return [$win.size get]
    }
}

