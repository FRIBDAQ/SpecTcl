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

#  This is the toplevel of the new SpecTcl GUI... loosely based on the
#  capabilities, but not the structure of the old treeparmaeter GUI
#  written by D. Bazin.

lappend auto_path [file dirname [info script]]

package require SpecTclGui

::FolderGui::startFolderGui
