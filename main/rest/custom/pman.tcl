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
# @file pman.tcl
# @brief Supports the 'pman' command via REST.
# @author Ron Fox <fox@nscl.msu.edu>
#
package require json::write
Direct_Url /spectcl/pman SpecTcl_pman
#-----------------------------------------------------------------------------
# utilities.

namespace eval SpecTcl {}

##
# SpecTcl::_pipelineToJson
#   Turn a single pipeline specification into a JSON object.
#
# @param spec - 2 element list from e.g. pmain current
# @return json encoded object with name and processors attributes.
#
proc SpecTcl::_pipelineToJson {spec} {
    set name [lindex $spec 0]
    set procs [SpecTcl::_jsonStringArray [lindex $spec 1]]
    
    return [json::write object                                \
        name [json::write string $name]                       \
        processors $procs                                     \
    ]
}

#-------------------------------------------------------------------------------
#    REST interface.
##
# SpecTcl_pman/create
#  Create a new SpecTcl analysis pipeline with the given name.
#  The pipeline is initially empty.
#
# @param name - name of the new pipeline.
#
proc SpecTcl_pman/create {name} {
    set ::SpecTcl_pman/create application/json
    
    set status [catch {
        pman mk $name
    } msg]
    if {$status} {
        return [SpecTcl::_returnObject  \
            "'pman mk' command failed"  \
            [json::write string $msg]   \
        ]
    }
    return [SpecTcl::_returnObject OK]
}
##
# SpecTcl_pman/ls
#   Lists the names of the analysis pipelines that currently are defined.
#
# @param pattern -optional glob pattern that restricts the names
#                 to those that match it.
#
proc SpecTcl_pman/ls {{pattern *}} {
    set ::SpecTcl_pman/ls application/json
    
    set status [catch {
        pman ls $pattern
    } info]
    if {$status} {
        return [SpecTcl::_returnObject \
            "'pman ls' command failed"  \
            [json::write string $info]  \
        ]
    }
    return [SpecTcl::_returnObject OK                               \
        [json::write array {*}[SpecTcl::_jsonStringArray $info]]    \
    ]
}
##
# SpecTcl_pman/current
#    Provide information about the current event processor.
# @return detail is an object with two attributes:
#      - name  - name of the pipeline.
#      - processors - array of event processor names.
#
proc SpecTcl_pman/current { } {
    set  ::SpecTcl_pman/current application/json
    set status [catch {
        pman current
    } info]
    if {$status} {
        return [SpecTcl::_returnObject "'pman current' command failed" \
            [json::write string $info]                                 \
        ]
    }
    return [SpecTcl::_returnObject OK [SpecTcl::_pipelineToJson $info]]
}
##
# SpecTcl_pman/lsall
#
#  Return a list of all pipelines and their event processors.
#
# @param pattern - Optional pattern with glob characters that restricts
#            the pipelines listed.
# @return detail is an array of objects such as SpecTcl_pman/current returns
#
proc SpecTcl_pman/lsall {{pattern *}} {
    set ::SpecTcl_pman/lsall application/json
    
    set status [catch {
        pman ls-all $pattern
    } info]
    if {$status} {
        return [SpecTcl::_returnObject "'pman ls-all' command failed" \
            [json::write string $info]                                \
        ]
    }
    set result [list]
    foreach pipeline $info {
        lappend result [SpecTcl::_pipelineToJson $pipeline]
    }
    return [SpecTcl::_returnObject OK                                \
        [json::write array {*}$result]                              \
    ]
}
##
# SpecTcl_pman/lsevp
#   Lists the names of the event processors that have been registered with
#   SpecTcl.
#
# @param pattern -optional glob pattern that restricts the set listed.
# @return detail is a JSON array of event processor names.
#
proc SpecTcl_pman/lsevp {{pattern *}} {
    set ::SpecTcl_pman/lsevp application/json
    
    set status [catch {
        pman ls-evp $pattern
    } info]
    if {$status} {
        return [SpecTcl::_returnObject "'pman ls-evp' command failed"   \
            [json::write string $info]
        ]
    }
    return [SpecTcl::_returnObject OK                                \
        [SpecTcl::_jsonStringArray $info]                         \
    ]
}
##
# SpecTcl_pman/use
#
#  Select an event processing pipeline for use.  The
#  pipline is used, from then on to process events from raw form
#  to parameters.
#
# @param name - name of the pipeline to use.
#
proc SpecTcl_pman/use {name} {
    set ::SpecTcl_pman/use application/json
    
    set status [catch {
        pman use $name
    } msg]
    if {$status} {
        return [SpecTcl::_returnObject "'pman use' command failed"    \
            [json::write string $msg]                                 \
        ]
    }
    return [SpecTcl::_returnObject OK]
}
##
# SpecTcl_pman/add
#
#  Add an event processor to the end of a pipeline.
#
# @param pipeline  - name of the pipeline.
# @param processor - Name of the event processor.
#
proc SpecTcl_pman/add {pipeline processor} {
    set ::SpecTcl_pman/add application/json
    
    set result [catch {
        pman add $pipeline $processor
    } msg]
    if {$result} {
        return [SpecTcl::_returnObject "pman 'add' command failed" \
            [json::write string $msg]                              \
        ] 
    }
    return [SpecTcl::_returnObject OK]
}
##
# SpecTcl_pman/rm
#    Remove a named event processor from a processing pipeline.
# @param pipeline - pipeline name.
# @param processor - name of the processor to remove.
#

proc SpecTcl_pman/rm {pipeline processor} {
    set ::SpecTcl_pman/rm application/json
    
    set status [catch {
        pman rm $pipeline $processor
    } msg]
    if {$status} {
        return [SpecTcl::_returnObject "'pman rm' command failed" \
            [json::write string $msg]                             \
        ]
    }
    return [SpecTcl::_returnObject OK]
}
##
# SpecTcl_pman/clear
#   Remove all event processors from a pipeline.
#
# @param  pipeline - name of the pipeline.
#
proc SpecTcl_pman/clear {pipeline} {
    set ::SpecTcl_pman/clear application/json
    
    set status [catch {
        pman clear $pipeline
    } msg ]
    if {$status} {
        return [SpecTcl::_returnObject "'pman clear' command failed" \
            [json::write string $msg]                                \
        ]
    }
    return [SpecTcl::_returnObject OK]
}
##
# SpecTcl_pman/clone
#    Duplicate an existing pipeline
#
# @param source  - the pipeline that will be cloned.
# @param new     - the name of the pipeline that will be created.
#
proc SpecTcl_pman/clone {source new} {
    set ::SpecTcl_pman/clone application/json
    
    set status [catch {
        pman clone $source $new
    } msg]
    if {$status} {
        return [SpecTcl::_returnObject "'pman clone' command failed" \
            [json::write string $msg]                                \
        ]
    }
    return [SpecTcl::_returnObject OK]
}