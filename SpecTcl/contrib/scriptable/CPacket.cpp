

/*
	Implementation file for CPacket for a description of the
	class see CPacket.h
*/

////////////////////////// FILE_NAME.cpp ///////////////////////////

// Include files required:
#include <config.h>
#include "CPacket.h"    				
#include "CModuleDictionary.h"
#include <TCLInterpreter.h>
#include <TCLResult.h>
#include <TranslatorPointer.h>
#include <RangeError.h>
#include <TCLString.h>
#include <vector>
#include <assert.h>
#include "CIntConfigParam.h"
#include "CBoolConfigParam.h"
#include "CSegmentUnpacker.h"
#include "CModule.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


// Static attribute storage and initialization for CPacket

/*!
    Create an object of type CPacket
    \param rName (const string& [in]):
      The name of the module.
    \param rInterp (CTCLInterpreter& [in]):
      The interpreter on which the module will be registered.
    \param pDictionary (CModuleDictionary& [in]):
      Pointer to the dictionary in which all the available modules
      will be registered.
*/
CPacket::CPacket (const string& rName,
		  CTCLInterpreter& rInterp,
		  CModuleDictionary* pDictionary) :
  CSegmentUnpacker(rName, rInterp),
  m_nId(-1),
  m_fPacketize(false),
  m_pModules(pDictionary),
  m_nPacketSize(0),
  m_pPacketBase(0)
{ 
  assert(pDictionary);

  // Register the configurable items:

  AddBoolParam("packetize", false); 
  AddIntParam("id", -1);

}
/*!
    Called to destroy an instance of CPacket
*/
 CPacket::~CPacket ( )
{
  OnDelete();
}


// Functions for class CPacket

/*! 

Description:

This function unpacks our subpacket into sensible
SpecTcl parameters. In our case this is pretty much
the process of delegating to the segment unpackers
in m_Decoders.

The operation of this function depends on the 
m_fPacketize flag (set from the packetize
configuration parameter):

m_fPacketize == true:

- if m_nId does not match return
- If there's a match, store the
  packet size as m_nPacketSize
  and one-by-one invoke the
  Unpack members of all of the
  CSegmentUnpackers in m_Modules.
  Ensure we never overrun m_nPacketSize

m_fPacketize == false:
- If !isOwned we are the top level unpacker,
  store m_nPacketSize.
- isOwned::
       m_nPacketsize = m_pOwner->getPacketSize()
- Invoke the Unpack members of all the CSegementUnpacker
  objects in m_Modules ensuring we don't overrun the packet
  size (m_nPacktsize).


\pre none

\post none

Parameters:

\param pBuffer (TranslatorPointer<UShort_t> [in]):
     Refers to a tranlating pointer that points to the start
     of the event segment we are being asked to decode.
\param rEvent (CEvent& [out]):
     Refers to the event array.  This array is the set of parameters
    into which data are unpacked.
\param rAnalyzer (CAnalyzer& [in]):
     refers to the analyzer. In case the decoders need to use
    any of its services.
\param rDecoder (CBufferDecoder& [in]):
    Refers to the buffer decoder in case the analyzer needs to make
    use of any of its services.


\return TranslatorPointer<UShort_t>
\retval translating pointer that points just past where
    the segment unpackers in m_Modules have
    successfully decoded.

\throw  CEventFormatError

Pseudo code:

\verbatim
m_nPacketsize = -1
m_pPacketBase = pBuffer

!m_pOwner::m_nPacketSize = (*pBuffer++)-1 // Top level unpacker.
!m_fPacketize::  
   m_nPacketSize == getPacketSize(pBuffer)
   pBuffer = UnpackModules(pBuffer, m_nPacketSize)
   return pBuffer
m_fPacketize::
    if pBuffer[1] == m_nId
       m_nPacketSize = *pBuffer -2
       pBuffer += 2
       pBuffer = UnpackModules(pBuffer ...)
     return pBuffer
   
\endverbatim

*/
TranslatorPointer<UShort_t> 
CPacket::Unpack(TranslatorPointer<UShort_t> pBuffer, 
		CEvent& rEvent, CAnalyzer& rAnalyzer, 
		CBufferDecoder& rDecoder)  
{
  TranslatorPointer<UShort_t> pBase = pBuffer;
  // Set up m_nPacketsize and m_pPacketBase for the decode:

  m_nPacketSize = -1;
  m_pPacketBase = &pBuffer;

  // If we are the root unpacker, We have the raw event size as our
  // first guestimate of the packet size;
  
  if(!getOwner()) {
    m_nPacketSize = *pBuffer - 1;
    ++pBuffer;			// Point past  the word count.
    if(m_nPacketSize <= 0) {
      return pBuffer;		// Empty!!
    }
  }

  // If packetization is on, We have a count and then an Id:
  // we don't do anything if the id doesn't match us.
  if(m_fPacketize) {
    if(m_nId == pBuffer[1]) {
      m_nPacketSize = *pBuffer - 2; // Update the packetsize...
      pBuffer += 2;
      m_pPacketBase = &pBuffer;
    } else {
      return pBuffer;
    }
  }
  // If not packetizing, we just have raw data and need to get the
  // packet size either from ourselves (root) or our parent):

  else {
    m_nPacketSize = getPacketSize(pBuffer);
    m_pPacketBase = &pBuffer;

  }
  if(m_nPacketSize <= 0) {	// Empty packet... stop right there.
    return pBuffer;
  }

  // Regardless, if we got this far, unpack the remainder
  // and size check what we got:

  TranslatorPointer<UShort_t> pEnd = UnpackModules(pBuffer,
						   rEvent,
						   rAnalyzer,
						   rDecoder);
  int nSize = (pEnd.getOffset() - pBuffer.getOffset())/sizeof(UShort_t);
  if( nSize > m_nPacketSize) {
    throw CRangeError(0, m_nPacketSize,
		      nSize, "CPacket::Unpack tagged packet");
  }
  if(!getOwner()) {		// Root knows size.
    pBase += m_nPacketSize+1;
    return pBase;
  }
  else if(m_fPacketize) {	// Packetized knows size...
    pBase += m_nPacketSize+2;
    return pBase;
  }
  else {			// Have to trust the user
    return pEnd;
  }
}  

