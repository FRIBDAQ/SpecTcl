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

/** @file:  BindTraces.h
 *  @brief: Define trace scripts for sbind/unbind - singleton.
 */

#ifndef BINDTRACES_H
#define BINDTRACES_h

#include <list>

class CTCLInterpreter;
class CTCLObject;


class BindTraceSingleton {
private:
    static BindTraceSingleton*  m_pInstance;
private:
    std::list<CTCLObject*> m_sbindTraces;
    std::list<CTCLObject*> m_unbindTraces;
private:
    BindTraceSingleton();
public:
    static BindTraceSingleton& getInstance();
    
    // Trace maintenance:
public:
    void addSbindTrace(CTCLInterpreter& interp, CTCLObject& script);
    void removeSbindTrace(CTCLObject& script);
    
    void addUnbindTrace(CTCLInterpreter& interp, CTCLObject& script);
    void removeUnbindTrace(CTCLObject& script);
    
    // Trace invocation:
    
    void invokeSbind(
        CTCLInterpreter& interp, CTCLObject& spectrunName, CTCLObject& xamineId
    );
    void invokeUnbind(
        CTCLInterpreter& interp, CTCLObject& spectrumName, CTCLObject& xamineId
    );
private:
    void add(std::list<CTCLObject*>& callbacks, CTCLInterpreter& interp, CTCLObject& script);
    void remove(std::list<CTCLObject*>& callbacks, CTCLObject& script);
    void invoke(
        std::list<CTCLObject*>& callbacks,
        CTCLInterpreter& interp, CTCLObject& spectrumName, CTCLObject& xamineId
    );
};  


#endif