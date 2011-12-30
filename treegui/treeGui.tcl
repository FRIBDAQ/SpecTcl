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

#
# Build the top level and a notebook:
#
toplevel .treegui
set treenotebook [ttk::notebook .treegui.notebook]

#
#  Now builds and load the widgets into the  notebook:
#


# First the MC Rewrite widgets:

spectrumTabActions spectrum_tab -widget $treenotebook.spectra
$treenotebook add $treenotebook.spectra -text Spectra -sticky new

parametersTabActions parametesr -widget $treenotebook.parameters
$treenotebook add $treenotebook.parameters -text Parameters -sticky new

variableTabActions variables -widget $treenotebook.variables
$treenotebook add $treenotebook.variables -text Variables -sticky new

gateTabActions gates -widget $treenotebook.gates
$treenotebook add $treenotebook.gates -text Gates -sticky new

#  Now the folder gui as a new tab:

::FolderGui::startFolderGui .treegui $treenotebook
$treenotebook add $::FolderGui::folderGuiBrowser -text Folders -sticky nsew

pack $treenotebook -expand 1 -fill both
