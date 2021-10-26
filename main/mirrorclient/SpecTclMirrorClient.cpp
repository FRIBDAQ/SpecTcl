/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  SpecTclMirrorClient.cpp
 *  @brief: Implements the APi described in SpecTclMirrorClient.h
 */

#include "SpecTclMirrorClient.h"
#include "MirrorClientInternals.h"
#include <client.h>
#include <os.h>

#include <stdexcept>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <unistd.h>

static const char* ExecDirs=SPECTCL_BIN;
static const unsigned MAP_RETRY_SECS=1;
static const unsigned MAP_RETRIES=10;


static unsigned lastError = MIRROR_SUCCESS;

static const char* ppMessages[] = {
    "Successful completion",
    "The specified REST service name is not advertised in that host",
    "The specified MIRROR service name is not advertised in that host",
    "Unable to get the name of the logged in user",
    "Unable to retrieve memory size",
    "Unable to retrieve the list of existing mirrors.",
    "Unable to set up the mirror client"
    
};
static const unsigned nMsgs = sizeof(ppMessages)/sizeof(const char*);

// Utility functions.

/**
 * translatePort
 *    Static function to take a port number and translate it:
 *
 *  @param host  - Host on which the translation is done (see port argument)
 *  @param port  - Port to translate.
 *  @param user  - User that's advertised the port if so.
 *  @param status - Status to set in lastError  on failure.
 *  @return int  - port number
 *  @throws std::runtime_error if not able to translate.
 *  Translation is as follows:
 *  -   If the port is numerical, It is converted to an integer and returned.
 *  -   If not, the host, user and port string are used to attempt to do
 *      a translation via the DAQ port manager in that system and the
 *      result is returned.
 */
static int
translatePort(const char* host, const char* port, const char* user, unsigned status)
{
    // Try to convert the string to an integer.
    char* endptr;
    unsigned result = strtoul(port, &endptr, 0);
    if (endptr != port) {
        // successful conversion:
        
        lastError = MIRROR_SUCCESS;
        return result;
    }
    // Use the port manager.
    
    lastError = status;             // LookupPort throws:
    result = LookupPort(host, port, user);
    lastError = MIRROR_SUCCESS;     // NO exception thrown.
    return result;
}
/**
 * isLocalHost
 *    Determines if a host name is the same as the localhost.
 *    -  If host == localhost it's local.
 *    -  if host == results of gethostname() it's local.
 *    -  If host == the fqdn it's local.
 *  @param host - hostname to check.
 *  @return bool - true if this system is host.
 *  
 */
static bool
isLocalHost(const char* host)
{
    std::string strHost(host);
    if (strHost == "localhost" ) return true;
    char gottenhostname[1024];
    memset(gottenhostname, 0, 1024);  // ensure null termination.
    if (gethostname(gottenhostname, sizeof(gottenhostname) - 1)) {
        throw std::runtime_error("gethostname() failed");
    }
    if (strHost == gottenhostname) return true;
    if (strHost == Os::getfqdn(host)) return true;
    
    
    return false;
    
    
}
/**
 * MapMemory
 *    Map memory that's local.
 * @param name - key for the memory.
 * @param size - bytes of spectrum memoryt.
 * @return void* pointer to the memory.
 * @retval nullptr - could not map.
 */
