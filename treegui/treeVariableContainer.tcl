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
package require treeVariableEditor

package provide treeVariableContainer 1.0

##
# Contains tree variables editors and the load/save strip at the bottom of that widget.
#
# OPTIONS:
#   -variables - Delegated to treeVariableEditor.
#   -lines     - Delegated to treeVariableEditor.
#   -current   - Delegated to treeVariableEditor.
#   -selectcmd - Delegated to treeVariableEditor.
#

snit::widget treeVariableContainer {
    hulltype ttk::frame

    delegate option -variables to editors
    delegate option -lines     to editors
    delegate option -current   to editors
    delegate option -selectcmd to editors
    delegate option -loadcmd   to editors
    delegate option -setcmd    to editors
    delegate option -array     to editors

    delegate method loadEditor to editors
    
    ##
    # install the components and lay them out.
    #
    # @param args - option value sets that determine the initial configuration of the 
    #               widget.
    #
    constructor args {

	install editors as treeVariableEditor $win.editors

	$self configurelist $args


	grid $win.editors
	
    }
}
