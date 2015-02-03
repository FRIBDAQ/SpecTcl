// This implementation of TreeParameter is based on the ideas and original code of::
//    Daniel Bazin
//    National Superconducting Cyclotron Lab
//    Michigan State University
//    East Lansing, MI 48824-1321
//

#ifndef __TREETESTSUPPORT_H
#define __TREETESTSUPPORT_H

#include <string>

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

class CParameter;
class CTreeParameter;
class CEvent;
class CTCLInterpreter;
class CTreeVariable;
class CTreeVariableProperties;

class TreeTestSupport
{
  static CTCLInterpreter* m_pInterpreter;
public:
  static void ClearMap();
  static CParameter* getParameter(CTreeParameter& param);
  static bool isRegistered(CTreeParameter& param);
  static CEvent*     getEvent();
  static void        ThrowIfNoParameter(CTreeParameter& param, const char* pd);
  static void        ThrowIfNoEvent(CTreeParameter& param, const char* pd);

  static CTCLInterpreter* getInterpreter();
  static void             InitTestInterpreter();
  static void             TeardownTestInterpreter();

  static void  ClearVariableMap();
  static CTreeVariableProperties* getVariableProperties(CTreeVariable& Properties);

};


void
CheckConstructed(CTreeParameter& rParam, std::string errorcomment,
		 std::string name, UInt_t bins, double start, double stop,
		 double inc, std::string units, bool changed);

std::string
MakeElementName(std::string base, int low, int num, int index);
#endif
