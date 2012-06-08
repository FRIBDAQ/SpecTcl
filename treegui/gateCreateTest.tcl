lappend auto_path .
package require gateCreate

gateCreate .c -gates [list a b c.d c.e {$abc} {c d e}] -createcmd [list create %G %T %D]
pack .c

proc create {name type definition} {
    puts "Create gate $name of type $type defined as: $definition"
}