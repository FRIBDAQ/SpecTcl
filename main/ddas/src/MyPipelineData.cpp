#include "MyPipelineData.h"
#include <iostream>

////
//
MyPipelineData::MyPipelineData()
{
  m_chanHit.clear();
}

MyPipelineData::MyPipelineData(const MyPipelineData& rhs) :
  m_chanHit(rhs.m_chanHit)
{}

