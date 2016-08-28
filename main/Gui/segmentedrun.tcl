##
#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2013.
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
#	     East Lansing, MI 48824-1321

##
# @file segmentedrun.tcl
# @brief Support to analyze segmented runs.
#
# Motivation:
#    NSCLDAQ's event logger segments runs into 2GB chunks.  This allows runs to 
#    exist on file systems that limit file-size to 2GB.  It also allows NFS2
#    to serve up these file regardless of the capabilities of the underlying
#    file system.
#
#    This file contains software that recognizes when a user is asking to reply
#    a single file that is part of a segmented run and allows one to process
#    all of the files in that run.  Note that this is done by generating
#    a cat command that can be attached as a pipe data source so the merged
#    data are never acdstually on disk.  This not only saved disk space
#    but allows for operation in the same restrictive environments that motivated
#    the 2GB file limit.
#

package provide segmentedrun 1.0

##
# listRunFiles
#    Given an efvent file that follows the NSCLDAQ file naming convention:
#    run-nnnn-ss.evt, returns an ordered list of all of the event files
#    segments that are in the same run.  For a single segment run, only the
#    input file name will be returned.
#
# @param  file - path to the selected file.
# @return list - ordered list of all file segments in the run.
# @note   NSCLDAQ's naming convention ensures that an alphabetic sort of 
#         the filenames will produce segments in chronological order of acquisition
#         under the assumption (we make) that there are 100 or fewer segments in 
#         a run.
#         
#
proc listRunFiles {file} {
    # Break the filename up into its path components
    # dir - the directory path.
    # ext - The file extension
    # name- the filename (minus the extension).


    set dir [file dirname $file]
    set name [file tail $file]
    set ext  [file extension $file] 
    set name [join [lrange [split $name .] 0 end-1] .]

    # Construct a glob pattern to search for all files in this run.
    #   name is of the form run-nnnn-ss where the segment name - ss needs to be
    #   replaced with *:
    #
    set globName [string replace $name end-1 end *]
    set globPattern [file join $dir $globName$ext]

    # Return the sorted files that match the glob pattern.  There must be at 
    # least one if $file exists to begin with.
    # 
    set fileList [glob $globPattern]
    return [lsort -increasing $fileList]
}