#ifndef MYPARAMETERS2_H
#define MYPARAMETERS2_H

#include <config.h>
#include <TreeParameter.h>
#include <string>
#include "MyVariables.h"

//____________________________________________________________
// Structure 2 containing some detector information
//
struct MyExClass2 {
  CTreeParameter x;
  CTreeParameter y;
  CTreeParameter z;  

  void Initialize(std::string name);
  void Reset();
};

//____________________________________________________________
// Structure containing some detector information
//
struct MyExClass {
  CTreeParameter energy;
  CTreeParameter ecal;
  CTreeParameter time;

  void Initialize(std::string name);
  void Reset();
};

//____________________________________________________________
// Struct for top-level class
// 
//  This contains daughter structures MyExClass1 and MyExClass2

struct MyParameters2 {

  MyExClass  ex1;
  MyExClass2 ex2;

  MyVariables var;
  
  // Ctor
  MyParameters2();
  // Dtor
  ~MyParameters2(){};
  // Copy Ctor
  MyParameters2(const MyParameters2& rhs);

  void Reset();
};

#endif
