// DiskFile.h:
//    This file defines the CDiskFile class.
//    This is essentially the CFile class using it's
//    default implementations for everything.
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto: fox@nscl.msu.edu
//
// (c) Copyright 1999 NSCL, ALl rights reserved
//
////////////////////////////////////////////////////////////////

#ifndef __DISKFILE_H
#define __DISKFILE_H

#ifndef __FILE_H
#include "File.h"
#endif


#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

class CDiskFile : public CFile {
public:
  // All we need are relay constructors:
  //

  CDiskFile() : CFile() {}
  CDiskFile(UInt_t nFd) : CFile(nFd) {}
  virtual ~CDiskFile() {}
  CDiskFile(const CDiskFile& rhs) : CFile(rhs) {}

  CDiskFile& operator=(const CDiskFile& rhs) {
    if(this != &rhs) CFile::operator=(rhs);
    return *this;
  }
  Int_t operator==(const CDiskFile& rhs) {
    return CFile::operator==(rhs);
  }
  Int_t operator!=(const CDiskFile& rhs) {
    return CFile::operator!=(rhs);
  }


};

#endif
