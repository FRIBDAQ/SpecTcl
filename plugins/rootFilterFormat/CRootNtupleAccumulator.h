
#ifndef CROOTNTUPLEACCUMULATOR_H
#define CROOTNTUPLEACCUMULATOR_H
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

#include <vector>
#include <string>


class TNtuple;
class TFile;

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
  TFile*            m_pFile;
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
  void accumulate(std::vector<double> event);
};


#endif
