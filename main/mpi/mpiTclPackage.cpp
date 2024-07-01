/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerizza
             Simon Giraud
             Aaron Chester
             Jin Hee Chang
	     FRIB
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file mpiTclPackage.cpp
 *  @brief Initialization for the MPITcl package.
 */


#include <tcl.h>
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <TCLVariable.h>
#include <Globals.h>
#include <TclPump.h>


static const char*  version="1.0";
extern "C" {

    /**
     * Mpitcl_init
     *    Initialize the MPITcl package:
     *   - Create the mpi namespace.
     *   - Populate the mpi namespace with the following variables:
     *       *  mpi::environment - mpi or serial.
     *       *  mpi::rank - our rank numbger or 0 if there is none.
     *       *  mpi::role - Process role -  from rank.
     *   - Define commands in the MPI namespace:
     *       mpi::send and mpi::bcast.
     */

    int Mpitclpackage_Init(Tcl_Interp* pInterpreter) {
        CTCLInterpreter* pInterp = new CTCLInterpreter(pInterpreter);  // Tcl++ interp wrapper.
        int status;
        Tcl_CreateNamespace(pInterpreter, "mpi", nullptr, nullptr);
        

        // Set the execution environment:

        CTCLVariable executionEnvironment(pInterp, "mpi::environment", TCLPLUS::kfFALSE);
        executionEnvironment.Set(isMpiApp() ? "mpi" : "serial");

        // Figure out our rank:

        CTCLObject rank;
        rank.Bind(pInterp);
        rank = myRank();
        CTCLVariable rankVar(pInterp, "mpi::rank", TCLPLUS::kfFALSE);
        rankVar.Set(std::string(rank).c_str());

        // Now the role:

        std::string role;
        switch (myRank()) {
        case MPI_ROOT_RANK:
            role = "root";
            break;
        case MPI_EVENT_SINK_RANK:
            role = "event-sink";
            break;
        default:
            role = "worker";
        }
        CTCLVariable roleVar(pInterp, "mpi::role", TCLPLUS::kfFALSE);
        roleVar.Set(role.c_str());


        // IF all went well, publish the package:

        status = Tcl_PkgProvide(pInterpreter, "mpi", version);
        if (status != TCL_OK) {
            pInterp->setResult("Failed to provide the 'mpi' package");
            return status;
        }

        return TCL_OK;
    }
}
