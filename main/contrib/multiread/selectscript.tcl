proc CancelMultipleReadDialog {} {
 destroy .dialog
}

proc MultipleReadFill {} {
    global FileList
    global searchMask

    set FileList [glob  -nocomplain $searchMask]
}

proc StartMultipleReads {} {
    lappend command attach -pipe cat
    foreach file [.dialog.l curselection] {
      lappend command [.dialog.l get $file]
    }
    puts $command
    eval $command
    start
    CancelMultipleReadDialog     ;# To destroy the dialog.
}

proc MultipleReadLayout {}  {

  grid config .dialog.l .dialog.s -row 0 -sticky nws
  grid rowconfigure .dialog 0 -weight 1
  grid columnconfigure .dialog 0 -weight 1
  grid config .dialog.sx -row 1 -sticky ew
  grid config .dialog.d .dialog.t -row 2
  grid config .dialog.ok .dialog.cancel .dialog.filter -row 3

}

proc MultiRead { {mask *.evt} } {
    global searchMask
    global FileList
    set searchMask $mask

#   Create the user interface elements:

    toplevel .dialog

    listbox  .dialog.l -width 50 -selectmode extended -listvariable FileList \
                       -yscrollcommand ".dialog.s set" \
                       -xscrollcommand ".dialog.sx set"
    scrollbar .dialog.s -command ".dialog.l yview" -orient vertical
    scrollbar .dialog.sx -command ".dialog.l xview" -orient horizontal

    entry    .dialog.d -textvariable searchMask
    label    .dialog.t -text "Search mask"
    button   .dialog.ok     -text Ok     -command StartMultipleReads
    button   .dialog.cancel -text Cancel -command CancelMultipleReadDialog
    button   .dialog.filter -text Filter -command MultipleReadFill

    MultipleReadFill

    MultipleReadLayout
}