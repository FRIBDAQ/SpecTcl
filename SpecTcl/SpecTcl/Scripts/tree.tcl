# *************************************************************************
# tree.tcl
# by Chase Evan Bolen
# tree data structure using tcl arrays
# I forgot the normal tree terminology (its been a while), so bear with me.
# *************************************************************************
proc newchild {name {value {}}} {
    upvar $name tree
    if ![info exists tree(point)] {
	set tree(point) 1
	set tree($tree(point),parent) NULL
	set tree(count) 1
	set tree(root) 1
	set newchild $tree(point)
	set tree($tree(point),children) 0
    } else {
	set newchild [incr tree(count)]
	set tree($newchild,children) 0
	set tree($newchild,parent) $tree(point)
	if {[numchild tree] == 0} {
	    set tree($tree(point),children) $newchild
	} else {
	    append tree($tree(point),children) " $newchild"
	}
    }
    set tree($newchild) $value

    return 0
}

proc numchild {name} {
    upvar $name tree
    if ![info exists tree(point)] {
	return -1
    } else {
	if {$tree($tree(point),children) != 0} {
	    return [llength $tree($tree(point),children)]
	} else {
	    return 0
	}
    }
}
proc movedown {name child} {
    upvar $name tree
    if ![info exists tree(point)] {
	return -1
    } else {
	if {$child > [numchild tree]} {
	    return -1
	} else {
	    set newpoint [lindex $tree($tree(point),children) [expr {$child - 1}]]
	    set tree(point) $newpoint
	}
    }
    return 0
}
proc getval {name} {
    upvar $name tree
    if ![info exists tree(point)] {
	return NULL
    } else {
	return $tree($tree(point))
    }
}
proc moveup {name} {
    upvar $name tree
    if {![info exists tree(point)] || [expr {$tree(point) == $tree(root)}]} {
	return -1
    } else {
	set tree(point) $tree($tree(point),parent)
    }
    return 0
}

proc setroot {name} {
    upvar $name tree
    if ![info exists tree(point)] {
	return -1
    } else {
	set tree(point) $tree(root)
    }
    return 0
}
proc setval {name value} {
    upvar $name tree
    if ![info exists tree(point)] {
	return -1
    } else {
	set tree($tree(point)) $value
    }
    return 0
}
# Deletes current child and all below.
proc deletechild {name} {
    upvar $name tree  
    if ![info exists tree(point)] {
	return -1
    } else {
	if {[numchild tree] == 0} {
	    set point $tree(point)
	    moveup tree
	    set i [lsearch -exact $tree($tree(point),children) $point]
	    set tree($tree(point),children) [lreplace $tree($tree(point),children) $i $i]
	    unset tree($point)
	    unset tree($point,children)
	    unset tree($point,parent)
	
	} else {
	    for {set i 1} {$i <= [numchild tree]} {incr i} {	
		movedown tree 1
		deletechild tree	
	    }
	    deletechild tree
	}
    }
    return 0   
}
# Returns level of current child, with 0 being root level and each level
# down increasing by 1.
proc getlevel {name} {
    upvar $name tree  
    if ![info exists tree(point)] {
	return -1
    } else {
	set point $tree(point)
	set l 0
	while {![moveup tree]} {incr l}
	set tree(point) $point
	return $l
    }
}
proc newroot {name {value {}}} {
    upvar $name tree
    if ![info exists tree(point)] {
	set tree(point) 
	set tree($tree(point),parent) NULL
	set tree(count) 1
	set newroot $tree(point)
	set tree($tree(point),children) 0
    } else {
	set newroot [incr tree(count)]
	set tree($newroot,children) $tree(root)
	set tree($newroot,parent) NULL
	set tree($tree(root),parent) $newroot
	set tree(root) $newroot
    }
    set tree($newroot) $value
    return 0
}
# 'grafts' childvalue onto newparentval as a new child
proc setparent {name childvalue newparentval} {
    upvar $name ptree
    if ![info exist ptree] {
	return -1
    } else {
	set point $ptree(point)
	set parent [lindex [findall ptree $newparentval] 0]
	set child [lindex [findall ptree $childvalue] 0]
	if {[expr {$parent == 0}] || [expr {$child == 0}]} {
	    return -1
	} else {
	    moveto ptree $childvalue
	    moveup ptree
	    set ind [lsearch -exact $ptree($ptree(point),children) $child]
	    set ptree($ptree(point),children) [lreplace $ptree($ptree(point),children) $ind $ind]
	    set ptree($child,parent) $parent
	    moveto ptree $newparentval
	    if ![numchild ptree] {
		set ptree($ptree(point),children) $child
	    } else {
		lappend ptree($ptree(point),children) $child
	    }
	}
	set ptree(point) $point
	return 0
	
    }
}

# Doesnt actually move anything, just finds the first instance of 'value' and 
# sets it as the current child.
proc moveto {name value} {
    upvar $name ptree
    if ![info exist ptree] {
	return -1
    } else {	
	set point $ptree(point)
	set loc [lindex [findall ptree $value] 0]
	if {$loc == 0} {
	    set ptree(point) $point
	    return -1
	}
	set ptree(point) $loc
	return 0
    }
}

# Doesnt find all occurences of 'value', just finds first one 
# using array index order rather than tree order.
proc findall {name value} {
    upvar $name ptree
    if ![info exist ptree] {
	return -1
    } else {
	set point $ptree(point)
	set n $ptree(count)
	for {set i 1} {$i <= $n} {incr i} {
	    if ![catch {expr {$ptree($i) == $value}} result] {
		if {$ptree($i) == $value} {
		    set ptree(point) $i
		    set lev [getlevel ptree]
		    set ptree(point) $point
		    return "$i $lev"
		}
	    }
	}
	return 0
    }
}
	
# Returns first instance (index and level) of 'value' existing
# downward from current child.
proc existdown {name value} {
    upvar $name ptree
    if {[getval ptree] != $value} {
	set n [numchild ptree]
	if {$n > 0} {
	    for {set i 1} {$i <= $n} {incr i} {
		movedown ptree $i
		set found [existdown ptree $value]
		moveup ptree
		if {$found != 0} {
		    return $found
		}
	    }
	} else {
	    return 0
	}
    } else {
	set msg "$ptree(point) [getlevel ptree]"
	return $msg
    }
    return 0
}