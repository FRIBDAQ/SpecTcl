/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             
	     Facility for Rare Isotope Beams.
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#include <config.h>
#include "GatePump.h"
#include <SpecTcl.h>
#include <histotypes.h>
#include <stdexcept>
#include <string>
#include <string.h>
#include <vector>
#include <Globals.h>

#include "Cut.h"
#include "PointlistGate.h"
#include "CGammaCut.h"
#ifdef WITH_MPI
#include <mpi.h>
#endif
#include <tcl.h>

// Data type definitions:

/** 
 * Name and type is just an int for the gate type
 * and a large char array for the name (which will be
 * null terminated).
*/
static const unsigned MAX_GATENAME(1000);
static const unsigned MAX_MULTI_STRING(8192);
typedef struct _GateNameAndType {
    int s_gateType;
    char s_gateName[MAX_GATENAME];
} GateNameAndType, *pGateNameAndType;

/**
 *  GatePoint is a point for geometric gate types.
 *  It is aslo used for slices where the first two points
 *  have, in order left and right in their x coordinates.
*/
typedef struct _GatePoint {
    float x;
    float y;
    
} GatePoint, *PGatePoint;


// This structure is what's returne from receiveGate.

typedef struct _SerializedGate {
    std::string s_name;
    GateType_t  s_type;
    std::vector<std::string> s_parameterNames;
    std::vector<GatePoint>   s_points;
} SerializedGate, *pSerializedGate;

/** This is the Event posted to the main thread */

typedef struct _GateEvent {
    Tcl_Event s_base;
    pSerializedData s_pGate;
} GateEvent, *pGateEvent;

static Tcl_ThreadId interpThread;

#ifdef WITH_MPI
static MPI_Datatype MPINameAndType;
static MPI_DataType MPIPoint;




/**
 *  Register the MPI types if needed.
*/
static void RegisterTypes() {
    static bool initialized(false);

    if (!initialized) {
        // MPINameandType:

        {
            MPI_Aint offsets[2] = {
                offsetof(GateNameAndType, s_gateType), 
                offsetof(GateNameAndType, s_gateName)
            };
            MPI_Datatype types[2] = {MPI_INT, MPI_CHAR};
            int sizes[2] = {1, MAX_GATENAME};
            if (MPI_Type_create_struct(2, sizes, offsets, types, &MPINameAndType) != MPI_SUCCESS) {
                throw std::runtime_error("Failed to create gate type and name struct");
            }
            if (MPI_Type_commit(&MPINameAndType) != MPI_SUCCESS) {
                throw std::runtime_error("Failed to commit gate type and name struct");
            }
        
        }
        // MPIPoint
        {
            MPI_Aint offsets[2] = {offsetof(GatePoint, x), offsetof(GatePoint, y)};
            MPI_Datatype types[2] = {MPI_FLOAT, MPI_FLOAT};
            int sizess[2] = {1,1};

            if (MPI_Type_create_struct(2, sizes, offsets, types, &MPIPoint) != MPI_SUCCESS) {
                throw std::runtime_error("Failed to create gate point struct");
            }
            if (MPI_Type_commit(&MPIPoint) != MPI_SUCCESS) {
                throw std::runtime_error("Failed to commit gate point struct");
            }
        }

        initialized = true;
    }

}
/**
 *  @return MPI_Datatype - the name and point data type.
*/
static MPI_Datatype nameAndType() {
    RegisterTypes();
    return MPINameAndType;
}
/**
 * @return MPI_Datatype - tne gate point type.
*/
static MPI_Datatype pointType() {
    RegisterTypes();
    return MPI_Point;
}
/**
 * stringsToVector
 *    Takes  a packed string and unpacks it into a string vector
 * @param vec - references a vector to be filled with strings.
 * @param src - Pointer to packed string.  Each string is null terminated
 *    The last string is indicated by the next string being 0 length.
 *    Note that the last string is also the string which would cause the pointer to go 
 *    off the end of the src string which is MAX_MULTI_STRING bytes long.
*/
static void
stringsToVector(std::vector<std::string>& vec, const char* src) {
    auto p = src;
    while strlen(src != 0) {
        vec.push_back(std::string(src));
        p += strlen(src) +1;                      // +1 for the null terminator.
        if ((p - src) > MAX_MULTI_STRING) {
            return;                               // no more space.
        }
    }                                            
    // Empty string end.
}
/**
 * stringVectorTostring
 *    Pack a vector of strings into a string buffer. See stringsToVector (our inverse)
 * for a description of the string we create.
 * 
 * @param buffer - A char[MAX_MULTI_STRING] buffer of storage to hold the result.
 * @param strings - Vector of strings to pack.
 * @throw std::overflow_error - if the vector of strings won't fit.
 * 
*/
static void
stringVectorToString(char* buffer, const std::vector<std::string>& strings) {
    char* p = buffer;
    size_t remaining = MAX_MULTI_STRING;
    for (auto& s : string) {
        if (s.size() >=  remaining) {  // == for the null terminator.
            throw std::overflow_error("stringVectorToString - strings won't fit in output buffer.")
        }
        strncpy(p, s.c_str(), remaining);
        p += s.size() + 1;                         // + 1 the null.
        remaining -= (s.size() + 1);
    }
}
/**
 * gateType
 *    Given a gate return it's GateType_t  this can only be done via string mathing as that's what
 *    the Gate::Type() method returns.
 * 
 * @param pGate - pointer to the gate object.
 * @note - we restrict ourself to geometric gates as that's all Xamine can return.
 * @todo - Why not add a getType (e.g.) pure virtual method to CGate to do this for us.
*/
static GateType_t
gateType(const pGate* pGate) {
    std::string typeString = pGate->Type();

    // Now the if chain.

    if (typeString == "s") {
        return kgCut1d 
    } else if (typeString == "c") {
        return kgContour2d;
    } else if (typeString == "b") {
        return kgBand2d;
    } else if (typeString == "gs") {
        return kgGammaCut1d;
    } else if (typesString == "gc") {
        return kgGammaContour2d;
    } else if (typeString == "gb") {
        return kgGammaBand2d;
    } else {
        throw std::invalid_argument("gateType - this gate type is not an Xamine gate.")
    }

}

