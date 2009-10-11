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
#include "FilterBufferDecoder.h"
#include "CXdrMemInputStream.h"
#include <buftypes.h>
#include <string.h>
#include <Analyzer.h>
#include <Iostream.h>
#include <Globals.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


static int inline numBitmasks(Int_t nParameters)
{
  return (nParameters  + sizeof(unsigned)*8 - 1)/
			(sizeof(unsigned)*8);
}


/*!
  Simple constructor.  The Decoder is constructed with:
  - A null translated buffer.
  - Parameter count of 0.
  - Buffersize of zero.
  - Type of 0.
  - Entity count of zero.

*/
CFilterBufferDecoder::CFilterBufferDecoder() :
  m_pTranslated(0),
  m_nParamCount(0),
  m_nBytes(0),
  m_nCurrentType(0),
  m_nEntities(0),
  m_fSeenHeader(false),
  m_pTranslator(0)
{
}
/*!
   Copy constructor.  If the translated buffer exists, it is
   copied (deep copy) into us.
*/
CFilterBufferDecoder::CFilterBufferDecoder(const CFilterBufferDecoder& rhs)

{
  CopyIn(rhs);			// Common code with assignment.
}
/*!
   Destructor.
*/
CFilterBufferDecoder::~CFilterBufferDecoder()
{
  delete []m_pTranslated;
  delete m_pTranslator;
}
/*!
  Assignment.
*/
CFilterBufferDecoder&
CFilterBufferDecoder::operator=(const CFilterBufferDecoder& rhs)
{
  if(&rhs != this) {
    CopyIn(rhs);
  }
  return *this;
}
/*!
   Comparison:   Things compare equal if all fields are equal
   and the first m_nBytes of the translated buffers compare equal.
*/
int
CFilterBufferDecoder::operator==(const CFilterBufferDecoder& rhs)
{
  if( (m_nParamCount == rhs.m_nParamCount)   &&
      (m_nBytes      == rhs.m_nBytes)        &&
      (m_nCurrentType == rhs.m_nCurrentType) &&
      (m_nEntities    == rhs.m_nEntities)    &&
      (memcmp(m_pTranslated, rhs.m_pTranslated, m_nBytes) == 0)) {
    return TRUE;
  } 
  else {
    return FALSE;
  }
}
/*!
  Comparison:  things compare not equal if they don't compare
  equal :-).
*/
int
CFilterBufferDecoder::operator!=(const CFilterBufferDecoder& rhs)
{
  return !(operator==(rhs));
}
/*!
  get the buffer body.  In our case, the buffer body is always
  the m_pTranslated buffer... if this is null, we return null.
*/
const Address_t 
CFilterBufferDecoder::getBody() 
{
  return m_pTranslated;
}

/*! 
  Return the size of the body.  This is m_nBytes
*/
UInt_t
CFilterBufferDecoder::getBodySize() 
{
  return m_nBytes;
}
/*!
   Return the run number.  Filtered data has no run number
   since a filtered file can be the amalgamation of several disjoint
   runs.  0 is unconditionally returned.
*/
UInt_t
CFilterBufferDecoder::getRun()
{
  return 0;
}
/*!
   Return the number of entities in the translated buffer.
   This is just m_nEntities.
*/
UInt_t
CFilterBufferDecoder::getEntityCount()
{
  return m_nEntities;
}
/*!
  Return the sequence number of the buffer. For filtered
  data, there are no sequence numbers, therefore 0 is returned
  always.
*/
UInt_t
CFilterBufferDecoder::getSequenceNo()
{
  return 0;
}
/*!
    Get the number of lam registers that may appear in the data
stream.  This is a deprecated field and we'll unconditionally
return a 0 for it.
*/
UInt_t
CFilterBufferDecoder::getLamCount()
{
  return 0;
}
/*!
  Get the number of pattern registers that may appear in the data
  stream.  This is a deprecated field and we'll unconditionally
  return a 0 for it... even though there is a big pattern field
  in the filtered data...that is described by the header.
*/
UInt_t
CFilterBufferDecoder::getPatternCount()
{
  return 0;
}
/*!
   Return the buffer type.
   This returns the value of m_nCurrentType.
*/
UInt_t
CFilterBufferDecoder::getBufferType()
{
  return m_nCurrentType;
}
/*!
  Returns byte order signatures for the current buffer.
  Since the actual buffer passed to SpecTcl is in native
  byte format (translated from an Xdr buffer), the 
  byte ordering is always the native machine byte ordering.
*/
void
CFilterBufferDecoder::getByteOrder(Short_t& Sig16,
			       Int_t& Sig32)
{
  Sig16 = 0x0102;
  Sig32 = 0x01020304;
}
/*!
   Returns the run title string.  Filtered data has no
   run title (it can in fact be an amalgamation of several
   runs.  We unconditionally return the string:
   "Filtered Data - no title"
*/
string
CFilterBufferDecoder::getTitle()
{
  return string("Filtered Data - no title");
}
/*!
  Return a translator pointer for this buffer.
  Since buffers are always in native byte order,
  we return a pointer to a non-swapping translator.
*/

