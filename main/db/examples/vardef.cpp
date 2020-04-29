#include <SpecTclDatabase.h>
#include <SaveSet.h>
#include <DBTreeVariable.h>

#include <iostream>
#include <stdlib.h>

int main(int argc, char** argv)
{
    if (argc !=2) {
        std::cerr << "Usage: vardb database-file\n";
        exit(EXIT_FAILURE);
    }
    SpecTclDB::CDatabase db(argv[1]);
    SpecTclDB::SaveSet* svset = db.getSaveSet("a saveset");
    
    delete svset->createVariable("p1.slope", 1.0, "KeV/Counts");
    delete svset->createVariable("p1.offset", 0.0, "KeV");
    
    delete svset->createVariable("p2.slope", 1.0, "KeV/Counts");
    delete svset->createVariable("ps.offset", 0.0, "KeV");
    
    auto vars = svset->listVariables();
    std::cout << " Tree variables: \n";
    for (int i = 0; i < vars.size(); i++) {
        const auto& info = vars[i]->getInfo();
        std::cout << info.s_name << " = " << info.s_value
                  << info.s_units << std::endl;
        
        delete vars[i];
    }
    delete svset;
    exit(EXIT_SUCCESS);
}
