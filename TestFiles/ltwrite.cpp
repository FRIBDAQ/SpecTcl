/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2009.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

static const char* Copyright = "(C) Copyright Michigan State University 2010, All rights reserved";
#include <config.h>
#include "TapeFile.h"
#include "Exception.h"
#include <Iostream.h>
#include <errno.h>
#include <stdio.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <TCLApplication.h>
#include <stdlib.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


void Usage()
{
  cerr << "Usage:\n";
  cerr << "   ltwrite device file [file...]\n";
  cerr << "\n Writes a labelled tape containing the files labelled TEST\n";
}

int
main(int nArgs, char** pArgs)
{
  nArgs--;
  pArgs++;
  if(nArgs < 2) {
    Usage();
    exit(-1);
  }
  char* pTape = pArgs[0];
  nArgs--;
  pArgs++;

  // Open access to the tape volume and initialize it:
  // The blocksize will be 8192 for all files:

  try {
    CTapeFile::Initialize(std::string(pTape),
			  std::string("TEST"));
    
    CTapeFile tape(pTape);
    
    tape.setBlocksize(8192);
    
    // Now for each file, create it and write it:
    //
    for(; nArgs > 0; nArgs--, pArgs++) {
      int fd = open(pArgs[0], 
		    O_RDONLY);
      if(fd < 0) {
	cerr << pArgs[0] << endl;
	perror("Failed to open input file");
	exit(errno);
      }
      tape.Open(std::string(pArgs[0]), kacWrite | kacCreate);
      char buffer[8192];
      while(read(fd, buffer, sizeof(buffer))) {
	tape.Write(buffer, sizeof(buffer));
      }
      tape.Close();
    }
  }
  catch(CException& rExcept) {
    cerr << rExcept.ReasonText() << endl;
  }
}
void *gpEventSource;
void *gpTCLApplication(0);
