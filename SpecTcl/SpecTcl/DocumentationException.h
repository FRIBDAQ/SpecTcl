/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

*/
/*
  Author:
      Ron Fox
      NSCL
      Michigan State University
      East Lansing, MI 48824-1321
*/
#ifndef __DOCUMENTATIONEXCEPTION_H
#define __DOCUMENTATIONEXCETPION_H

#ifndef __CEXCEPTION_H
#include <Exception.h>
#endif


/*!
    This class defines exceptions that may occur in the documentation buffer
    processing subsystem.  
*/
class CDocumentationException : public CException
{
public:
  // Data types:

  typedef enum {
    NoSuchVariable,
    NoSuchElement,
    NoSuchHandler
  } ExceptionReason;

private:
  ExceptionReason             m_Why;
public:
  CDocumentationException(ExceptionReason why,
			  const char* pDoing);
  virtual ~CDocumentationException();

  // Class functions:

  virtual const char* ReasonText() const;
  virtual Int_t ReasonCode() const;
  static STD(string) ReasonToText(int code);
  static ExceptionReason CodeToReason(int code);

};


#endif
