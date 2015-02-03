lappend auto_path .
package require gateSelBar

gateSelBar .g -updatecmd [list callback update %M] -maskcmd [list callback maskchanged %M]
pack .g


proc callback {name mask} {
    puts "$name callback invoked mask is now $mask"
}
