This directory provides a script that allows you to set up playback
of a number of event files automatically.   The script produces a widget 
that consists of:
 - A list box with extended select mode, the list box will be stocked
   with files that match a search mask (e.g. ~/eventfiles/*.evt).
 - An entry containing a glob file  search mask that can be altered.
 - Control buttons:
   o Ok  Accepts the selected files and constructs a pipe data source
     of the cat command operating on all selected event files.
   o Cancel drops the dialog and does nothing.
   o Filter Reloads the list box with files that match the 
     glob filter pattern in the filter entry.

To use (for example):
  1. Source selectscript.tcl into your SpecTcl in e.g. SpecTclRC.tcl
  2. Create and pack a GUI button that when clicked invokes:
     MultiRead mypattern
  
     e.g.:
       button .multiread -text "Playback Multiple" \  
                         -command "MultiRead /user/analysis/myexp/event/*.evt"
       pack   .multiread

Ron Fox August 5, 2002
NSCL
Michigan State University
East Lansing, MI 48824-1321
fox@nscl.msu.edu
