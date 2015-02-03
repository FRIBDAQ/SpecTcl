#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2009.
#
#    You may use this software under the terms of the GNU public license
#    (GPL).  The terms of this license are described at:
#
#     http://www.gnu.org/licenses/gpl.txt
#
#    Author:
#             Ron Fox
#	     NSCL
#	     Michigan State University
#	     East Lansing, MI 48824-1321

package require Tk
package require snit
package require gateCreate
package require gateDeleteBar
package require gateTable
package require gateSelBar



package provide gateContainer 1.0


##
#  This provides a snit megawidget that encapsulates the gate tab.
#  The main purpose of this class is to encapsulate behavior betweeen
#  the widgets while only exporting callbacks that require interaction
#  with SpecTcl (the model) back to the controller class.
#
#  Layout is just a simple top to bottom one;
#   +----------------------------------+
#   |      gateCreate widget           |
#   +----------------------------------+
#   |    gateDeleteBar                 |
#   +----------------------------------+
#   |    gateTable widget.             |
#   +----------------------------------+
#   |   gateSelBar                     |
#   +----------------------------------+
#
#  OPTIONS:
#
# Delegated to the gateSelBar :
#     -updatecmd   - Script to run when the update gates list button is clicked.
#     -maskcmd     - Script to run when the mask has changed.
# 
#       For both of these %M substitutes with the current value of the mask.
#
# Delegated to the gateTable:
#      -gates     - Returns/sets the gates in the table.
#      -command   - Gate double clicked %N substitutes for the gate name clicked.
#
# Delegated to the gateDeleteBar:
#
#     - deleteselected - script invoked when the "Delete Selected" button is clicked.
#     - deleteall      - script invoked when the Delete All button is clicked.
#
# Delegated to gateCreate:
#     -createcmd - Script invoked when the create/replace button is clicked.
#                    %G %T %D substitutions as documented in gateCreate.tcl
#     -menugates  - Delegated to -gates; Used to populate the gate dropdown.
#                   this normally is populated with all of the gates rather than
#                   just the masked set in the table.
#     -type       - gate type (SpecTcl)
#     -gatename   - Gate name
#     -definition - Gate definition.
#     -typename   - Gate typename
#
#  METHODS:
#
# Delegated to the gateTable
#
#      getsel    - Returns the gates that are selected.
#
# AUTONOMOUS ACTIONS:
#  Double clicking a gate in the gateTable widget loads its definition into the
#  gateCreate widget fields.
#
snit::widget gateContainer {
    hulltype ttk::frame

    # Options handled by the gateSelBar widget:

    delegate option -updatecmd to gatemask
    delegate option -maskcmd   to gatemask
    delegate option -mask      to gatemask

    # Options handled by the gate table widget:

    delegate option -gates  to gatetable
    delegate option -command to gatetable

    # Options handled by the delete bar:

    delegate option -deleteselected to gatedelete
    delegate option -deleteall      to gatedelete

    # Options delegated to the gate creator:

    delegate option -createcmd  to gatecreate
    delegate option -menugates  to gatecreate as -gates
    delegate option -gatename   to gatecreate
    delegate option -definition to gatecreate
    delegate option -type       to gatecreate
    delegate option -typename   to gatecreate

    delegate option * to gatetable; # remaining opts configure the table.

    # Methods delegated to gateTable:

    delegate method getsel to gatetable
    delegate method * to hull


    ##
    # Construct the widget.
    # We must construct and layout the widget prior to processing options.
    #
    # @param args option name value pairs list.
    #
    constructor args {
	install  gatemask  using gateSelBar $win.gatesel
	install  gatetable using gateTable $win.gatetable
	install  gatedelete using gateDeleteBar $win.gatedelete
	install  gatecreate using gateCreate $win.gatecreate


	grid $win.gatecreate -sticky ewn
	grid $win.gatedelete -sticky ewn
	grid $win.gatetable  -sticky nsew
	grid $win.gatesel    -sticky ews
	
	grid columnconfigure $win all -weight 1
	grid rowconfigure    $win 0   -weight 0
	grid rowconfigure    $win 1   -weight 0
	grid rowconfigure    $win 2   -weight 1
	grid rowconfigure    $win 3   -weight 0


	$self configurelist $args
    }



}
