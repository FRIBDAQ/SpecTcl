#
#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2005.
#
#    You may use this software under the terms of the GNU public license
#    (GPL).  The terms of this license are described at:
#
#     http://www.gnu.org/licenses/gpl.txt
#
#     Author:
#             Ron Fox
#            NSCL
#            Michigan State University
#            East Lansing, MI 48824-1321
#
#
package provide guiutilities 1.0

#  Central, commonly used utility procs.

# pathToName name
#     Convert a browser path to an item name by
#     removing the first element of the path.
# Parameters:
#   name  - the item name.
#
proc pathToName {name} {
    set path [split $name .]
    set path [lrange $path 1 end]
    set result [join $path .]
    return $result
}
# setEntry widget text
#     Set the contents of an entry widget.
# Parameters:
#   widget   - Widget name/path.
#   text     - Text to load into the entry.
proc setEntry {widget text} {
    $widget delete 0 end
    $widget insert end $text
}
