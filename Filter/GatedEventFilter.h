/*
  GatedEventFilter.h
*/

// Preprocessor includes, definitions.
#ifndef __GATEDEVENTFILTER_H //Required for current class
#define __GATEDEVENTFILTER_H

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
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
   CGatedEventFilter(string&,
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