/*! 

Description:

Get the size of the current packet.

\pre 

\post 

Parameters:

\param p (TranslatorPointer<UShort_t> [in]):
    Pointer to current location in the buffer.


\return int
   Upper bound on the number of words from
   p to the end of the current event.


Pseudo code:

\verbatim
m_nPacketsize > 0:: 
    return (m_nPacketsize - (m_PacketStart - p))
m_nPacketsize <= 0::
    m_nPacketSize = m_pOwner->getPacketSize(p)
    return m_nPacketSize   
   
\endverbatim

*/
int 
CPacket::getPacketSize(TranslatorPointer<UShort_t> p)  
{ 
  if(m_nPacketSize > 0) {
    return m_nPacketSize - 
      ((p.getOffset() - m_pPacketBase->getOffset())/sizeof(UShort_t));
  }
  else {
    CPacket* pOwner = getOwner();
    assert(pOwner);		// Can't be top-level.!!!
    m_nPacketSize = pOwner->getPacketSize(p);
    return m_nPacketSize;
  }
}  

/*! 

Description:

Does all prepartion to process the first event.
- Process the configuration parametres into
  m_nId and m_fPacketize
- Invoke each m_Modules Setup()


\pre none

\post none

Parameters:

\param rAnalyzer       (CAnalyzer& [in]):
   Reference to the analyzer that is running this show.
\param rHistogrammer   (CHistogrammer& [in]):
   Reference to the histogrammer that keeps our parameters etc.



\throw string
Never set id. string

Pseudo code:

\verbatim
n_fPacketize = value of packetize parameter
if(m_fPacketize) {
   m_nId = value of id parameter
   if(m_nId < 0) {
     throw string("Never set id")
}
foreach Module in m_Decoders
   Setup()
\endverbatim

*/
void 
CPacket::Setup(CAnalyzer& rAnalyzer, CHistogrammer& rHistogrammer)  
{ 
  ParameterIterator p = Find("packetize");
  assert(p != end());
  CBoolConfigParam* pb = (CBoolConfigParam*)(*p);
  m_fPacketize = pb->getOptionValue();
  if(m_fPacketize) {
    p = Find("id");
    assert(p != end());
    CIntConfigParam* pi = (CIntConfigParam*)(*p);
    m_nId = pi->getOptionValue();
    if(m_nId < 0) {
      throw string("CPacket::Setup Packetizing turned on but no id set!!");
    }
    
  }
  ModuleIterator i = m_Decoders.begin();
  for(; i != m_Decoders.end(); i++) {
    CSegmentUnpacker* pModule = *i;
    pModule->Setup(rAnalyzer, rHistogrammer);
  }
}  