BufferTranslator*
CFilterBufferDecoder::getBufferTranslator()
{

  return m_pTranslator;

}
/*!
    Function call operator is called when a new buffer
    becomes available.  What we do is:
    - Translate the buffer to m_pTranslated.
      Unfortunately, this implies we know something of the event
      structure.  However, the input buffer is in Xdr format so
      this must be done to preserve compatibility with existing
      event processor code.
    - Process the buffer setting up and calling OnPhysics as needed.
      \param nBytes    (UInt_t [in]):
          Size of the buffer in bytes.  This is the physical size
	  not the used size.
      \param pBuffer   (Address_t [in]):
          Pointer to the buffer.
      \param rAnalyzer (CAnalyzer& [in]):
          Reference to the analyzer.

*/
void
CFilterBufferDecoder::operator()(UInt_t     nBytes, 
				 Address_t  pBuffer,
				 CAnalyzer& rAnalyzer)
{
  try {
    CXdrMemInputStream xdr(nBytes, pBuffer); // This can decode xdr->
    xdr.Connect();
    
    TranslateBuffer(xdr,nBytes, rAnalyzer);
  } 
  catch(string error) {
    cerr << "string exception in Buffer decoder ";
    cerr << error << endl;
  }
  catch (const char* p) {
    cerr << "Character* exception in buffer decoder. ";
    cerr << p << endl;
  }
  catch (...) {
    cerr << "Some other exceptoin in buffer decoder.\n";
  }
}
/*!
   Translate a buffer from Xdr -> native format.  The
   translation is done with analysis and dispatching:
   - If the first string in the buffer indicates the buffer starts
   with a header event, first the header alone is translated. This
   translation includes enough of a decode to allow us to know
   how big events will be (how many parameters are defined), and
   this information is saved for the ProcessBuffer function. We
   setup and dispatch a call to the analyzer's OnOther member).
   - The remainder of the buffer (headers can only be the first
   entity in the buffer) are translated and dispatched to OnPhysics.

   \param xdr (CXdrInputStream& [in]):
      XDR input stream from which the data comes.
   \param nBytes (UInt_t [in]):
      Upper bound on the number of bytes required for  the 
      data buffer.
   \param rAnalyzer (CAnalyzer& [in]):
     The analyzer we will call back to.

*/
void
CFilterBufferDecoder::TranslateBuffer(CXdrInputStream& xdr, 
				      UInt_t           nBytes,
				      CAnalyzer&       rAnalyzer)
{
  string itemtype;
  
  // get some storage we can work with:

  delete []m_pTranslated;
  m_pTranslated = 0;		// Not really necessary but good.
  m_pTranslated = new char[nBytes * 2]; // If single this is needed since
                                        // each float -> double
  delete m_pTranslator;
  m_pTranslator = new NonSwappingBufferTranslator(m_pTranslated);

  // Look at the first item in the buffer, and handle it if it's
  // a header.

  xdr >> itemtype;
  if(itemtype == string("header")) {
    ProcessHeader(xdr, rAnalyzer);
    xdr >> itemtype;		// Setup for what's to come...
    if (itemtype != string("event")) {
      gpRunControl->OnEnd();
      cerr << "Event source does not appear to be a filter file!! ending\n";
      return;
    }
    m_isSingle = dataWidth(nBytes, xdr.getBuffer());

    
  }

  // The following attempt to detect unsuitable buffer types:


  // Need to see a header before much of anytyhing.////

  if (!m_fSeenHeader) {
    gpRunControl->OnEnd();	// Declare premature end...and...
    cerr << "Event source does not appear to be a fiter file!!! ending\n";
    return;
  }


  // Now deal with the rest of the buffer, itemtype must be
  // processed into the destination buffer.

  ProcessEvents(xdr, rAnalyzer);
  
}
/*!
    Process a header event.  The header is decoded (we need the
    number of parameters), and put into the translation buffer.
    The resulting buffer is then dispatched to OnOther
    so the decoder can figure out how to bind parameters.
    \param xdr          (CXdrInputStream& [in]):
       Input stream that has the data.
    \param rAnalyzer    (CAnalyzer& rAnalyzer [in]):
       The anlyzer we are calling back to.

    \note  There's an implicit assumption that the header
    will fit in a buffer.   This is part of the filter spec.
    so we're good there.

*/
void
CFilterBufferDecoder::ProcessHeader(CXdrInputStream& xdr,
				    CAnalyzer&       rAnalyzer)
{
  m_nBytes = 0;			// Current size of buffer is 0.
 
  int      nItemSize;
  char*    pCursor(m_pTranslated); // Insertion pointer.
  bool     testing = (&rAnalyzer == NULL);

  // Insert the header string...
  // In the line below, the extra space in the strlen is not
  // a typeo... it's there to ensure that the null terminator
  // in the header string gets copied too.

  nItemSize = strlen("header ");
  if (!testing) {
    memcpy(pCursor, "header", nItemSize);	
    m_nBytes += nItemSize;
    pCursor  += nItemSize;
  }

  // Next is the number of parameters that are in the filtered
  // data set, we need this in order to be able to 
  // know how to figure out how many items are in a physics event:

  xdr >> m_nParamCount;
  nItemSize = sizeof(int);
  if (!testing) {
    memcpy(pCursor, &m_nParamCount, nItemSize);
    m_nBytes += nItemSize;
    pCursor  += nItemSize;
  }

  // Now there will be m_nParamCount strings:

  for(int i =0; i < m_nParamCount; i++) {
    string name;
    xdr >> name;		// Pull the name from the buffer
    if (!testing) {
      nItemSize = name.size() + 1;
      memcpy(pCursor, name.c_str(), nItemSize);
      m_nBytes += nItemSize;
      pCursor  += nItemSize;
    }
  }
  m_fSeenHeader = true;		// We've seen a header.

  // Set up for and call the OnOther member of the analyzer.
  
  m_nEntities    = m_nParamCount; // Each parameter is an entity.
  m_nCurrentType = PARAMDESCRIP; // Parameter description 'buffer'.
  if (!testing) {
    rAnalyzer.OnOther(m_nCurrentType, *this);
  }
  
}

