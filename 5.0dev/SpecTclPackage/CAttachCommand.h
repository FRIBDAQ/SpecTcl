/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2009.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#ifndef _CATTACHCOMMAND_H
#define _CATTACHCOMMAND_H

/**
 * @file CAttachCommand.h
 * @brief Define the singleton attach command.
 */


#ifndef _TCLOBJECTPROCESSOR_H
#include <TCLObjectProcessor.h>
#endif

class CDataSource;

/**
 * @class CAttachCommand
 * 
 *   implements the spectcl::attach command.
 *   the format of this command is:
 * \verbatim
 *      spectcl::attach uri
 * \endverbatim
 *
 * Where uri is the URI of the data source.  The protocol part of the data source
 * determines the type of data source object to be used.
 */
class CAttachCommand : public CTCLObjectProcessor
{
  // singleton-ness demands that there be a private variable with the instance pointer
  // and that constructors be private:

private:
  static CAttachCommand* m_pInstance;

  // The datasource:

private:
  CDataSource* m_pDataSource;

private:
  CAttachCommand();
  ~CAttachCommand();


  // singleton-ness also demands that there be a way to get the instance, creating if need be.

public:
  static CAttachCommand* instance();
  
  // Execute command.

public:
  int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);

  // Utilties:

private:
  static void createInstance();
  std::string getURIProtocol(std::string  uri);
  std::string getURITail(std::string uri);

};

#endif
