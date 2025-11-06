

#include "DDASBuiltUnpacker.h"
#include "ParameterMapper.h"

#include <DDASHit.h>
#include <DDASHitUnpacker.h>
#include <DataFormat.h>

#include "Globals.h"
#include <TCLAnalyzer.h>

#include <algorithm>
#include <cstdint>


using namespace std;
using namespace ddasfmt;


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
        ::ufmt::FragmentIndex parsedFragments(p16);

        // loop over the fragments
        for (auto& fragInfo : parsedFragments ) {

          // determine whether the user cares about this based on the source id
          if ( binary_search(m_sourceIds.begin(), m_sourceIds.end(), fragInfo.s_sourceId) ) {
            parseAndStoreFragment(fragInfo);
          }

        }

        return kfTRUE;
    }

      /////////
      ///
      /**
       * @note ASC (11/6/25): Follow procedure outlined in
       * DDASBuiltFitUnpacker and DDASFitHitUnpacker:
       *   - Cast fragment to ring item
       *   - Check if the body header is empty or not and set the body
       *     pointer appropriately
       *   - Calculate the body size and define the extent of the hit
       *   - Unpack
       */
      Bool_t CDDASBuiltUnpacker::parseAndStoreFragment(::ufmt::FragmentInfo& info) 
    {
	DDASHitUnpacker unpacker; // DDASFormat hit unpacker
	DDASHit hit;              // Unpacked data goes here
	const uint32_t* pBody;    // Pointer to Pixie data payload
	uint32_t bodyHeaderSize;  // Bytes in body header

	auto pItem = reinterpret_cast<const ::ufmt::RingItem*>(info.s_itemhdr);
	
	// RingItem struct allows us to discriminate between data formats:
	// either s_mbz == 0 in the case of no body header or it is the first
	// 32-bit word of the header (size word) and is therefore non-zero
	
	if (pItem->s_body.u_noBodyHeader.s_mbz) {
	    pBody = reinterpret_cast<const uint32_t*>(pItem->s_body.u_hasBodyHeader.s_body);
	    bodyHeaderSize = pItem->s_body.u_hasBodyHeader.s_bodyHeader.s_size;
	} else {
	    pBody = reinterpret_cast<const uint32_t*>(pItem->s_body.u_noBodyHeader.s_body);
	    bodyHeaderSize = sizeof(uint32_t);
	}

	uint32_t bodySize = pItem->s_header.s_size - bodyHeaderSize - sizeof(::ufmt::RingItemHeader);
	const uint32_t* pEnd = pBody + bodySize;

	unpacker.unpack(pBody, pEnd, hit);
	m_channelList.push_back(hit);
	
	return kfTRUE;
    }

  } // end DDAS namespace
} // end DAQ namespace
