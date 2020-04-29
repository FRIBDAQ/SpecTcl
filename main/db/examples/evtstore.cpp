#include <SpecTclDatabase.h>
#include <SaveSet.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>

void createEvent(std::vector<int>& params, std::vector<double>& values)
{
    // 100 parameters, 50/50 chance the parameter is present.
    //  parameter values are a uniform random between -100.0 and
    //  +100.0 ..and I don't care if that's inclusive or exclusive.
    
    for (int i =0; i < 100; i++) {
        if (drand48() < 0.5) {
            params.push_back(i);
            values.push_back(drand48()*200.0 - 100.0);
        }
    }
}

int main(int argc, char** argv)
{
    if (argc != 3) {
        std::cerr << "Usage:  evtstore db-filename nevents\n";
    }
    const char* dbFile = argv[1];
    int         nEvents = strtoul(argv[2], nullptr, 0);
    
    SpecTclDB::CDatabase db(dbFile);
    SpecTclDB::SaveSet* pSaveSet = db.getSaveSet("a saveset");
    
    int runId =
        pSaveSet->startRun(1, "Some junk Data", time(nullptr));
    
    std::vector<int> params;
    std::vector<double> values;
    srand48(time(nullptr));        // Seed random with time.
    void* ctx = pSaveSet->startEvents(runId);
    for (int i =0; i < nEvents; i++) {
        params.clear();
        values.clear();             // new event.
        createEvent(params, values);
        pSaveSet->saveEvent(
            runId, i, params.size(), params.data(), values.data()
        );
        
        if (i %100 == 0) {    // every 100 events commit the transaction.
            pSaveSet->endEvents(ctx);
            ctx  = pSaveSet->startEvents(runId);
        }
        
    }
    pSaveSet->endEvents(ctx);
    
    pSaveSet->endRun(runId, time(nullptr));

    exit(EXIT_SUCCESS);
}
