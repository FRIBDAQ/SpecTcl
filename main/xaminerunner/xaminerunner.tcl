
package require SpecTclUtils
package require Tk

wm withdraw .

##
# useMirroring
#   If we need to use mirroring, this is called.
#

proc useMirroring {} {
    puts "Use mirroring"
    set m [Xamine::getMirrorMemory]
    puts "Xamine get mirror memory got $m"
    Xamine::Xamine genenv [lindex $m 0] [lindex $m 1]
    Xamine::Xamine start
    Xamine::connectGates
}


set host [Xamine::getHost]
Xamine::initRestClient $host  [Xamine::getPort] [Xamine::getUser]

close stdin

if {[Xamine::isLocal $host]} {

    # Local SpecTcl
    
    
    
    set m [Xamine::getLocalMemory]
    set key [lindex $m 0]
    set size [lindex $m 1]
    if {[Xamine::Xamine checkmem $key $size]} {
        Xamine::Xamine genenv $key $size
        Xamine::Xamine start
        
    
        Xamine::connectGates
    } else {    
        
        useMirroring
    }
} else {
    # Remote SpecTcl

    useMirroring
}
