// Implementation of the CPortManagerException
// See the header for more information.

// Author:
//       Ron Fox
//       NSCL
//       Michigan State University
//       East Lansing, MI 48824-1321
//
#include <config.h>
#include "CPortManagerException.h"


using namespace std;


//  String table for the reason code:

static const char* ReasonStrings[] = {
	"No Ports Available",
	"Only clients local to the server can allocate ports",
	"Connection to port management server failed"
};
static const int numReasonStrings = sizeof(ReasonStrings)/sizeof(char*);

/*!
	Construct a port manager exception.
	
	\param host (string in):
	    Name of the host to which the port manager object was connected
	    when the exception was thrown.
	\param why   (CPortManagerException::Reason in):
	    The reason the port manager felt it had to throw an exception.
	\param doing (string in):
	    What the port manager was doing when it threw the exception.
*/
CPortManagerException::CPortManagerException(string host,
					     CPortManagerException::Reason why,
					     string doing) :
  CException(doing),
  m_sHost(host),
  m_eReason(why)
{
}

/*!
  COpy Construction:
*/
CPortManagerException::CPortManagerException(const CPortManagerException& rhs) :
  CException(rhs)
{
  *this = rhs;
}
/*!
   Destroy a port manager exception.   
   For now this is just a no-op.
*/
CPortManagerException::~CPortManagerException()
{
}
/*!
   Assign this port manager exception to one on the rhs of an assignment
   operator.  This is done as a member by member copy.
   \param rhs (CPortManagerException in):
      The object on the rhs of the assignment.
   
   \return CPortManagerException&
   \retval  *this
*/
CPortManagerException&
CPortManagerException::operator=(const CPortManagerException& rhs)
{
	if(this != &rhs) {
		CException::operator=(rhs);
		m_sHost    = rhs.m_sHost;
		m_eReason  = rhs.m_eReason;
	}
	return *this;
}
/*!   Check for equality between *this and an object on the rhs of the
      == operator.  Equality is defined as base class equality and all
      of our fields equal to those of the rhs.
     
      \param rhs (CPortManagerException in)
          The object on the rhs of the comparison.
      \return int
      \retval  1   if equal
      \retval  0   if not equal.
*/
int
CPortManagerException::operator==(const CPortManagerException& rhs)
{
	return (CException::operator==(rhs)             &&
	        (m_sHost     == rhs.m_sHost)              &&
	        (m_eReason   == rhs.m_eReason));
}
/*!
	Check for inequality between *this and an object on the rhs of the !=
	operator.  Inequality is defined as the logical inverse of equality.
      \param rhs (CPortManagerException in)
          The object on the rhs of the comparison.
      \return int
      \retval  0   if equal
      \retval  1   if not equal.

*/
int
CPortManagerException::operator!=(const CPortManagerException& rhs)
{
	return !(*this == rhs);
}

// ---------------------------------------------------------

/*!
 *   Provide the full reason text for the  exception.  This 
 *   consists of a string of the order:
 *   "Exception thrown in port manager communicating with <host> : <reason>
 * 	
*/
const char*
CPortManagerException::ReasonText() const
{
	string ReasonText;
	ReasonText  = "Exception thrown in port manager communicating with ";
	ReasonText += m_sHost;
	ReasonText += " : ";
	ReasonText += ReasonCodeToText(ReasonCode());
	
	m_sReasonText = ReasonText;
	return m_sReasonText.c_str();
}

/*!
	Return the reason for the failure as an int code.  This is required
	by the interface of the exception class.  We will just cast
	the value of m_eReason to an int.. so that if the user wants, they
	can cast it back to  a CPortManagerException::Reason to do switch
	based actions.
	
*/
int
CPortManagerException::ReasonCode() const
{
	return static_cast<Reason>(m_eReason);
}
/*!
   Return a string that corresponds to the integerized reason code
   passed in.  This is done by looking up the string in the ReasonStrings
   table.
*/
string 
CPortManagerException::ReasonCodeToText(int code)
{
	if ((code >= 0) && (code < numReasonStrings)) {
		return string(ReasonStrings[code]);
	} 
	else {
		return string("ReasonCodeToText: Invalid reason code");
	}
}

// --------------------------------------------------------

/*!
 * Operator to put a formatted version of the exception to some output
 * stream
 * 
 * \param f (ostream& [in]
 *     Reference to an output stream into which to put the textualized
 *     exception.
 * \param e (CPortManagerException& [in])
 *     The exception to write into the stream.
 * \return ostream&
 * \retval f.
 */
 ostream&
 operator<<(ostream& f, const CPortManagerException& e)
 {
 	f << e.ReasonText() << " : " << e.WasDoing();
 	
 	return f;
 }

