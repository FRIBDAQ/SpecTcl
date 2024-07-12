/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins
             NSCL
             Michigan State University
             East Lansing, MI 48824-1321

 */
//  CSpectrum1D.h:
//
//    This file defines the CSpectrum1D class.
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:fox@nscl.msu.edu
//
//  Copyright 1999 NSCL, All Rights Reserved.
//
/////////////////////////////////////////////////////////////
/*
  Change Log:
  $Log$
  Revision 5.2  2005/06/03 15:19:24  ron-fox
  Part of breaking off /merging branch to start 3.1 development

  Revision 5.1.2.1  2004/12/21 17:51:25  ron-fox
  Port to gcc 3.x compilers.

  Revision 5.1  2004/11/29 16:56:08  ron-fox
  Begin port to 3.x compilers calling this 3.0

  Revision 4.2.4.1  2004/10/27 12:38:40  ron-fox
  optimize performance of Spectrum1DL histogram increments.  Total
  performance gain was a factor of 2.8.  The 'unusual' modifications
  are documented via comments that indicate they were suggested by profile
  data.

  Revision 4.2  2003/04/01 19:53:46  ron-fox
  Support for Real valued parameters and spectra with arbitrary binnings.

*/

#ifndef SPECTRUM1DL_H  //Required for current class
#define SPECTRUM1DL_H

#include "Spectrum1d.h"       // For compatibility.



#endif