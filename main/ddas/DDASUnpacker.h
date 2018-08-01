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



#ifndef DAQ_DDAS_DDASUNPACKER_H
#define DAQ_DDAS_DDASUNPACKER_H

#include <config.h>
#include "DDASHit.h"
#include "DDASHitUnpacker.h"

#include <EventProcessor.h>
#include <TranslatorPointer.h>
#include <TCLAnalyzer.h>

#include <vector>
#include <cstdint>



namespace DAQ {
  namespace DDAS {

    // Forward declarations
    class CParameterMapper;

    /*! \brief Raw data unpacker for unbuilt data
     *
     * The user should use this to process data upstream of or in the absence
     * of the event builder. This processes raw data and generates one DDASHit
     * object per event. The user is expected to derive a class from the
     * ParameterMapper base class and pass that object to this unpacker to call
     * at the end of parsing each event. In that way, this class will deal with
     * the raw data and allow the user to interact with it at a much higher
     * level. This class will take ownership of the user's mapper and will
     * attempt to delete it at the time of destruction. For that reason, it is
     * important that the user dynamically allocates their mapper on the heap.
     * A proper way to do this is:
     * 
     * \code
     * MyParameterMapper* pMapper = new MyParameterMapper;
     * CDDASUnpacker unpacker(*pMapper);
     * \endcode
     *
     * This unpacker does not support byte-swapping. Don't worry, it is not
     * likely to be an issue in the future unless you try to analyze your
     * system on a computer with a strange architecture.
     */
    class CDDASUnpacker : public  CEventProcessor
    {

      private:
        CParameterMapper*    m_pParameterMapper;  ///< the user's mapper
        DDASHitUnpacker      m_unpacker;          ///< the unpacker
        std::vector<DDASHit> m_channelList;       ///< reusable hit list

      public:

        /*! \brief Constructor
         * \param rParameterMapper  user's mapper (must be dynamically allocated, 
         *                                         ownership transfers to class)
         *
         */
        CDDASUnpacker(CParameterMapper& rParameterMapper);

        /*! Destructor   */
        ~CDDASUnpacker();

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
        void setParameterMapper(CParameterMapper& pParameterMapper);

        /*! Retrieve the active parameter */
        CParameterMapper& getParameterMapper() const;

        /*! \brief Process the raw data and call user's mapper
         *
         * This is designed to process data from the DDAS readout program. See
         * the documentation in \ref readout to understand the structure.
         * 
         * This sets the event size in the CEvent.
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
    };

  } // end DDAS namespace
} // end DAQ namespace

#endif






