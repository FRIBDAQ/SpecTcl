#include "DDASBuiltUnpacker.h"
#include "ParameterMapper.h"

#include "DDASHit.h"
#include "DDASHitUnpacker.h"

#include "PipelineData.h"
#include <ThreadAnalyzer.h>
#include <ZMQRDPatternClass.h>

#include "Globals.h"

#include <algorithm>
#include <cstdint>
#include <mutex>


using namespace std;

bool debug = false;
std::mutex m;

namespace DAQ {
  namespace DDAS {

    ///////
    ///
    CDDASBuiltUnpacker::CDDASBuiltUnpacker(const std::set<uint32_t>& validSourceIds)
      : m_sourceIds(validSourceIds),
	m_channelList(),
	max(-1)
    {
    }
    CDDASBuiltUnpacker::CDDASBuiltUnpacker(const CDDASBuiltUnpacker& rhs) :
      CEventProcessor(rhs),
      m_sourceIds(rhs.m_sourceIds),
      m_channelList(rhs.m_channelList),
      max(rhs.max)
    {
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
    
    std::vector<DDASHit>
    CDDASBuiltUnpacker::copyChannelList(std::vector<DDASHit>& chnList)
    {
      std::vector<DDASHit> tmp;
      for (auto & element : chnList) {
	tmp.push_back(element);
      }
      return tmp;
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
      std::vector<DDASHit> tmp;
      tmp.clear();

      tmp = m_channelList; //copyChannelList(m_channelList);

      setEventSize(pEvent, rDecoder, rAnalyzer, trans);

      uint16_t* p16 = reinterpret_cast<uint16_t*>(pEvent);

      // parse all of the fragments that we care about
      Bool_t goodToSort = selectivelyParseData(p16, tmp, thread);

      // Pass the unpacked data to the user for assignment to their data structures
      //
      // note: m_pParameterMapper can never be a nullptr
      if (debug){
	for (auto & element : tmp) {
	  std::cout << " energy: " << element.GetEnergy() << " channel length: " << element.GetChannelLength() << " slotID: " << element.GetSlotID()
		    << " crate ID: " << element.GetCrateID() << " channelID: " << element.GetChannelID() << " ModMSPS: " << element.GetModMSPS() << std::endl;
	}
      }

      m_pParameterMapper->mapToParameters(tmp, rEvent);
      
      return goodToSort;
    }
    
    /////////
    ///
    void CDDASBuiltUnpacker::setEventSize(const Address_t pEvent, 
                                          CBufferDecoder& rDecoder, CAnalyzer& rAnalyzer, BufferTranslator& trans) {

      TranslatorPointer<uint32_t> p32(trans, pEvent);
      CThreadAnalyzer& rAna = dynamic_cast<CThreadAnalyzer&>(rAnalyzer);
      rAna.SetEventSize(*p32);
    }

    /////////
    ///
    Bool_t CDDASBuiltUnpacker::selectivelyParseData(uint16_t* p16, std::vector<DDASHit>& channellist, long thread)
    {
      // index the fragments
        FragmentIndex parsedFragments(p16);

        // loop over the fragments
        for (auto& fragInfo : parsedFragments ) {

          // determine whether the user cares about this based on the source id
          if ( binary_search(m_sourceIds.begin(), m_sourceIds.end(), fragInfo.s_sourceId) ) {
            parseAndStoreFragment(fragInfo, channellist, thread);
          }

        }

        return kfTRUE;
    }

    ////////
    ///
    Bool_t CDDASBuiltUnpacker::parseAndStoreFragment(FragmentInfo& info, std::vector<DDASHit>& channellist, long thread) 
    {
      DDASHitUnpacker unpacker;

      auto pBody      = reinterpret_cast<uint32_t*>(info.s_itembody);
      size_t bodySize = *pBody; // # of 16-bit words in body (inclusive)

      // parse the body of the ring item
      m_channelList.emplace(m_channelList.end());
      unpacker.unpack(pBody, pBody+bodySize/sizeof(uint16_t), m_channelList.back() );
      return kfTRUE;
    }

    
  } // end DDAS namespace
} // end DAQ namespace
