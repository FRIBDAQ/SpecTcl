/*
	Implementation file for CScriptableUnpacker for a description of the
	class see CScriptableUnpacker.h
*/

////////////////////////// FILE_NAME.cpp ///////////////////////////

// Include files required:

#include <config.h>
#include "CScriptableUnpacker.h"    				
#include "CPacket.h"
#include "CModuleCommand.h"
#include "CModuleDictionary.h"
#include "CCAENDigitizerCreator.h"
#include "CCAENV830Creator.h"
#include "CPacketCreator.h"

#include <Analyzer.h>
#include <BufferDecoder.h>
#include <TCLInterpreter.h>
#include <Globals.h>
#include <TranslatorPointer.h>
#include <Exception.h>
#include <EventFormatError.h>
#include <TCLAnalyzer.h>
#include <Histogrammer.h>


#include <Iostream.h>

#include <assert.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

/*!
    Create an object of type CScriptableUnpacker
    \param CreatorName (string [in]):
       Name (command) that will be given to the module creator.
    \param TopLevelName (string [in]):
       Name (command) that will be given to the top level packet. 
*/
CScriptableUnpacker::CScriptableUnpacker (string CreatorName,
					  string TopLevelName) 
   : 
  m_fSetupDone(false),
  m_sCreatorName(CreatorName),
  m_sTopLevelName(TopLevelName),
  m_pTopLevel(0),
  m_pModuleCommand(0)
{ 
} 

/*!
    Called to destroy an instance of CScriptableUnpacker
*/
CScriptableUnpacker::~CScriptableUnpacker ( )
{
  delete m_pTopLevel;
  delete m_pModuleCommand;
}

// Functions for class CScriptableUnpacker

/*! 

Description:

Called when a scripted unpacker module is 
connected to the analyzer.  In this function we
do the final construction phase of putting together the
unpacker components:
- A module dictionary is created (we don't hang on to
  it however).
- The module command is created and registered
  with the TCL interpreter.
- All known module type creators are created and
  registered with the module command.
- A CPacket is created and registered with the interpreter
  as the unpacker.  This object will be the top level
  unpacker for the entire system.

\pre gpInterpreter != NULL

\post none

Parameters:

\param rAnalyzer (CAnalyzer& [in]):
   reference to the analyzer object that we don't
   need but that the interface specification required.


\return Bool_t
\retval kfTRUE - indicating everything got glued together
    correctly.
\retval kfFALSE - if there were problems gluing everything together.
  


Pseudo code:

\verbatim
try
   pDictionary  = new CDictionary
   m_pModuleCommand = new CModuleCommand(...pDictionary)
   RegisterCreators
   m_pTopLevel = new CPacket (..."unpacker"...)
   return kfTRUE
catch
   return kfFALSE
\endverbatim

*/
Bool_t
CScriptableUnpacker::OnAttach(CAnalyzer& rAnalyzer)  
{
  // Check the precondition:
 
  assert(gpInterpreter);
    
  CModuleDictionary* pDictionary = new CModuleDictionary;
  m_pModuleCommand               = new CModuleCommand(*gpInterpreter,
						      m_sCreatorName,
						      *pDictionary);
  RegisterCreators();		// Register the module creators.
  m_pTopLevel                    = new CPacket(m_sTopLevelName,
					       *gpInterpreter,
					       pDictionary);
  return kfTRUE;
}  

/*! 

Description:

Performs necessary begin run operations.
- Calls the top level unpacker's setup function.
- Sets m_fSetupDone -> true.




\post m_fSetupDone = true

Parameters:

\param rAanalyzer (CAnalyzer& [in]): 
     The analyzer that is invoking us.
\param rDecoder (CBufferDecoder& [in]):
     The buffer decoder that can understand
     the structure of the buffer we are untangling.

\return Bool_t
\retval kfTRUE - All the setups succeeded.
\retval kfFALSE - Some setup threw an exception.

\throw  

Pseudo code:

\verbatim
Asssert that m_fSetupDone == false
try
   m_pTopLevel.Setup()
   m_fSetupDone = true;
   return kfTRUE
catch
   return kfFALSE
\endverbatim

*/
Bool_t 
CScriptableUnpacker::OnBegin(CAnalyzer& rAnalyzer, 
			     CBufferDecoder& rDecoder)  
{
  // Check the pre-condition:

  Bool_t returnvalue;
  try {
    // Setup the top level objects, mark m_fSetupDone true and return true:
   
    m_pTopLevel->Setup(rAnalyzer, *(CHistogrammer*)gpEventSink);
    m_fSetupDone = true;

    returnvalue =  kfTRUE;
  } 
  catch (...) {
    returnvalue =  kfFALSE;
  }

  // Check the postcondition:

  return returnvalue;
}  

