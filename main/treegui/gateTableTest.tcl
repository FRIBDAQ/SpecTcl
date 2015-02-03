set gateList [list \
{contour_000 0 c {{event.raw.00 event.raw.01} {310.000000 816.333374} {516.666687 744.000000} {754.333374 713.000000} {485.666687 496.000000}}} \
{slice_002 1 gs {{306.000000 647.000000} {event.raw.00 event.raw.01 event.raw.02 event.raw.03 event.raw.04 event.raw.05 event.raw.06 event.raw.07 event.raw.08 event.raw.09}}} \
{slice_004 2 s {event.raw.00 {327.000000 534.000000}}} ]

lappend auto_path .
package require gateTable

gateTable .t -height 25 -gates $gateList 
pack .t -expand 1 -fill x

