#include <SpecTclDatabase.h>
#include <SaveSet.h>

#include <iostream>
#include <stdlib.h>


// accept data:

void sink(const SpecTclDB::SaveSet::Event& event)
{
    static int evt = 0;
    if ((evt % 50) == 0) {
      std::cout << "Dumping event " << evt << ":\n";
        for (int i =0; i < event.size(); i++)  {
            std::cout << "   param# " << event[i].s_number
                << " = " << event[i].s_value << std::endl;
        }
    }
    evt++;
}


int main (int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "Usage:: evtget db-filename\n";
        exit(EXIT_FAILURE);
    }
    SpecTclDB::CDatabase db(argv[1]);
    SpecTclDB::SaveSet* pSet = db.getSaveSet("a saveset");
    
    int runid = pSet->openRun(1);
    void* ctx = pSet->openEvents(runid);
    
    SpecTclDB::SaveSet::Event event;
    while(pSet->readEvent(ctx, event)) {
        sink(event);
    }
    
    pSet->closeEvents(ctx);
    delete pSet;
    exit(EXIT_SUCCESS);
}

