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

#
#  Script to start the tree paramter gui in a tabbed notbook.
#

lappend auto_path [file join $SpecTclHome TclLibs] [file join $SpecTclHome Script]

#
#  Load the pacakges that make up the new mcgui:
#

package require variableTabActions
package require parametersTabActions
package require gateTabActions
package require spectrumTabActions
package require SpecTclGui
package require treeUtilities

##
# updateParameterMenus
#
#    Trace called when there's a change to the parameter list:
#
# @param op -- operation performed, add or delete - don't care
# @param which -- parameter added or deleted.
#
proc updateParameterMenus {op which} {
    set params [::treeutility::parameterList]
    $::treenotebook.parameters configure -parameters $params;      # refresh tree parameter tab.
    $::treenotebook.spectra configure -parameters $params;         # Spectra menubuttons too.
}

#-----------------------------------------------------------------------------
#  Package entry point

if {![winfo exists .treegui]}  {	# Don't do it twice!
    #
    # Build the top level and a notebook:
    #
    toplevel .treegui
    set treenotebook [ttk::notebook .treegui.notebook]
    
    #
    #  Now builds and load the widgets into the  notebook:
    #
    
    
    # First the MC Rewrite widgets:
    set time [time {
	spectrumTabActions spectrum_tab -widget $treenotebook.spectra} 1
	      ]
    $treenotebook add $treenotebook.spectra -text Spectra -sticky nsew

    set time [time {
        parametersTabActions parameters -widget $treenotebook.parameters} 1
	]

    $treenotebook add $treenotebook.parameters -text Parameters -sticky nsew
    
    set time [time {
	variableTabActions variables -widget $treenotebook.variables} 1
	      ]

    $treenotebook add $treenotebook.variables -text Variables -sticky nsew

    set time [time {
	gateTabActions gates -widget $treenotebook.gates} 1
	      ]

    $treenotebook add $treenotebook.gates -text Gates -sticky nsew
    
    #  Now the folder gui as a new tab:
    
    set time [time {
	::FolderGui::startFolderGui .treegui $treenotebook} 1
	  ]

    $treenotebook add $::FolderGui::folderGuiBrowser -text Folders -sticky nsew
    
    grid $treenotebook -sticky nsew
    grid $::FolderGui::folderGuiStatusFrame -sticky sew
    
    grid columnconfigure .treegui all -weight 1
    grid rowconfigure    .treegui  0   -weight 1
    grid rowconfigure    .treegui  1   -weight 0
    
    #   Set traces on parameter changes so that we can  update their menus:
    
    parameter -trace [list updateParameterMenus]
}