set cfg [open mona.config r]
set nqdc [gets $cfg]

for {set q 0} {$q < $qdc} {incr q} {
#
#    This is ihn both unpacker and reader.
#
    set name [gets $cfg]
    set globalinfo [gets $cfg]
    set thresholds [gets $cfg]
    set params [gets $cfg]

# how to configure a unpacker
    
    if {[info var SpecTclHome] !=  ""} {
	module create $name caev7xx 
	$name config crate [lindex globalinfo 0]
	$name config slot  [lindex globalinfo 1]
	$name config parameters $params
	unpack add $name
    } else {

# how to configure a reader

	module create $name caenv812
	$name config crate [lindex globalinfo 0]
	$name config slot  [lindex globalinfo 1]
	$name config qpedestal [lindex globalinfo 2]
	$name config thresholds $thresholds
	readout add $name
    }
}
close $cfg



