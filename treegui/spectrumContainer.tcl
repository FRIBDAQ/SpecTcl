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
package require spectrumManipulation
package require spectrumMaskWidget
package require spectrumTable


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

    # Spectrum mask options:

    delegate option -mask     to mask
    delegate option -updatecmd to mask

    # Spectrum table options and methods.

    delegate option -spectra   to table
    delegate option -selectcmd to table
    delegate method getSelection to table

    # Spectrum manipulation options

    delegate option -all       to spectrum
    delegate option -clearcmd  to spectrum
    delegate option -deletecmd to spectrum
    delegate option -dupcmd    to spectrum
    delegate option -ungatecmd to spectrum

    ##
    # Construction is just  installing and laying out the components.
    #
    # @param args - configuration option/names.
    #
    constructor args {
	install fileio   using definitionFileWidget $win.fileio
	install table    using spectrumTable        $win.table -height 15
	install mask     using spectrumMaskWidget   $win.mask
	install spectrum using spectrumManipulation $win.spectrum

	grid $fileio   -sticky ew
	grid $spectrum -sticky ew
	grid $table    -sticky ew
	grid $mask     -sticky ew

	grid columnconfigure $win all -weight 1

	$self configurelist $args

    }
}
