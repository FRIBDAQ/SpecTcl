#!/bin/sh
# -*- tcl -*-
# The next line is executed by /bin/sh, but not tcl \
exec tclsh "$0" ${1+"$@"}

#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2014.
#
#    You may use this software under the terms of the GNU public license
#    (GPL).  The terms of this license are described at:
#
#     http://www.gnu.org/licenses/gpl.txt
#
#    Authors:
#             Ron Fox
#             Giordano Cerriza
#	     NSCL
#	     Michigan State University
#	     East Lansing, MI 48824-1321


##
# @file   restclient.tcl
# @brief  Base level client code of SpecTcl REST server.
# @author Ron Fox <fox@nscl.msu.edu>
#

package provide SpecTclRESTClient 1.0
package require snit
package require http
package require json

##
# @class SpecTclRestClient
#
#   Client to the SpecTcl REST server.  The goal is to implement, essentialy,
#   every SpecTcl command as a REST server/client method set.
#
#  OPTIONS:
#    -host  - host SpecTcl is running in (default is localhost)
#    -port  - Port SpecTcl REST server is listening  (defaults to 8080).
#
#  METHODS:
#    applyGate  - Apply gates to spectra.
#    applyList   - List gate applications.
#
#    attachSource - Attach data source (must still start).
#    attachList   - Return attachment strings.
#
#    sbindAll     - sbind -all
#    sbindSpectra - sbind spectrum list.
#    sbindList    - List spectrum bindings.
#
#    fitCreate   - Create a new fit.
#    fitUpdate   - Update a fit.
#    fitDelete   - Delete a fit.
#    fitList     - List fits.
#
#    foldApply   - Apply a gamma gate as a fold to a spectrum.
#    foldList    - List folds.
#    foldRemove  - Unfold a spectrum.
#
#    channelSet  - Set a spectrum channel value.
#    channelGet  - Retrieve a spectrum channel value.
#
#    spectrumClear  - Clear spectrum.
#    spectrumClearAll - Clear all spectra.
#
#    spectrumProject - Project a 2d spectrum.
#
#    spectrumStatistics - Get spectrum oveflow/underflow statistics
#
#    treeparameterCreate
#    treeparameterList
#    treeparameterListNew
#    treeparameterSet
#    treeparameterSetInc
#    treeparameterSetBins
#    treeparameterSetSetUnits
#    treeparameterSetLimits
#    treeparameterCheck
#    treeparameterUncheck
#    treeparameterVersion
#
snit::type SpecTclRestClient {
    option -host -default localhost
    option -port -default 8080
    
    variable domain spectcl
    
    constructor args {
        $self configurelist $args
    }
    #--------------------------------------------------------------------------
    # Private variables
    #
    
    ##
    #  _makeUrl
    #    Create an arbitrary request URL - note that all request are GET so
    #    that's easy.
    # @param subdomain - the subdomain of the URL.
    # @param query     - dict containing the query key/value pairs.
    #                    We'll shimmer that into a list format.
    # @return string - the complete URL.
    #
    method _makeUrl {subdomain query} {
        set querypart [http::formatQuery {*}$query]
        set url http://$options(-host):$options(-port)/$domain/$subdomain?$querypart
        return $url
    }
    ##
    # _request
    #   Given a URI:
    #   - Starts the request.
    #   - waits for completion.
    #   - Decodes the resulting JSON into a dict.
    #   - If the status field is not OK error with the vaLue of 'detail'
    #   - otherwise, return the full JSON response.
    #
    # @param url - url complete with any query suffixes.
    # @return dict - the full JSON response
    #
    method _request {url} {
        set token [http::geturl $url]
        http::wait $token
        set status [http::ncode $token]
        
        # Httpd error:
        
        if {$status != 200} {
            set message "[http::error $token] [http::data $token]"
            http::cleanup token
            error $message
        }
        
        set rawData [http::data $token]
        # puts $rawData;      # Uncomment to debug reply errors.
        set parseOk [catch {
            set json [json::json2dict $rawData]
        } msg]
        http::cleanup $token
        
        #  Could be errors parsing the JSON or errors flagged in the
        #  json:
        
        if {$parseOk} {
            error "JSON Parse Failed: $msg : $::errorInfo"
        }
        # Debugging here - if there's no detail then likely we have  a
        # server error - let's throw an error with the rawdData as the message:
        #
        if {![dict exists $json status ]} {
            error "SpecTcl Server error: $rawData"
        }
        
        set status [dict get $json status]
        if {$status ne "OK"} {
            error "Request failed by SpecTcl: $status :  [dict get $json detail]"
        }
        
        
        # Success return what interests the caller.
        
        return $json
    }
    ##
    #  _listToQueryList
    #    Given a query parameter name and a list of items,
    #    returns  list of parameter-name item1 paramter-name item2...
    # @param queryArg - name of the query parameter.
    # @param items    - items to put into the list.
    #
    proc _listToQueryList {queryArg items} {
        set result [list]
        foreach item $items {
            lappend result $queryArg $item
        }
        return $result
    }
    
    #---------------------------------------------------------------------------
    #
    #  Apply command jackets.
    #
    
    ##
    # applyGate
    #   Apply a gate to a list of spectra.  Note each application of a gate
    #   to one spectrum is a transaction.
    #
    # @param gate - name of the gate to apply.
    # @param spectra - list of spectra
    #
    method applyGate {gate spectra} {
        set query [dict create gate $gate]
        
        # The request returns nothing of interest so just loop.
        
        foreach spectrum $spectra {
            dict set query spectrum $spectrum
            $self _request [$self _makeUrl apply/apply $query]
        }
    }
    ##
    # applyList
    #   List gate applications
    # @param pattern - spectrum match pattern optional -defaults to *
    #                  which matches everything.
    # @return list of dicts dict containing spectrum and gate (name).
    #
    method applyList {{pattern *}} {
        set query [dict create pattern $pattern]
        set json [$self _request [$self _makeUrl apply/list $query]]
        return [dict get $json detail]
    }
    #--------------------------------------------------------------------------
    # Attach command jackets.
    
    ##
    # attachSource
    #   Attach a data source.
    # @param stype   - 'pipe' or 'file'
    # @param source - Data source specification (e.g. filename for type == 'file').
    # @param size  - optional blocking factor - defaults to 8192.
    # @param format - Optional data format defaults to ring
    #
    method attachSource {stype source {size 8192} {format ring}} {
        set query [dict create type $stype source $source size $size format $format]
        $self _request [$self _makeUrl attach/attach $query]
    }
    ##
    # attachList
    # @return string that describes the current attachment:
    #
    method attachList {} {
        set result [$self _request [$self _makeUrl attach/list [dict create]]]
        return [dict get $result detail]
    }
    #-------------------------------------------------------------------------
    #  Sbind command jackets.
    
    ##
    # sbindAll
    #   Sbind all spectra.
    #
    method sbindAll {} {
        $self _request [$self _makeUrl sbind/all [dict create]]
    }
    
    ##
    # sbindSpectra
    #
    #  Sbind a list of spectra.
    #  
    # @param spectra - the spectra to bind.
    #
    method sbindSpectra {spectra} {
        set qparams [_listToQueryList  spectrum $spectra]
        
        $self _request [$self _makeUrl sbind/sbind $qparams]
    }
    ##
    # sbindList
    #   Return a list of the bindings.
    #
    # @param pattern - glob pattern to limit the spectra looked at.
    # @return list of dicts containing spectrumid - the id of the spectrum,
    #                 name - name of the spectrum and -binding -the binding id.
    #
    method sbindList {{pattern *}} {
        set result [$self _request [$self _makeUrl sbind/list [dict create pattern $pattern]]]
        
        return [dict get $result detail]
    }
    #-------------------------------------------------------------------------
    # fit command jackets.
    
    ##
    # fitCreate
    #    Create a new fit object.
    #
    # @param name = fit name.
    # @param spectrum - fit spectrum.
    # @param low, high - Channel fit limits.
    # @param ftype - fit type.
    #
    method fitCreate {name spectrum low high ftype} {
        $self _request [$self _makeUrl fit/create [dict create             \
            name $name spectrum $spectrum low $low high $high              \
            type $ftype                                                     \
        ]]
    }
    ##
    # fitUpdate
    #    Update the paramters of fits that match the pattern.
    #
    # @param pattern - glob pattern to match the fit names update.
    #
    method fitUpdate {{pattern *}} {
        $self _request [$self _makeUrl fit/update [dict create pattern $pattern]]
    }
    ##
    #  fitDelete
    #   Delete a single fit
    # @param name  - Name of the fit to delete.
    #
    method fitDelete {name} {
        $self _request [$self _makeUrl fit/delete [dict create name $name]]
    }
    ##
    # fitList
    #    List information about fits.
    #    list of dicts containing name - fit name, spectrum -spectrum name.
    #    type -fit type, low, high -fit limits and parameter - a dict specific
    #    to the fit type that contains the fit parameters.
    #
    # @param pattern - glob pattern. Fits with names that match this are in the
    #         result.
    #
    method fitList {{pattern *}} {
        set json [$self _request [$self _makeUrl fit/list [dict create pattern $pattern]]]
        
        return [dict get $json detail]
        
    }
    #--------------------------------------------------------------------------
    # fold command jackets.
    #
    
    ##
    # foldApply
    #    Apply a fold to one or more spectra.
    # @param gate  - name of the gate to apply the fold to.
    # @param spectra - list of spectra to fold with that gate.
    #
    method foldApply {gate spectra} {
        set queryList [list gate $gate {*}[_listToQueryList spectrum $spectra]]
        $self _request [$self _makeUrl fold/apply $queryList]
    }
    ##
    # foldList
    #    List folds  on spectra that match the pattern
    # @param pattern - only spectra that match this optional glob pattern are
    #                  listed.  Defaults to * which matches all spectra.
    #
    method foldList {{pattern *}} {
        set info [$self _request [$self _makeUrl fold/list [dict create pattern $pattern]]]
        return [dict get $info detail]
    }
    ##
    # foldRemove
    #   Remove a fold from a spectrum.
    #
    # @param spectrum  - name of spectrum to unfold.
    #
    method foldRemove {spectrum} {
        $self _request [$self _makeUrl fold/remove [dict create spectrum $spectrum]]
    }
    #-------------------------------------------------------------------------
    # jackets channel set/get.
    #
    #  Note we freely use that -1 is an invalid channel coordinate.
    #
    
    ##
    # channelGet
    #  Return the value of a channel
    #
    # @param spectrum  - name of a spectrum.
    # @param xchannel  - x channel value
    # @param ychannel  - optional y channel value (only for 2d spectra).
    #
    method channelGet {spectrum xchannel {ychannel -1}} {
        
        set qparams [dict create spectrum $spectrum xchannel $xchannel]
        if {$ychannel != -1} {
            dict set qparams ychannel $ychannel
        }
        
        set info [$self _request [$self _makeUrl channel/get $qparams]]
        return [dict get $info detail]
    }
    ##
    # channelSet
    #   Set the value of a channel
    #
    # @param spectrum  - name of spectrum.
    # @param value     - value to set.
    # @param xchannel  - xchannel to set.
    # @param ychannel  - optional (only needed if spectrum is 2d) y channel.
    #
    method channelSet {spectrum value xchannel {ychannel -1}} {
        set qparams \
            [dict create spectrum $spectrum value $value xchannel $xchannel]
        if {$ychannel != -1} {
            dict set qparams ychannel $ychannel
        }
        
        $self _request [$self _makeUrl channel/set $qparams]
    }
    #--------------------------------------------------------------------------
    # clear jackets.. these spectcl/spectrum/zero
    
    ##
    # clearSpectrum
    #    clear the contents of one spectrum (actually the name can be a glob pattern)
    #
    # @param name  - name of the spectrum to clear.
    #
    method spectrumClear {name} {
        $self _request [$self _makeUrl spectrum/zero [dict create pattern $name]]
    }
    ##
    # spectrumClearAll
    #
    method spectrumClearAll {} {
        return [$self spectrumClear *]
    }
    #-------------------------------------------------------------------------
    # project command jacket.
    
    ##
    # spectrumProject
    #
    # @param old - old spectrum name.
    # @param new  - new spectrum name.
    # @param direction - Projection direction x|y.
    # @param snapshot - Boolean snapshot flag.
    # @param contour - If provided only the part within the contour is projected.
    #
    method spectrumProject {old new direction snapshot {contour ""} } {
        set qdict [dict create \
            snapshot $snapshot source $old newname $new direction $direction
        ]
        if {$contour ne ""} {
            dict set qdict contour $contour
        }
        $self _request [$self _makeUrl project $qdict]
    }
    #--------------------------------------------------------------------------
    # specstats jacket.
    #
    
    ##
    # spectrumStatistics
    #   @param pattern  - the glob pattern matching spectrum names we want
    #                     stats for.
    #   @return list of dicts containing name, underflows and overflows keys.
    #
    method spectrumStatistics {{pattern  *}} {
        set info [$self _request [$self _makeUrl specstats [dict \
            create pattern $pattern]]]
        return [dict get $info detail]
    }
    #-------------------------------------------------------------------------
    # treeparameter jackets
    
    ##
    # treeparameterCreate
    #   Make a new tree parameter.
    # @param name - name of the parameter.
    # @param low  - Low limit
    # @param high  - High limit.
    # @param bins  - number of recommended bins.
    # @param units - optional units (defaults to "").
    # No useful return value.
    #
    method treeparameterCreate {name low high bins {units ""}} {
        set qparams [dict create \
            name $name low $low high $high bins $bins units $units \
        ]
        $self _request  [$self _makeUrl /parameter/create $qparams]
    }
    ##
    #    treeparameterList
    #   List the tree parameters.
    # @param filter - GLOB filter on the names of the tree parameters listed.
    #    note that this defaults to * which lists all of them.
    #
    # @return list of dicts each dict descrbing a tree parameter with the keys:
    #    - name   - name of a parameter.
    #    - bins   - number of bins.
    #    - low    - Low limit.
    #    - high   - high limt.
    #    - units  - unts.
    #
    # @note the REST interface returns all parameter not just the tree parameters.
    #      we filter the non-tree parameters out.
    #
    method treeparameterList {{filter *}} {
        set raw [$self _request [$self _makeUrl                   \
            /parameter/list [dict create filter $filter]             \
        ]]
        set result [list]
        foreach param [dict get $raw detail] {
            if {[dict exists $param bins]} {
                lappend result $param;              # it's a tree parameter.
            }
        }
        
        return $result
        
    }
    ##
    #    treeparameterListNew
    #   Returns the list of tree parameters that were created via
    #    treeparameter -create by any means.
    # @return list of strings.
    #
    method treeparameterListNew {} {
        set raw [$self _request [$self _makeUrl /parameter/listnew [dict create]]]
        return [dict get $raw detail]
    }
    ##
    #    treeparameterSet
    #  Sets new values for all aspects of a tree parameter (treeparameter -set).
    #
    # @param name - name of the parameter.
    # @param bins - Suggested binning.
    # @param low  - Suggested low limit.
    # @param high - Suggested high limit.
    # @param units - optional units (defaults to "").
    #
    #  Nothing useful is returned.
    #
    method treeparameterSet {name bins low high {units ""} } {
        set pdict [dict create\
            name $name bins $bins low $low high $high units $units \
        ]
        $self _request [$self _makeUrl /parameter/edit $pdict]
    }   
    ##
    #    treeparameterSetInc
    #  Modify the increment of a parameter.  This requires some fancy footwork
    #  as the inc is derived from low, high and bins and is
    #   (high-low)/bins  We're going to set only bins  so bins = |(high-low)/inc|
    # @param name  - name of the parameter.
    # @param newinc  - New channel increment.
    #
    method treeparameterSetInc {name newinc} {
        set current [$self treeparameterList $name]
        if {[llength $current] ==1 } {
            set info [lindex $current 0]
            set low [dict get $info low]
            set high [dict get $info hi]
            set bins [dict get $info bins]
            set newbins [expr int(abs(($high-$low)/$newinc) + 0.5)]; #rounded.
            $self _request [$self _makeUrl \
                /parameter/edit [dict create name $name bins $newbins]  \
            ]
        } else {
            error "$name is not an unambiguous tree parameter name."
        }
    }
    ##
    #    treeparameterSetBins
    # Set the number of bins in a tree parameter.
    #
    # @param name - name of the parameter.
    # @param newbins - new bins value.
    #
    method treeparameterSetBins {name newbins} {
        set qdict [dict create name $name bins $newbins]
        $self _request [$self _makeUrl /parameter/edit $qdict]
    }
    ##
    #    treeparameterSetUnits
    #     Change the units of a tree parameter.  Note that currently there's
    #     no way to remove units (set them to an empty string), since an empty
    #     units string will just not change anything.
    #
    # @param name - name of the parameter.
    # @param units -Units name
    #
    method treeparameterSetUnits {name units} {
        if {$units eq ""} {
            error "Setting units name to '' is not supported"
        }
        set qdict [dict create name $name units $units]
        $self _request [$self _makeUrl /parameter/edit $qdict]
    }
    ##
    #    treeparameterSetLimits
    #    set the limit meta data on a tree parameter.
    #
    # @param name - name of the parameter.
    # @param low  - low limit.
    # @param high  - high limit.
    #
    method treeparameterSetLimits {name low high} {
        set qdict [dict create name $name low $low high $high]
        $self _request [$self _makeUrl /parameter/edit $qdict]
    }
    ##
    #    treeparameterCheck
    #  Return the tree parameter check flag for a single tree parameter.
    #  this is set if anybody changed the tree parameter definition since
    #  the program started.
    #
    # @param name - name to check
    #
    method treeparameterCheck {name} {
        set data [$self _request [$self _makeUrl /parameter/check \
                [dict create name $name]]]
        return [dict get $data detail]
    }
    ##
    #    treeparameterUncheck
    #  uncheck a tree paramter modification flag
    #
    # @param name - name to uncheck.
    #
    method treeparameterUncheck {name} {
        $self _request [$self _makeUrl /parameter/uncheck [dict create name $name]]
    }
    #    treeparameterVersion
    #
    #  Return the tree parameter version string.
    #
    # @return string.
    #
    method treeparameterVersion {} {
        set data [$self _request [$self _makeUrl /parameter/version [dict create]]]
        return [dict get $data detail]
    }
}