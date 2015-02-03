#
#   Setup the code.

package require configFile




set paramnum 100

#  Process the adcs..
#

set cnum 1
foreach adc [lsort [array names adcConfiguration]] {

    #  Make the parameters, 1d/2d spectra for this adc

    set slot      $adcConfiguration($adc)
    for {set i 0} {$i < 32} {incr i 2} {
	set basename [format det%03d $cnum]

	set name1 $basename.e
	set name2 $basename.t

	parameter $name1 [incr paramnum]
	parameter $name2 [incr paramnum]

	spectrum $name1 1 $name1 {{0 4095 4096}}
	spectrum $name2 1 $name2 {{0 4095 4096}}
	spectrum $basename.TvsE  2 [list $name1 $name2]  {{0 4095 512} {0 4095 515}}

	# Add the parameter mapping for the decoder.

	paramMap $slot $i $name1
	paramMap $slot [expr $i + 1] $name2

	incr cnum
    }
}
sbind -all

#  Add the rates gui ... integrate it with the SpecTcl GUI

set here [info script]
set here [file dirname $here]
source [file join $here ratesGui.tcl]

.gui.b update


