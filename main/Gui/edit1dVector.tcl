package provide edit1dVector 1.0
package require snit
package require guiutilities


##
# provides an editor for 1v spectra.  
# This is very much like the edit1d.
# The difference is that rather than
# the browser showing parmaeters, its showing vectors.
#
#
#  Our first try at implementation will be to
#  make an edit1d component, delegate everyting to
# it _but_ handle the vectorscdript to select the
# 'param4eter'
# so:

package require edit1d

snit::widget edit1dVec {
    component editor
    option -browser
    delegate option * to editor
    delegate method * to editor

    variable browser

    # For the constructor we 
    #  - install the component, 
    #  - connect to the browser and:
    #      * attach my -vectdorscript to fill in the
    #         edit1dVec component
    #      * attach a -filtervectors script to filter the vector
    #         list such that only the vectors not in the spectrum
    #         are used.
    #
    constructor args {
        $self configurelist $args
        install editor using edit1d $win.editor -browser $options(-browser)
        grid $editor -sticky nsew
       

        set browser $options(-browser)

        #  Set up the browser scripts we'll handle.

        $browser configure -vectorscript [mymethod _selectVector]
        $browser configure -filtervectors [mymethod _filterVectors]
    }
    ##
    # _selectVector
    #
    #  Called to select a vector.  This
    #  loads the named vector into the edit1d's parameter slot.
    #  
    # @param path - path to the item in the tree.
    method _selectVector {path} {
        # The vector name is the last path element:
        catch {
        puts _selectVector
        puts $path
        set name [::pathToName $path]
        puts $name
        set properties [lindex [treeparamvec -list $name] 0]

        set low [dict get $properties low]
        set high [dict get $properties high]
        set bins [dict get $properties bins]
        set units [dict get $properties units]

        set priorVec [$editor getParameters]
        $editor populateParameter $name $low $high $bins $units

        # Remove the parameter from the list and
        # add any old one back:

        $browser deleteElement vectors $name
        if {[llength [treeparamvec -list priorVec]] > 0} {
            $browser addNewVector $name
        }
        } msg 
        puts $msg
        puts $::errorInfo
    }
    ##
    #  _filterVectors
    #
    #  Called for each vector to determine if it should be displayed by the
    #  browser.  If there is a parameter loaded in the 1d editor, that
    #  named vector is not displayed in the vector list.
    #
    # @param desc - the vector description.
    # @return bool - true to display, false otherwise.
    #   
    method _filterVectors {desc} {

    }
}