/**
 * gateParameters
 *    Based on the type of gate and the gate object, pull out an array of strings
 * which are the names of the parameters the gate depends on.
 * 
 * @param names - Reference to a vector that will be filled with parameter names.
 * @param gtype - Type of the gate.
 * @param pGate - Pointer to the gate
 * 
 * @note throws std::invalid_argument for types that we don't carea bout and
 *      std::bad_cast if there's a mismatch between the gate type and type of pGate.
 * 
*/
static 
gateParameters(std::vector<std::string>& names, GateType_t gtype, CGate* pGate) {
    auto api = SpecTcl::getInstance();
    switch (gtype) {
    case kgCut1d:
        {
            CCut& rCut = dynamic_cast<CCut&>(*pGate);
            auto pid = rCut.getId();
            auto pParameter = api->FindParameter(pid);
            if (!pParameter) {
                throw std::invalid_argument("gateParameters - gate as non-existent parameters");
            }
            names.push_back(pParameter.getName());
        }
        break;
    case kgBand2d:
    case kgContour2d:
        {
            PointlistGate& rGate = dynamic_cast<PointlistGate&>(*pGate);
            auto xid  = rGate.getxId();
            auto yid = rGate.getyId();

            CParameter* px = api.FindPraameter(xid);
            CParameter* py = api.FindParameter(yid);
            if (px == nullptr || py == nullptr) {
                throw std::invalid_argument("gateParameters - gate as non-existent parameters");
            }
            names.push_back(px->getName());
            names.push_back(py->getName());
        }
        break;
    case kgGammaCut1d:
        {
            CGammaCut& rCut = dynamic_cast<CGammaaCut&>(*pGate);
            auto ids = rCut.getParameters();
            for (auto id: ids) {
                CParameter* p = api.FindParameter(id);
                if (!p) {
                    throw std::invalid_argument("gateParameters - gate as non-existent parameters");
                }
                names.push_back(p->getName();)
            }
        }
    case kgGammaContour2d: {
        CGammaContour& rC = dynamic_cast<CGammaContour&>(*pGate);
        auto ids  = rC.getParameters();
        for (auto id: ids) {
            CParameter* p = api.FindParameter(id);
            if (!p) {
                throw std::invalid_argument("gateParameters - gate as non-existent parameters");
            }
            names.push_back(p->getName();)
        }
    }
    case kgGammaBand2d: {
        CGammaBand& b = dynamic_cast<CGammaBand&>(*pGate);
        auto ids  = rC.getParameters();
        for (auto id: ids) {
            CParameter* p = api.FindParameter(id);
            if (!p) {
                throw std::invalid_argument("gateParameters - gate as non-existent parameters");
            }
            names.push_back(p->getName();)
        }
    }
    default: {
        throw std::invalid_argument("gateParameters - the gate type is not one Xamine should give us.");
    }
    } 

}

