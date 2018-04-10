

#include "DDASBuiltUnpacker.h"
#include "ParameterMapper.h"

#include "DDASHit.h"
#include "DDASHitUnpacker.h"

#include "Globals.h"
#include <TCLAnalyzer.h>

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
      m_pParameterMapper(&rParameterMapper)
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
    Bool_t 
      CDDASBuiltUnpacker::operator()(const Address_t pEvent,
                                CEvent&         rEvent,
                                CAnalyzer&      rAnalyzer,
                                CBufferDecoder& rDecoder)
      {
        m_channelList.clear();

        setEventSize(pEvent, rDecoder, rAnalyzer);

        uint16_t* p16 = reinterpret_cast<uint16_t*>(pEvent);

        // parse all of the fragments that we care about
        Bool_t goodToSort = selectivelyParseData(p16);

        // Pass the unpacked data to the user for assignment to their data structures
        //
        // note: m_pParameterMapper can never be a nullptr
        m_pParameterMapper->mapToParameters(m_channelList, rEvent);

        return goodToSort;
      }

    /////////
    ///
    void CDDASBuiltUnpacker::setEventSize(const Address_t pEvent, 
                                          CBufferDecoder& rDecoder, CAnalyzer& rAnalyzer) {

        TranslatorPointer<uint32_t> p32(*(rDecoder.getBufferTranslator()), pEvent);
        CTclAnalyzer& rAna = dynamic_cast<CTclAnalyzer&>(rAnalyzer);
        rAna.SetEventSize(*p32); 

    }


    /////////
    ///
    Bool_t CDDASBuiltUnpacker::selectivelyParseData(uint16_t* p16)
    {
      // index the fragments
        FragmentIndex parsedFragments(p16);

        // loop over the fragments
        for (auto& fragInfo : parsedFragments ) {

          // determine whether the user cares about this based on the source id
          if ( binary_search(m_sourceIds.begin(), m_sourceIds.end(), fragInfo.s_sourceId) ) {
            parseAndStoreFragment(fragInfo);
          }

        }

        return kfTRUE;
    }

    ////////
    ///
    Bool_t CDDASBuiltUnpacker::parseAndStoreFragment(FragmentInfo& info) 
    {
      DDASHit hit;
      DDASHitUnpacker unpacker;

      auto pBody      = reinterpret_cast<uint32_t*>(info.s_itembody);
      size_t bodySize = *pBody; // # of 16-bit words in body (inclusive)

      // parse the body of the ring item 
      unpacker.unpack(pBody, pBody+bodySize/sizeof(uint16_t), hit );

      m_channelList.push_back(hit);
    }

  } // end DDAS namespace
} // end DAQ namespace
