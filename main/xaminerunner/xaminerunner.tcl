
package require SpecTclUtils
package require Tk

wm withdraw .


set host [Xamine::getHost]

if {[Xamine::isLocal $host]} {

    # Local SpecTcl
    
    Xamine::initRestClient $host  [Xamine::getPort]
    
    set m [Xamine::getLocalMemory]
    Xamine::Xamine genenv [lindex $m 0] [lindex $m 1]
    Xamine::Xamine start
    
    #after 1500;    # Wait for Xamine to be alive.

    Xamine::connectGates
    
    
    close stdin
} else {
    # Remote SpecTcl

    puts stderr "Remote SpecTcl is not yet supported"
    exit -1
}
