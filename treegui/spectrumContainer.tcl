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
package require definitionFileWidget

package provide spectrumContainer 1.0

##
# Provides the container for the entire 
# spectrum maniuplation tab.  This is a megawidget that is laid out as follows:
#
# +--------------------------------------------------------------------+
# |  spectrumType widget                       | definitionFile Widget |
# +--------------------------------------------------------------------+
#
# OPTIONS:
#   All options for all components are exposed as is unless otherwise noted.
#
#  METHODS:
#   All public component methods are exposed as is unless otherwise noted.
#

snit::widget spectrumContainer {
    hulltype ttk::frame

    # definitionFileWidget  options:

    delegate option -filename     to fileio
    delegate option -accumulate   to fileio
    delegate option -makefailsafe to fileio
    delegate option -loadcmd      to fileio
    delegate option -savecmd      to fileio

    ##
    # Construction is just  installing and laying out the components.
    #
    # @param args - configuration option/names.
    #
    constructor args {
	install fileio using definitionFileWidget $win.fileio
	grid $win.fileio

	$self configurelist $args
    }
}
