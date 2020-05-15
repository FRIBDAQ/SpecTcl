#include <SpecTclDatabase.h>
#include <SaveSet.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>


std::vector<uint32_t> getScalers()
{
    static uint32_t       chan1 = 0;
    std::vector<uint32_t> result;
    for (int i = 0; i < 32; i++) {
        result.push_back(chan1*i);
    }
    
    chan1++;
    return result;
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "Usage: sclstore database-name\n";
        exit(EXIT_FAILURE);
    }
    SpecTclDB::CDatabase db(argv[1]);
    SpecTclDB::SaveSet* pS = db.getSaveSet("a saveset");
    
    time_t now =time(nullptr);
    int offset = 0;
    int runid = pS->startRun(2, "This is run 2", now);
    
    // simulate Read every 10 seconds 100 times:
    
    for (int i = 0; i < 100; i++) {
        now += 10;
        auto scalers = getScalers();
        pS->saveScalers(
            runid, 1, offset, offset+10, 1, now, 32, scalers.data()
        );
        offset += 10;
    }
    pS->endRun(runid, now);
    
    delete pS;
    exit(EXIT_SUCCESS);
}
