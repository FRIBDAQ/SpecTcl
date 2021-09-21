
package require SpecTclUtils
package require Tk

wm withdraw .

Xamine::initRestClient [Xamine::getHost] [Xamine::getPort]
set m [Xamine::getLocalMemory]
Xamine::Xamine genenv [lindex $m 0] [lindex $m 1]
Xamine::Xamine start

#after 1500;    # Wait for Xamine to be alive.

Xamine::connectGates
