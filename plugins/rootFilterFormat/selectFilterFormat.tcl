package require snit


#
#   Widget to select filter formats.
#   This widget displays the disabled filters
#   Next to each filter widget is a pulldown menu of legal formats
#   The pulldown is initialized to display the current format of each
#   filter.
#   Pulling down the menu and selecting a menu item sets that filter's
#   format.
#
#    At the bottom of the widget is some information about the
#    filter formats.
#
snit::widget filterFormat {
    hulltype toplevel
    constructor args {

	variable format

	# List the Filters

	label $win.filtertitle -text {Inactive Filters:}
	grid $win.filtertitle -

	label $win.filtername   -text Name
	label $win.filtergate   -text Gate
	label $win.filterfile   -text File
	label $win.filterfmt    -text Format

	grid $win.filtername   $win.filtergate $win.filterfile $win.filterfmt -sticky w

	set fnum 0

	foreach filter [inactiveFilters] {
	    set name [lindex $filter 0]
	    set gate [lindex $filter 1]
	    set file [lindex $filter 2]
	    set fmt  [lindex $filter 5]

	    label $win.name$fnum -text $name
	    label $win.gate$fnum -text $gate
	    label $win.file$fnum -text $file
	    label $win.fmt$fnum  -text $fmt

	    bind $win.fmt$fnum <Button-1> [mymethod selectFormat $name $win.fmt$fnum %X %Y]

	    grid $win.name$fnum $win.gate$fnum $win.file$fnum $win.fmt$fnum -sticky w

	    incr fnum
	}
	
	# List the known filter formats:

	set formats [filterFormatList]


	label $win.formats -text {Key of filter formats: }
	grid $win.formats -


	foreach format $formats {
	    set keyword [lindex $format 0]
	    set descr   [lindex $format 1]
	    label $win.${keyword}key  -text $keyword  
	    label $win.${keyword}descr  -text $descr
	    grid  $win.${keyword}key  $win.${keyword}descr  -sticky w
	    
	}
	# Method to dismiss:

	button $win.dismiss -text {Dismiss} -command [list destroy $self]
	grid $win.dismiss
    }

    #   Method to create/display the popup menu that 
    #   will select the format for a specific filter.
    #
    method selectFormat {filter label x y} {
	puts "Pop up for $filter widget: $label @ $x $y"
	set name $win.filterformatmenu
	destroy  $name

	set formats [filterFormatList]

	# construct the menu:

	menu $name
	$name add command -label $filter -command ""
	$name add separator
	foreach format $formats {
	    set fname [lindex $format 0]
	    $name add radio -label $fname -variable ::${selfns}::format -value $fname \
		-command [mymethod formatSelected $filter $fname  $name $label]
	}
	tk_popup $name $x $y
    }
    #  Called when the filter format for a filter has been selected.
    #
    method formatSelected {filtername formatname menuwidget labelwidget} {
	filter -format $filtername $formatname
	$labelwidget config -text $formatname
	destroy  $menuwidget
    }

    # 
    #   Return the list of filter formats as follows:
    #   list of entries, one per format.
    #   Each entry is a pair, of keyword/description.
    #

    proc filterFormatList {} {
	
	# Our only handle on this is to anlyze the help text.
	# The lines after the line that reads
	# "filter formats are:"
	# are the descriptions.. with keyword - description.

	catch {filter -help} helpText
	
	set helpText [split $helpText "\n"]
	set linenum 0
	foreach line $helpText {
	    if {[string trim $line] eq "filter formats are:"} {
		break
	    }
	    incr linenum
	}
	incr linenum;				#  line number of first format.
	set descriptions [lrange $helpText $linenum end]
	set result [list]
	foreach line $descriptions {
	    set format [split $line -]
	    set key   [string trim [lindex $format 0]]
	    set descr [string trim [lindex $format 1]]

	    lappend result [list $key $descr]
	    
	}
	return $result
	
    }

    #
    #   Returns the list of inactive filters:

    proc inactiveFilters {} {
	set filters [filter -list]
	set result [list]
	foreach filter $filters {
	    if {[lindex $filter 4] eq "disabled"} {
		lappend result $filter
	    }
	}
	return $result
    }
}

#  If the filter format dialog does not exist,
#  create it:
#
proc filterFormatDialog {} {
    set name .filterformatdialog
    if {![winfo exists  $name]} {
	filterFormat $name
    }
}

#   Add the dialog to the filter menu:


.topmenu.filter add command -label {Filter Format...} \
                            -command [list filterFormatDialog]
