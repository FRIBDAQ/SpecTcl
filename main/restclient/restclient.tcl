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
        #puts $rawData;      # Uncomment to debug reply errors.
        set parseOk [catch {
            set json [json::json2dict $rawData]
        } msg]
        http::cleanup $token
        
        #  Could be errors parsing the JSON or errors flagged in the
        #  json:
        
        if {$parseOk} {
            error "JSON Parse Failed: $msg : $::errorInfo"
        }

        set status [dict get $json status]
        if {$status ne "OK"} {
            error "Request failed by SpecTcl: $status :  [dict get $json detail]"
        }
        
        # Success return what interests the caller.
        
        return $json
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
        set qparams [list]
        foreach spectrum $spectra {
            lappend qparams  spectrum $spectrum
        }
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
}