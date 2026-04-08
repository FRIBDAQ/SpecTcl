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
 * @file CVectorGates.h
 * @brief Define  gate classes on vector valued parameters.
 */

 #ifndef CVECTORGATES_H
 #define CVECTORGATES_H

#include "Gate.h"
#include "Point.h"
#include "CTreeParameterVector.h"
#include <vector>
#include <string>

/**
 * @class CVectorGate
 * 
 * This is the base class for vector gates.  The difference
 * between the varous vector gates is really only the implementation
 * of 'inGate'.  Much of this is similar to CCut.
 */
class CVectorGate : public CGate {
private:
    Float_t m_fLow;                // Low cut value.
    Float_t m_fHigh;               // High cut value.
protected:
    CTreeParameterVector m_vector; // Vector we check the gate on.

    // Canonicals:
public:
    CVectorGate(Float_t low, Float_t high, const CTreeParameterVector& vec);
    CVectorGate(const CVectorGate& other);
    virtual ~CVectorGate();
    CVectorGate& operator=(const CVectorGate& rhs);
    int operator==(const CVectorGate& rhs) const;
    int operator!=(const CVectorGate& rhs) const;

    // Selectors:

    Float_t low() const;
    Float_t high() const;
    CTreeParameterVector getVector() const;

    // Mutators:
protected:
    void setLow(Float_t low);
    void setHigh(Float_t high);
    void setVector(const CTreeParameterVector& vector);

    // Vitual method overrides:

public:
    
    virtual   CConstituentIterator Begin ()  ;
    virtual   CConstituentIterator End ()  ;
    virtual   UInt_t Size ()  ;
    virtual   std::string GetConstituent (CConstituentIterator& rIterator)  ;
    virtual   Bool_t inGate(CEvent& rEvent, const std::vector<UInt_t>& Params);
    virtual   Bool_t inGate (CEvent& rEvent) = 0;
    virtual   Bool_t inGate (Float_t x);
    virtual std::vector<FPoint>          getPoints();
    virtual std::string                  getVectorName() const;
};

/**
 *  @class CVectorAndGate
 * 
 * Subclass of CVectorGate that requires all vector elements to be in the gate. 
 */
class CVectorAndGate : public CVectorGate {
public:
    CVectorAndGate(Float_t low, Float_t high, const CTreeParameterVector& vec);
    CVectorAndGate(const CVectorAndGate& other);
    virtual ~CVectorAndGate();
    CVectorAndGate& operator=(const CVectorAndGate& rhs);
    int operator==(const CVectorAndGate& rhs) const;
    int operator!=(const CVectorAndGate& rhs) const;
public:
    virtual   std::string Type ()  const;
    virtual   Bool_t inGate (CEvent& rEvent);
};

/**
 *  @class CVectorOrGate 
 *    Subclass of CVectorGate that requires at least one vector element to
 * be in the gate.
 */

class CVectorOrGate : public CVectorGate {
public:
    CVectorOrGate(Float_t low, Float_t high, const CTreeParameterVector& vec);
    CVectorOrGate(const CVectorOrGate& other);
    virtual ~CVectorOrGate();
    CVectorOrGate& operator=(const CVectorOrGate& rhs);
    int operator==(const CVectorOrGate& rhs) const;
    int operator!=(const CVectorOrGate& rhs) const;
public:
    virtual   std::string Type ()  const;
    virtual   Bool_t inGate (CEvent& rEvent);
};

#endif

