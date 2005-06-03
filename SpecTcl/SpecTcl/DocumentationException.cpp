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
#include <config.h>
#include "DocumentationException.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

/*! 
   Construct an exception, presumably prior to throwing it.
   \param why (CExceptionReason):
       The reason the exception is being thrown.
   \param pDoing (const char*)
       What the program was doing when the exception got thrown.
*/
CDocumentationException::CDocumentationException(CDocumentationException::ExceptionReason why,
						 const char* pDoing) :
  CException(pDoing),
  m_Why(why)
{
}
/*!
  Destruction is  no-op.
*/
CDocumentationException::~CDocumentationException()
{}

/*!  Return a textual reason for the exception.
     Note: This is not re-entrant nor recursive due to the static
     result string.  In SpecTcl's context, this is not an issue.
*/
const char*
CDocumentationException::ReasonText() const
{
  static string result;
  result = "Exception in documentation buffer subsystem: ";
  result += ReasonToText(ReasonCode());
  result += " while: ";
  result += WasDoing();

  return result.c_str();
}
/*!
   Return the encoded reason for the failure.  This is just an integer cast
   of the m_Why member.  It can be converted back into a 
   CDocmentationException::ExceptionReason by feeding it into CodeToReason.

*/
Int_t
CDocumentationException::ReasonCode() const
{
  return (int)m_Why;
}
/*!
   Convert the integerized reason code into an exception reason again.
*/
CDocumentationException::ExceptionReason
CDocumentationException::CodeToReason(int code)
{
  return (ExceptionReason)code;
}
/*!
  Convert the integerized reason code to a text string.
 */
string
CDocumentationException::ReasonToText(int code)
{
  switch ((ExceptionReason)code) {
  case NoSuchVariable:
    return string("No such variable");
  case NoSuchElement:
    return string("No such array element");
  case NoSuchHandler:
    return string("No such change handler");
  default:
    return string("BUGBUG : reason code is invalid in ReasonToText");
  }

}
