#include <SpecTclDatabase.h>
#include <SaveSet.h>
#include <DBGate.h>

#include <iostream>
#include <stdlib.h>


int main (int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "Usage:: gatedef database-name\n";
        exit(EXIT_FAILURE);
    }
    
    SpecTclDB::CDatabase db(argv[1]);
    SpecTclDB::SaveSet*  svset = db.getSaveSet("a saveset");
    

    // Assume  pardef has been run on this database defining
    // p1 and p2.
    // Ignore errors.
    
    // 1-d gate
    
    std::vector<const char*> p1ds = {"p1"};
    
    delete svset->create1dGate("1d", "s", p1ds, 100, 200);
    
    // 2-d gate
    
    std::vector<const char*> p2ds = {"p1", "p2"};
    std::vector<std::pair<double, double>> pts2d {
      {100, 0}, {200, 0}, {200, 200}, {100, 200}  
    };
    delete svset->create2dGate("2d", "c", p2ds, pts2d);
    
    // Compound gate:
    
    std::vector<const char*> gatenames={"1d", "2d"};
    delete svset->createCompoundGate("compound", "+", gatenames);

    // Mask gate

    delete svset->createMaskGate("mask", "em", "p1", 0x55555555);
    
    const auto& gates = svset->listGates();
    std::cout << "Gates in the saveset:\n";
    for (int i = 0; i < gates.size(); i++) {
        std::cout << gates[i]->getInfo().s_info.s_name << std::endl;
        delete gates[i];
    }
    
    delete svset;
    exit(EXIT_SUCCESS);
}
