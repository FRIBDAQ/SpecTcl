package provide SpecTclHttpdServer 1.0


# Server configuration.

namespace eval SpecTcl {
    variable here [file dirname [info script]]
    variable lib [file normalize [file join $here lib]]
}
lappend auto_path $::SpecTcl::lib
package require md5
package require httpd 1.6
package require httpd::version
package require httpd::utils
package require httpd::counter
package require httpd::config



## 
# Init and start the server:
#

proc Config {item value} {
    global Config
    set Config($item) $value
}
proc startSpecTclHttpdServer {port} {
global Config




set v 3.5.1



Config home [file normalize [file join $::SpecTcl::here bin]]
Config config /dev/null
Config virtual {}
Config host [info hostname]
Config port $port
Config https_host "";		# Not using https for now.
Config https_port ""
Config ipaddr ""
Config https_ipaddr ""


Config docRoot [file join $::SpecTcl::here  htdocs]
Config library [file join $::SpecTcl::here  custom]; # Tcl files auto-sourced at httpd startup.
Config main    [file normalize [file join $::SpecTcl::here bin httpdthread.tcl]]; #  Startup script for configured server.
Config uid     "";				      # Run as me.
Config gid     ""
Config lib     $::SpecTcl::lib; # W::SpecTcl::here the tclhttpd libraries live.

Config debug   0
Config compat 3.3

Config webmaster "";		# No webmaster for this
Config secsPerMinute 60
Config secs    $Config(secsPerMinute)

Config threads 0
Config gui     0;		# Don't want the damned gui.

##
#  Logging
#    - Need to know how to turn this off
#
Config LogFile [file join [pwd] tclhttp.log]
Config LogFlushMinuts 1
Config LogDebug       1
Config CompressProg   gzip;	# assume in path.

Config MaxFileDescriptors 256
Config limit default

##
# SSL Configuration:  - Should be disabled above so should not matter.

Config SSL_REQUEST 0
Config SSL_REQUIRE 0
Config SSL_CADIR    ""
Config SSL_CERTFILE ""
Config SSL_KEYFILE ""
Config USE_SSL2     1
Config USE_SSL3     1
Config USE_TLS1     0
Config SSL_CIPHERS ""



# Authentication Configuration

# The auth config creates a default authentication array.
# To create passwords for user $u and group $g
# Config Auth {user,$u $password group,$g group}
# Note: these passwords are plaintext, so it's not very secure.
#
# If Auth is undefined, a random password
# is created and logged per invocation of the server
# If Auth is defined (even if empty, it is considered authoritative and
# no default passwords are generated.
Config Auth {}

# Default password file - used if .htaccess doesn't specify AuthUserFile
# this defaults to the authentication array authdefault()
#Config AuthUserFile {}

# Default group file - used if .htaccess doesn't specify AuthGroupFile
# this defaults to the authentication array authdefault()
#Config AuthGroupFile {}

# Default mail servers - the smtp servers to use when sending mail
Config MailServer {}

config::init $Config(config) Config


unset ::SpecTcl::here
unset ::SpecTcl::lib



if {$Config(debug)} {
    puts stderr "auto_path:\n[join $::auto_path \n]"
    if {[catch {package require httpd::stdin}]} {
	puts "No command loop available"
	set Config(debug) 0
    }
}

if {$Config(compat)} {
    if {[catch {package require httpd::compat}]} {
	puts stderr "tclhttpd$Config(compat) compatibility mode failed."
    } else {
	# Messages ::SpecTcl::here just confuse people
    }
}




puts "Starting $Config(port) $Config(host)"




Httpd_Init


Httpd_Server $Config(port) $Config(host) $Config(ipaddr)

if {[catch {source $Config(main)} message]} then {
    global errorInfo
    set error "Error processing main startup script \"[file nativename $Config(main)]\"."
    append error "\n$errorInfo"
    error $error
}



# vwait forever
}
proc Thread_Enabled {} {return 0}
proc Thread_Respond {args} {return 0}

##
#
#  Locate a port on which nobody is listening by incrementally searching from base on up
#
proc ::SpecTcl::findFreePort {base} {
    set host [info host]
    while {1} {
	if {[catch {socket $host $base} fd]} {
	    return $base
	} else {
	    close $fd;		# Someone there.
	    incr base;		#  Try the next port.
	}
    }
}