/*! 

Description:

Called prior to object deletion.  We remove and disown all the 
modules in m_Modules.

\pre none

\post m_Modules.size() == 0

Parameters:

none

\return void
none


Pseudo code:

\verbatim
foreach module in m_Modules
   assert module.isOwned
   Disown module
\endverbatim

*/
void 
CPacket::OnDelete()  
{
  ModuleIterator i = m_Decoders.begin();
  for(; i != m_Decoders.end(); i++) {
    CSegmentUnpacker* pModule = *i;
    assert(pModule->isOwned()); // It's got to be our too!!!
    assert(pModule->getOwner() == this);
    pModule->Disown();		// Disown deletes him from our list.
  }
  m_Decoders.erase(BeginDecoders(),
		   EndDecoders()); // Clear our list too.
  CSegmentUnpacker::OnDelete();	// Removes me from my own packet.

}  

/*! 

Description:

Processes commands directed at this object.
Commands are assumed to consist of a subcommand
followed by subcommand specific parameters.
We process the subcommands:
- add - Add a module to m_Modules
- list   - list the modules in m_Modules.
- remove - Removes a module from m_Modules.
anything else is passed to the base class for processing.

\pre none

\post none

Parameters:

\param rInterp  (CTCLInterpreter& [in]):
    Reference ot the interpreter that is processing this command.
\param rResult (CTCLResult& [out]):
    Reference to the result string.  This will be filled witht the command's
    output.
\param nArgs (int [in]):
    Number of command parameters
\param pArgs
    pointe rto the command parameters.

\return int
\retval TCL_OK -Command successfully processed, rResult has the command result string.
\retval TCL_ERROR- command failed. rResult has the reason why and the command usage.



Pseudo code:

\verbatim
Next argument (0'th is base command).
argument == "add"::
  next argument
  return AddModuleCommane
argument == "list"::
  next argument
  return ListModulesCommand
argument == "remove"::
   next argument
   return RemoveModuleCommand
argument != "add" && argument != "list" && argument != "remove"::
   With original parameters...
   if s = CSegmentUnpacker::operator() != TCL_OK
      append our Usage() to result.
   return s

\endverbatim

*/
int 
CPacket::operator()(CTCLInterpreter& rInterp, 
		    CTCLResult& rResult, 
		    int nArgs, char** pArgs)  
{ 
  // We are not interested in the command name parameter
  // but in case we need to delegate to the base class we must preserve
  // the originals.

  int     argc = nArgs-1;
  char**  argv = pArgs+1;
  if(argc == 0) {
    // Build up the combined usage string..

    rResult  = "Insufficient command parameters\n";
    rResult += Usage();
    return TCL_ERROR;
  }
  // Add  new modules to our set of decoders.

  if(string(*argv) == string("add")) {
    argc--;
    argv++;
    return AddModuleCommand(rInterp,
			    rResult,
			    argc, argv);
  }
  // List the set of modules we are decoding.

  else if (string(*argv) == string("list")) {
    argc--;
    argv++;
    return ListModulesCommand(rInterp,
			      rResult,
			      argc, argv);
  }
  // Remove a module from the decoder list.

  else if (string(*argv) == string("remove")) {
    argc--;
    argv++;
    return RemoveModuleCommand(rInterp,
			       rResult,
			       argc,argv);
  }
  // Delegate to the base class.

  else {
    return CSegmentUnpacker::operator()(rInterp, rResult,
				      nArgs, pArgs);
  }
}  

