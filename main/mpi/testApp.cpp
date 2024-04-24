// This is a test application for MPI Tcl handling.
// Rank 0 is just an interpreter. we define
// a MPITcl command that in rank 1 sets a non-empty result
// and in other ranks an empty result.
// Rank1 and higher get commands via the TclPump and 
// sit in an event loop with stdin closed.
// If we invoke "testCommand1"  we should get a hello word like 
// response
// if we invoke "testCommand2" we should get an error response
// with a message about that.

#include <config.h>
#include "TclPump.h"
#include "MPITclCommand.h"
#include "MPITclCommandAll.h"
#include <tcl.h>
#include "TCLInterpreter.h"
#include "TCLObject.h"
#include <iostream>
#ifdef WITH_MPI
#include <mpi.h>
#endif
CTCLInterpreter* pInterpreter;
int savedArgc;
char** savedArgv;

// Wrapped helper1:

class Helper1 : public CTCLObjectProcessor {
public:
    Helper1(CTCLInterpreter& interp, const char* cmd) :
        CTCLObjectProcessor(interp, cmd, true) {}
    int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)  {
        std::cerr << " Invoked! " << myRank() << std::endl;
        if (myRank() == 1) {
            std::cerr << "Setting result in rank 1\n";
            interp.setResult("Hello world\n");
        }
        return TCL_OK;
    }
};

class Helper2 : public CTCLObjectProcessor {
public:
    Helper2(CTCLInterpreter& interp, const char* cmd) : 
    CTCLObjectProcessor(interp, cmd, true) {}
    int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv) {
        std::cerr <<"Helper2 in " << myRank() << std::endl;
        if (myRank() ==1) {
            interp.setResult("Hello world from an MPI application");
            return TCL_OK;
        } else {
            interp.setResult("Planned Error.");
            return TCL_ERROR;
        }
    }
};

class Helper3 : public CTCLObjectProcessor {
public:
    Helper3(CTCLInterpreter& interp, const char* cmd) : 
        CTCLObjectProcessor(interp, cmd, true) {}
    int operator() (CTCLInterpreter& interp, std::vector<CTCLObject>& objv) {
        std::cerr << "Running in " << myRank() << std::endl;
        return TCL_OK;
    }
};

// Create the wrapped interpreter and register
// testCommand1 and testCommand2.
//  helper1 is the actual for testCommand1 and helper2 for testCommmand2.
int appMain(Tcl_Interp* pInterp) {
    pInterpreter = new CTCLInterpreter(pInterp);

    new CMPITclCommand(
        *pInterpreter, "testCommand1",
        new Helper1(*pInterpreter, "testCommand1")
    );
    new CMPITclCommand(
        *pInterpreter, "testCommand2",
        new Helper2(*pInterpreter, "testCommand2")
    );
    new CMPITclCommandAll(*pInterpreter, "testCommand3",
        new Helper3(*pInterpreter, "testCommand3"));
    if (isMpiApp()) {
#if WITH_MPI
        int threadModel;
        MPI_Init_thread(&savedArgc, &savedArgv, MPI_THREAD_MULTIPLE, &threadModel);
#endif
        std::cerr << "Running as an MPI app. my rank: " 
            << myRank() << std::endl;
        if (myRank() > 0) {
            // Close stdin and start the pump:
            int mode;
            auto chan = Tcl_GetChannel(pInterp, "stdin",  &mode);
            Tcl_UnregisterChannel(pInterp, chan);
            startCommandPump(*pInterpreter);
            //Enter the event loop -- I think this is ok....

            while (true) {
                Tcl_ReapDetachedProcs();
                struct Tcl_Time timeout;
                timeout.sec = 1000;
                timeout.usec = 0;
                if (Tcl_WaitForEvent(&timeout)) {
                    std::cerr << "Event loop exiting\n";
                    Tcl_Exit(-1);
                }
                while (Tcl_DoOneEvent(TCL_ALL_EVENTS | TCL_DONT_WAIT))
                    ;
            }
        }
    } else {
        std::cerr << "Running as a serial app\n";
    }
    return TCL_OK;
}
// Just start up Tcl.
// 
int main(int argc, char**argv) {
    savedArgc = argc;
    savedArgv = argv;
    Tcl_Main(argc, argv, appMain);
}