//
// TCLTest.cpp:
//    Test of the TCL/TK object stuff:
//
// 
#include <tk.h>

#include "TCLApplication.h"
#include "TCLVariable.h"
#include "TCLProcessor.h"
#include <iostream.h>
#include <string>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>


extern "C" {
  char* tcl_RcFileName;
}

class MyEcho : public CTCLProcessor
{
public:
  MyEcho() :
    CTCLProcessor("MyEcho", (CTCLInterpreter*)kpNULL)
  {}
 
  virtual int operator()(CTCLInterpreter& rInt,
			 CTCLResult&      rResult,
			 int              nArgs,
			 char**            pArgs);
};

class MyApp : public CTCLApplication
{

public:
  virtual int operator()();
protected:
  void RegisterCommands();
};

MyApp app;
CTCLApplication* gpTCLApplication = &app;

CTCLVariable Rcfile(std::string("tcl_rcFileName"),
		    kfFALSE);

MyEcho echocmd;

int MyApp::operator()()
{
  Rcfile.Bind(getInterpreter());
  Rcfile.Set("~/.wishrc", TCL_GLOBAL_ONLY);

  echocmd.Bind(getInterpreter());
  echocmd.Register();


  return TCL_OK;
}


int
MyEcho::operator()(CTCLInterpreter& rInt,
		   CTCLResult&      rResult,
		   int              nArgs,
		   char**            pArgs)
{
  for(int i = 0; i < nArgs; i++) {
    cerr << pArgs[i] << " ";
  }
  cerr << endl;

  return TCL_OK;
}