/*! 

Description:

Processes the "add" subcommand.
Each remaining command parameter is treated
as a module name.  If All modules exist, they are
added to the m_Modules list. If any module does
not exist, none are added to the list.


\pre none

\post none

Parameters:

\rInterp (CTCLInterpreter& [in])
    reference to the interpreter that runs this command.
\rResult (CTCLResult & [out])
    Reference to the result string that is produced by this command.
\nArgs (int [in]):
    The number of remaining command line parameters.
\pArgs (char** [in]):
    The command line parameters.

\return int
\retval TCL_OK - the command succeded. rResult is empty.
\retval TCL_ERROR - The command failed and rResult says why (along with the
     usage string).



Pseudo code:

\verbatim
foreach parameter on the line
   if m_pModules.Find(parameter)
      push parameter -> goodlist
   else 
      push parameter -> badlist
if badlist.size() == 0
   foreach module in goodlist
      add module to m_Modules
   return TCL_OK
else 
   foreach module in badlist
      append element module -> rResult
   return TCL_ERROR

\endverbatim

*/
int 
CPacket::AddModuleCommand(CTCLInterpreter& rInterp, 
			  CTCLResult& rResult, 
			  int nArgs, char** pArgs)  
{ 

  // Need to try to add at least one module: 

  if(!nArgs) {
    rResult  = "Need at least one module...";
    rResult +=Usage();
    return TCL_ERROR;
  }

  // Iterate through the modules looking them up:

  vector<CModuleDictionary::ModuleIterator> good;
  vector<string>                            bad;

  while(nArgs) {
    string ModuleName(*pArgs);
    pArgs++;
    nArgs--;
    CModuleDictionary::ModuleIterator p = m_pModules->Find(ModuleName);
    if(p != m_pModules->end() && !(p->second->isOwned()) ) {
      good.push_back(p);	// Save the good iterators...
    }
    else {
      bad.push_back(ModuleName); // Save the bad names...
    }
  }
  // If there are no bad modules all the good modules can be added to
  // m_Decoders (end of the list).
  //
  if(bad.size() == 0) {
    for(int i =0; i < good.size(); i++) {
      AddModule(good[i]->second);	// Add the module's pointer to the list.
    }
    return TCL_OK;
  }
  // If there are bad modules, use their names to form the error message
  //
  else {
    rResult = "Some modules you wanted to add don't exist or are already owned: ";
    for(int i = 0; i < bad.size(); i++) {
      rResult += bad[i];
      if(i != bad.size() -2) {	// Put a comma after all but the last one...
	rResult += ", ";
      }
    }
    return TCL_ERROR;
  }
  return TCL_ERROR;
}  

/*! 

Description:

Remove the module named by the next
parameter from the module list m_Modules.

\pre none

\post none

Parameters:

\rInterp (CTCLInterpreter& [in])
    reference to the interpreter that runs this command.
\rResult (CTCLResult & [out])
    Reference to the result string that is produced by this command.
\nArgs (int [in]):
    The number of remaining command line parameters.
\pArgs (char** [in]):
    The command line parameters.

\return int
\retval TCL_OK - Module successfully removed.
\retval TCL_ERROR - There was something wrong.  rResult will describe the
     errorr.


Pseudo code:

\verbatim
if nArgs == 1
   If exists *pArgs in m_pModules
   RemoveModule(named pArgs)
   return TCL_OK
else
   Build usage string -> Result
   return TCL_ERROR
\endverbatim

*/
int 
CPacket::RemoveModuleCommand(CTCLInterpreter& rInterp, 
			     CTCLResult& rResult, 
			     int  nArgs, char** pArgs)  
{ 
  if(nArgs == 1) {
    string ModuleName(*pArgs);
    ModuleIterator p = FindDecoder(ModuleName);
    if(p != EndDecoders() && ((*p)->isOwned())  &&
       ((*p)->getOwner() == this)) {
      try {
	RemoveModule(p);
	return TCL_OK;
      }
      catch(string msg) {
	rResult = msg;
	return TCL_ERROR;
      }
      catch(...) {
	rResult = "Unexpected exception type caught in CPacket::RemoveModuleCommand";
	return TCL_ERROR;
      }
    }
    else {
      rResult = "Remove: Module ";
      rResult += ModuleName;
      rResult += " does not exist or is already owned.";
      return TCL_ERROR;
    }
  } 
  else {
    rResult  = "Remove must only have a single parameter";
    rResult += Usage();
    return TCL_ERROR;
  }
}  

