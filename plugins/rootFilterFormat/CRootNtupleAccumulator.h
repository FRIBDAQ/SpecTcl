
#ifndef __CROOTNTUPLEACCUMULATOR_H
#define __CROOTNTUPLEACCUMULATOR_H
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

#ifndef __STL_VECTOR
#include <vector>
#ifndef __STL_VECTOR
#define __STL_VECTOR
#endif
#endif


#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif


class TNtuple;

/*!
   This class accumulates root ntpules and writes them to file.
   The idea is that you open the file, describe the ntuple
   accumulate it and then close the file.   Note that given how Root
   works, it's the close that actually writes the file.
*/
class CRootNtupleAccumulator
{
private:
  TNtuple*          m_pNtuple;
  std::string       m_filename;
  
public:
  CRootNtupleAccumulator();
  ~CRootNtupleAccumulator();

  // Ntuples  don't seemto have copy constructors so:

private:
  CRootNtupleAccumulator(const CRootNtupleAccumulator& rhs);
  CRootNtupleAccumulator& operator=(const CRootNtupleAccumulator& rhs);
  int operator==(const CRootNtupleAccumulator& rhs) const;
  int operator!=(const CRootNtupleAccumulator& rhs) const;

public:

  // Interface functions:

  void open(const char* pFilename);
  void close();
  void createNtuple(std::string ntupleName,
		    std::vector<std::string> parameterNames);
  void accumulate(std::vector<float> event);
};


#endif
