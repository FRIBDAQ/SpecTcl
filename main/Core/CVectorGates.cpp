/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2027.

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
/**
 * @file CVectorGates.cpp
 * @brief Implement gates on vector valued parameters.
 */

#include "CVectorGates.h"
#include "SingleItemIterator.h"
#include <sstream>

/**
 * constructor
 *   @param low - gate low limit.
 *   @param high - gate high limit.
 *   #param vec - The vector to check against.
 */
CVectorGate::CVectorGate(Float_t low, Float_t high, const CTreeParameterVector& vec) :
    m_fLow(low), m_fHigh(high), m_vector(vec)
{}

/** copy constructor
 * @param other - the gate we are copy constructing into us:
 */

CVectorGate::CVectorGate(const CVectorGate& other) :
    m_fLow(other.m_fLow), m_fHigh(other.m_fHigh), m_vector(other.m_vector)
{}

/**
 * destructor
 */
CVectorGate::~CVectorGate() {}

/**
 * assignment:
 * 
 * @param rhs - the gate being assigned to this.
 * @return CVectorGate& reference to this.
 */
CVectorGate&
CVectorGate::operator=(const CVectorGate& rhs) {
    if (this != &rhs) {
        m_fLow = rhs.m_fLow;
        m_fHigh= rhs.m_fHigh;
        m_vector = rhs.m_vector;
    }
    return *this;
}
/**
 * Equality comparison
 * 
 * @param rhs - gate we're comparing to.
 * @return int - non zero if equality.
 * @note equality means the same vector and same limits.
 * 
 */
int
CVectorGate::operator==(const CVectorGate& rhs) const {
    return (
        (m_fLow == rhs.m_fLow) && 
        (m_fHigh == rhs.m_fHigh) &&
        (m_vector == rhs.m_vector)
    );
}
/**
 *  inequality comparison
 * @param rhs - gate we're comparing to.
 * @return int - non zero if not equal
 * @note just the logical inverse of operator==
 */
int
CVectorGate::operator!=(const CVectorGate& rhs) const {
    return !operator==(rhs);
}

/** 
 * low
 * @return Float_t  the gate low limit.
 * 
 */
Float_t
CVectorGate::low() const {
    return m_fLow;
}
/**
 * high
 * @return Float_t high limit of the gate.
 */
Float_t
CVectorGate::high() const {
    return m_fHigh;
}
/**
 *  getVector
 * @return CTreeParameterVector - vector the gate is evaluated on.
 */
CTreeParameterVector
CVectorGate::getVector() const {
    return m_vector;                     // copy constructs.
}

/**
 * setLow
 *   @param low - new gate low limit.
 */
void
CVectorGate::setLow(Float_t low) {
    m_fLow = low;
}
/**
 * setHigh
 *   @param  high - new gate high limit.
 */
void
CVectorGate::setHigh(Float_t high) {
    m_fHigh = high;
}
/**
 * setVector
 *   @param vec - new vector that gate is checked on.
 */
void
CVectorGate::setVector(const CTreeParameterVector& vec) {
    m_vector = vec;
}


// Constituent iterators... gates are made up of constituents that have
// some textual representation.  In our case, like Cut, we
// use a single item iterator that will just return our limits.

/**
 * Begin
 *    Start constituent iteration.
 * 
 * @return CConstituentIterator
 */
CConstituentIterator
CVectorGate::Begin() {
    CSingleItemIterator i;
    return CConstituentIterator(i);
}
/**
 * End
 *    End iterator.
 * 
 * @param CConstituentIterator
 */
CConstituentIterator
CVectorGate::End() {
    CConstituentIterator i = Begin();
    i++;
    return i;
}
/**
 *  Size
 *    Return the number of items the constituent iterator an return.
 * 
 * @return Size_t
 * @retval 1
 */
UInt_t
CVectorGate::Size() {
    return 1;
}
/**
 * getConstituent
 * 
 * @param p - a constituent iterator.
 * @return std::string - empty string if iterator is end else vectorname low high
 * 
 */
std::string
CVectorGate::GetConstituent(CConstituentIterator& p) {
    std::string result;
    auto e = End();
    if (p != e) {
        std::stringstream svalue;
        svalue << m_vector.name() << ' ' << m_fLow << ' ' << m_fHigh;
        result = svalue.str();
        
    } else {
        result = "";
    }
    return result;
}
/**
 * inGate 
 *  @param rEvent - the event to operate on.
 *  @param params - vecto of some parameters
 *  @return Bool_t if the gate was made.  We just return inGate(rEvent) which subclasses implement.
 */
Bool_t
CVectorGate::inGate(CEvent& rEvent, const std::vector<UInt_t>& params) {
    return inGate(rEvent);          // implemented in concrete classes.
}
/**
 * inGtae
 *   @param x - value to check
 *   @return Bool_t  true if x is in the limits of the gate.
 *   @note as with CCut, the gate is inclusive of the low and exclusive of high.
 */
Bool_t
CVectorGate::inGate(Float_t x) {
    return ((x >= low()) && (x < high()));
}
/**
 * getPoints
 *    Returns a vector with (low, 0.0) (high, 0.0) in it.
 * 
 * @return std::vector<FPoint>
 */
std::vector<FPoint>
CVectorGate::getPoints() {
    std::vector result = {
        FPoint(low(), 0.0),
        FPoint(high(), 0.0)
    };
    return result;
}
/**
 * getVectorName
 *    @return - get the name of the vector.
 */
std::string
CVectorGate::getVectorName() const {
    return m_vector.name();
}
 /////////////////////////////// Base class implementation (CVectorGate):


 ////////////////////////////// And gate implementation (CVectorAndGate):

 ///////////////////////////// Or gate implementation (CVectorOrGate):