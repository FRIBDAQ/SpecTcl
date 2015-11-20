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
#include "TCLObjectPackage.h"
#include "TCLPackagedObjectProcessor.h"


/*!
   Add a command.  The command is added to the end of the list
   and its onAttach member is called with the package as a parameter.

   @param processor - Pointer to the packaged object processor.

*/
void
CTCLObjectPackage::addCommand(CTCLPackagedObjectProcessor* processor)
{
  m_commands.push_back(processor);
  processor->onAttach(this);
}

/*!
   Start an iteration through the commands:
   @return CTCLObjectPackage::CommandIterator
   @retval Beginning iterator.
*/
CTCLObjectPackage::CommandIterator
CTCLObjectPackage::begin()
{
  return m_commands.begin();
}

/*!
   Test for end of iteration through the commands:

   @return CTCLObjectPackage::CommandIterator
   @retval end of iteration iterator.
*/
CTCLObjectPackage::CommandIterator
CTCLObjectPackage::end()
{
  return m_commands.end();
}

