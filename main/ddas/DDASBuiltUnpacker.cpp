#include "DDASBuiltUnpacker.h"
#include "ParameterMapper.h"

#include "DDASHit.h"
#include "DDASHitUnpacker.h"

#include "Globals.h"
//#include <TCLAnalyzer.h>
#include <ThreadAnalyzer.h>
#include <ZMQSenderClass.h>

#include <algorithm>
#include <cstdint>

using namespace std;

namespace DAQ {
  namespace DDAS {

    ///////
    ///
    CDDASBuiltUnpacker::CDDASBuiltUnpacker(const std::set<uint32_t>& validSourceIds, 
					   CParameterMapper& rParameterMapper)
      : m_sourceIds(validSourceIds),
	m_channelList(),
	max(-1),
	m_pParameterMapper(&rParameterMapper)
    {
      m_VectorList.resize(100);
    }

    CDDASBuiltUnpacker::~CDDASBuiltUnpacker() {
      // we always pass a parameter mapper in by reference so this is never a nullptr
      delete m_pParameterMapper;
    }

    //////
    ///
    void CDDASBuiltUnpacker::setValidSourceIds(const std::set<uint32_t>& validSourceIds) 
    {
      m_sourceIds = validSourceIds;
    }

    //////
    ///
    std::set<uint32_t> CDDASBuiltUnpacker::getValidSourceIds() const
    {
      return m_sourceIds;
    }

    //////
    ///
    void CDDASBuiltUnpacker::setParameterMapper(CParameterMapper& rParameterMapper) 
    {
      m_pParameterMapper = &rParameterMapper;
    }

    //////
    ///
    CParameterMapper& CDDASBuiltUnpacker::getParameterMapper() const
    {
      return *m_pParameterMapper;
    }

    /////////
    ///
    Bool_t 
      CDDASBuiltUnpacker::operator()(const Address_t pEvent,
				     CEvent&         rEvent,
				     CAnalyzer&      rAnalyzer,
				     CBufferDecoder& rDecoder,
				     BufferTranslator& trans,
				     long thread)
    {
      bool doesExist = std::find(std::begin(m_threadId), std::end(m_threadId), thread) != std::end(m_threadId);
      if (!doesExist)
	{
	  // add the thread id in the vector
	  m_threadId.push_back(thread);
	  // create the element of the vector of vector of DDASHit with a copy of the channelList
	  m_VectorList[thread] = m_channelList;
	  // max thread number
	  if (thread > max)
	    max = thread;
	  // once we have the number of threads running it's time to resize the vector of vector
	  if (m_threadId.size() == max+1)
	    m_VectorList.resize(max+1);
	}	
      
      m_VectorList[thread].clear();

      setEventSize(pEvent, rDecoder, rAnalyzer, trans);
      
      uint16_t* p16 = reinterpret_cast<uint16_t*>(pEvent);

      // parse all of the fragments that we care about
      Bool_t goodToSort = selectivelyParseData(p16, thread);

      // Pass the unpacked data to the user for assignment to their data structures
      //
      // note: m_pParameterMapper can never be a nullptr
      m_pParameterMapper->mapToParameters(m_VectorList[thread], rEvent);

      return goodToSort;
    }
    
    /////////
    ///
    void CDDASBuiltUnpacker::setEventSize(const Address_t pEvent, 
                                          CBufferDecoder& rDecoder, CAnalyzer& rAnalyzer, BufferTranslator& trans) {

      TranslatorPointer<uint32_t> p32(trans, pEvent);
      CThreadAnalyzer& rAna = dynamic_cast<CThreadAnalyzer&>(rAnalyzer);
      rAna.SetEventSize(*p32);
      //      std::cout << rAna.GetEventSize() << std::endl;
      
      //      TranslatorPointer<uint32_t> p32(*(rDecoder.getBufferTranslator()), pEvent);
      //      CTclAnalyzer& rAna = dynamic_cast<CTclAnalyzer&>(rAnalyzer);
      //      rAna.SetEventSize(*p32); 

    }


    /////////
    ///
    Bool_t CDDASBuiltUnpacker::selectivelyParseData(uint16_t* p16, long thread)
    {
      // index the fragments
        FragmentIndex parsedFragments(p16);

        // loop over the fragments
        for (auto& fragInfo : parsedFragments ) {

          // determine whether the user cares about this based on the source id
          if ( binary_search(m_sourceIds.begin(), m_sourceIds.end(), fragInfo.s_sourceId) ) {
            parseAndStoreFragment(fragInfo, thread);
          }

        }

        return kfTRUE;
    }

    ////////
    ///
    Bool_t CDDASBuiltUnpacker::parseAndStoreFragment(FragmentInfo& info, long thread) 
    {
      DDASHit hit;
      DDASHitUnpacker unpacker;

      auto pBody      = reinterpret_cast<uint32_t*>(info.s_itembody);
      size_t bodySize = *pBody; // # of 16-bit words in body (inclusive)

      // parse the body of the ring item 
      unpacker.unpack(pBody, pBody+bodySize/sizeof(uint16_t), hit );

      m_VectorList[thread].push_back(hit);
    }

  } // end DDAS namespace
} // end DAQ namespace
