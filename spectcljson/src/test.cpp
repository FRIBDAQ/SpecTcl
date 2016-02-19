
#include <string>
#include <iostream>
#include <fstream>
#include <json/json.h>
#include "JsonToTH1.h"
#include <TFile.h>

using namespace std;
using namespace Json;

int main() {

  Value value;
  {
    std::ifstream file("../test2.json");
    file >> value;
  }

  auto pHist = Converters::jsonToTH1(value);

  {
    value.clear();
    std::ifstream file("../content1d.json");
    file >> value;
  }
  Converters::fillTH1(value,*pHist.get());

  TFile f("out.root","RECREATE");
  pHist->Write();
  f.Close();


  return 0;
}
