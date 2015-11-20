/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#include <config.h>
#include "TCLPackagedObjectProcessor.h"
#include "TCLInterpreter.h"
#include "TCLObjectPackage.h"  

/*!
  Construction passes on to the base class:
  @param interp     - TCL Interpreter on which this command will be defined.
  @param command    - Name of the command.
  @param registerMe - If true, construction registers the command.
*/
CTCLPackagedObjectProcessor::CTCLPackagedObjectProcessor(CTCLInterpreter&  interp,
							 std::string       command,
							 bool              registerMe) :
  CTCLObjectProcessor(interp, command, registerMe),
  m_package(0)
{}

/*!
  Destruction is a NOOP:
*/
CTCLPackagedObjectProcessor::~CTCLPackagedObjectProcessor()
{}

/*!

    Attachment saves the package pointer for later and invokes the
    Initialize method (which can get the package via
    getPackage if it needs.

    @param package - Pointer to the package.
*/
void 
CTCLPackagedObjectProcessor::onAttach(CTCLObjectPackage* package)
{
  m_package = package;
  Initialize();
}

/*!
   Return the package pointer so that derived classes can invoke package
   facilities
   @return CTCLObjectPackage*
*/
CTCLObjectPackage*
CTCLPackagedObjectProcessor::getPackage()
{
  return m_package;
}

/*!
  Default initialize is null. The derived class can override this however:
*/
void
CTCLPackagedObjectProcessor::Initialize()
{
}

