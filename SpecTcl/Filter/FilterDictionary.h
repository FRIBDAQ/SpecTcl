/*
  FilterDictionary.h
*/

// Preprocessor includes, definitions.
#ifndef __FILTERDICTIONARY_H //Required for current class
#define __FILTERDICTIONARY_H

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#define __HISTOTYPES_H
#endif

#ifndef __DICTIONARY_H
#include "Dictionary.h"
#define __DICTIONARY_H
#endif

#ifndef __PARAMETER_H
#include "Parameter.h"
#define __PARAMETER_H
#endif

#ifndef __EVENTSINK_H
#include "EventSink.h"
#define __EVENTSINK_H
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#define __HISTOTYPES_H
#endif

#ifndef __GATECONTAINER_H
#include <GateContainer.h>
#define __GATECONTAINER_H
#endif

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

#ifndef __STL_VECTOR
#include <vector>
#define __STL_VECTOR
#endif

#ifndef __OUTPUTEVENTSTREAM_H
#include <OutputEventStream.h>
#define __OUTPUTEVENTSTREAM_H
#endif

#ifndef __EVENT_H
#include <Event.h>
#define __EVENT_H
#endif

#ifndef __GATEDEVENTFILTER_H
#include <GatedEventFilter.h>
#define __GATEDEVENTFILTER_H
#endif

// Class. (Singleton Pattern)
class CFilterDictionary : public CDictionary<CGatedEventFilter*> {
  // Attributes.
 private:
  static CFilterDictionary* m_pInstance;

  // Constructors. (Destructor is never called.)
 protected:
  CFilterDictionary();

  // Additional functions.
 public: //protected:
  static CFilterDictionary* GetInstance();
}; // CFilterDictionary.

typedef CFilterDictionary::DictionaryIterator CFilterDictionaryIterator;

#endif
