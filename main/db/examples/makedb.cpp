#include <SpecTclDatabase.h>
#include <iostream>
#include <stdlib.h>

int main(int argc, char** argv)
{
  if (argc != 2) {
    std::cerr << "Usage: makedb db-filenme\n";
    exit(EXIT_FAILURE);
  }

  SpecTclDB::CDatabase::create(argv[1]);

  exit(EXIT_SUCCESS);
}