/**
 *  gatePoints
 *    Retrieves the points that define a gates acceptance region.
 * 
*/
/**
 * receiveGate
 *    This is called to receive a broadcast gate.
 * 
 * @return pSerializedGate - pointer to a 'newd' serialized gate.
 * caller owns the storage and must destroy it.  Data are assumed to be
 * MPI_Bcast from MPI_EVENT_SINK_RANK which is the rank that communicates
 * with Xamine if it's running.
*/
static pSerializedGate
receiveGate() {
    pSerializedGate result = new SerializedGate;

    // Get the gate name and type:

    {
        GateNameAndType buffer;
        if (MPI_Bcast(
            &buffer, 1, nameAndType(),
             MPI_EVENT_SINK_RANK, MPI_COMM_WORLD
            ) != MPI_SUCCESS) {
            throw std::runtime_error("receiveGate failed to get name and type");
        }
        result->s_type = static_cast<GateType_t>(buffer.s_gateType);
        result->s_name = buffer.s_gateName;
    }
    
    // Get the parameter names.

    {
        char buffer[MAX_MULTI_STRING];
        if (MPI_Bcast(
            buffer, MAX_MULTISTRING, MPI_CHAR, 
            MPI_EVENT_SINK_RANK, MPI_COMM_WORLD) != MPI_SUCCESS) {
                throw std::runtime_error("Failed to read parameter names");
            }
        stringsIntoVector(result->s_parameterNames, buffer);
    }
    // get the point count:

    int numPoints;
    {
        if (MPI_Bcast(&numPoints, 1, MPI_INT, MPI_EVENT_SINK_RANK, MPI_COMM_WORLD) != MPI_SUCCESS) {
            throw std::runtime_error("Failed to read number of gate points.");
        }
    }
    // Now the points themselves: note that if this is really slow, 
    // we can get all the points at once doing an auto-ptr of
    // an array of numPoints GatePoint items and then marshalling them
    // in a second stop.

    for (int i = 0; i < numPoints; i++) {
        GatePoint buffer;
        if (MPI_Bcast(
            &buffer, 1, pointType(), MPI_EVENT_SINK_RANK, MPI_COMM_WORLD) != MPI SUCCESS
        ) {
            throw std::runtime_error("Failed to read an event point");
        }
        result->s_points.push_back(buffer);
    }


    return result;
}



/**
 *  gateEventHandler
 *    This is the event handler for gates posted from the pump to the
 * interpreter in this process.
 * 
 * @param pRawEvent - the Event posted as a Tcl_Event*
 * @param flags     - flags we ignore.
 * @return int      - 1 indicating we're done.
 * 
*/
static int
gateEventHandler(Tcl_Event* pRawEvent, int flags) {
    pGateEvent pEvent = reinterpret_cast<pGateEvent>(pRawEvent);
    pSerializedGates pGateInfo = pEvent->s_pGate;
    // We really can only get geometric gates:

    CGate* pNewGate(nullptr);
    pApi = SpecTcl::getInstance();
    switch pgateInfo->s_type {
    case kgCut1d:
        pNewGate = pApi->CreateCut(
            pGateInfo->s_parameterNames[0], 
            pGateInfo->s_points[0].x, pGateInfo->s_points[1].x
        );
        break;
    case kgContour2d:
        pNewGate = pApi->CreateContour(
            pGateInfo->s_parameterNames[0], pGateInfo->s_parameterNames[1],
            pGateInfo->s_points
        );
        break;
    case kgBand2d:
        pNewGate = pApi->CreateBandContour(
            pGateInfo->s_parameterNames[0], pGateInfo->s_parameterNames[1],
            pGateInfo->s_points
        );
        break;
    case kgGammaCut1d:
        pNewGate = pApi->CreateGammaCut(
            pGateInfo->s_points[0].x, pGateInfo->s_points[1].x,
            pGateInfo->s_parameterNames
        );
        break;
    case kgGammaBand2d:
        pNewGate = pApi->CreateGammaBand(
            pGateInfo->s_points, pGateInfo->s_parameterNames
        );
        break;
    case kgGammaContour:
        pNewGate = pApi->CreateGammaContour(
            pGateInfo->s_points, pGateInfo->s_parameterNames
        );
        break;
    default:    
        // pNewGate is already initialized to nullptr.
    }
    if (pNewGate) {
        pApi->addGate(pGateInfo->s_name, pNewGate);

        // pNewGate was cloned into the container so:

        delete pNewGate;
    } else {
        throw std::runtime_error("Got an unrecognized gate type in gateEventHandler");
    }
    delete pGateInfo;
    return 1;    // Caller frees the event.
}
/**
 *  GateThread
 *    This function is the thread function for receiving gates 
 * All we really do is a loop over:
 *    *  Alocate a GateEvent.
 *    *  Receive a gate into the the s_pGate element of that event.
 *    *  Post the event to the Tcl interpreter thread.
 * 
 * 
*/