/*!
  Process the event segment of an xdr encoded filter buffer.
  It is a bad format error to not have seen a header by now,
  so we'll throw a CEventFormatError if this is the case, otherwise,
  - Fill events into the output buffer until the 'end of file'
    on the input stream.
  - Dispatch the 'buffer' to OnPhysics.
  \param xdr       (CXdrInputStream& [in]):
     Source of the data.
  \param rAnalyzer (CAnalyzer& rAnalyzer [in]):
     Object whose OnPhysics we'll call back.

  \note By now the event's identifying string has been
        pulled out of the buffer, but we know what it was.
	('event').

  \note We assume events don't span buffer boundaries.
				    
*/
void
CFilterBufferDecoder::ProcessEvents(CXdrInputStream& xdr,
				    CAnalyzer&       rAnalyzer)
{
  char* pCursor(m_pTranslated);	// Insertion pointer.
  int   nSize(0);		// Number of inserted bytes.
  int   nItemSize;
  m_nEntities = 0;

  while(xdr.isOpen()) {		// Closes when done.

    // Copy the identifier:

    nItemSize = sizeof("event"); // sizeof counts the null, strlen won't.
    memcpy (pCursor, "event", nItemSize);
    nSize   += nItemSize;
    pCursor += nItemSize;

    // Read the bitmasks and count the number of set bits.

    nItemSize = sizeof(unsigned);
    int nBitmasks = numBitmasks(m_nParamCount);

    int bits = 0;
    for(int i=0; i < nBitmasks; i++) {
      int mask;
      xdr >> mask;
      memcpy(pCursor, &mask, nItemSize);
      bits     += CountBits(mask);
      pCursor  += nItemSize;
      nSize    += nItemSize;
    }
    // Bits has the number of parameters that's present.
    // There's one float for each parameter.
			
    int nItemSize = sizeof(double);
    for (int i =0; i < bits; i++) {
      double item;
      float fitem;
      if (m_isSingle) {
	xdr >> fitem;
	item = fitem;
      } 
      else {
	xdr >> item;
      }
      memcpy(pCursor, &item, nItemSize);
      pCursor += nItemSize;
      nSize   += nItemSize;
    }
    m_nEntities++;		// Each event is an entity

    // If the file is still open, then we can read the next
    // header:

    string header;
    if(xdr.isOpen()) {
      xdr >> header;
      if (header != string("event")) {
	gpRunControl->OnEnd();
	cerr << "Event file does not appear to be a filter file!!! ending\n";
	return;
      }
    }
    
  }
  // Now that the events have been translated and put into a
  // native format event buffer, setup for and call the 
  // OnPhysics member of the analyzer.
 
  m_nBytes       = nSize;
  m_nCurrentType = DATABF;

  rAnalyzer.OnPhysics(*this);
}
/*!
   Count the number of set bits in an unsigned mask.
   This is used to determine how many events are actually in the
   buffer.
   \param mask (unsigned [in]):
      The mask to evaluate.

   \return int
      Number of set bits.
*/
int
CFilterBufferDecoder::CountBits(unsigned mask)
{
  // This is a nice portable way to find the top bit:

  unsigned ones = ~0;		// All bits set.
  unsigned bit  = ~(ones >> 1);	// Top bit set. (Unsigned shifts in 0.
  int count(0);

  while(bit) {
    if(bit & mask) count++;
    bit = bit >> 1;
  }
  return count;
}
/*!
  Copy a source decoder to *this.  Used by assignment and
  copy construction. The assumption is that:
  - m_pTranslated is dynamic storage.
  - rhs.m_nBytes is accurate.
  - More data will not be put into m_pTranslated without
    a new allocation.
  
 */
