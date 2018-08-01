/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


#ifndef DAQ_DDAS_DDASBUILTUNPACKER_H
#define DAQ_DDAS_DDASBUILTUNPACKER_H

#include <config.h>
#include "FragmentIndex.h"
#include "DDASHit.h"

#include <EventProcessor.h>
#include <TranslatorPointer.h>
#include <TCLAnalyzer.h>

#include <set>
#include <memory>
#include <cstdint>

namespace DAQ {
  namespace DDAS {

    // Forward declare the parameter mapper
    class CParameterMapper;

    /*! \brief Raw data unpacker for built DDAS data
     *
     * If attaching SpecTcl to a ring buffer downstream of the event builder,
     * this is the unpacker for you to use. It is an event processor that will
     * parse the data and construct a set of DDASHit objects that contain
     * the parse data elements. Because built data is composed of fragments 
     * from a set of different sources that are not necessarily all part of 
     * DDAS, the user must provide a set of source ids to concern themselves with.
     * If a fragment with source id X is found in the built item and the user
     * said that X is a valid source id, this will process it. If X is not a 
     * valid source id, then it will be skipped over for a different event processor
     * to parse. 
     *
     * The user obtains access to the data through an object derived from class
     * ParameterMapper.  That class is responsible for mapping the data in the
     * DDASHit objects, that are produced by this, to their TreeParameters.
     * This class takes ownership of the object and deletes it on destructions
     * so the user must dynamically allocate the ParameterMapper they pass in.
     * In other words, a proper way to set this up would be:
     *
     * \code
     * MyParameterMapper* pMapper = new MyParameterMapper;
     * CDDASBuiltUnpacker unpacker({0, 1, 2}, *pMapper);
     * \endcode
     *
     * This unpacker does not support byte-swapping. Don't worry, it is not
     * likely to be an issue in the future unless you try to analyze your
     * system on a computer with a strange architecture.
     */
    class CDDASBuiltUnpacker : public  CEventProcessor
    {

      private:
        std::set<uint32_t>    m_sourceIds;        ///< source ids to parse
        std::vector<DDASHit>  m_channelList;      ///< list of parsed data
        CParameterMapper*     m_pParameterMapper; ///< the user's mapper

      public:

        /*! Constructor
         *
         * \param validSourceIds    list of source ids to parse
         * \param rParameterMapper  user's mapper (must be dynamically allocated, 
         *                          ownership transfers to class)
         */
        CDDASBuiltUnpacker(const std::set<uint32_t>& validSourceIds, 
                          CParameterMapper& rParameterMapper);

        /*! Destructor*/
        ~CDDASBuiltUnpacker();

        // Setters and getters
        
        /*! \brief Pass in new set of source ids
         * \param validSourceIds  the list
         */
        void setValidSourceIds(const std::set<uint32_t>& validSourceIds);

        /*! Retrieve the list of source ids */
        std::set<uint32_t> getValidSourceIds() const;

        /*! \brief Pass in a different parameter mapper
         *
         * The user must allocate the parameter dynamically before passing it in.
         * For example:
         * \code
         * auto pMapper = new MyParameterMapper;
         * unpacker.setParameterMapper(*pMapper);
         * \endcode
         *
         * Ownership of the mapper is transferred into this object.
         *
         * \param rParameterMapper  the user's parameter mapper
         */
        void setParameterMapper(CParameterMapper& rParameterMapper);

        /*! Retrieve the active mapper   */
        CParameterMapper& getParameterMapper() const;

        /*! \brief Process the raw data and call user's mapper
         *
         * This is designed to process data downstream from the event builder. The parsing handles
         * the body of built items which has the following structure
         * 
         * | index  | description                  | size (bytes) |
         * |--------|------------------------------|--------------|
         * | 0      | inclusive byte size of body  | 4            |
         * | 1      | Frag #0 : timestamp          | 8            |
         * | 2      | Frag #0 : source id          | 4            |
         * | 3      | Frag #0 : payload size       | 4            |
         * | 4      | Frag #0 : barrier type       | 4            |
         * | 5      | Frag #0 payload: item header | depends      |
         * | ...    | Frag #1 : timestamp          | 8            |
         * | ...    | ...                          | ...          |
         *
         * This sets the event size.
         *
         * \param pEvent     address in buffer to begin parsing at
         * \param rEvent     current SpecTcl event
         * \param rAnalyzer  the analyzer
         * \param rDecoder   current buffer decoder
         *
         */
        virtual Bool_t operator()(const Address_t pEvent,
            CEvent&         rEvent,
            CAnalyzer&      rAnalyzer,
            CBufferDecoder& rDecoder);

        // Utility methods
      protected:
        void setEventSize(const Address_t pEvent, CBufferDecoder& rDecoder,
                          CAnalyzer& rAnalyzer);

        Bool_t selectivelyParseData(uint16_t* p16);

        Bool_t parseAndStoreFragment(FragmentInfo& fragInfo);
    };

  } // end DDAS namespace
} // end DAQ namespace

#endif






