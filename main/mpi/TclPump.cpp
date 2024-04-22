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
#include <config.h>
#include "TclPump.h"
#include "TCLInterpreter.h"
#include "TCLObject.h"
#include <stdlib.h>

#include <string>
#include <stdexcept>


#ifdef WITH_MPI
#include <mpi.h>
#else
typedef int MPI_Datatype;
#endif


/**
 * MPI's fixed size data types can be a pain in the a$$.  The problem is that
 * when we receive replies from transmitted commands, the result data size is unbounded.
 * In most cases it's small but, for some commands it can be large (e.g. parameter -list for a
 * Dirk-like system).  We therefore use the custom MPI struct below to receive the 
 * status and result in chunks.
 *   
 * In the struct below:
 *   *  status - will always contain the status of the command (hopefully TCL_OK).
 *   *  resultSize - will always contain the total size of the result.  The receiver will keep
 *                   reading until we have all of the bytes.
 *   * resultChunk - is a buffer that can contain the first/only/next chunk of the result string.
*/
#define MAX_TCL_CHUNKSIZE 500
typedef struct _MpiTclResultMsg {
    int status;     // Tcl status e.g. hopefully TCL_OK.
    int resultSize; // Total number of bytes in the stringified result.
    char resultChunk[MAX_TCL_CHUNKSIZE];  // Result or chunk.
} MpiTclResultMsg, *pMpiTclResultMsg;
MPI_Datatype TclResultType;


/** Similarly, when we send a command, we may need to send it in chunks because the length of
 * a command is unbounded (after all, this could be a script consider, for example sending the entire
 * tree gui).
 * We use the same concept:
*/

typedef struct _MpiTclCommandChunk {
    int commandLength;                     // Length of the command.
    char commandChunk[MAX_TCL_CHUNKSIZE];  // command chunk.
} MpiTclCommandChunk, *pMpiTclCommandChunk;
MPI_Datatype TclCommandChunk;

static bool typesRegistered(false);

/* Local function to register our types if not done yet: */
static void registerCustomTypes() {
#ifdef WITH_MPI
    if (typesRegistered) return;
    if (isMpiApp()) {
        { 
            // MPITclResultMsg :
            MPI_Aint offsets[3] = {offsetof(MpiTclResultMsg, status), 
                offsetof(MpiTclResultMsg, resultSize),
                offsetof(MpiTclResultMsg, resultChunk)
                };
            int lengths[3] = {1,1, MAX_TCL_CHUNKSIZE};
            MPI_Datatype dtypes[3] = {
                MPI_INTEGER, MPI_INTEGER, MPI_CHARACTER
            };
            if (MPI_Type_create_struct(3, lengths, offsets, dtypes, &TclResultType) != MPI_SUCCESS) {
                throw std::runtime_error("Unable to create TclResultType MPI custom type.");
            }
            if (MPI_Type_commit(&TclResultType) != MPI_SUCCESS) {
                throw std::runtime_error("Unable to commit TclResultType MPI custom type");
            }

        {
            // MpiTclCommandChunk.

            MPI_Aint offsets[2] = {offsetof(MpiTclCommandChunk, commandLength),
                offsetof(MpiTclCommandChunk, commandChunk)
            };
            int lengths[2] = {1, MAX_TCL_CHUNKSIZE};
            MPI_Datatype dtypes[2] = {MPI_INTEGER, MPI_CHARACTER};

            if (MPI_Type_create_struct(2, lengths, offsets, dtypes, &TclCommandChunk) != MPI_SUCCESS) {
                throw std::runtime_error("Unable to create TclCommandChunk MPI custom data type");
            }
            if (MPI_Type_commit(&TclCommandChunk) != MPI_SUCCESS) {
                throw std::runtime_error("Unable to commit TclCommandChunk MPI custom data type");
            }
        }    
            

        }
    }
    typesRegistered = true;          // Only reigster once.
#endif
}

/*
   Local function to return the various data types:
*/
static MPI_Datatype getTclResultType() {
    registerCustomTypes();
    return TclResultType;
}
static MPI_Datatype getTclCommandChunkType() {
    registerCustomTypes();
    return TclCommandChunk;
}


/** 
 * @return bool -true if we are not only built with MPI but were run with mpirun or mpiexec
 * 
 *    If run with mpirun or mpi exec:
 * If openmpi OMPI_COMM_WORLD_SIZE will be in the env, if MPICH, MPI_RANK will be: 
*/

bool isMpiApp() {
#ifdef WITH_MPI
    return getenv("OMPI_COMM_WORLD_SIZE") || getenv("PMI_RANK");
#else  
    return false;
#endif
}


/**
 *  Execute a tcl command either locally or by spraying it out to the
 *  world and collectin the return stuff.
 * @param rInterp - the local interpreter.
 * @param words  - The command words.
 * @return int - Status of the command (e.g. TCL_OK).
 * 
*/
int ExecCommand(CTCLInterpreter& interp, std::vector<CTCLObject>& words) {
    if (isMpiApp()) {
        return TCL_OK;
    } else {
        // Local command evaluation:

        std::vector<Tcl_Obj*> objv;
        for (size_t i = 0; i < words.size(); i++) {
            objv.push_back(words.at(i).getObject());
        }
        return Tcl_EvalObjv(interp.getInterpreter(), objv.size(), objv.data(), TCL_EVAL_GLOBAL);
    }
}