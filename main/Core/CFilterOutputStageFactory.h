#ifndef CFILTEROUTPUTSTAGEFACTORY_H
#define CFILTEROUTPUTSTAGEFACTORY_H
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

#include <string>
#include <list>

class CFilterOutputStageCreator;
class CFilterOutputStage;

/*!
   This is a factory for generating Filter output stages.
   Filter output stages are responsible for formatting events
   that pass the filter into an output file.  This separation
   of concerns, filtering and formatting allows SpecTcl to be used
   to prepare events for arbitrary follow-on analysis software (e.g. root,
   paw etc).

   The factory is a participant in the following software patterns:
   - singleton - only one factory will exist in SpecTcl at a time.
   - extensible factory - the set of formatters is not hardwired into
     this class, but represented as a run-time configurable list of 
     creator helpers.  For each creation, the factory will attempt to
     find a matching creator for the type of formatter requested.
     See CFilterOutputStageCreator for the requirements of creators and
     the register member of this class to register new creators.
     See CFilterOutputStage for the requirements of a filter output stage.

*/
class CFilterOutputStageFactory {
private:
  typedef std::list<CFilterOutputStageCreator*>   CreatorList;
  typedef CreatorList::iterator                   CreatorIterator;
  typedef CreatorList::const_iterator             ConstCreatorIterator;
  
  CreatorList                       m_creators;

  static CFilterOutputStageFactory*  m_pInstance;

  // Canonicals for singletons are private.
private:
  CFilterOutputStageFactory();
  ~CFilterOutputStageFactory();
  CFilterOutputStageFactory(const CFilterOutputStageFactory& rhs);           // unimplemented.
  CFilterOutputStageFactory& operator=(const CFilterOutputStageFactory& rhs); // unimplemented
  int operator==(const CFilterOutputStageFactory& rhs) const;                // unimplemented.
  int operator!=(const CFilterOutputStageFactory& rhs) const;                // unimplemented.
  
public:
  static CFilterOutputStageFactory& getInstance();
  
  CFilterOutputStage* create(std::string type) const;
  void                Register(CFilterOutputStageCreator& creator);
  std::string         document() const;

private:
  void registerStockCreators();

};



#endif
