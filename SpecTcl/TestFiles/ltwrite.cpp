#include "TapeFile.h"
#include "Exception.h"
#include <iostream.h>
#include <errno.h>
#include <stdio.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

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
