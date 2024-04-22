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
#include <string.h>
#include <stdexcept>


#ifdef WITH_MPI
#include <mpi.h>
#else
typedef int MPI_Datatype;
#endif


// Tag that identifes a messages as being TCL related:

static const int MPI_TCL_TAG  = 1;

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



// Book keeping for replies to a Tcl command:

typedef struct _ProcessResult { // Note rank will be the index of the item in the vector.
    int status;                 // Tcl status of replier.
    int remainingResult;        // Remainng characters to get.
    std::string result;         // Result string built up  from resposes from that rank.
} ProcessResult;


bool allRepliesIn(std::vector<ProcessResult>& replies) {
    for (auto& reply: replies) {
        if (reply.remainingResult > 0) {
            return false;                    // This one has more.
        }
    }

    return true;
}

void fillResponse(ProcessResult& result, MpiTclResultMsg& msg) {
    // If this is a new rank.. initialize remaining result:

    if (result.remainingResult == std::string::npos) {
        result.remainingResult = msg.resultSize;
    }
    // Figure how much data to append:

    size_t nbytes;
    if (result.remainingResult < MAX_TCL_CHUNKSIZE) {
        nbytes = result.remainingResult;
    } else {
        nbytes = MAX_TCL_CHUNKSIZE;
    }
    result.status = msg.status;
    result.result.append(msg.resultChunk, msg.resultChunk + nbytes);
    result.remainingResult -= nbytes;
}

// Ok all replies are in, get the status and set the interp result.

static int
getStatusFrom(CTCLInterpreter& interp, std::vector<ProcessResult>& replies) {
    int status = TCL_OK;
    size_t maxReply;
    size_t maxRank;
    for (int i = 0; i < replies.size(); i ++) {

        if (replies[i].status != TCL_OK) {
            interp.setResult(replies[i].result);
            return replies[i].status;
        } else {
            if (replies[i].result.size() > maxReply) {
                maxRank = i;
                maxReply = replies[i].result.size();
            }
        }
    }
    // Good status, set the result from replies[rank]

    interp.setResult(replies[maxRank].result);
    return TCL_OK;

}
// constructMpiTclStatus
//  Given that we just sprayed a command to the slave processes, 
//  Collect the responses from all of the slaves and construct/set a result and return
// a status as follows:
//  *  Pull in all replies from all processes.
//  * if any process returned other than TCL_ERROR, set the result from the first such and
//    retunr that status.
//  * If all of the processes returned TCL_OK, in a properly constructed app (and hopefully
//    SpecTcl is properly constructed, only one of the repliers will have a non-empty result)
//    Set the result from that one.
//

static int constructMpiTclStatus(CTCLInterpreter& interp) {
#ifdef WITH_MPI
    std::vector<ProcessResult> replies;

    // Build up the initial replies vector note that rank 0 is us and therefore done.
    // we just put in some non-zero value for the other remaining results so there's more to do:

    ProcessResult value;
    value.status = TCL_OK;
    value.remainingResult = 0;
    replies.push_back(value);
    value.remainingResult = std::string::npos;
    value.status = -100;   // Some non Tcl status.
    int num;
    if (MPI_Comm_size(MPI_COMM_WORLD, &num) != MPI_SUCCESS) {
        throw std::runtime_error("constructMpiTclStatus could not get comm size");
    }
    num -= 1;
    for (int i =0; i < num; i++) {
        replies.push_back(value);
    }

    while(!allRepliesIn(replies)) {
        MpiTclResultMsg msg;
        MPI_Status status;
        if(
            MPI_Recv(&msg, 1, getTclResultType(), MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status) 
            != MPI_SUCCESS
        ) {
            throw std::runtime_error("Failed to get a result message");
        }
        int from = status.MPI_SOURCE;
        fillResponse(replies[from], msg);

    }
    return getStatusFrom(interp, replies);
#else
    return TCL_OK;
#endif
}

// Static function to build words into a string:

static std::string wordsToString(std::vector<CTCLObject>& objv) {
    std::string result;
    for (auto& w : objv) {
        result += std::string(w);
        result += " ";
    }
    return result;
}

// Do a Tcl command in the MPI envirionment.
// this means spraying it to the rest of the app and
// gathering the replies...then constructing a result and status from those replies.
// Note we'll only get called in the MPI runtime so we don't check for that...since
// ExecCommand calls isMpiApp to determine if we should even be called.
//
// Assumption:  We are rank 0.
static int MPIExecCommand(CTCLInterpreter& interp, std::vector<CTCLObject>& words) {
#ifdef WITH_MPI
    std::string command = wordsToString(words);
    size_t len = command.size();
    size_t remaining = len + 1;     // Null terminator.
    int    thisChunk;
    int chunkstart = 0;
    while (remaining) {
        if (remaining > MAX_TCL_CHUNKSIZE) {
            thisChunk = MAX_TCL_CHUNKSIZE;
        } else {
            thisChunk = remaining;
        }

        // Constrcut the message and broadcast it:

        MpiTclCommandChunk chunk;
        chunk.commandLength = len + 1;           // Null terminator.
        memcpy(chunk.commandChunk, command.substr(chunkstart, thisChunk).data(), thisChunk);
        if (MPI_Bcast(&chunk, 1, getTclCommandChunkType(), 0, MPI_COMM_WORLD) != MPI_SUCCESS) {
            throw std::runtime_error("Failed to send command to slaves");
        }
        return constructMpiTclStatus(interp);
    }
#else   
    return TCL_OK;    
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