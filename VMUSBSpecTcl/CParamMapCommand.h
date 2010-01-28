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

#ifndef __CPARAMMAPCOMMAND_H
#define __CPARAMMAPCOMMAND_H

#ifndef __TCLOBJECTCOMMAND_H
#include <TCLObjectProcessor.h>
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

#ifndef __STL_MAP
#include <map>
#ifndef __STL_MAP
#define __STL_MAP
#endif
#endif


#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


// Forward class definitions:

class CTCLObject;
class CTCLInterpreter;

/*!
   This class implements the Tcl paramMap command extension.
   This class is used to create and maintain a mapping between
   digitizer slots and parameter names in SpecTcl.  This is used
   by the unpacker to know where to put parameters it has unpacked.
   
   The command form is:
  
\verbatim
   paramMap modulename moduletype slot channels
\endverbatim

   Where:
    - modulename is the name of the module whose channel map is being created.
    - moduletype is a integer that represents the type of the module to the3 unpacker.
    - slot       is the virtual slot of the module if it has one.
    - channels   is the list of channel names to enter into the map.

   Note that at the time the paramMap command is issued, the parameter
   names in channels  must have already been defined.  In this way we are able to put
   parameter numbers in the mapping so that at unpack time we don't have
   to manipulate pesky strings and do time consuming lookups.

*/
class CParamMapCommand : public CTCLObjectProcessor
{
  // Exported data types:

public:
  struct AdcMapping
  {
    std::string name;		// Allows clients to know who they are.
    int vsn;
    int type;
    int map[256];		// big enough to handle about anything
    void* extraData;            // Pointer for data the unpacker can hang on this.

    AdcMapping() :
       vsn(-1), type(-1) { for(int i=0; i < 256; i++) map[i] = -1;}
    AdcMapping(const AdcMapping& rhs) : name(rhs.name), vsn(rhs.vsn), type(rhs.type) {
      
      for(int i =0; i < 256; i++) map[i] = rhs.map[i];
      extraData = rhs.extraData;
    }
    AdcMapping& operator=(const AdcMapping& rhs) {
      name = rhs.name;
      vsn = rhs.vsn;
      type= rhs.type;
      for (int i =0; i < 256; i++) map[i] = rhs.map[i];
      extraData = rhs.extraData;
    }
    int& operator[](int i) { return map[i]; }

  };
  typedef std::map<std::string, AdcMapping> ParameterMap;
  typedef ParameterMap::iterator            ParameterMapIterator;

private:
  static ParameterMap   m_theMap;

public:
  // Constructors and (disallowed) cannonicals:

  CParamMapCommand(CTCLInterpreter& interp);
  virtual ~CParamMapCommand();
private:
  CParamMapCommand(const CParamMapCommand& rhs);
  CParamMapCommand& operator=(const CParamMapCommand& rhs);
  int operator==(const CParamMapCommand& rhs) const;
  int operator!=(const CParamMapCommand& rhs) const;

public:
  static ParameterMap& getMap();

  // Virtual function overrides.
protected:
  int operator()(CTCLInterpreter& interp,
		 STD(vector)<CTCLObject>& objv);

  // utilities:

private:
  static STD(string) Usage();
};

#endif
