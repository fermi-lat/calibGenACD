#include <string>
#include <fstream>

#include "../src/AcdJobConfig.h"

#include "../src/AcdCalibLoop_Digi.h"
#include "../src/AcdPedestalFitLibrary.h"

int main(int argn, char** argc) {

  // configure
  AcdJobConfig jc("runPedestal.exe","This utility makes pedestal files from input digi files");

  Int_t parseValue = jc.parse(argn,argc); 
  switch ( parseValue ) {
  case 0: // ok to proceed
    break;  
  case 1: // called -h option terminate processesing normally
    return 0; 
  default: 
    return parseValue;  // parse failed, return failure code
  }

  Bool_t okToContinue = jc.checkDigi();
  if ( ! okToContinue ) return 1; // no input, fail

  // build filler & run over events
  AcdCalibLoop_Digi r(AcdCalibBase::PEDESTAL,jc.digiChain(),jc.optval_P(),jc.config());
  //r.setCalType(AcdCalibBase::PEDESTAL);
  r.go(jc.optval_n(),jc.optval_s());    

  // do fits
  AcdPedestalFitLibrary pedFitter(AcdPedestalFitLibrary::MeanValue);
  AcdPedestalFitMap* peds = r.fitPedestals(pedFitter);

  // output
  std::string pedTextFile = jc.outputPrefix() + "_ped.txt";
  std::string pedXmlFile = jc.outputPrefix() + "_ped.xml";
  std::string outputHistFile = jc.outputPrefix() + "_ped.root";

  r.writeHistograms(AcdCalibBase::PEDESTAL, outputHistFile.c_str());
  peds->writeTxtFile(pedTextFile.c_str(),jc.instrument().c_str(),jc.timeStamp().c_str(),pedFitter.algorithm(),r);
  peds->writeXmlFile(pedXmlFile.c_str(),jc.instrument().c_str(),jc.timeStamp().c_str(),pedFitter.algorithm(),r);

  return 0;
}