/*! 

Description:

Produces a list of the set of modules that 
are in the m_Modules list.  The list will
be a properly formatted TCL list.  Each element
of the list will be a 2 element sublist that contains
the module's name and type.


\pre none

\post none

Parameters:

\param rInterp   (CTCLInterpreter& [in]):
     The interpreter that is running this command.
\param rResult  (CTCLResult         [out]):
     Will be written with the result string from this 
    command (hopefully the list).
\param nArgs   (int [in]):
     number of command line parameters.
\param pArgs   (char** [in])
     remaining command line parameters.

\return int
\retval TCL_OK - the command worked, rResult is 
   the list.
\retval TCL_ERROR - the command failed, an rResult
   says why.


Pseudo code:

\verbatim

foreach module in m_Modules
       result begin sublist
       result append element module.getName
       result append element module.getType
       result end sublist
return TCL_OK
\endverbatim

*/
int 
CPacket::ListModulesCommand(CTCLInterpreter& rInterp, 
			    CTCLResult& rResult, 
			    int nArgs, char** pArgs)  
{ 
  if (nArgs != 0) {
    rResult += "List should not have any command parameters: \n";
    rResult += Usage();
    return TCL_ERROR;
  }
  CTCLString listing;
  ModuleIterator p = BeginDecoders();
  for(; p != EndDecoders(); p++) {
    CSegmentUnpacker* pModule = *p;
    listing.StartSublist();
    listing.AppendElement(pModule->getName());
    listing.AppendElement(pModule->getType());
    listing.EndSublist();
  }
  rResult = (const char*)(listing);
  return TCL_OK;
}  

/*! 

Description:

Returns the usage string for the command.
We rely on our base class to get it started
and we only append information about the packet
specific options.

\pre none

\post none

Parameters:

\return string
\retval usage string.


Pseudo code:

\verbatim

\endverbatim

*/
string 
CPacket::Usage()  
{ 
  string result = CSegmentUnpacker::Usage();
  result += "\n    add module ...\n";
  result += "      list\n";
  result += "      remove module\n";
  return result;
}  

/*! 

Description:

Add a module given a pointer to it.
The module is added to the m_Modules list.

\pre pModule->isOwned == false

\post pModule->isOwned == true

Parameters:

\param pModule (CSegmentUnpacker* [in]):
      Pointer to the unpacker to add to m_Modules


\return void

\throw string
Module already belongs

Pseudo code:

\verbatim
if !isOwned
   pModule->Own(this)
   m_Modules add pModule
else
   throw stringa("Module already belongs to someone"
\endverbatim

*/
void 
CPacket::AddModule(CSegmentUnpacker* pModule)  
{
  if(!pModule->isOwned()) {
    pModule->Own(this);
    m_Decoders.push_back(pModule);
  }
  else {
    throw string("CPacket::AddModule module already owned!!!\n");
  }
}  

/*! 

Description:

Removes a module from m_Modules given a pointer
to it.
- Locate the module in the list.
- Erase from the list.
- Disown the module.

\pre pModule->isOwned == true

\post pModule->isOwned == false.

Parameters:

\return void

\throw string
not owned

Pseudo code:

\verbatim
if m_pModule.isOwned == false
   Locate module
   if located
      other RemoveModule
   else
       throw "Module not owned by us"
else
   throw "Module is not owned"

\endverbatim

*/
void 
CPacket::RemoveModule(CSegmentUnpacker* pModule)  
{
  // Check that module is owned and by us:

  if(!pModule->isOwned()) {
    throw string("CPacket::RemoveModule - Module not owned by anyone\n");
  }
  if(pModule->getOwner() != this) {
    throw string("CPacket::RemoveModule - Module owned, but not by us!!\n");
  }
  ModuleIterator p = FindDecoder(pModule->getName());
  assert(p != EndDecoders());	// we already know we own it.
  RemoveModule(p);
}  

