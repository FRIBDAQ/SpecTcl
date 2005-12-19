# ----------------------------------------------------------------------
#  EXAMPLE: simple notebook that can dial up pages
# ----------------------------------------------------------------------
#  Effective Tcl/Tk Programming
#    Mark Harrison, DSC Communications Corp.
#    Michael McLennan, Bell Labs Innovations for Lucent Technologies
#    Addison-Wesley Professional Computing Series
# ======================================================================
#  Copyright (c) 1996-1997  Lucent Technologies Inc. and Mark Harrison
# ======================================================================

option add *Notebook.borderWidth 2 widgetDefault
option add *Notebook.relief sunken widgetDefault

proc notebook_create {win} {
    global nbInfo

    frame $win -class Notebook
    pack propagate $win 0

    set nbInfo($win-count) 0
    set nbInfo($win-pages) ""
    set nbInfo($win-current) ""
    return $win
}

proc notebook_page {win name} {
    global nbInfo

    set page "$win.page[incr nbInfo($win-count)]"
    lappend nbInfo($win-pages) $page
    set nbInfo($win-page-$name) $page

    frame $page

    if {$nbInfo($win-count) == 1} {
        after idle [list notebook_display $win $name]
    }
    return $page
}

proc notebook_display {win name} {
    global nbInfo

    set page ""
    if {[info exists nbInfo($win-page-$name)]} {
        set page $nbInfo($win-page-$name)
    } elseif {[winfo exists $win.page$name]} {
        set page $win.page$name
    }
    if {$page == ""} {
        error "bad notebook page \"$name\""
    }

    notebook_fix_size $win

    if {$nbInfo($win-current) != ""} {
        pack forget $nbInfo($win-current)
    }
    pack $page -expand yes -fill both
    set nbInfo($win-current) $page
}

proc notebook_fix_size {win} {
    global nbInfo

    update idletasks

    set maxw 0
    set maxh 0
    foreach page $nbInfo($win-pages) {
        set w [winfo reqwidth $page]
        if {$w > $maxw} {
            set maxw $w
        }
        set h [winfo reqheight $page]
        if {$h > $maxh} {
            set maxh $h
        }
    }
    set bd [$win cget -borderwidth]
    set maxw [expr $maxw+2*$bd]
    set maxh [expr $maxh+2*$bd]
    $win configure -width $maxw -height $maxh
}
