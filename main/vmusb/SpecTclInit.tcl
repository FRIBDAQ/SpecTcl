#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2005.
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


# (C) Copyright Michigan State University 2014, All rights reserved 
set DisplayMegabytes 200
set ParameterCount   256
set EventListSize    1
set ParameterOverwriteAction query
set SpectrumOverwriteAction  query
set splashImage $SpecTclHome/doc/hh00706_.jpg;   # SpecTcl icon for splashscreen.


#  Load the mpi::Send stuff:

load [file join $SpecTclHome lib libMPITclPackage.so]
package require mpi
