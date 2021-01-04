/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  CAENModuleHits.h
 *  @brief: Provides for a module worth of CAENHits.
 */
#ifndef CAENMODULEHITS_H
#define CAENMODULEHITS_H
#include <vector>

class CAENHit;
/**
 * @class
 *    CAENModuleHits
 *       Contains the hits associated with a module.
 *       Note that these are stored as pointers to a set of undifferentiated
 *       CAENHit base class items.  THe hits are assumed to have been
 *       'newe'd in to existence.
 */
class CAENModuleHits {
private:
    int                       m_nSourceId;
    std::vector<CAENHit*>     m_Hits;
public:
    CAENModuleHits(int sid=0);
    CAENModuleHits(const CAENModuleHits& rhs);
    CAENModuleHits& operator=(const CAENModuleHits& rhs);
    
    virtual ~CAENModuleHits();
    void clear();
    void addHit(CAENHit* pHit);
    const std::vector<CAENHit*>& getHits() const;
    int getSourceId() const;
private:
    void copyIn(const CAENModuleHits& rhs);
};

#endif