/*! 

Description:

Removes a module from m_Modules
given an iterator to the mdule.

\pre *Module.isOwned == true

\post *Module.isOwned == false

Parameters:

\param Module (ModuleIterator [in])
   iterator within m_Modules to module.

\return void

\throw   string
Module not owned.


Pseudo code:

\verbatim
if *module->isOwned
   erase module from m_Modules
else
   throw "Module not owned."
\endverbatim

*/
void 
CPacket::RemoveModule(ModuleIterator module)  
{
  // Ensure the module is owned and by us.
  
  CSegmentUnpacker* pModule = *module;
  if(pModule->getOwner() == this) {
    m_Decoders.erase(module);
    pModule->Disown();
  }
  else {
    throw string("CPacket::RemoveModule Module not owned by this");
  }
}
/*!
   Description:

Returns the module's type string.  The module type string is used for
documentation purposes to describe what the module does.

\pre none
\post none

\return string
\retval  The string that identifies the purpose of the unpacking module.

*/
string
CPacket::getType() const 
{
  return string("module-container");
}
/*!
   Description:
     Locates a decoder by name in the list

Parameters:
\param name  (const string&):
   Name of the module we are looking for.


\pre none
\post none
\return CPacket::ModuleIterator
  \retval iterator to the found object.
  \retval end() if the module is not found.

Pseudo code:
\verbatim
   foreach segment in m_Decoders
     If segment.getName() == name  return 
   return end
\endverbatim
*/
CPacket::ModuleIterator
CPacket::FindDecoder(const string& name)
{
  ModuleIterator p = BeginDecoders();
  for(; p != EndDecoders(); p++) {
    if((*p)->getName() == name) {
      return p;
    }
  }
  return EndDecoders();
}
/*!
  Descrption:
  Returns the number of decoders in the decoder list.

Parameters:
   none

\pre none
\post none

\return int
 \retval Number of decoders in the list.

Pseudo Code:

\verbatim
return m_Decoders.size()
\endverbatim
 */
int
CPacket::NumDecoders() const
{
  return m_Decoders.size();
}
/*!
  Description:
    return a begin of iteration iterator for the Decoder list.

Parameters:

none

\pre   none
\post  none

\return CPacket::ModuleIterator
\retval Start of iteration iterator for the decoder list.

Pseudo code:
\verbatim
return m_Decoders.begin()
\endverbatim

 */
CPacket::ModuleIterator
CPacket::BeginDecoders()
{
  return m_Decoders.begin();
}
/*!
  Description:

  Parameters:

  none

  \return CPacket::Iterator
  \retval end of iteration iterator for the decode list.

  \pre  none
  \post none

PSeudo code
\verbatim
  return m_Decoders.end();
\endverbatim


 */
CPacket::ModuleIterator
CPacket::EndDecoders()
{
  return m_Decoders.end();
}
/*!

Description:

   Unpack the set of modules in the unpacker list for this packet.
   
Parameters:

\param pBuffer   (TranslatorPointer<UShort_t> [in]):
   Pointer to the event segment to decode.
\param rEvent    (CEvent& rEvent [out]):
   Reference to the event vector into which the data are decoded.
\param rAnalyzer (CAnalyzer [in]):
   Reference to the analyzer that ultimately called us.
\param rDecoder  (CBufferDecoder[in]):
   Reference to the buffer decoder that understands the gross buffer format.



\return  TranslatorPointer<UShort_t>
\retval Advanced past the decoded section.

*/
TranslatorPointer<UShort_t>
CPacket::UnpackModules(TranslatorPointer<UShort_t> pBuffer,
		       CEvent&                     rEvent,
		       CAnalyzer&                  rAnalyzer,
		       CBufferDecoder&             rDecoder)
{
  ModuleIterator p  = BeginDecoders();
  for(; p != EndDecoders(); p++) {
    CSegmentUnpacker* pModule = *p;
    pBuffer = pModule->Unpack(pBuffer, rEvent, rAnalyzer, rDecoder);
    if((pBuffer.getOffset() - m_pPacketBase->getOffset())/sizeof(UShort_t)
                                                         >= m_nPacketSize) {
      return pBuffer;
    }
  }
  return pBuffer;
}
