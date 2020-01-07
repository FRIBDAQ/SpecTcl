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
# @file dialogwrapper.tcl
# @brief Simplify the creation of modal dialogs.
# @author Ron Fox <fox@nscl.msu.edu>
#
package require Tk
package require snit

package provide dialogwrapper 1.0

#-------------------------------------------------------------------------------
#  Dialogs:  These take the forms above and integrate them with a dialog
#            that has Ok/Cancel buttons.
#            Each dialog is a pair:  A snit megawidget that displays the dialog
#            and a convenience routine that returns the output of the dialog.
#

##
# DialogWrapper
#   Wraps a widget in a dialog container this includes:
#   *  OK/Cancel buttons located below the widget.
#   *  Method to become application modal.
#   *  Delegation of all unrecognized options and methods to the contained
#      widget<methodsynopsis>
# OPTIONS
#  -form    - Sets the widget that will appear in the control area.
#  -showcancel - Determines if the widget displays the cancel button.  If
#                false, only the Ok button is displayed.
# METHODS
#  controlarea - Returns the parent that should be used when creating the
#                -form widget.
#  modal    - grabs events and blocks until either OK or Cancel is clicked.
#             Returns either Ok, Cancel or Destroyed to indicate what
#             caused the exit from modality.
#
snit::widgetadaptor DialogWrapper {
    component controlarea;                 # The wrapped widget.

    option -form -default ""  -configuremethod _setControlArea
    option -showcancel -default true   -configuremethod _showCancelConfig
    delegate option * to controlarea
    delegate method * to controlarea
    
    variable action "";               # Will contain the event that ended modality.
    
    ##
    # constructor
    #   lays out the generic shape of the dialog and fills in the
    #   action area.  The dialog is of the form:
    #
    #  +-----------------------------------+
    #  |  frame into which controlarea     | (control area)
    #  |  is put                           |
    #  +-----------------------------------+
    #  | [OK]   [Cancel]                   | (action area).
    #  +-----------------------------------+
    #
    constructor args {
        installhull using ttk::frame
        
        ttk::frame $win.controlframe -relief groove -borderwidth 2
        ttk::frame $win.actionframe
        ttk::button $win.actionframe.ok     -text Ok
        ttk::button $win.actionframe.cancel -text Cancel
        
        grid $win.actionframe.ok $win.actionframe.cancel
        grid $win.controlframe -sticky nsew
        grid $win.actionframe

        grid rowconfigure $win 0 -weight 1       
        grid columnconfigure $win 0 -weight 1       
        grid rowconfigure $win.controlframe 0 -weight 1       
        grid columnconfigure $win.controlframe 0 -weight 1       
 
        $self configurelist $args
    }
    ##
    # destructor
    #   Ensure there are no callback handlers left:
    #
    destructor {
        catch {$win.actionframe.ok     configure -command [list]}
        catch {$win.actionframe.cancel configure -command [list]}
        catch {$win bind <Destroy> [list]}
        
    }
    #-------------------------------------------------------------------------de
    #  Public methods
    
    ##
    # controlarea
    #
    # @return widget path - the parent of any -form widget.
    #
    method controlarea {} {
        return $win.controlframe
    }
    ##
    # modal
    #   Enters the modal state:
    #   *   Adds handlers for the buttons and destroy event.
    #   *   grabs events to $win
    #   *   vwaits on the action var.
    #   *   When the vwait finishes, kills off the handlers.
    #   *   Returns the contents of the action variable which will have been
    #       set by the action handler that fired.
    #
    # @return string the action that ended the wait
    #        * Ok - the Ok button was clicked.
    #        * Cancel - the Cancel button was clicked.
    #        * Destroy - The widget is being destroyed.
    #
    method modal {} {
        $win.actionframe.ok configure      -command [mymethod _setAction Ok]
        $win.actionframe.cancel configure -command  [mymethod _setAction Cancel]
        bind $win <Destroy>                         [mymethod _setAction Destroyed]
        
        # Here's the modal section.
        
        grab set $win
        vwait [myvar action]
        catch {grab release $win};         # catch in case we're being destroyed.
        
        #  Catches here in case the windows being configured have been
        #  destroyed...
        
        catch {$win.actionframe.ok configure -command [list]}
        catch {$win.actionframe.cancle configure -command [list]}
        catch {bind $win <Destroy> [list]}
        
        if {[info exists action]} {
            return $action
        } else {
            return "Destroyed";                # Destruction too far in process
        }
    }
    
    #-------------------------------------------------------------------------
    # Configuration handlers:
    
    ##
    # _setControlArea
    #
    #   Option handler for the -form configuration option.  This sets the
    #   widget that will be contained in the control area.
    #
    # @param optname - Name of the option being configured (-form)
    # @param widget  - Widget path to the object that should be pasted into the form.
    #
    # @note - The form is installed as the controlarea component so that options
    #         and methods can be delegated to it.
    #
    method _setControlArea {optname widget} {
        install controlarea using set widget
        grid $widget -in $win.controlframe -sticky nsew
        
        set options($optname) $widget
    }
    ##
    # _showCancelConfig
    #    Modify the value of the -showcancel option.
    #
    # @param optname - option name.
    # @param value   - new value.
    #
    method _showCancelConfig {optname value} {
        set old $options($optname)
        set options($optname $value)
        
        if {$old != $value} {
            if {$value} {
                grid $win.actionframe.cancel -row 0 -column 1
            } else {
                grid forget $win.actionframe.cancel
            }
        }
    }
    #---------------------------------------------------------------------------
    #  Private action handlers.
    
    ##
    # _setAction
    #   Set the action variable to a specific value.
    #
    method _setAction value {
        set action $value
    }
}