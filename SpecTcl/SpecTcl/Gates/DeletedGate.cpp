//  CDeletedGate.cpp
// For convenience, gates which are deleted are
// replaced with this special type of 'false' gate.
// the only difference between this and a  false gate is
// that the type string is -deleted-

//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//
//////////////////////////.cpp file/////////////////////////////////////////////////////

//
// Header Files:
//

#include "DeletedGate.h"                               

static const char* Copyright = 
"CDeletedGate.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CDeletedGate

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    std::string Type (  )
//  Operation Type:
//     Selector.
//
std::string 
CDeletedGate::Type() 
{

// Exceptions:  

  return std::string("-deleted-");
}
