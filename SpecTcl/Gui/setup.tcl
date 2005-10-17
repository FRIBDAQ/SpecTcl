# Test setup file that will create  a bunch of spectra.

# Parameter that is not:

parameter george 50
for {set i 60} {$i < 70} {incr i} {
    parameter not.tree.parameter.$i $i
}

# 1-d spectra:

foreach parameter [parameter -list] {
    set name [lindex $parameter 0]
    spectrum $name 1 $name {{0 1023 1024}}
}

# 2-d spectra:

foreach {i j} {00 01 02 03} {
    spectrum event.raw.$i-vs-$j 2 "event.raw.$i event.raw.$j" {{0 1023 256} {0 1023 256}}
}
spectrum not.tree.2d 2 {not.tree.parameter.60 not.tree.parameter.61} {{0 1023 256} {0 1023 256}}

# Gamma 1/2 spectra

foreach i {00 01 02 03 04 05 06 07 08 09} {
    lappend g1params event.raw.$i
}
spectrum event.raw.gammas g1 $g1params {{0 1023 1024}}
spectrum event.raw.gcor   g2 $g1params {{0 1023 256} {0 1023 256}}
spectrum event.raw.summary s $g1params {{0 1023 10234}}

unset g1params
foreach i {60 61 62 63 64  65 66 67 68 69} {
    lappend g1params not.tree.parameter.$i
}
spectrum not.tree.parameter.gammas g1 $g1params {{0 1023 1024}}
spectrum not.tree.parameter.gcor   g2 $g1params {{0 1023 256} {0 1023 256}}
spectrum not.tree.parameter.summary s $g1params {{0 1023 1024}}

#  Bitmask spectrum.

spectrum not.tree.parameter.bits b not.tree.parameter.60 {{0 31 32}}

# Strip chart spectrum

parameter time 100
set clocktime 0
proc updatetime {} {
    global clocktime
    after 1000 updatetime
    incr clocktime
}
pseudo time {} {
    global clocktime
    return $clocktime
}

spectrum chart S {time event.raw.00} {{0 20481 20480}}

# spectrum not.tree.parameter.stripchart S {time not.tree.parameter.60} {{0 1023 1024}}

gate &mask   am {not.tree.parameter.60 0xa5a5a5}
gate ==mask  em {not.tree.parameter.61 0x5a5a5a}
gate !=mask  nm {not.tree.parameter.62 0x111111}


gate slice s {event.raw.00 {100 200}}
gate aband b {event.raw.00 event.raw.01 {
                                {115.886719 775.242188}
                                {171.832031 619.394531}
                                {459.550781 455.554688}
                                {819.199219 451.558594}
                                {859.160156 383.625000}}}
gate a.second.band b {event.raw.00 event.raw.01 {
                                {39.960938 439.570312}
                                {367.640625 255.750000}
                                {747.269531 243.761719}
                                {915.105469 227.777344}}}
gate invisible  b {event.raw.00 event.raw.02 {
                                   {39.960938 439.570312}
                                   {367.640625 255.750000}
                                   {747.269531 243.761719}
                                   {915.105469 227.777344}}}

gate acontour c {event.raw.02 event.raw.03 {
                                {115.886719 775.242188}
                                {171.832031 619.394531}
                                {459.550781 455.554688}
                                {819.199219 451.558594}
                                {859.160156 383.625000}}}

gate and * {acontour aband slice}
gate or  + {aband slice acontour}
gate not - {slice}

gate constants.true T {}
gate constants.false F {}

gate gslice gs {{453.55 546.46}
                    {event.raw.00 event.raw.01 event.raw.02
                    event.raw.03 event.raw.04 event.raw.05
                    event.raw.06 event.raw.07 event.raw.08 event.raw.09}}
gate gband gb {{{239.765625 543.468750}
               {583.429688 447.562500}
               {867.152344 415.593750}}
                {event.raw.00 event.raw.01 event.raw.02 event.raw.03
                event.raw.04 event.raw.05 event.raw.06 event.raw.07
                event.raw.08 event.raw.09}}
gate gcontour gc {{{359.648438 611.402344}
                   {291.714844 335.671875}
                   {751.265625 243.761719}
                   {827.191406 735.281250}}
                    {event.raw.00 event.raw.01 event.raw.02 event.raw.03 event.raw.04
                    event.raw.05 event.raw.06 event.raw.07 event.raw.08 event.raw.09}}

fold -apply gcontour event.raw.gammas
fold -apply gslice   event.raw.gcor


apply slice event.raw.01 george

sbind -all

lappend auto_path [file dirname [info script]]
package require SpecTclGui


filter test slice event.raw.00
filter -file ./test.flt test
filter -enable test

filter second or {event.raw.00 event.raw.01 event.raw.02}
filter -file ./test2.flt second

filter third and {event.raw.03 event.raw.04 event.raw.05 event.raw.06}
filter -file ./test3.flt third

filter fourth not george