static Tcl_ThreadCreateType 
gateThread (ClientData cd) {
    
    // Runs forever 

    while (true) {
        pGateEvent p = reinterpret_cast<pGateEvent>Tcl_Alloc(sizeof(GateEvent));
        if (!p) {
            throw std::runtime_error("gateThread - failed to allocate a GateEvent object.");
        }
        p->s_base.proc = gateEventHandler;
        p->s_base.nextPtr = nullptr;
        p->s_pGate = receiveGate();
        Tcl_ThreadQueueEvent(interpThread, &(p->s_base), TCL_QUEUE_TAIL);
    }
}



#endif                    // WITH_MPI.

/**
 * startGatePump
 *  Start the Xamine gate pump thread.  Note this does nothing if we are not built with
 * MPI _and_ running in the  MPI env.
*/
void startGatePump() {
#ifdef WITH_MPI
    if (gMPIParallel) {
        Tcl_CreateThread(&interpThread, gateThread, nullptr, TCL_THREAD_DEFAULT, TCL_THREAD_NOFLAGS);
    }
#endif
}
/**
 * broadcastGate
 *    Broadcast a gate to all of the receivers. This should be called  in MPI_EVENT_SINK_RANK.
 *   All other ranks have threads in receiveGate().
 * 
 * @param name - name of the gate.
 * @param pGate - The gate definition.
 * 
 * @note - this is a no-op if we are not built with MPI or built with MPI but not running under mpirun e.g.
 * @note - we don't retain an interest in the pGate object once we exit.
*/
void
broadcastGate(std::string name, CGate* pGate) {
#ifdef WITH_MPI
    if(gMPIParallel) {
        // Translate the gate type string to a gate type.

        GateNameAndType firstMsg;
        firstMsg.s_gateType = s_gateType: gateType(pGate);
        strncpy(firstMsg.s_gateName, name.c_str(), MAX_GATENAME);
        if (MPI_Bcast(
            &firstMsg, 1, nameAndType(), 
            MPI_EVENT_SINK_RANK, MPI_COMM_WORLD) != MPI_SUCCESS) {
                throw std::runtime_error("broadcastGate - failed to send name and type");
        }
        // Next the parameters:    

        std::string parameters;
        gateParameters(parameters, firstMsg.s_gateType, pGate);
        char params[MAX_MULTI_STRING];
        stringVecToSTring(params, parameters);
        if(MPI_Bcast(
            params, MAX_MULTI_STRING, MPI_CHAR, 
            MPI_EVENT_SINK_RANK, MPI_COMM_WORLD
        ) != MPI_SUCCESS) {
            throw std::runtime_error("broadcastGate - failed to send parmeter names");
        }
        // Next the number of parameters and the parameters themselves:

        std::vector<GatePoint> points;
        GatePoints(points, firsMsg.s_gateType, pGate);
        int npts = points.size();
        if (MPI_Bcast(
            &npts, 1, MPI_INT,
            MPI_EVENT_SINK_RANK, MPI_COMM_WORLD) != MPI_SUCCESS) {
            throw std::runtime_error("broadcastGate -failed to send point count.");
        }
        // and the points:

        for (auto& pt : points) {
            if (MPI_Bcast(
                &pt, 1, pointType(), MPI_EVENT_SINK_RANK, MPI_COMM_WORLD
            ) != MPI_SUCCESS) {
                throw std::runtime_error("broadcastGate -failed to send a point");
            }
        }

    }
    
#endif
}





