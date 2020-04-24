#include <SpecTclDatabase.h>
#include <SaveSet.h>
#include <DBParameter.h>

#include <iostream>
#include <stdexcept>
#include <stdlib.h>

int main(int argc, char** argv)
{
  if(argc != 2) {
    std::cerr << "Usage:  pardef db-filename\n";
    exit(EXIT_FAILURE);
  }

  SpecTclDB::SaveSet* pSave(nullptr);
  try {
    SpecTclDB::CDatabase db(argv[1]);
    pSave = db.getSaveSet("a saveset");
    delete pSave->createParameter("p1", 100);
    delete pSave->createParameter("p2", 101, -10.0, 1.0, 100, "cm");
    
    auto params = pSave->listParameters();
    std::cout << "The following parameters are defined:\n";
    for (int i =0; i < params.size(); i++) {
      std::cout << params[i]->getInfo().s_name << std::endl;
      delete params[i];
    }
    SpecTclDB::DBParameter* p = pSave->findParameter("p2");
    std::cerr << " I found parameter: " << p->getInfo().s_name << std::endl;
    delete p;
    
  }
  catch (std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    delete pSave;
    exit(EXIT_FAILURE);
  }

  delete pSave;
  exit(EXIT_SUCCESS);
}