void
CFilterBufferDecoder::CopyIn(const CFilterBufferDecoder& rhs)
{
  delete []m_pTranslated;
  delete m_pTranslator;
  m_pTranslator = new NonSwappingBufferTranslator(m_pTranslated);
  m_pTranslated = 0;

  // Copy the translated buffer:

  if(rhs.m_pTranslated) {
    m_pTranslated = new char[rhs.m_nBytes];
    delete [] m_pTranslator;
    m_pTranslator = new NonSwappingBufferTranslator(m_pTranslated);
    memcpy(m_pTranslated, rhs.m_pTranslated, rhs.m_nBytes);
  }

  // Now the easy stuff:

  m_nParamCount  = rhs.m_nParamCount;
  m_nBytes       = rhs.m_nBytes;
  m_nCurrentType = rhs.m_nCurrentType;
  m_nEntities    = rhs.m_nEntities;
  m_fSeenHeader  = rhs.m_fSeenHeader;
  m_pTranslator  = rhs.m_pTranslator;

}
/*
**  returns true if the buffer is a single precision buffer
**  this is the case if after processig the first event as single precision,
**  We get to an 'event' header again:
**  Parameters:
**    UInt_t nBytes  - Size of input buffer.
**    void*  pBuffer - Pointer to a 'header' buffer.
*/
bool
CFilterBufferDecoder::dataWidth(UInt_t nBytes, void* pBuffer)
{
  CXdrMemInputStream xdr(nBytes, pBuffer);
  xdr.Connect();


  string type;
  xdr >> type;			// 'header'
  ProcessHeader(xdr, *(reinterpret_cast<CAnalyzer*>(NULL)));	// Skip the header.
  xdr >> type;                  // 'event'
  int nBitmasks = numBitmasks(m_nParamCount);

  int bits=0;

  // Flip through the bit masks counting then number of parameters
  // to expect:

  for (int i=0; i < nBitmasks; i++) {
    int mask;
    xdr >> mask;
    bits += CountBits(mask);
  }
  // skip them as if they were floating:

  for (int i=0; i < bits; i++) {
    float param;
    xdr >> param;
  }
  // If it's single precision we are at an 'event' header except for
  // the very pathological case of a single output event.. in that case
  // alles ist verloren.


  xdr >> type;
  return type == "event";
}
