#include "DDASBuiltFitUnpacker.h"
#include "FitParameterMapper.h"

#include "DDASHit.h"
#include "FitHitUnpacker.h"

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
    CDDASBuiltFitUnpacker::CDDASBuiltFitUnpacker(const std::set<uint32_t>& validSourceIds)
      : m_sourceIds(validSourceIds),
	m_channelList(),
	max(-1)
    {
    }
    CDDASBuiltFitUnpacker::CDDASBuiltFitUnpacker(const CDDASBuiltFitUnpacker& rhs) :
      CEventProcessor(rhs),
      m_sourceIds(rhs.m_sourceIds),
      m_channelList(rhs.m_channelList),
      max(rhs.max)
    {
    }
    CDDASBuiltFitUnpacker::~CDDASBuiltFitUnpacker() {
      // we always pass a parameter mapper in by reference so this is never a nullptr
      delete m_pParameterMapper;
    }

    //////
    ///
    void CDDASBuiltFitUnpacker::setValidSourceIds(const std::set<uint32_t>& validSourceIds) 
    {
      m_sourceIds = validSourceIds;
    }

    //////
    ///
    std::set<uint32_t> CDDASBuiltFitUnpacker::getValidSourceIds() const
    {
      return m_sourceIds;
    }

    //////
    ///
    void CDDASBuiltFitUnpacker::setParameterMapper(FitParameterMapper& rParameterMapper) 
    {
      m_pParameterMapper = &rParameterMapper;
    }

    //////
    ///
    FitParameterMapper& CDDASBuiltFitUnpacker::getParameterMapper() const
    {
      return *m_pParameterMapper;
    }

    /////////
    ///

    std::vector<DDASFitHit>
    CDDASBuiltFitUnpacker::copyChannelList(std::vector<DDASFitHit>& chnList)
    {
      std::vector<DDASFitHit> tmp;
      for (auto & element : chnList) {
        tmp.push_back(element);
      }
      return tmp;
    }

    /////////
    ///
    Bool_t 
     CDDASBuiltFitUnpacker::operator()(const Address_t pEvent,
				       CEvent&         rEvent,
				       CAnalyzer&      rAnalyzer,
				       CBufferDecoder& rDecoder,
				       BufferTranslator& trans,
				       long thread)
      {
        m_channelList.clear();

	setEventSize(pEvent, rDecoder, rAnalyzer, trans);

        uint16_t* p16 = reinterpret_cast<uint16_t*>(pEvent);

        // parse all of the fragments that we care about
	Bool_t goodToSort = selectivelyParseData(p16, m_channelList, thread);

        // Pass the unpacked data to the user for assignment to their data structures
        //
        // note: m_pParameterMapper can never be a nullptr
	m_pParameterMapper->mapToParameters(m_channelList, rEvent);

        return goodToSort;
      }

    /////////
    ///
    void CDDASBuiltFitUnpacker::setEventSize(const Address_t pEvent, 
                                          CBufferDecoder& rDecoder, CAnalyzer& rAnalyzer, BufferTranslator& trans) {

      TranslatorPointer<uint32_t> p32(trans, pEvent);
      CThreadAnalyzer& rAna = dynamic_cast<CThreadAnalyzer&>(rAnalyzer);
      rAna.SetEventSize(*p32);
    }

    /////////
    ///
    Bool_t CDDASBuiltFitUnpacker::selectivelyParseData(uint16_t* p16, std::vector<DDASFitHit>& channellist, long thread)
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
    Bool_t CDDASBuiltFitUnpacker::parseAndStoreFragment(FragmentInfo& info, std::vector<DDASFitHit>& channellist, long thread) 
    {
      DDASFitHit hit;
      FitHitUnpacker unpacker;

      auto pBody      = reinterpret_cast<uint32_t*>(info.s_itemhdr);
      size_t bodySize = *pBody; // # of 16-bit words in body (inclusive)

      // parse the body of the ring item 
      // unpacker.decode(pBody,  hit );
      // m_channelList.push_back(hit);
      m_channelList.emplace(m_channelList.end());
      unpacker.unpack(pBody, pBody+bodySize/sizeof(uint16_t), m_channelList.back() );

      return kfTRUE;
    }

  } // end DDAS namespace
} // end DAQ namespace
