#include <SpecTclDatabase.h>
#include <SaveSet.h>
#include <DBApplications.h>


#include <iostream>
#include <stdlib.h>


int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "Usage:: applydef database-name\n";
        exit(EXIT_FAILURE);
    }
    SpecTclDB::CDatabase db(argv[1]);
    SpecTclDB::SaveSet* svset = db.getSaveSet("a saveset");
    
    delete svset->applyGate("1d", "s2");
    delete svset->applyGate("compound", "s1");
    
    auto applications = svset->listApplications();
    std::cout << "Gate applications\n";
    for (int i = 0; i < applications.size(); i++) {
        std::cout << applications[i]->getGateName()
                  << " is applied to "
                  << applications[i]->getSpectrumName() << std::endl;
        delete applications[i];
    }
    delete svset;
    exit(EXIT_SUCCESS);
    
}