/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2008

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#ifndef __HANDLEMANAGER_H
#define __HANDLEMANAGER_H

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

/**
 ** This class provides a manager for handles.
 ** A handle is a text string that will be handed out to stand for
 ** A pointer to something.  When handed back to a Tcl command
 ** this package helps retrieve the original handle.
 ** Storage poitned to by these handles is managed by the client.
 **
 */
class CHandleManager {

  // Data types:

private:
  typedef std::map<std::string, void*> HandleMap;
  typedef HandleMap::iterator HandleIterator;

  // Private object data/attributes

private:
  std::string m_prefix;
  int         m_index;  
  HandleMap   m_handles;


  // Canonicals:

public:
  CHandleManager(std::string prefix);
private:
  CHandleManager(const CHandleManager& rhs);
  CHandleManager& operator=(const CHandleManager& rhs);
  int operator==(const CHandleManager& rhs) const;
  int operator!=(const CHandleManager& rhs) const;

  // Handle operations:
public:
  std::string add(void* p);
  void*       find(std::string handle);
  void        unregister(std::string handle);

  
};

#endif
