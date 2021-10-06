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

/** @file:  MirrorDirectory.h
 *  @brief: Define a class that willl hold the list of clients mirroring display memory
 */
#ifndef MIRRORDIRECTORY_H
#define MIRRORDIRECTORY_H
#include <string>
#include <map>
#include <vector>
class CriticalSection;                // For thread safety.
class CTCLMutex;

/**
 * @class MirrorDirectory
 *    This class provides a directory of all the display memory mirrors
 *    clients are attempting to maintain.  Each mirror is characterized
 *    by:
 *    *   A client host.
 *    *   A client SYSV shared memory key.
 *
 *   Since the directory  is maintained by both the main and server threads,
 *   access is threadsafe.
 */
class MirrorDirectory
{
public:
    typedef std::pair<std::string, std::string> HostKey;  // hostname, keyname.
    
private:
    CTCLMutex*                          m_pGuard;
    std::map<std::string, std::string>  m_directory;
public:
    MirrorDirectory();
    virtual ~MirrorDirectory();
    
private:
    MirrorDirectory(const MirrorDirectory& rhs);
    MirrorDirectory& operator=(const MirrorDirectory& rhs);
    int operator==(const MirrorDirectory& rhs);
    
public:
    void put(std::string& host, std::string key);     // Threadsafe.
    std::string get(std::string& host);                   // threadsafe.
    std::vector<HostKey> list();                        // threadsafe.
};


#endif