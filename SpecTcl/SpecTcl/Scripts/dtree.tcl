# *************************************************************************
# dtree.tcl
# by Chase Evan Bolen
# dependancy tree data structure
# Uses tree structure from tree.tcl but adds new rules to make dependancy tree.
# New rules:
# Every value must be unique.
# A child with a high level value mean that it is depended upon by its parent (with lower level value). 
# Adding an item checks first that that item doesnt already exist.  
# If it doesnt currently exist it is inserted at level 1, just below the root.
# *************************************************************************
global SpecTclHome
source $SpecTclHome/Script/tree.tcl
proc additem {name {value {}}} {
    upvar $name ptree
    if ![info exist ptree] {
	newchild ptree {*%*%MainRoot%*%*}
	newchild ptree $value
	set ptree(missingevent) ""
    } else {
	if {[findall ptree $value] ==  0} {
	    set point $ptree(point)
	    setroot ptree
	    newchild ptree $value
	    set ptree(point) $point
	} else {
	    return -1
	}
    return 0
    }
    
}
# Procedure says that all values of args (one or many) are depended on by value1.
# Each value of args is processed individually.
# If args exist at or above the level of value1 then they are made children 
# of value1 (so that they are now below value1). If they are already below value1
# nothing is done.
proc dependon {name value1 args} {
    # args=child value1=parent
    upvar $name ptree
    if ![info exist ptree] {
	return -1
    } else {
	set args [string trimleft $args "{"]
	set args [string trimright $args "}"]
	if {[llength $args] > 1} {
	    dependon ptree $value1 [lindex $args 0]
	    dependon ptree $value1 [lrange $args 1 end]
	} else {
	    set point $ptree(point)
	    set exist1 [findall ptree $value1]
	    set exist2 [findall ptree $args]
	    if {[expr {$exist1 == 0}] || [expr {$exist2 == 0}]} {
		deleteup ptree $value1
		return -2		
		# handle missing gates here
	    } else {
		set level [lindex [findall ptree $args] 1]
		set newlevel [expr {[lindex [findall ptree $value1] 1] + 1}]
		if {$newlevel > $level} {
		    setparent ptree $args $value1
		}
		return 0
	    }
	}
    }
}
# outputs the tree in order of highest level to lowest (most depended on to most dependant)
proc outlist {name} {
    upvar $name ptree
    if ![info exist ptree] {
	return -1
    } else {
	setroot ptree
	if {[numchild ptree] > 1} {
	    set n $ptree(count)
	    set maxlev 0
	    for {set i 2} {$i <= $n} {incr i} {
		if ![catch {expr {$ptree($i) == 1}} result] {
		    set level [lindex [findall ptree $ptree($i)] 1]
		    if {$level > $maxlev} {
			set maxlev $level
		    }
		    lappend plist($level) $ptree($i)
		}
	    }
		for {set j $maxlev} {$j > 0} {incr j -1} {
		    foreach element $plist($j) {
			lappend tot $element
		    }
	    }
	    return $tot
	} else {
	    return {}
	}
    }
}
# This procedure deletes up toward the root from the child with value: value.  
# All children not directly in the upward path are set as level 1.  It is used
# When an item (B) is dependant on a missing item (C).  The dependant item (B) is useless without
# the depended on item (C) so (B) is deleted, which makes its parent (A) (that item which is dependant
# on (B) also useles.  This will continue until the process reaches the root (root is not deleted). 
# However, children of the deleted items are not effected by the dependancies of the 
# parent so they are moved to level 1.
proc deleteup {name value} {
    upvar $name ptree
    if ![info exist ptree] {
	return -1
    } else {
	set currentval [getval ptree]
	moveto ptree $value
	while {[getval ptree] != "*%*%MainRoot%*%*"} {
	    set n [numchild ptree]
	    if {$n > 0} {
		movechildrenroot ptree
	    }
	    missingevent ptree
	    deletechild ptree	    
	}
	moveto ptree $currentval
    }
    return 0
}

proc movechildrenroot {name} {
    upvar $name ptree
    if ![info exist ptree] {
	return -1
    } else {
	set currentval [getval ptree]
	set n [numchild ptree]
	for {set i 1} {$i <= $n} {incr i} {
	    movedown ptree 1
	    setparent ptree [getval ptree] {*%*%MainRoot%*%*}
	    moveto ptree $currentval
	}
    }
}
# This procedure is used to set the missingevent.  This event
# is called when an item is dependant on a missing item (as described above).  
# the tcl code set as the missingevent body will be executed for each 
# deleted item.
proc setmissingevent {name body} {
 upvar $name ptree
    if ![info exist ptree] {
	return -1
    } else {
	set ptree(missingevent) $body
    }
}
# This procedure is called for each item deleted.  It evaluates the body
# of the missingevent.
proc missingevent {name} {
upvar $name ptree
    if ![info exist ptree] {
	return -1
    } else {
	if {$ptree(missingevent) != ""} {
	    eval $ptree(missingevent)
	}
    }
}
