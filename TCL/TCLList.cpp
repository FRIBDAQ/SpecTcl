//  CTCLList.cpp
// Encapsulates a TCL List.  Lists in TCL are 
// sets of strings with space delimeters e.g.:
//    { This is a list }     - Four element list.
//
//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//
//////////////////////////.cpp file/////////////////////////////////////////////////////

//
// Header Files:
//


#include "TCLList.h"                               
#include "TCLInterpreter.h"
#include <string.h>
#include <assert.h>
#include <malloc.h>		// Since Tcl uses malloc/free

static const char* Copyright = 
"CTCLList.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CTCLList

//////////////////////////////////////////////////////////////////////////
//
// Function:
//    CTCLList(CTCLInterpreter* pInterp, const char* am_pList)
//    CTCLList(CTCLInterpreter* pInterp, const std::string& rList)
//
// Operation Type:
//    Constructor
CTCLList::CTCLList(CTCLInterpreter* pInterp, const char* am_pList) :
  CTCLInterpreterObject(pInterp),
  m_pList(0)
{
  assert(am_pList != (char*)kpNULL);

  m_pList = new char[strlen(am_pList) + 1];
  strcpy(m_pList, am_pList);
}
CTCLList::CTCLList(CTCLInterpreter* pInterp, const std::string& rList) :
  CTCLInterpreterObject(pInterp),
  m_pList(0)
{
  m_pList = new char[strlen(rList.c_str()) + 1];
  strcpy(m_pList, rList.c_str());

}
//////////////////////////////////////////////////////////////////////////
//
// Function:
//   int operator==(const CTCLList& aCTCLList)
// Operation type:
//   equality comparison operator.
//
int
CTCLList::operator==(const CTCLList& aCTCLList) {
  //
  //  If both list pointers are null, things are equal...
  //
  if((m_pList           == (char*)kpNULL) &&
     (aCTCLList.m_pList == (char*)kpNULL))
    return (int)kfTRUE;
  // 
  // If either but not both pointers are null, then unequal:
  //
  if( (m_pList           == (char*)kpNULL) ||
      (aCTCLList.m_pList == (char*)kpNULL))
    return (int)kfFALSE;
  //
  //  Otherwise, let strcmp decide...
  //
  return (strcmp(m_pList, aCTCLList.m_pList) == 0);
}
//////////////////////////////////////////////////////////////////////////
//
// Function:
//   void setList(const char* pList)
// Operation type:
//   Protected mutator
//
void
CTCLList::setList(const char* pList)
{
  assert(pList != (const char*)kpNULL);
  delete []m_pList;
  m_pList = new char[strlen(pList) + 1];
  strcpy(m_pList, pList);

  
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int Split ( StringArray& rElements )
//  Operation Type:
//     Selector.
//
int 
CTCLList::Split(StringArray& rElements) 
{
// Splits the list into an array of std::strings
// Note that the array is initially cleared by this
//  member.  The number of entries in the  array
// reflects the numer of entries in the list.
//  NOTE:
//    The array is not cleared, so any list elements are
//    appended to the end of the existing array.
//
// Formal Parameters:
//      std::string& rElements
// Returns:
//      TCL_OK  on success.
//

  int      nElements;
  char**   pElements;

  int result = Split(nElements, &pElements);
  if(result == TCL_OK) {
    for(int i = 0; i < nElements; i++) {
      rElements.push_back(pElements[i]);
    }
    free(pElements);
  }
  return result;

}
//////////////////////////////////////////////////////////////////////////
//
// Function:
//   int Split(int& n, char*** p)
// Operation type:
//   Selector.
//
int
CTCLList::Split(int& n, char***p)
{
  CTCLInterpreter* pInterp = AssertIfNotBound();
  
  if(m_pList == (char*)kpNULL)
    return TCL_OK;		// Treat null ptr as empty list.
  
  
  int result = Tcl_SplitList(pInterp->getInterpreter(), m_pList, &n, p);
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    const char* Merge ( const StringArray& rElements )
//  Operation Type:
//     mutator
//
const char* 
CTCLList::Merge(const StringArray& rElements) 
{
// Fills the list object with the characters in the
// string array, and returns a pointer to the list
// Note that list storage is managed by the object
// so the client should not delete the pointer.
//
// Formal Parameters:
//    std::string&  rElements:
//        String array containing the elements to 
//        insert in the string.
//  Returns:
//      Pointer to the generated string.
//

  int     n         = rElements.size();
  char**  pElements = new (char*)[n];

  for(int i = 0; i < n; i ++) {
    pElements[i] = (char*)rElements[i].c_str();
  }
  Merge(n, pElements);

  delete []pElements;
  
}
/////////////////////////////////////////////////////////////////////////
//
// Function:
//    const char* Merge(int n, char** pList)
// Operation Type:
//    Mutator.
//
const char*
CTCLList::Merge(int n, char** pElements)
{
  char* pList = Tcl_Merge(n, pElements);
  setList(pList);
  free(pList);
}
/////////////////////////////////////////////////////////////////////////
//
// Function
//    void Doassign(const CTCLList& rRhs)
// Operation Type:
//    Protected utility.
//
void
CTCLList::DoAssign(const CTCLList& rRhs)
{
  char* pRhs = rRhs.m_pList;
  if(m_pList) {
    setList(pRhs);
  }
  else {
    delete []m_pList;
    m_pList = (char*)kpNULL;
  }
}

