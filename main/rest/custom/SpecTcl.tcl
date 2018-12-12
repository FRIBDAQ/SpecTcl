set here [file dirname [info script]]
source [file join $here max.tcl];         # Make sure max is patched.

namespace eval SpecTcl {}

proc SpecTcl::mainMenu {id} {

    set result   "<div class='$id'>"

    append result  [html::minorList {Spectra spectra.tml
	Parameters parameters.tml
        Gates gates.tml
	{Spectrum display} displayer.tml
    }]


    append result </div>
    

    return $result

}
#
# Returns the java script to create a tabbed look:
#
proc SpecTcl::menuTabs {id} {
    set selector "jQuery('$id').tabs({
       ajaxOptions: {
         error: function (xhdr, status, index, anchor) {
            jQuery(anchor.hash).html(\"Failed to load tab\");
         }
       }
    });"

    return $selector
			      
}
#
# Returns html for a parameter table.
#
proc SpecTcl::paramtable {} {
    set parameters [treeparameter -list]
    set result {<table rules='cols'>}
    append result <tr><th>Name</th><th>Low</th><th>High</th><th>Bins</th><th>Units</th></tr>
    append result {<tbody >}
    foreach parameter $parameters {
        append result {<tr class="data">}
	append result "<td>[lindex $parameter 0]</td>"
	append result "<td>[lindex $parameter 2]</td>"
	append result "<td>[lindex $parameter 3]</td>"
	append result "<td>[lindex $parameter 1]</td>"
	append result "<td>[lindex $parameter 5]</td>"
	append result <tr>
     }
    append result </tbody>
    append result {</table>}
    
    return $result
}
#
#  Returns the html for a spectrum table.
#

proc SpecTcl::spectable {} {

    # Build an array of gate applications keyed by spectrum:

    array set appliedGates [list]
    foreach application [apply -list] {
	set spectrum [lindex $application 0]
	set gate     [lindex $application 1]
	set appliedGates($spectrum) $gate
    }

    set spectra [spectrum -list]
    set result {<table width='100%' rules="cols"  id='spectrumTable'>}
    append result {<thead   id='spectrumHeader'><tr>
                       <th width='10%'>Name</th>
	               <th width='5%'>Type</th>
                       <th width='20%'>Parameters</th>
                       <th width='25%'>Axes</th>
                       <th width='45%' align='left'>Gate</th>
	          </tr></thead>}
    append result {<tbody >}
    foreach spectrum $spectra {
	set name [lindex $spectrum 1]
	append result {<tr class='data'>}
	append result "<td class='spectrumName'>$name</td>"
	append result <td>[lindex $spectrum 2]</td>

	#spectrum names:

	append result <td><table>
	foreach parameter [lindex $spectrum 3] {
	    append result <tr><td>$parameter</td></tr> 
	}
	append result </table></td>

	# Axis definitions:

	append result <td><table>
	foreach axis [lindex $spectrum 4] {
	    set low [lindex $axis 0]
	    set hi  [lindex $axis 1]
	    set bins [lindex $axis 2]
	    
	    append result <tr><td>($low - $hi) : $bins bins</td></tr>
	}
	append result </table></td>

	append result <td>
	if {[array names appliedGates $name] ne "" } {
	    
	    set gate $appliedGates($name)
	    set gatename [lindex $gate 0]
	    set gateType [lindex $gate 2]
	    set gateDesc [lindex $gate 3]
	    if {$gateType ne "T"} {
		append result "<span class='gatename'>$gatename</span>"
	    }
	    append result " <span class='gatedef'> $gateType $gateDesc</span>"
	    
       	}
	append result </td>
	    
	append result </tr>

    }
    append result </tbody>
    append result </table>
    return $result
}
#
# Returns the html for a gate table:
#
proc SpecTcl::gates {} {
    set result {<table rules="cols">}
    append result {<tr><th>Name</th><th>Type</th><th>Definition</th></tr>}
    append result {<tbody >}
    append result [ html::foreach gate [gate -list] {
	<tr class="data">
	<td>[lindex $gate 0]</td>
	<td>[lindex $gate 2]</td>
	<td>[lindex $gate 3]</td>
	</tr>
    }
		    ]
    append $result </tbody></table>
    return $result
}
##
# return the jQuery to set even rows of a table to a 
# specific background color:
#
#  @param id - selector of some element that encloses the table (e.g. a <div>).
#  @param class - Class attached to each <tr> at the top level of the table.
#  @param color - The desired background color
#
# @note - in order to support nested tables properly, the assumption is that
#         the <tr>'s are of class .data
#
proc SpecTcl::highlightTableRows {id class color} {
    return "
   jQuery('$id tbody tr.$class:even').addClass('evenrows');
"
}
##
# Produce hover /highlight code for a selctor:
#
# @param selector
# @param class - the CSS class that produces the highlight.
#
proc SpecTcl::hoverHighlight {selector class} {
    return "
   jQuery('$selector').hover(
      function() {
        jQuery(this).toggleClass('$class');
      },
      function() {
        jQuery(this).toggleClass('$class');
      } 
    );
"
}
##
#  Produce hover/show code:  This code will trigger on a hover
#  over a span with a specific class name to show a sibling
#  span with a different classname..and then hide that span
#  when the hover ends.
#
# @param hoverClass - the class of the span that is selected for hovering.
# @param showHideClass -The class of the sibling span that is shown/hidden.
#
proc SpecTcl::hoverShow {hoverClass showHideClass} {
    return "
      jQuery('span.$hoverClass').hover(
        function() {
          jQuery(this).next('span.$showHideClass').toggle('slow');
        },
        function() {
          jQuery(this).next('span.$showHideClass').toggle('slow');
        }
      );
"
}
##
# Load a bit of javascript given the URI:
#  
# @param uri - the URI where the javascript is stored.
#
proc SpecTcl::loadJavascript {uri} {
    return "
<script 
    type='text/javascript'
    src='$uri'>
</script>"
}
##
# Produce script tags to load jquery:
#
proc SpecTcl::loadJquery {} {
    return [::SpecTcl::loadJavascript $::SpecTcl::jquery]

}
##
# Produce script tags to load jqueryUI:
#
proc SpecTcl::loadJqueryUI {} {
    return [::SpecTcl::loadJavascript ${::SpecTcl::jquery-ui}]

}

##
# Load d3:
#
proc SpecTcl::loadD3 {} {

    return [::SpecTcl::loadJavascript $::SpecTcl::d3]
}
##
#  Load a css style file given its URI
#
#  @param uri
#
proc SpecTcl::loadCSS uri {
    return "
<link
   rel='stylesheet'
   href='$uri'
   type='text/css'
   media='all'
/>
"
}
##
# Load the JQuery CSS:
#
proc SpecTcl::loadJqueryCSS {} {
    return [::SpecTcl::loadCSS $::SpecTcl::jquerycss]
}
##
# Load SpecTcl CSS:
#
proc SpecTcl::loadSpecTclCSS {} {
    return [::SpecTcl::loadCSS $::SpecTcl::spectclcss]
}
