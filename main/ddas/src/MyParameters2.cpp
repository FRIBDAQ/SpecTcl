#include "MyParameters2.h"

void
MyExClass::Initialize(std::string name)
{
  energy.Initialize(name+".energy", 16);
  time.Initialize(name+".time", 32);
  ecal.Initialize(name+".ecal", 16);
}

void
MyExClass::Reset()
{
  energy.Reset();
  time.Reset();
  ecal.Reset();
}

void
MyExClass2::Initialize(std::string name)
{
  x.Initialize(name+".x", 16);
  y.Initialize(name+".y", 16);
  z.Initialize(name+".z", 16);  
}

void
MyExClass2::Reset()
{
  x.Reset();
  y.Reset();
  z.Reset();
}

MyParameters2::MyParameters2() 
{
  ex1.Initialize("example.ex1");
  ex2.Initialize("example.ex2");  
}

MyParameters2::MyParameters2(const MyParameters2& rhs):
  ex1(rhs.ex1),
  ex2(rhs.ex2)
{
  CTreeParameter::BindParameters();  
}

void
MyParameters2::Reset()
{
  ex1.Reset();
  ex2.Reset();  
}


