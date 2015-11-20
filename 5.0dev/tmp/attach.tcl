lappend auto_path /usr/opt/spectcl/5.0/lib
package require spectcl
package require catdecoder
spectcl::pipline set decoder cat
spectcl::attach file://../input.evt
exit