/*! 

Description:

Calls OnBegin as the action is identical.


\post m_fSetupDone == true

Parameters:

\param rAnalyzer (CAnalyzer& [in]):
    Analyzer that called uus.
\param rDecoder (CBufferDecoder& [in]):
    Buffer decoder that understands this buffer
structure.


\return Bool_t
\retval kfTRUE  = All setup correctly.
\retval kfFALSE = Problems getting setup.

\throw  

Pseudo code:

\verbatim
OnBegin()
\endverbatim

*/
Bool_t 
CScriptableUnpacker::OnResume(CAnalyzer& rAnalyzer, 
			      CBufferDecoder& rDecoder)  
{
  return OnBegin(rAnalyzer, rDecoder); // Takes care of pre/post asserts too.
}  

/*! 

Description:

Sets m_fSetupDone to ensure that we will
setup the unpackers regardless what the next
buffer looks like.

\pre none (could be the first buffer).

\post m_fSetupDone == false

Parameters:

\param rAnalyzer (CAnalyzer& [in]):
     The analyzer that called us.
\param rDecoder (CBufferDecoder& [in]):
     The buffer decoder that understands our buffer format.

\return Bool_t
\retval kfTRUE - only possible to succeed.

\throw  

Pseudo code:

\verbatim
m_fSetupDone = false
return kfTRUE
\endverbatim

*/
Bool_t 
CScriptableUnpacker::OnEnd(CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder)  
{
  m_fSetupDone = false;		// Ensure setup is done on next start-like buf.

  // assert the postcondition and return.

  assert(!m_fSetupDone);	
  return kfTRUE;
}  

/*! 

Description:

Invokes OnEnd as the processing is identical.

\pre none

\post kfSetupDone == false

Parameters:

\param rAnalyzer (CAnalyzer& [in]):
   The analyzer that called us.
\param rDecoder (CBufferDecoder& [in]):
   The buffer decoder that understands the structure
 of the buffer we are decoding.


\return Bool_t
\retval kfTRUE - this must succeed

\throw  

Pseudo code:

\verbatim
return OnEnd

\endverbatim

*/
Bool_t 
CScriptableUnpacker::OnPause(CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder)  
{
  return OnEnd(rAnalyzer, rDecoder); // Assertions get run by OnEnd too.
}  

/*! 

Description:

If m_fSetupDone is false, OnBegin is called
to get things setup correctly.  Otherwise
nothing happens.

\pre none

\post m_fSetupDone == true

Parameters:

\param rAnalyzer (CAnalyzer& [in]):
    Reference to the analyzer that called us.
\param rDecoder (CBufferDecoder& [in]):
    Reference to the buffer decoder that understands
 how to decode this buffer.


\return Bool_t
\retval kfTRUE -Either all the setups were done correctly or 
     m_fSetupDone was already true.
\retval kfFALSE - One of the setups failed.

\throw  

Pseudo code:

\verbatim
!m_fSetupDone::return OnBegin()
m_fSetupDone::return kfTrue
   
\endverbatim

*/
Bool_t 
CScriptableUnpacker::OnOther(CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder)  
{ 
  Bool_t returnvalue;
  if(!m_fSetupDone) {
    returnvalue =  OnBegin(rAnalyzer, rDecoder);
  }
  else {
    returnvalue =  kfTRUE;
  }
  /// Check postconditionand return appropriate value:

  assert(m_fSetupDone);
  return returnvalue;
}  

