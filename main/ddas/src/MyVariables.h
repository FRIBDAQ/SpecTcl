#ifndef __MYVARIABLES_H
#include <TreeParameter.h>
#endif

struct MyVar1
{
  CTreeVariable c1;
  CTreeVariable c2;
  CTreeVariable c3;

  void Initialize(std::string name);
};

struct MyVar2
{
  CTreeVariable d1;
  CTreeVariable d2;
  CTreeVariable d3;

  void Initialize(std::string name);
};


struct MyVariables
{
  MyVar1 var1;
  MyVar2 var2;  

  // Ctor
  MyVariables();
  // Dtor
  ~MyVariables(){};  
  // Copy Ctor
  MyVariables(const MyVariables& rhs);
};