static void*
MapMemory(const char* name, size_t size)
{
    int status =
            Xamine_MapMemory(const_cast<char*>(name, size, &pResult)
        if (status) {
            return pResult;
        } else {
            return nullptr;
        }   
}

/**
 * mapSpecTclLocalMemory
 *   Find out what the SpecTcl shared memory name is and map it.
 *
 *  @param host  - name of the host.
 *  @param port  - REST server port.
 *  @param size  - Bytes of spectrum memory.
 *  @return void* - Pointer to the spectrum  memory.
 *  @retval nullptr - failed to map.
 */
static void*
mapSpecTclLocalMemory(const char* host, int port, size_t size)
{
    try {
        auto key = GetSpecTclSharedMemory(host, port);
        Xamine_shared* pResult;
        return MapMemory(key.c_str(), size);
    }
    catch(...) {
        return nullptr;
    }
}

/**
 * getMirrorIfLocal
 *    Given the lists of mirrors that SpecTcl is exporting, including the
 *    shared memory it created for itself, if one matches our needs,
 *    map it and return a pointer to that map.  There are a few special cases,
 *    however:
 *    - SpecTcl is running locally - in that case rather than attempting to
 *      create a mirror, we can just map to SpecTcl's own shared memory.
 *    - SpecTcl is running locally in a persistent container - in that
 *      case maps to SpecTcl's shared memory will fail because
 *      the container has a separate SYS-V IPC namespace so SpecTcl's
 *      shared memory is invisibl.  In that case we do need to treat this
 *      like mirroring
 * @param host - Host on which SpecTcl is running.
 * @param rest    - REST port number.
 * @param mirrors - mirrors currently being maintained.
 * @param size    - Spectrum memory size.
 * @return void*  - Pointer to the shared memory.
 * @retval nullptr - If local mapping is not possible (mirror needs to be setup).
 *
 */
static void*
getMirrorIfLocal(
    const char* host, int rest, const
    std::vector<MirrorInfo>& mirrors, size_t size
)
{
    // Handle the special case of SpecTcl run locally (both of them).
    if (isLocalHost(host)) {
        return mapSpecTclLocalMemory(host, rest, size);
    } else {
        for (auto item : mirrors) {
            if (sameHost(host, item.m_host)) {
                return MapMemory(item.m_memoryName, size);
            }
        }
    }
    // No match
    
    return nullptr;
}
/**
 * startMirroring
 *    -  Run the mirrorclient program to start mirroring.
 *    -  Wait a bit to let the mirrorclient produce its shared memory.
 *    -  Get mirror information and use getMirrorIfLocal to map to it.
 *       This bit of waiting and mapping can be repeated a few times.
 * @param host - host in which SpecTcl is running.
 * @param mirror - Port on which the SpecTcl mirror server is listening.
 * @param rest   - Port on which the SpecTcl REST server is listening.
 * @param size   - Spectrum bytes.
 * @return void*   - Pointer to specTcl mirrored memory.
 * @retval nullptr - if we can't do all this stuff.
 */
void*
startMirroring(const char* host, int mirror, int rest, size_t size)
{
    pid_t child = fork();
    if (child == -1) {
        lastError = MIRROR_SETUPFAILED;
        return nullptr;
    }
    if (child) {
        // Parent
        
        void* pResult(nullptr);
        for (int i =0; i < MAP_RETRIES; i++) {
            sleep(MIRROR_RETRY_SECS);
            auto mirrors = GetMirrorList();
            pResult = getMirrorIfLocal(host, rest, size);
            if (pResult) break;
        }
        return pResult;
    } else {
        // child
        
        // Close stdin,out,error
        
        close(0);
        close(1);
        close(2);
        pid_t session = setsid();          // Create a new session.
        if (session < 0) {
            exit(EXIT_FAILURE);            // failed
        }
        
        // formulate the mirrorclient command and arguments.
        // Since we're passing integer ports we don't need --user.
        
        std::string program(SPECTCL_BIN);
        program += "/mirrorclient";
        
        std::string hostarg = "--host=";
        hostarg            +=  host;
        
        std::string mirrorarg = "--mirrorport=";
        mirrorarg += std::to_string(mirror);
        
        std::string restarg = "--restport=";
        restarg +=  std::to_string(rest);
        
        execl(
            program.c_str(),
            hostarg.c_str(), mirrorarg.c_str(), restarg.c_str(),
            nullptr
        );
        // If we got here the execl failed.
        
        exit(EXIT_FAILURE);
    }
}

// External entries:
/**
*   getSpecTclMemory
*     Return a pointer to a SpecTcl mirror memory.
*     @param host - host on which SpecTcl is running.
*     @param rest - Port on which rest server is running.  If this can be
*                   translated to a number it's treated as a numeric port number.
*                   If not, we interact with the NSCLDAQ port manager in host to
*                   resolve the port number.
*     @param mirror - Mirror port, treated identically to rest.
*     @param user   - If not null, this is the user running SpecTcl otherwise
*                     the current user is used.  This is noly important
*                     for service name translations.
*     
*/
extern "C" {
void*
getSpecTclMemory(const char* host, const char* rest, const char* mirror, const char*user)
{
    if (!user) {
        user = getlogin();
        if (!user) {
            lastError = MIRROR_CANTGETUSERNAME;
            return nullptr;
        }
    }
    int restPort, mirrorPort;
    try {
        restPort = translatePort(host, rest, user, MIRROR_NORESTSVC);
        mirrorPort = translatePort(host, mirror, user, MIRROR_NOMIRRORSVC);
    catch (...) {
        return nullptr;             // translatePort returns the
    }
    
    // Now that the ports are numeric, we can get the memory size and
    // see if there's already a local mirror:
    
    size_t spectrumBytes;
    try {
        spectrumBytes = GetSpectrumSize(host, restPort);
    }
    catch (...) {
        lastError = MIRROR_CANTGETSIZE;
        return nullptr;
    }
    std::vector<MirrorInfo> mirrors;
    try {
        mirrors = GetMirrorList(host, restPort);
    }
    catch (...) {
        lastError = MIRROR_CANTGETMIRRORS;
        return nullptr;
    }
    
    void* result =  getMirrorIfLocal(host, restPort, mirrors, spectrumBytes);    // If local map 
    if (result) return result;
    
    try {
        return startMirroring(host mirrorPort, restport, spectrumBytes);
    } catch(...) {
        lastError = MIRROR_SETUPFAILED;
    }
    
    return nullptr;
}

}


extern "C" {
int
errorCode()
{
    return lastError;
}
}

extern "C" {
const char*
errorString(unsigned code)
{
    if (code < nMsgs) {
        return ppMessages[lastError];
    } else {
        return "Invalid error code";
    }
}
}


