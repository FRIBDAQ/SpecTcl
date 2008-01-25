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



#include <config.h>
#include "CRootFilterOutputStage.h"
#include "CRootNtupleAccumulator.h"
#include "Event.h"
#include <math.h>



using namespace std;


/*!
  Creating the output stage just requires us to create an Ntuple accumulator:
*/
CRootFilterOutputStage::CRootFilterOutputStage() :
  m_nTuple(*(new CRootNtupleAccumulator))
{}

/*!
   Destroying the output stage cleanly requires that we not leak accumulators).
*/
CRootFilterOutputStage::~CRootFilterOutputStage()
{
  delete &m_nTuple;
}

/*!
  When asked to open the output file we can defer to the accumulator as it does all file handling:
  \param filename - name of the file that will be opened.
*/
void
CRootFilterOutputStage::open(string filename)
{
  m_nTuple.open(filename.c_str());
}

/*!
   When asked to close the file we once more defer to the ntuple:
*/
void
CRootFilterOutputStage::close()
{
  m_nTuple.close();
}
/*!
  Describing the event is what actually creates the ntuple we'll also save the indices
  so that we know how to pull the data out of a CEvent.
  \param paramterNames - vector of string parameter names.
  \param parameterIds  - vector of parameter ids (CEvent slot numbers).
*/
void
CRootFilterOutputStage::DescribeEvent(vector<string> parameterNames,
				      vector<UInt_t> parameterIds)
{
  m_parameterIds = parameterIds;
  m_nTuple.createNtuple(string("spectcl"), parameterNames);
}
/*!
  When an event is in it is marshalled to a vector of floats so that the root
  output stage is de-coupled from SpecTcl data types etc.
*/
void
CRootFilterOutputStage::operator()(CEvent& event)
{
  vector<float>  vEvent;
  for (int i=0; i < m_parameterIds.size(); i++) {
    UInt_t id = m_parameterIds[i];
    if (event[id].isValid()) {
      if ((event[id] < 0.0) || (event[id] > 1023.0)) {
      }
      vEvent.push_back(event[id]);
    }
    else {
      vEvent.push_back(nanf("NAN"));
    }
  }
  m_nTuple.accumulate(vEvent);
}
/*!
   Need to give the type of the output stage.
*/
string
CRootFilterOutputStage::type() const
{
  return string("rootntuple");
}
