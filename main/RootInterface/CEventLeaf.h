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

/** @file:  CEventLeaf.h
 *  @brief: Define a root TLeaf that gets data from a CEvent item.
 */
#ifndef CEVENTLEAF_H
#define CEVENTLEAF_H
#include <TLeafD.h>

class CEvent;                 //!< Getting data from this sort of thing.
class TBranch;
class TBuffer;

/**
 * @class CEventLeaf
 *     This class is derived from TLeaf.  It provides a leaf that can fetch
 *     data from an underlying CEvent object.  This is done by:
 *     - Providing a settable static CEvent object pointer.
 *     - Overriding the Value Getters so that values are gotten from that
 *       CEvent rather than from some pointer as Root normally operates.
 */
class CEventLeaf : public TLeafD
{
private:
    static CEvent*  m_pCurrentEvent;         // Event from which we'll get data.
private:
    unsigned m_nId;                          // Parameter id.
public:
    CEventLeaf();
    CEventLeaf(TBranch* parent, const char* name, unsigned id);
    
    virtual Double_t GetValue(Int_t i =0) const;  // Root defines all in terms of this by default.
    void FillBasket(TBuffer& b);
    
    static void setEvent(CEvent& rEvent);
    
    
    ClassDef(CEventLeaf, 1);
};

#endif