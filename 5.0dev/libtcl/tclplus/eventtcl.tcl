package provide EventLoop 2.0

#
#   Encapsulating tcl interpreters in event loops and 
#   supporting socket based connections to interpreters from
#   remote systems.
#
#   This package consists of several snit::type (s)
#   
#   ChannelInterpeter - An object that allows the interpreter
#                       to be run from a channel or channel pair.
#   EventTcl          - Implements an object that allows an ordinary
#                       Tcl interpreter to run an event loop but
#                       remain live to stdin commands.
#  TclServer          - Implements a socket server that creates
#                       ChannelInterpreter's for each connection
#  

package require snit

#
#   ChannelInterpreter - Allows access to an interpreter
#                        from a channel or channel pair.
# OPTIONS:
#   -inchannel        - The input channel
#   -outchannel       - The output channel.  If not specified, the
#                       outchannel is used.
#   -prompt1          - Script to perform tcl_prompt1 handling
#                       if not specified, no prompt is emitted.
#   -prompt2          - Script to perform tcl_prompt2 handling
#                       if not specified, no prompt is emitted.
#  -redirect          - if 1 (default) puts/gets/read are redefined to work
#                       as if the channels are stdin/stdout while
#                       commands are being executed. Otherwise
#                       puts e.g. will output data to the actual stdout.
# METHODS:
#   Only construction/destruction are supported.
#
# EXAMPLE:
#
#    set h [ChannelInterpreter create %AUTO% -inchannel stdin      \
#                                            -outchannel stdout    \
#                                            -prompt1 $tcl_prompt1 \
#                                            -prompt2 $tcl_prompt2]
# 
snit::type ChannelInterpreter {
    typevariable currentOutputChannel
    typevariable currentInputChannel

    option -inchannel ""
    option -outchannel ""
    option -prompt1    ""
    option -prompt2    ""
    option -redirect   1

    variable buffer ""

    constructor args {
	$self configurelist $args

	# We need at least an -inchannel:
	
	if {$options(-inchannel) eq ""} {
	    error "ChannelInterpreter requires an -inchannel option"
	}

	#  Set input buffering on the input channel 
	#  to line mode and set a readable file event.
	#
	fconfigure $options(-inchannel) -buffering line
	fileevent  $options(-inchannel) readable [mymethod onInput]
      
    }
    destructor {
	fileevent $options(-inchannel) readable [list]
    }
    #
    #  Reports EOF conditions on the input channel
    #
    method eof {} {
	return [eof $options(-inchannel)]
    }
    #
    #  Input handler, accumulate data into
    #  the buffer variable and execute it in the global scope
    #  when we have a complete line
    #
    method onInput {} {
	set c $options(-inchannel)

	if {![eof $c]} {
	    set line [gets $c]
	    set continuation 0
	    if {[regexp {\\$} $line]} {
		set line [string range $line 0 end-1]
		set continuation 1
	    } else {
		append line "\n" ;   # put the newline back in.
	    }
	    append buffer $line
	    if {!$continuation && [info complete $buffer]} {
		if {[string trimright $buffer] ne ""} {
		    set redirected 0
		    if {$options(-redirect)} {
			set redirected 1
			$self overrideStdio
		    }
		    set result [catch {uplevel #0 $buffer} msg]

		    # After all the script could reconfigure us.

		    if {$redirected} {
			$self restoreStdio
		    }
		    if {$result} {
			$self output "ERROR - $msg"
		    } else {
			if {$msg ne ""} {
			    $self output $msg
			}
		    }
		}
		set buffer ""
	    }
	    $self prompt
	} else {
	    fileevent $options(-inchannel) readable [list]
	}
    }
    typemethod output {} {
	return $currentOutputChannel
    }
    typemethod input {} {
	return $currentInputChannel
    }

    #----------------------------------------------------------------
    #
    #  Private methods clients should not call these.
    #
    # 
    # Override standard I/O commands
    #
    method overrideStdio {} {
	set currentOutputChannel [$self getOutput]
	set currentInputChannel  $options(-inchannel)

	# Replace puts:

	rename ::puts ::_puts
	proc ::puts {args} {
	    set string [lindex $args end]
	    set nonewline 0
	    set channel ""
	    if {[llength $args] > 1} {
		if {[lindex $args 0] eq "-nonewline"} {
		    set nonewline 1
		    if {[llength $args] == 3} {
			set channel [lindex $args 1]
		    }
		} else {
		    set channel [lindex $args 0]
		}
	    } 
	    if {($channel eq "") || ($channel eq "stdout")} {
		set channel [ChannelInterpreter output]
	    }
	    if {$nonewline} {
		_puts -nonewline $channel $string
		flush $channel
	    } else {
		_puts $channel $string
	    }
	    
	}
	# replace gets:

	rename ::gets ::_gets
	proc ::gets args {
	    if {[lindex $args 0] eq "stdin"} {
		lreplace $args 0 0 [ChannelInterpreter input]
	    }
	    return [uplevel 1 ::_gets $args]
	}

	# replace read:

	rename ::read ::_read
	proc ::read args {
	    if {[llength $args] == 1} {
		if {$args eq "stdin"} {
		    set args [ChannelInterpreter input]
		}
		return [::_read $args]
	    }
	    #  Either read -nonewline channel
	    # or      read channel nchars
	    #
	    set arg1 [lindex $args 0]
	    set arg2 [lindex $args 1]
	    if {$arg1 eq "-nonewline"} {
		# -nonewline channel
		if {$arg2 eq "stdin"} {
		    set arg2 [ChannelInterpreter input]
		}
		return [::_read $arg1 $arg2]
	    }
	    # channel count

	    if {$arg1 eq "stdin"} {
		set arg1 [ChannelInterpreter input]
	    }
	    return [::_read $arg1 $arg2]
	}
	
    }
    # put standard I/O commands back
    #
    method restoreStdio {} {
	rename ::puts {}
	rename ::_puts ::puts

	rename ::gets {}
	rename ::_gets ::gets

	rename ::read {}
	rename ::_read ::read
    }
    #
    #  figure out the output channel
    #
    method getOutput {} {
	set out $options(-outchannel)
	if {$out eq ""} {
	    set out $options(-inchannel)
	}
	return $out
    }
    # Send a string to the output channel with flush.
    #
    method output {string} {
	set out [$self getOutput]
	puts $out $string
	flush $out
    }
 
    # Prompt1:

    method prompt1 {} {
	if {$options(-prompt1) ne "" } {
	    uplevel #0 $options(-prompt1)
	}
    }
    # Prompt2:

    method prompt2 {} {
	if {$options(-prompt2) ne ""} {
	    uplevel #0 $options(-prompt2)
	}
    }
    #  Issue the prompt appropriate to the line buffer state:
    #
    method prompt {} {
	if {$buffer eq ""} {
	    $self prompt1
	} else {
	    $self prompt2
	}
    }


}                               
#
#   EventTcl
#     Encapsulates a ChannelInterpreter for stdin and stdout
#     and provides methods for entering and exiting the Tcl event loop.
#     This provides for a live stdin interpreter while still running the
#     event loop
#      All of methods and variables are implemented as class level
#     rather than object level since as they say in highlander:
#     "There can be only one".. and type methods are much easier to deal
#     with than the singleton pattern.
#
#  TYPE METHODS:
#    start  - Starts the event loop based interpreter.
#    stop
#  NOTES:
#    write traces are established on tcl_prompt1 and tcl_prompt2 so that
#  we can mimic tclsh's behavior with respect to those variables.
#  
#
snit::type EventTcl {
    typevariable  interp "";         # Will be our channel interpreter.
    typevariable  finish 0;          # This is what we vwait on.
    typevariable  afterId "";        # Used to exit if the interp gets an eof.
    typevariable  afterInterval 100; # How often to probe for eof on stdin.    
    #
    #  Start the event loop driven interpreter.
    #  - Create a channel interpretr on stdin/stdout
    #  - Set up the prompting either as specified by tcl_prompt1/tcl_prompt2
    #    or via our default prompters.
    #  - Set traces on the tcl_prompt1/tcl_prompt2 vars so we can
    #    dynamically update the prompting
    #  - vwait on finish to get the loop rolling.
    #
    typemethod start {} {
	
	# Create the channel interpreter if we don't already have one:
	
	if {$interp ne ""} {
	    error "Event loop based tclsh is already runninng"
	}
	set interp [ChannelInterpreter create %AUTO -inchannel stdin -outchannel stdout]
	$interp configure -redirect 0; # No need to redirect for stdin/stdout.
	#
	#  Set up the initial prompting.
	#
	if {[info globals tcl_prompt1] ne ""} {
	    $interp configure -prompt1 $::tcl_prompt1
	} else {
	    $interp configure -prompt1 [list EventTcl defaultPrompt1]
	}
	if {[info globals tcl_prompt2] ne ""} {
	    $interp configure -prompt2 $::tcl_prompt2
	} else {
	    $interp configure -prompt2 [list EventTcl defaultPrompt2]
	}
	# Set traces on tcl_prompt1 and tcl_prompt2
	# so that we can reconfigure our interpreter's prompting as they
	# change.
	trace add variable ::tcl_prompt1 [list write unset] [list EventTcl onPromptChange]
	trace add variable ::tcl_prompt2 [list write unset] [list EventTcl onPromptChange]
	
	# Establish a timed monitor for eofs on the interp.
	
	set afterId [after $afterInterval [list EventTcl checkEof]]

	# Outtput the prompt:

	$interp prompt1

	#
	#  Enter the event loop and stay there until finish is modified.
	#
	vwait [mytypevar finish]
	
	#
	#  Now that the event loop has exited reverse everything we've done
	#
	
	#  stop tracing tcl_prompt1/tcl_prompt2
	
	trace remove variable ::tcl_prompt1 [list write unset] [list EventTcl onPromptChange]
	trace remove variable ::tcl_prompt2 [list write unset] [list EventTcl onPromptChange]
	
	# Destroy the ChannelInterpreter and set interp to "" marking us
	# not running
	
	$interp destroy
	set interp ""
	
	# Cancel the after:
	
	if {$afterId ne ""} {
	    after cancel $afterId
	    set afterId ""
	}
   }
   #
   #  Stop the event loop based interpreter:
   #  - Trigger the end of the vwait and let that take care of everything else:
    typemethod stop {} {
	if {$interp eq ""} {
	    error "EventTcl is not running"
	}
	incr finish    
    }
    #---------------------------------------------------------------------
    #
    #  Private typemethods
    #
    
    
    # Check for eof on interp:
    #
    typemethod checkEof {} {
	if {[$interp eof]} {
	    exit 0
	}
	set afterId [after $afterInterval [list EventTcl checkEof]]
    }
    #  Emit the default interpremter prompt1
    #  This matches the tclsh prompt of "% ".
    #
    typemethod defaultPrompt1 {} {
	set o [$interp cget -outchannel];
	puts -nonewline $o "% "
	flush $o
    }
    #
    #  Emit the default interpreter prompt2
    #  This matches the tclsh prompt of nothing.
    typemethod defaultPrompt2 {} {
    }

    #
    #  Trace called when one of the prompting variables was modified.
    #  There are two possibilities:  The prompting variable was set to
    #  something, in which case we establish that as the new prompt
    #  or the prompting variable was unset in which case we re-establish
    #  our default prompt.
    #
    typemethod onPromptChange {name1 name2 op} {
	#
	# Regularize the name by removing leading ::'s
	#
	set name1 [string trimleft $name1 :]
	#
	#  Set up the default and option based on the variable name:
	#
	if {$name1 eq "tcl_prompt1"} {
	    set option   -prompt1
	    set default  [list EventTcl defaultPromp1]
	} else {
	    set option   -prompt2
	    set default [list EventTcl defaultPrompt2]
	}
	# If the op was write use $name1 as the option string:
	# otherwise use $default
	
	if {$op eq "write"} {
	    set name1 ::$name1
	    set prompter [set $name1]
	} else {
	    set prompter $default
	}
	# Configure the interp:
	
	$interp configure $option $prompter
	
	return ""
    }
}

#
#  TclServer Creates a server that creates a ChannelInterpreter for each
#  connection request.
#
#  OPTIONS:
#    -port        - Tcp/IP port on which connections are listened.
#    -connection  - Script called on connection, if defined.
#                   This is provided to allow the client to implement
#                   some access control scheme. The connection script
#                   is called just like the socket -server connection script
#                   If it returns 1 the connection is allowed, 0, the connection
#                   is closed and no ConnectionInterpreter Created.#
#
#                    The following exmple requires that connections only
#                    come from the localhost (127.0.0.1)
#
#                   proc localOnly {channel clientaddr clientport} {
#                       if {$clientaddr ne "127.0.0.1"} {
#                          return 0
#                       }
#                       return 1
#                   }
#                   TclServer create %AUTO% -port 999 -connection localOnly
#   -redirect     - defaults to 1 supplies the value of the -redirect option
#                   for the ChannelInterpreters created to process clients.
#
#                   
#  METHODS:
#    stop       - Stops honoring new connections.
#                 note that you should probably not destroy the object
#                 but let it continue to destroy ChannelInterpreters
#                 as they get disconnections.
# REQUIREMENTS:
#     The event loop must be running. That can be done however by
#  EventTcl start
#    if you want stdin/stdout to still work as interpreters.
#
snit::type TclServer {
    variable listenSocket "";        # socket I listen on.
    variable servers [list];         # list of servers.
    variable afterId "";             # Id of periodic after reaper.
    variable reapPollInterval 1000;  # ms Between reaper passes.
    
    option -port "";             # listen port
    option -connection [list];   # Connection script.
    option -redirect 1;		 # Value of redirect option for ChannelInterpreters
    
    #
    #  Construct the object.
    #  start listening on the socket and dispatching connection
    #  requests to 
    constructor args {
	$self configurelist $args
	
	if {$options(-port) eq ""} {
	    error "TclServer must have a -port configured"
	}
	
	set listenSocket [socket -server [mymethod onConnection] $options(-port)]
	
	set afterId [after $reapPollInterval [mymethod reap]]
	
    }
    #
    #  About all we can safely do is:
    #  - stop listening.
    #  - Make one more reaping pass
    #  - kill the periodic reaper.
    #  The servers that are still active will be a leak.
    #
    destructor {
	#
	#  In case stop was called:
	#
	if {$listenSocket ne ""} {
	    close $listenSocket
	}

	$self reapServers
	
	
	if {$afterId ne ""} {
	    after cancel $afterId
	}
    }
    
    #  Stop listening for additional connections
    #  The reaper continues to run.
    method stop {} {
	close $listenSocket
	set listenSocket ""
	
    }
    #-----------------------------------------------------------
    #
    #  Private methods:
    #
    
    #  Honor a connection request
    #  If there is a connection script it is called to see if we
    #  should honor the connection.
    #  If the connection can be honored, create a ChannelInterpreter
    #  on the socket and add it to our list.
    #
    #  Parameters:
    #    channel  - Socket connected to peer.
    #    client   - Tcp/IP address of the client.
    #    clientport - port of the client
    #
    method onConnection {channel client clientport} {
	if {$options(-connection) ne ""} {
	    set script $options(-connection)
	    set result [uplevel #0 $script $channel $client $clientport]
	    if {!$result} {
		close $channel
		return
	    }
	}
	
	#  The channel can be connected to an interpreter.
	
	set interp [ChannelInterpreter create %AUTO% -inchannel $channel \
                                       -redirect $options(-redirect)]
	lappend servers $interp
    }
    
    #  Make a pass through the servers list.
    #  If a server is at eof:
    #  close its channel
    #  destroy it
    #  remove it from the list.
    #  In order to not screw up the foreach loop, dead servers are first
    #  marked and then searched/removed.
    #
    method reapServers {} {
	
	# Mark the dead ones
	#
	set dead [list]
	foreach server $servers {
	    if {[$server eof]} {
		lappend dead $server
	    }
	}
	# Kill/remove them:
	
	foreach server $dead {
	    set socket [$server cget -inchannel]
	    $server destroy
	    close $socket
	    
	    set index [lsearch -exact $servers $server]
	    
	    if {$index != -1} {
		set servers [lreplace $servers $index $index]
	    }
	}
	
    }
    #
    #  Periodic reaper:
    #
    method reap {} {
	$self reapServers
	set afterId [after $reapPollInterval [mymethod reap]]
    }
    
}