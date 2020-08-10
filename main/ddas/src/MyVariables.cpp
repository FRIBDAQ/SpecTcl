#include "MyVariables.h"

MyVariables::MyVariables()
{
  var1.Initialize("var.var1");
  var2.Initialize("var.var2");  
}

MyVariables::MyVariables(const MyVariables& rhs):
  var1(rhs.var1),
  var2(rhs.var2)  
{}
  
void
MyVar1::Initialize(std::string name)
{
  c1.Initialize(name+".c1", 0.5, "");
  c2.Initialize(name+".c2", 0.5, "");
  c3.Initialize(name+".c3", 0.5, "");  
}

void
MyVar2::Initialize(std::string name)
{
  d1.Initialize(name+".d1", 0.5, "");
  d2.Initialize(name+".d2", 0.5, "");
  d3.Initialize(name+".d3", 0.5, "");  
}

