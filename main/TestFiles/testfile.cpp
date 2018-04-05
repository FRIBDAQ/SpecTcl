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
static const char* Copyright = "(C) Copyright Michigan State University 2010, All rights reserved";
//
//   Simple test program to create a test event file.
//   Use as follows:
//        testfile numblocks filename
//
//   numblocks of 8192 byte records will be written to filename.
//
//
// Author:
//   Ron Fox
//   NSCL
//   Michigan State University
//   East Lansing, MI 48824-1321
//  
//
static const  char* pCopyright=
"testfile.cpp - (c) Copyright NSCL 1999, all rights reserved\n";

#include <config.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <histotypes.h>
#include "TestFile.h"
#include "GaussianDistribution.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <TCLApplication.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

static const UInt_t nProgressCounter=100; // progress counter rollover.
static const UInt_t nRecordSize     = 8192; // Size of each record in the file.

void Usage() {
  cerr << "Usage: " << endl;
  cerr << "   testfile NumBlocks FileName" << endl;
}
int main(Int_t argc, Char_t** pArgs)
{
  if(argc != 3) {
    Usage();
    exit(-1);
  }
  cout << pCopyright;

  //  Get file size and name.

  UInt_t  nRecords = atoi(pArgs[1]);
  Char_t* pName    = pArgs[2];

  Int_t fd = creat(pName, 0666);
  if(fd < 0) {
    perror("Failed to create output file.");
    exit(errno);
  }
  
  // Create the test file:

  CGaussianDistribution d1(512.0, 128.0, 1024.0);
  CGaussianDistribution d2(256.0, 64.0,  1024.0);
  CGaussianDistribution d3(128.0, 32.0,  1024.0);
  CGaussianDistribution d4( 64.0, 16.0,  1024.0);
  CGaussianDistribution d5( 32.0,  8.0,  1024.0);

  CTestFile fT;
  fT.AddDistribution(d1);
  fT.AddDistribution(d2);
  fT.AddDistribution(d3);
  fT.AddDistribution(d4);
  fT.AddDistribution(d5);

  fT.Open("Who Cares", kacRead);

  for(UInt_t block = 0; block < nRecords; block++) {
    Char_t Buffer[nRecordSize];
    fT.Read(Buffer, nRecordSize);
    ssize_t status = write(fd, Buffer, sizeof(Buffer));
    if (status != sizeof(Buffer)) {
      perror("Failed to write a block to the test file");
      exit (-1);
    }
    if( (block % nProgressCounter) == 0) {
      cout << block << '\r';
      cout.flush();
    }
  }
  close(fd);
}
void* gpEventSource;
void* gpTCLApplication(0);
