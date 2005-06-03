# ----------------------------------------------------------------------
#  EXAMPLE: tabnotebook that can dial up pages
# ----------------------------------------------------------------------
#  Effective Tcl/Tk Programming
#    Mark Harrison, DSC Communications Corp.
#    Michael McLennan, Bell Labs Innovations for Lucent Technologies
#    Addison-Wesley Professional Computing Series
# ======================================================================
#  Copyright (c) 1996-1997  Lucent Technologies Inc. and Mark Harrison
# ======================================================================

option add *Tabnotebook.tabs.background #666666 widgetDefault
option add *Tabnotebook.margin 6 widgetDefault
option add *Tabnotebook.tabColor #a6a6a6 widgetDefault
option add *Tabnotebook.activeTabColor #d9d9d9 widgetDefault
option add *Tabnotebook.tabFont \
    -*-helvetica-bold-r-normal--*-120-* widgetDefault

proc tabnotebook_create {win} {
    global tnInfo

    frame $win -class Tabnotebook
    canvas $win.tabs -highlightthickness 0
    pack $win.tabs -fill x

    notebook_create $win.notebook
    pack $win.notebook -expand yes -fill both

    set tnInfo($win-tabs) ""
    set tnInfo($win-current) ""
    set tnInfo($win-pending) ""
    return $win
}

proc tabnotebook_page {win name} {
    global tnInfo

    set page [notebook_page $win.notebook $name]
    lappend tnInfo($win-tabs) $name

    if {$tnInfo($win-pending) == ""} {
        set id [after idle [list tabnotebook_refresh $win]]
        set tnInfo($win-pending) $id
    }
    return $page
}

proc tabnotebook_refresh {win} {
    global tnInfo

    $win.tabs delete all

    set margin [option get $win margin Margin]
    set color [option get $win tabColor Color]
    set font [option get $win tabFont Font]
    set x 2
    set maxh 0

    foreach name $tnInfo($win-tabs) {
        set id [$win.tabs create text \
            [expr $x+$margin+2] [expr -0.5*$margin] \
            -anchor sw -text $name -font $font \
            -tags [list $name]]

        set bbox [$win.tabs bbox $id]
        set wd [expr [lindex $bbox 2]-[lindex $bbox 0]]
        set ht [expr [lindex $bbox 3]-[lindex $bbox 1]]
        if {$ht > $maxh} {
            set maxh $ht
        }

        $win.tabs create polygon 0 0  $x 0 \
            [expr $x+$margin] [expr -$ht-$margin] \
            [expr $x+$margin+$wd] [expr -$ht-$margin] \
            [expr $x+$wd+2*$margin] 0 \
            2000 0  2000 10  0 10 \
            -outline black -fill $color \
            -tags [list $name tab tab-$name]

        $win.tabs raise $id

        $win.tabs bind $name <ButtonPress-1> \
            [list tabnotebook_display $win $name]

        set x [expr $x+$wd+2*$margin]
    }
    set height [expr $maxh+2*$margin]
    $win.tabs move all 0 $height

    $win.tabs configure -width $x -height [expr $height+4]

    if {$tnInfo($win-current) != ""} {
        tabnotebook_display $win $tnInfo($win-current)
    } else {
        tabnotebook_display $win [lindex $tnInfo($win-tabs) 0]
    }
    set tnInfo($win-pending) ""
}

proc tabnotebook_display {win name} {
    global tnInfo

    notebook_display $win.notebook $name

    set normal [option get $win tabColor Color]
    $win.tabs itemconfigure tab -fill $normal

    set active [option get $win activeTabColor Color]
    $win.tabs itemconfigure tab-$name -fill $active
    $win.tabs raise $name

    set tnInfo($win-current) $name
}
