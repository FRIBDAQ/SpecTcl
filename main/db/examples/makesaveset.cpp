#include <SpecTclDatabase.h>
#include <SaveSet.h>
#include <iostream>
#include <stdexcept>
#include <stdlib.h>


int main(int argc, char** argv)
{
  if (argc != 2) {
    std::cerr << "Usage: makesaveset database-name\n";
    exit(EXIT_FAILURE);
  }
  try {
    SpecTclDB::CDatabase db(argv[1]);
    
    SpecTclDB::SaveSet* pSaveset = db.createSaveSet("a saveset");
    delete pSaveset;
  }
  catch (std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }
  exit(EXIT_SUCCESS);

}
