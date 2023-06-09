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
/*!
  This is an abstract base class that is the base
  class for individual readout modules.
  This class provides support for a parameter
  map.  Parameter maps provide a mapping
  between module chnannel numbers and
  SpecTcl parameter ids.

*/

// Author:
//   Ron Fox
//   NSCL
//   Michigan State University
//   East Lansing, MI 48824-1321
//   mailto:fox@nscl.msu.edu
//
// Copyright 

#ifndef CMODULE_H  //Required for current class
#define CMODULE_H

//
// Include files:
//

                               //Required for base classes
#include "CSegmentUnpacker.h"
#include <TCLProcessor.h>        
#include <string> 
#include <vector>


class CModule  : public CSegmentUnpacker
{
private:
  

    std::vector<int> m_ParameterMap;  //!<  Maps channel # -> parameter id.  
   

public:
    //  Constructors and other canonical operations.

    CModule (const std::string& rName,CTCLInterpreter& rInterp); //!< Constructor.
    ~CModule ( ); //!< Destructor.
private:
    CModule (const CModule& rSource ); //!< Copy construction.
    CModule& operator= (const CModule& rhs); //!< Assignment.
    int operator== (const CModule& rhs) const; //!< == comparison.
    int operator!= (const CModule& rhs) const; //!< != comparison.
public: 

    
 public:
    
    void CreateMap (int nMapSize)   ;                    //!< Define an empty map.
    void MapElement (int nChannel, const std::string& sName)   ; //!< Make a  mapping.
    int Id (int nChannel)   ; 	                         //!< Translate map. 
    

};

#endif
