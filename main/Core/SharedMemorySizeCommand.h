/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2016.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
       Scientific Software Team
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#ifndef SHAREDMEMORYSIZECOMMAND_H
#define SHAREDMEMORYSIZECOMMAND_H

#include <TCLObjectProcessor.h>

class CTCLInterpreter;

/*! The shmemsize command
 *
 *  When a display exists, this provides the size of the shared memory 
 *  file that it is attached to. The REST server uses this for remote clients.
 */
class CSharedMemorySizeCommand : public CTCLObjectProcessor
{
public:
    CSharedMemorySizeCommand(CTCLInterpreter& rInterp);

    int operator()(CTCLInterpreter& rInterp, std::vector<CTCLObject>& obj);

};

#endif // SHAREDMEMORYSIZECOMMAND_H
