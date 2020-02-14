/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

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
#ifndef DDASFITHIT_H
#define DDASFITHIT_H

/** @file:  DDASFitHit.h
 *  @brief: Extends DDASHit to include fit data that's been tacked on to the
 *          end of a hit with traces.
 */

#include "DDASHit.h"               // Base class
#include "functions.h"              // Defines HitExtension.
#include "lmfit.h"
#include <stdexcept>

namespace DAQ {
    namespace DDAS {
        

/**
 * @class  DDASFitHit
 *     Encapsulates data for ddas hits that may have fitted traces.
 *     This is produced by FitHitUnpacker::decode.  This is basically
 *     just a DDASHit with extra fields.
 */

class DDASFitHit : public DAQ::DDAS::DDASHit
{
private:
    bool               m_haveExtension;
    ::DDAS::HitExtension m_Extension;
public:
    DDASFitHit() {Reset();}
    virtual ~DDASFitHit() {}
    
    void Reset() {
        m_haveExtension = false;
        DAQ::DDAS::DDASHit::Reset();    // Reset base class membrers.
    }
    
    
    void setExtension(const ::DDAS::HitExtension& extension) {
        m_Extension = extension;
        m_haveExtension = true;
    }
    bool hasExtension() const { return m_haveExtension; }
    const ::DDAS::HitExtension& getExtension() const {
        if (m_haveExtension) {
            return m_Extension;
        } else {
            throw std::logic_error("Asked for extension for event with none");
        }
    }
    
    
    
};
 
 ////////////////////////   Close namespaces //////////////

    }                          // DDAS
}                              // DAQ





#endif