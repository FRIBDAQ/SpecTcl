

#include "DDASUnpacker.h"
#include "DDASHitUnpacker.h"
#include "ParameterMapper.h"

#include <TCLAnalyzer.h>
#include <Globals.h>

#include <algorithm>
#include <cstdint>
#include <tuple>

using namespace std;

namespace DAQ {
  namespace DDAS {

    ///////
    ///
    CDDASUnpacker::CDDASUnpacker(CParameterMapper& rParameterMapper) 
      : m_pParameterMapper(&rParameterMapper),
        m_unpacker(),
        m_channelList()
    {
      // there is only ever 1 hit per event...
      m_channelList.push_back(DDASHit());


    }

    CDDASUnpacker::~CDDASUnpacker() {
      delete m_pParameterMapper;
    }

    //////
    ///
    void CDDASUnpacker::setParameterMapper(CParameterMapper& rParameterMapper) 
    {
      m_pParameterMapper = &rParameterMapper;
    }

    //////
    ///
    CParameterMapper& CDDASUnpacker::getParameterMapper() const
    {
      return *m_pParameterMapper;
    }

    /////////
    ///
    Bool_t 
      CDDASUnpacker::operator()(const Address_t pEvent,
                                CEvent&         rEvent,
                                CAnalyzer&      rAnalyzer,
                                CBufferDecoder& rDecoder)
      {
        setEventSize(pEvent, rDecoder, rAnalyzer);

        auto pBody = reinterpret_cast<uint32_t*>(pEvent);
        size_t bodySize = *pBody; // # of 16-bit words in body (inclusive)

        
        DDASHit& hit = m_channelList.front();
        hit.Reset(); // make sure we don't carry state between events

        // unpack into the reusable ddas hit object
        m_unpacker.unpack(pBody, pBody + bodySize/sizeof(uint16_t), hit);

        // Pass the data to the user's code
        //  (m_pParameterMapper cannot be a nullptr)
        m_pParameterMapper->mapToParameters(m_channelList, rEvent);

        return kfTRUE;
      }



    /////////
    ///
    void CDDASUnpacker::setEventSize(const Address_t pEvent, CBufferDecoder& rDecoder, CAnalyzer& rAnalyzer) {

        TranslatorPointer<uint32_t> p32(*(rDecoder.getBufferTranslator()), pEvent);
        CTclAnalyzer& rAna = dynamic_cast<CTclAnalyzer&>(rAnalyzer);
        rAna.SetEventSize(*p32); 

    }

  } // end DDAS namespace
} // end DAQ namespace
