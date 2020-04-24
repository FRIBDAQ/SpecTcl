#include <SpecTclDatabase.h>
#include <SaveSet.h>
#include <iostream>
#include <stdexcept>
#include <stdlib.h>


int main(int argc, char** argv)
{
  if (argc != 2) {
    std::cerr << "Usage: lssaveset db-filename\n";
    exit(EXIT_FAILURE);
  }
  SpecTclDB::CDatabase db(argv[1]);
  auto savesets = db.getAllSaveSets();
  std::cout << " Save sets in " << argv[1] << std::endl;
  for (int i =0; i < savesets.size(); i++) {
    std::cout << savesets[i]->getInfo().s_name << std::endl;
    delete savesets[i];
  }
}
