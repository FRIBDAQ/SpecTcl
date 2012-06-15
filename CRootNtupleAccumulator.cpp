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
#include "CRootNtupleAccumulator.h"

#include <TNtuple.h>
#include <TFile.h>

using namespace std;


/*!
  At construction time, there is no ntuple and the filename
  is empty as it normally is.
*/
CRootNtupleAccumulator::CRootNtupleAccumulator() :
  m_pNtuple(0),
  m_pFile(0)
{
  
}
/*!
   Destruction time; we will close the file if open, which will delete
   the ntuple
*/
CRootNtupleAccumulator::~CRootNtupleAccumulator()
{
  close();
}

/*!
  Opening a file is, for now just saving the filename so it can be used
  when writing the ntuple to disk

  \param pFilename  - name of the file to which the data will be saved.
*/
void
CRootNtupleAccumulator::open(const char* pFilename)
{
  m_filename = pFilename;
  
  if (!m_pFile) {
    m_pFile = new TFile(pFilename, "RECREATE");
  }
}
/*!
   Closing the file means that if the ntuple exists it will be written to
   file named by m_filename.
*/
void
CRootNtupleAccumulator::close()
{

  if(m_pFile) {
    m_pNtuple->Write();
    //    m_pFile->Write();
    m_pFile->Close();
    m_pFile = 0;
    m_pNtuple = 0;
  }
}
/*!
  Create the ntuple
  \param parameterNames - the names of the parameters that make up the
                          ntuple.
*/
void
CRootNtupleAccumulator::createNtuple(string ntupleName, 
				     vector<string> parameterNames)
{
  string description;
  string separator;

  // Create the ntuple description

  for (int i=0; i < parameterNames.size(); i++) {
    description += separator;
    description += parameterNames[i];
    separator    = ":";
  }


  m_pNtuple = new TNtuple(ntupleName.c_str(), "SpecTcl _> root output stage", description.c_str());
}
/*!
  Accumulate an event into the ntuple.
  \param event - a vector of floats to Fill into the NTuple.

*/
void
CRootNtupleAccumulator::accumulate(vector<double> event)
{
  // Can't do any of this if the ntuple does not exist.

  if (m_pNtuple) {
    Float_t* pEvent = new Float_t[event.size()];
    for (int i =0; i < event.size(); i++) {
      pEvent[i] = (Float_t)event[i];
    }
    m_pNtuple->Fill(pEvent);
    delete []pEvent;
  }
}
