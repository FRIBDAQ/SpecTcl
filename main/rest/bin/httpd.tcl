#!/bin/sh
#
# Tcl HTTPD
#
# This is the main script for an HTTP server. 
# To test out of the box, do
# tclsh httpd.tcl -debug 1
# or
# wish httpd.tcl -debug 1
#
# For a quick spin, just pass the appropriate settings via the command line.
# For fully custom operation, see the notes in README_custom.
#
# A note about the code structure:
# httpd.tcl	This file, which is the main startup script.  It does
#		command line processing, sets up the auto_path, and
#		loads tclhttpd.rc and httpdthread.tcl.  This file also opens
#		the server listening sockets and does setuid, if possible.
# tclhttpd.rc	This has configuration settings like port and host.
#		It is sourced one time by the server during start up
#		before command line arguments are processed.
# httpdthread.tcl	This has the bulk of the initialization code.  It is
#		split out into its own file because it is loaded by
#		by each thread: the main thread and any worker threads
#		created by the "-threads N" command line argument.
# ../lib	The script library that contains most of the TclHttpd
#		implementation
# ../tcllib	The Standard Tcl Library.  TclHttpd ships with a copy
#		of this library because it depends on it.  If you already
#		have copy installed TclHttpd will attempt to find it.
#
# TclHttpd now requires Tcl 8.0 or higher because it depends on some
#	modules in the Standard Tcl Library (tcllib) that use namespaces.
#	In practice, some of the modules in tcllib may depend on
#	new string commands introduced in Tcl 8.2 and 8.3.  However,
#	the server core only depends on the base64 and ncgi packages
#	that may/should be/are compatible with Tcl 8.0
#
# Copyright (c) 1997 Sun Microsystems, Inc.
# Copyright (c) 1998-2000 Scriptics Corporation
# Copyright (c) 2001-2002 Panasas
#
# See the file "license.terms" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# RCS: @(#) $Id: httpd.tcl,v 1.55 2004/05/19 05:44:28 welch Exp $
#
# \
exec tclsh "$0" ${1+"$@"}





if {$Config(debug)} {
    puts stderr "auto_path:\n[join $auto_path \n]"
    if {[catch {package require httpd::stdin}]} {
	puts "No command loop available"
	set Config(debug) 0
    }
}

if {$Config(compat)} {
    if {[catch {package require httpd::compat}]} {
	puts stderr "tclhttpd$Config(compat) compatibility mode failed."
    } else {
	# Messages here just confuse people
    }
}




