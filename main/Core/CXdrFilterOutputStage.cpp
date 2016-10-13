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
#include "CXdrFilterOutputStage.h"

#include "CXdrOutputStream.h"
#include <EventFilter.h>
#include <Event.h>
#include <ErrnoException.h>

#include <errno.h>

using namespace std;


/*!
   Construct the output stage.. When constructed, the output stage can't actually do anything.
*/
CXdrFilterOutputStage::CXdrFilterOutputStage() :
  m_pOutputEventStream(0)
{
}
/*!
   Destruction will get rid of the output stream which will summarily close it if
   necessary.  
*/
CXdrFilterOutputStage::~CXdrFilterOutputStage()
{
  delete m_pOutputEventStream; 
}

/*!
  Open the file.  If the file is already open, this is an error.
  If the file does not exist, the CXdrOutputStream will most likely
  toss an error.

  \param filename - The name of the file to open.

  \throw CErrnoException [EEXIST] if we are already open.
  \throw string from CXdrOutputStream if the file cannot be open for write (e.g.
                                      you're trying to create it in a directory that
				      does not allow write access by you.
*/

void
CXdrFilterOutputStage::open(string filename)
{
  if (m_pOutputEventStream) {
    errno = EEXIST;
    throw CErrnoException("Opening XDR Filter output stream");
  }
  else {
    m_pOutputEventStream = new CXdrOutputStream(filename); // Construct and open.
  }
}

/*!
  Close the stream.  If the stream is not open, throw an exception.

  \throw CErrnoException [ENOENT] if the stream is not open.
*/
void
CXdrFilterOutputStage::close()
{
  if (m_pOutputEventStream) {
    delete m_pOutputEventStream;
    m_pOutputEventStream = 0;
  }
  else {
    errno = ENOENT;
    throw CErrnoException("Closing CDR Filter output stream");
  }
}

/*!
   Called to describe the event to the output stream.  This will usually
   happen just after the open.
   \param parameterNames   - The names of the parameters that are going to be
                             written to the event file.
   \param parameterIds     - For each corresponding name in parameterNames,
                             this is the CEvent index of that parameter.
    
   \throw CErrnoException [ENOENT] if the file is not open, as we'll have to
                              write the headher record/
*/
void
CXdrFilterOutputStage::DescribeEvent(vector<string> parameterNames,
				     vector<UInt_t> parameterIds)
{
  if (!m_pOutputEventStream) {
    errno = ENOENT;
    throw CErrnoException("Describing event to closed output stream");
  }

  // We're going to need the parameter ids ourselves to fish them out of
  // the events:

  m_vParameterIds = parameterIds;

  // Now format the header and write it out.

  int parameterCount = parameterNames.size();

  (*m_pOutputEventStream) << "header";
  (*m_pOutputEventStream) << parameterCount;
  for (int i= 0; i < parameterCount; i++) {
    (*m_pOutputEventStream) << (parameterNames[i]);
  }
}

/*!
   Write an event to the output stream.  If the output stream is not open, this is an
   error.
   \param event - The event to write. We are only interested in the offsets in the
                  m_vParameterIds vector. 
   \throw CErrnoException [ENOENT] if the file is not open.
*/
void
CXdrFilterOutputStage::operator()(CEvent& event)
{
  if (!m_pOutputEventStream) {
    errno = ENOENT;
    throw CErrnoException("Writing event to output stream");
  }

  // we need to first construct and write the bitmasks.

   int nParams = m_vParameterIds.size();
   int nBitmaskwords = ((nParams + sizeof(unsigned)*8 - 1) /
			(sizeof(unsigned)*8)); // Assumes 8 bits/byte
   unsigned* Bitmask = new unsigned[nBitmaskwords];

   for(int i = 0; i < nBitmaskwords; i++) {
     Bitmask[i] = 0;
   }
   
   // Figure out the bit mask:  A bit is set for each valid parameter:
   
   int nValid = 0;
   for(int i =0; i < nParams; i++) {
     int id = m_vParameterIds[i];
     if((id < event.size()) && event[id].isValid()) {
       setBit(Bitmask, i);
       nValid++;
     }
   }

   // No point in writing an event that has no valid parameters in the 
   // selected subset.
   //
   if(!nValid) {
     delete []Bitmask;
     return;
   }

   // Declare required freespace to allow the output stream to close:
   // the buffer if this event doesn't fit.
   
   size_t intsize   = m_pOutputEventStream->sizeofInt();
   size_t floatsize = m_pOutputEventStream->sizeofFloat();
   size_t hdrsize   = m_pOutputEventStream->sizeofString("event");


   m_pOutputEventStream->Require((nBitmaskwords*intsize +
                                 nValid*floatsize       +
                                 hdrsize)); // Fudge??
   
   // Write the header:
   
   *m_pOutputEventStream << "event";

   
   // Write the bitmask:
   
   for(int i =0; i < nBitmaskwords; i++) {
      *m_pOutputEventStream << Bitmask[i];
   }

   // Write the valid parameters:
   
   for(int i =0; i < nParams; i++) {
     int id = m_vParameterIds[i];
     if((id < event.size()) && event[id].isValid()) {
       *m_pOutputEventStream << event[id];
     }
   }
   delete []Bitmask;
}

/*!
   Return the output stage type.
*/
string
CXdrFilterOutputStage::type() const
{
  return string("xdr");
}

/*
   Set a bit in a bit vector.  The bit is assumed to fit in the vector.
*/
inline void
CXdrFilterOutputStage::setBit(unsigned* bits, unsigned offset)
{
		// The code below assumes 8 bits per byte ...
   int element = offset/(sizeof(unsigned)*8);
   int mask    = 1 << (offset % (sizeof(unsigned)*8));
   bits[element] |= mask;
}
