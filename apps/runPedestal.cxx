#include <string>
#include <fstream>

#include "../src/AcdJobConfig.h"

#include "../src/AcdCalibLoop_Digi.h"
#include "../src/AcdPedestalFit.h"
#include "../src/AcdCalibMap.h"
#include "../src/AcdPadMap.h"
#include "../src/AcdCalibUtil.h"

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
  AcdCalibLoop_Digi r(AcdCalibData::PEDESTAL,jc.digiChain(),jc.optval_P(),jc.config());
  r.go(jc.optval_n(),jc.optval_s());    

  // do fits
  AcdPedestalFitLibrary pedFitter(AcdPedestalFitLibrary::MeanValue);
  AcdCalibMap* peds = r.fit(pedFitter,AcdCalibData::PEDESTAL,AcdCalib::H_RAW);

  // output
  std::string pedTextFile = jc.outputPrefix() + "_ped.txt";
  std::string pedXmlFile = jc.outputPrefix() + "_ped.xml";
  std::string outputHistFile = jc.outputPrefix() + "_ped.root";
  std::string outputRootFile = jc.outputPrefix() + "_pedFit.root";
  std::string outputPlotFile = jc.outputPrefix() + "_ped_";

  r.writeHistograms(AcdCalib::H_RAW, outputHistFile.c_str());
  peds->writeTxtFile(pedTextFile.c_str(),jc.instrument().c_str(),jc.timeStamp().c_str(),pedFitter.algorithm(),r);
  peds->writeXmlFile(pedXmlFile.c_str(),jc.instrument().c_str(),jc.timeStamp().c_str(),pedFitter.algorithm(),r);
  peds->writeResultsToTree(outputRootFile.c_str());  
  AcdPadMap* padMap = AcdCalibUtil::drawPeds(*(r.getHistMap(AcdCalib::H_RAW)),*peds,outputPlotFile.c_str());  
  AcdCalibUtil::saveCanvases(padMap->canvasList(),"",".gif"); 

  return 0;
}







