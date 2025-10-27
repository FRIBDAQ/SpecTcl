/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins
	     Aaron Chester
	     FRIB
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** 
 * @file  DDASFitHit.h
 * @brief Extends DDASHit to include fit data that's been tacked on to the
 * end of a hit.
 */

#ifndef DDASFITHIT_H
#define DDASFITHIT_H

#include <DDASHit.h> // Base class

#include <stdexcept>

#include "fit_extensions.h" // Defines HitExtension.

namespace ddastoys {

    /**
     * @class DDASFitHit
     * @brief Encapsulates data for DDAS hits that may have fitted traces.
     *
     * @details
     * These objects are produced by `DDASFitHitUnpacker::decode()`. 
     * They are basically just ddasfmt::DDASHits with some extra fields.     
     */

    class DDASFitHit : public ddasfmt::DDASHit
    {
    private:
	bool m_haveExtension;     //!< True iff has extension data.
	HitExtension m_extension; //!< The extension data.
      
    public:
	DDASFitHit() { Reset(); } //!< Constructor.
	virtual ~DDASFitHit() {}  //!< Destructor.

	/**
	 * @brief Assignment operator.
	 * @param rhs Reference to DDASFitHit for assignment.
	 * @return Reference to lhs.
	 * @details
	 * Calls base class operator= and sets the hit extension (if present).
	 */
	DDASFitHit& operator=(const DDASFitHit& rhs) {
	    if (this != &rhs) {
		ddasfmt::DDASHit::operator=(rhs);
		m_haveExtension = false;
		if (rhs.hasExtension()) {
		    auto ext = rhs.getExtension();
		    setExtension(ext);
		}
	    }
	    return *this;
	}

	/** @brief Reset the hit information. */
	void Reset() {
	    m_haveExtension = false;
	    ddasfmt::DDASHit::Reset(); // Reset base class membrers.
	}
	/** 
	 * @brief Set the hit extension information for this hit. 
	 * @param extension Reference to the extension for this hit.
	 */
	void setExtension(const HitExtension& extension) {
	    m_extension = extension;
	    m_haveExtension = true;
	}
	/**
	 * @brief Check whether hit has a fit extension.
	 * @return True if the hit contains an extension, false otherwise.
	 */
	bool hasExtension() const { return m_haveExtension; }
	/** 
	 * @brief Get the extension data from the current hit.
	 * @throw std::logic_error If the hit does not contain an extension.
	 * @return Reference to the extension of the current hit.
	 */      
	const HitExtension& getExtension() const {
	    if (m_haveExtension) {
		return m_extension;
	    } else {
		throw std::logic_error(
		    "Asked for extension for event with none"
		    );
	    }
	}   
    };
    
} // namespace ddastoys

#endif
