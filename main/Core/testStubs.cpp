// This implementation of TreeParameter is based on the ideas and original code of::
//    Daniel Bazin
//    National Superconducting Cyclotron Lab
//    Michigan State University
//    East Lansing, MI 48824-1321
//

#include <config.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "TreeTestSupport.h"
#include "CTreeVariable.h"
#include "CTreeVariableProperties.h"
#include "CTreeParameter.h"

#include <TCLInterpreter.h>
#include <Parameter.h>
#include <Event.h>
#include <tcl.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

// Stubs  testing possible without needing to link with the entire SpecTcl thingy.

// Global variables some of the libs expect:





// Stub implemenation of just enough of SpecTcl::

#include <SpecTcl.h>
#include <Parameter.h>


class CParameter;


static int paramId(0);
// Assign a parameter id.

UInt_t
SpecTcl::AssignParameterId()
{
  return paramId++;

}
CTCLInterpreter*
SpecTcl::getInterpreter()
{
  return TreeTestSupport::getInterpreter();
}


// Find Parameter as simple as possible:

CParameter*
SpecTcl::FindParameter(string name)
{
  return (CParameter*)NULL;
}
// Add a parameter as simply as possible:

CParameter*
SpecTcl::AddParameter(string name, UInt_t id, string units)
{
  return new CParameter(name, id, units.c_str());
}


////////////////////////////


void
TreeTestSupport::ClearMap()
{
  CTreeParameter::testClearMap();
}
CParameter*
TreeTestSupport::getParameter(CTreeParameter& param)
{
  return param.m_pParameter;
}


bool
TreeTestSupport::isRegistered(CTreeParameter& param)
{
  return param.isRegistered();
}
CEvent*
TreeTestSupport::getEvent()
{
  return CTreeParameter::m_pEvent;
}
void
TreeTestSupport::ThrowIfNoParameter(CTreeParameter& param, const char* pd)
{
  string pds(pd);
  param.ThrowIfNoParameter(pds);
}
void
TreeTestSupport::ThrowIfNoEvent(CTreeParameter& param, const char* pd)
{
  string pds(pd);
  param.ThrowIfNoEvent(pds);
}

#include "Asserts.h"


void
CheckConstructed(CTreeParameter& rParam, string errorcomment,
		      string name, UInt_t bins, double start, double stop,
		      double inc, string units, bool changed)
{
  string comment;
  comment = errorcomment + ": Name";
  EQMSG(comment, name, rParam.getName());

  comment = errorcomment + " : bins";
  EQMSG(comment, bins, rParam.getBins());

  comment = errorcomment + " : start";
  EQMSG(comment, start, rParam.getStart());

  comment = errorcomment + " : stop";
  EQMSG(comment, stop, rParam.getStop());

  comment = errorcomment + " : inc";
  EQMSG(comment, inc,  rParam.getInc());

  comment = errorcomment + " : units";
  EQMSG(comment, units, rParam.getUnit());

  comment = errorcomment + " : change flag";
  EQMSG(comment, changed, rParam.hasChanged());


}
string
MakeElementName(string base, int low, int num, int index)
{
  double lowdigits = log10((double)(abs((long int)low))) +1.0;
  double hidigits  = log10((double)(abs((long int)(low + num)))) + 1.0;
  
  int  numDigits    = (int)(fmax(lowdigits, hidigits));
  
  //// Produce the digit creation format
  //
  char format[100];
  snprintf(format, sizeof(format), "%s.%%%d.%dd", base.c_str(), 
	   numDigits, numDigits);
  char name [100];
  snprintf(name, sizeof(name), format, index);

  return string(name);
}


CTCLInterpreter* TreeTestSupport::m_pInterpreter(0);

CTCLInterpreter*
TreeTestSupport::getInterpreter()
{
  return m_pInterpreter;
}
void
TreeTestSupport::TeardownTestInterpreter()
{
  if(m_pInterpreter) {		// Allows multicalls if needed.
    Tcl_Interp* pRawInterp = m_pInterpreter->getInterpreter();
    delete m_pInterpreter;
    m_pInterpreter = (CTCLInterpreter*)NULL;
    Tcl_DeleteInterp(pRawInterp);
  }

}

void 
TreeTestSupport::InitTestInterpreter()
{
  // Don't double make:

  if(!m_pInterpreter) {
    Tcl_Interp* pRawInterp = Tcl_CreateInterp();
    m_pInterpreter = new CTCLInterpreter(pRawInterp);
  }
}

void
TreeTestSupport::ClearVariableMap()
{
  CTreeVariable::CleanMap();
}

CTreeVariableProperties*
TreeTestSupport::getVariableProperties(CTreeVariable& Properties)
{
  return Properties.m_pVariable;
}