/*! 

Description:

- If necessary, OnBegin is called to setup the parameter mappings.
- The Event size is set from the size of the event in the buffer.
- A translating pointer is constructed from the beginning of the 
event.
- The TopLevel's Unpack member is called to unpack the event.


\pre none

\post m_fSetupDone == true

Parameters:

\param pBuffer (const Address_t [in])
    Pointer to the start of the event.
\param rEvent (CEvent& [out]):
    The event 'array' that the unpackers will set with 
    the decoded parameter values.
\param rAnalyzer (CAnalzyer& [in]):
    The analyzer that called us.
\param rDecoder (CBufferDecoder& [in]):
   The buffer decoder.

\return Bool_t
\retval kfTRUE - All of the decoders worked.
\retval kfFASLE - Some decoder or setup failed.

\throw  

Pseudo code:

\verbatim
try
  if( OnOther(...))

   Construct translating buffer pointer p
   Eventsize = *p
   rAnalyzer(cast->CTCLAnalyzer&).setEventSize(EventSize)
   pfinal = m_TopLevel.Unpack(...)
   (pfinal - p <= EventSize:: throw CEventFormatError
    ~ ^                    :: return kfFALSE
   
catch
   return kfFALSE
\endverbatim

*/
Bool_t 
CScriptableUnpacker::operator()(const Address_t pBuffer, CEvent& rEvent, 
				CAnalyzer& rAnalyzer, 
				CBufferDecoder& rDecoder)  
{
  // Calling on other ensures that if necessary, Setup gets called to
  // bind the parameters.

  Bool_t retval = OnOther(rAnalyzer, rDecoder);

  if(retval) {
    // Create the translating pointer and get the event size.
    TranslatorPointer<UShort_t> p(*(rDecoder.getBufferTranslator()), pBuffer);
    UShort_t nWords = *p;
   
    // Let the analyzer know how big the event is...

    CTclAnalyzer& rAna((CTclAnalyzer&)rAnalyzer);
    rAna.SetEventSize(nWords*sizeof(UShort_t));

    //  Invoke the unpacker and check the returned size.
    //  Exceptions are handled as follows:
    //     CEventFormatError exceptions get rethrown since the
    //     analyzer knows how to deal with them
    //     Others we can understand get mapped to an error string
    //     and a kfFalse return.
    //  ... gets mapped to a false return with a generic message:
    //  Upon return from the top level unpacker we ensure that we
    //  are not past the event boundary. If we are we throw our own
    //  event format error
    try { 
      TranslatorPointer<UShort_t> pEnd = m_pTopLevel->Unpack(p,
							     rEvent, rAnalyzer, rDecoder);
      UShort_t nProcessed = (pEnd.getOffset() - p.getOffset())/sizeof(UShort_t);
      if(nProcessed > nWords) {
	throw CEventFormatError((int)CEventFormatError::knSizeMismatch,
				string("CScriptableUnpacker  from toplevel"),
				(UInt_t*)pBuffer, nWords, 0,
				nWords);
      }
      retval = kfTRUE;     
    }
    catch (CEventFormatError& rExcept) { // Need to catch and rethrow so that
      throw;			// subsequent catch blocks don't catch it.
    }
    catch (CException& rExcept) {
      cerr << "CScriptableUnpacker::operator() caught a SpectTcl exception: "
	   << rExcept.ReasonText() << endl;
      return kfFALSE;
      
    }
    catch (string msg) {
      cerr << "CScriptableUnpacker::operator() caught a string exception: "
	   << msg << endl;
      retval = kfFALSE;
    }
    catch (char* msg) {
      cerr << "CScriptableUnpacker::operator() caught a char* exception: "
	   << msg <<endl;
      retval = kfFALSE;
    }
    catch (...) {
      cerr << "CScriptableUnpaker::operator() caught an "
	   << "unexpected exception type: "
	   << endl;
      retval = kfFALSE;
    }
  }

  // Check post condition and return:

  assert(m_fSetupDone);
  return retval;
}
/*! 

Description:

Registers a module creator with our module command object.  There are two ways to
extend or modify the set of modules the system can create:
- Invoke this member function to register additional creational objects.
  Note that since the module command is only created at OnAttach time, the
  correct sequence is to first register the unpacker, and then 
  register the additional creators.
- Derive from this class and implement an override for RegisterCreators
  (that optionally may call ours).  Register the creators you want in that
  member. (RegisterCreators is gaurenteed to be called when m_pModuleCommand
  is valid).

\pre m_pModuleCommand != null

\post none

Parameters:

\param rType (const string & [in]):
    The name of the module type the creator being registered will create.
\param pCreator (CModuleCreator* [in]):
   The creator for rType.


\return void

Pseudo code:

\verbatim
assert the precondition.
m_pModuleCommand->RegisterCreator...   
\endverbatim

*/
void
CScriptableUnpacker::RegisterCreator(const string& rType,
				     CModuleCreator* pCreator)
{

  if(pCreator) {
    m_pModuleCommand->RegisterCreator(rType, *pCreator);
  } 
  else {
    throw string("CScriptableUnpacker::RegisterCreator - received null pCreator");
  }
}
/*! 

Description:
  Registers all of the module creators.  The table below shows the set of
module creators that get registered by default:
\verbatim
+-----------+--------------------------------+----------------------------+
|  Type     | Creator class registered.      |  Description               |
+-----------+--------------------------------+----------------------------+
| caenvme   |                                |                            |
+-----------+                                |Common unpacking class for  |
| caenv775  |                                |CAEN '32 channel digitizer  |
+-----------+ CCAENDigitzerCreator           |family of modules'.         |
| caenv785  |                                |                            |
+-----------+                                |                            |
| caenv792  |                                |                            |
+-----------+--------------------------------+----------------------------+
| caenv830  | CCAENV830Creator               | CAEN V830 scaler module    |
+-----------+--------------------------------+----------------------------+
| packet    | CPacketCreator                 | (un)tagged packets         |
+-----------+--------------------------------+----------------------------+

\endverbatim


\pre none

\post

Parameters:

Pseudo code:

\verbatim
for each of the supported modules, invoke our RegisterCreator
(that way preconditions get asserted).
   
\endverbatim

*/
void
CScriptableUnpacker::RegisterCreators()
{
  RegisterCreator("caenvme",  (new CCAENDigitizerCreator("caenvme",
							 m_pModuleCommand)));
  RegisterCreator("caenv775", (new CCAENDigitizerCreator("caenv775",
							 m_pModuleCommand)));
  RegisterCreator("caenv785", (new CCAENDigitizerCreator("caenv785",
							 m_pModuleCommand)));
  RegisterCreator("caenv1785",(new CCAENDigitizerCreator("caenv1785",
							 m_pModuleCommand)));
  RegisterCreator("caenv792", (new CCAENDigitizerCreator("caenv792",
							 m_pModuleCommand)));
  RegisterCreator("caenv830", (new CCAENV830Creator("caenv830",
						    m_pModuleCommand)));
  RegisterCreator("packet",   (new CPacketCreator("packet",
						  m_pModuleCommand)));
  
}
