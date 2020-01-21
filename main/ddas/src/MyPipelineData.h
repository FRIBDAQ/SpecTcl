#ifndef MYPIPELINEDATA_H
#define MYPIPELINEDATA_H

#include <PipelineData.h>
#include <vector>

// Class responsible for allowing thread-safe objects 
class MyPipelineData : public DAQ::DDAS::CPipelineData
{
 public:
  std::vector<int> m_chanHit;

  // Constructor.
  MyPipelineData();
  // Copy constructor
  MyPipelineData(const MyPipelineData& rhs);    
  // Clone constructor
  virtual MyPipelineData* clone() { return new MyPipelineData(*this); }

};
#endif

