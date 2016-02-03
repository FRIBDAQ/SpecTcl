/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/*
  GatedEventFilter.h
*/

// Preprocessor includes, definitions.
#ifndef __GATEDEVENTFILTER_H //Required for current class
#define __GATEDEVENTFILTER_H

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif


#ifndef __HISTOTYPES_H
#include <histotypes.h>
#define __HISTOTYPES_H
#endif


#ifndef __EVENTFILTER_H
#include <EventFilter.h>
#define __EVENTFILTER_H
#endif

// Forward class definitions.

class CGateContainer;
class Event;
   
// Class.
/*!
     A gated event filter is a subclass of CEventFilter that
   uses a SpecTcl gate as the criterion for selecting events.
   In this implementation, we use the default output event formatting
   mechanisms that are implemented in the base class.
   
*/
class CGatedEventFilter : public CEventFilter {

   // Attributes.

   CGateContainer* m_pGateContainer;

 public:
  // Constructors.
   CGatedEventFilter(CGateContainer*  pGate=(CGateContainer*)kpNULL);
   CGatedEventFilter(std::string&,
						 CGateContainer*  pGate=(CGateContainer*)kpNULL);
   virtual ~CGatedEventFilter();

   /// Disallowed canonicals:
 
private:
   CGatedEventFilter(const CGatedEventFilter&);
   CGatedEventFilter& operator=(const CGatedEventFilter&);
   int operator==(const CGatedEventFilter&);
   int operator!=(const CGatedEventFilter&);
public:

   // Selectors:

   CGateContainer* getGateContainer() {
      return m_pGateContainer;
   }

  // Additional functions.

public:
   void setGateContainer(CGateContainer&);
   std::string getGateName();
   Int_t getGateID();
 protected:
   Bool_t CheckCondition(CEvent&);

};

#endif
