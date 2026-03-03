##
#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2025.
#
#    You may use this software under the terms of the GNU public license
#    (GPL).  The terms of this license are described at:
#
#     http://www.gnu.org/licenses/gpl.txt
#
#    Author:
#             Ron Fox
#	     NSCL
#	     Michigan State University
#	

# @file metadataeditor.tcl
# @brief provides the GUI for a metadata editor.
# This is part of issue #299 - supporting metadata in SpecTcl.

package provide MetaDataEditor 1.0

package require Tk;               # We are a GUI.
package require snit;             # for megawidgets.


##
# MetadataEditor
#
#  provides a metadata editor that looks like this:
#
#   +------------------------------------------------------+
#   |  <metadata name> <value>  [Modify]                   |
#   |       [New...]                                       |
#   |   +------------------------------------------+       |
#   |   |  Metadata in editable treeview           |       |
#   ...                 ...                                |
#   |   +------------------------------------------+       |
#   +------------------------------------------------------+
#
#  OPTIONS:
#     -metadata dict of current metadata (readonly).
# PUBLIC METHODS:
#     load  - Loads a dict of metadata into the GUI. All old
#             information are removed first.
#
# METADATA DICTs:
#   A key data structure is the metadata dict.  This is a dict whose
#   keys are metadata item names and whose values are 
#   Their values.  e.g. [dict create a b c d]  describes
#   two metadata items with names a and c and values b and d
#   respectively.
#
#  Makes use of the MetadataPrompter.
#
snit::widget MetadataEditor {
    option -metadata -default [dict create] -readonly 1 -cgetmethod _getmetadata
    delegate option * to hull
    ##
    # constructor.  Note that we don't have a commit button.
    #  That's up to whomever uses us.
    #  The Modfy and New buttons are handled purely internally.
    #
    constructor args {
        $self configurelist $args;                  # Not really any to configure.

        
        # Top part is for the selected item new/modify buttons:

        set md [labelframe $win.metadata -relief groove -text metadata]
        ttk::label $md.name -width -10;   #name is loaded here.
        ttk::entry $md.value
        ttk::button $md.commit -text "Modify" -state disabled -command [mymethod _updateMetadata]
        ttk::button $md.new -text "New..." -command [mymethod _newMetadata] -state disabled

        grid $md.name $md.value $md.commit
        grid x $md.new
        grid $md -sticky nsew

        # Metadata view.
        ttk::scrollbar $win.treescroll -orient vertical -command [list $win.tree yview]
        ttk::treeview $win.tree \
            -show headings -columns [list name value] -displaycolumns [list name value] \
            -selectmode browse -yscrollcommand [list $win.treescroll set]
        $win.tree heading name -text name
        $win.tree heading value -text value
        grid $win.tree $win.treescroll -sticky nsew

        # Selecting a metadata item loads it into the editor section.

        bind $win.tree <<TreeviewSelect>> [mymethod _loadMetaEditor]

    }

    ##
    # Load the metadata into the widget from an external dict:

    #  @param desc metadata dict - see above.
    #
    method load desc {
        
        # now the meatdata:

        set metadata $desc
        set existing [$win.tree children {}]
        $win.tree delete $existing;      # Clear the tree.
        $win.metadata.commit configure -state normal
        $win.metadata.new    configure -state normal

        dict for {key value} $metadata {
            $win.tree insert {} end -values [list $key $value]
        }
        
    }
    #  Private methods:

    ##
    # _getmetadata
    #   Marshall the metadata from the tree:
    #
    #  @param opt -name of the option (always -metdata)
    #  @return dict keyed by metadata name and with metadata values
    
    method _getmetadata opt {
        set result [dict create]
        foreach child [$win.tree children {}] {
            set key_value [$win.tree item $child -values]
            dict append result [lindex $key_value 0] [lindex $key_value 1]
        }

        return $result
    }
    ##
    # _loadMetaEditor
    #   Load the metadata editor with the currently selected metadata
    #   The metadata commit button is enabled so that once editing is 
    #   done the new value can be updated in the tree view.
    #
    method _loadMetaEditor {} {
        set selected [$win.tree selection]
        if {[llength $selected] == 0} {
            return ; # there's no selection actually.
        }
        set selectData [$win.tree item $selected -values]
        $win.metadata.name configure -text [lindex $selectData 0]
        $win.metadata.value delete 0 end
        $win.metadata.value insert 0 [lindex $selectData 1]
        $win.metadata.commit configure -state normal
    }
    ##
    # _updateMetadata
    #   The metadata Modify button was clicked.  Pull the data from the
    #   metadata label and entry and update the table.
    #
    method _updateMetadata {} {
        set name [$win.metadata.name cget -text]
        set value [$win.metadata.value get]
        
        # Find the metadata item by name in the tree and update it.

        set item [$self _findMetadata $name]
        if {$item ne ""} {
            $win.tree item $item -values [list $name $value]
        }
    }
    ##
    #   _newMetadata
    #    Prompt for a new metadata item.  If one is added and has both name and value not empty,
    #    it is appended to the tree.  This uses the MetaDataPrompter dialog below.
    #
    method _newMetadata {} {
        MetadataPrompter $win.prompt
        set md [$win.prompt get]
        destroy $win.prompt
        if {$md eq ""} return;            # Cancdled.
        set name [lindex $md 0]
        set value [lindex $md 1]

        if {$name eq "" || $value eq ""} {
            tk_messageBox -parent $win -icon error -type ok \
                -message {Meta data must have both a name and a value; neither can be blank}
        } else {
            # Don't allow duplicates
            if {[$self _findMetadata $name] ne ""} {
                tk_messageBox -parent $win -icon error -type ok \
                    -message "$name is an existing metadata name, duplicates are not allowed"
            } else {
                $win.tree insert {} end -values [list $name $value]
            }
        }
    }
    #-- utility methods:

    #  _findMetadata - find a metadata item given its name:
    # Returnns an empty string if not found.

    method _findMetadata {name} {
        foreach item [$win.tree children {} ] {
            if {$name eq [lindex [$win.tree item $item -values] 0]} {
                return $item
            }
        }
        #  Not found.
        return ""
    }

}
##
#  MetadataPrompter
#    This is a modal dialog that prompts a user for a new bit of metadata.
#
#  Layout:
#   +--------------------------------+
#   | Name: [    ] Value [    ]      |
#   +--------------------------------+
#   | [Ok]      [Cancel]             |
#   +--------------------------------+
#
# Usage:
#\verbatim
#    MetadataPrompter .somepath
#    .somepath get
#\endverbatim
#
#  the get returns a two element list of name, value
#  The list is empty if cancel was clicked.
#
snit::widget MetadataPrompter {
    hulltype toplevel

    # The buttons set this to Ok for the ok button Cancel for the cancel button.
    # If the dialog is destroyed via its window controls, that's an implied cancel so...
    variable action Cancel;    
    variable  hiddenFrame
    constructor args {
        #  there are no args so go directly to layout.

        # The top part has the prompt stuff wrapped in a frame.

        set value [ttk::frame $win.value]
        ttk::label $value.namelbl -text "Name: " -relief groove
        ttk::entry $value.name
        ttk::label $value.valuelbl -text "Value: "
        ttk::entry $value.value

        grid $value.namelbl $value.name $value.valuelbl $value.value
        grid $value -sticky nsew

        # The button (action) part has the buttons:

        set action [ttk::frame $win.action]
        ttk::button $action.ok -text Ok -command [mymethod _onOk]
        ttk::button $action.cancel -text Cancel -command [mymethod _onCancel]
        grid $action.ok $action.cancel
        grid $action

    }
    ##
    # get
    #    crate a hidden frame, set modal and focus then wait for the
    #    hidden frame to be destroyed.
    # @return Returns the dialog 'value'.
    # 
    # @note The caller must destroy the dialog.
    #
    method get {} {
        set hiddenFrame [frame $win.hidden]
        focus $win
        tkwait window $hiddenFrame
        #  If the action variable does not exist we were destroyed by window controls:

        if {[catch {set action}]} {
            return [list]
        }

        # One of the buttons was clicked _or_ we were destroyed:

        if {$action eq "Cancel"} {
            return [list]
        } else {
            return [list [$win.value.name get] [$win.value.value get]]
        }
    }

    #  private methods (button handler)

    method _onOk {} {
        set action Ok
        destroy $hiddenFrame
    }
    method _onCancel {} {
        set action Cancel
        destroy $hiddenFrame
    }

}