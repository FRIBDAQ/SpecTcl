#include <SpecTclDatabase.h>
#include <SaveSet.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>


void processScalers(const SpecTclDB::SaveSet::ScalerReadout& readout)
{
    double start = readout.s_startOffset;
    start /= readout.s_divisor;
    double end   = readout.s_stopOffset;
    end   /= readout.s_divisor;
    std::cout << "Scaler readout: Sid: " << readout.s_sourceId
	      << " from " << start << " to " << end << " seconds into the run \n";
    std::cout << "Readout occured at: " << ctime(&readout.s_time) << std::endl;
    
    for (int i =0; i < readout.s_values.size(); i++) {
        std::cout << "   channel " << i << " : " << readout.s_values[i] << std::endl;
    }
    std::cout << "------------------------------\n";
}   

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "Usage:: sclget database-file\n";
        exit(EXIT_FAILURE);
    }
    SpecTclDB::CDatabase db(argv[1]);
    SpecTclDB::SaveSet* pS = db.getSaveSet("a saveset");
    
    int runid = pS->openRun(2);
    auto info = pS->getRunInfo(runid);
    std::cout << "Dumping scalers for run: " << info.s_runNumber << std::endl;
    std::cout << "Title: " << info.s_title << std::endl;
    std::cout << "Start time: " << ctime(&info.s_startTime) << std::endl;
    std::cout << "End time  : " << ctime(&info.s_stopTime) << std::endl;
    
    void* ctx = pS->openScalers(runid);
    SpecTclDB::SaveSet::ScalerReadout scalers;
    while(pS->readScaler(ctx, scalers)) {
        processScalers(scalers);
    }
    
    pS->closeScalers(ctx);
    
    
    delete pS;
    exit(EXIT_SUCCESS);
}
