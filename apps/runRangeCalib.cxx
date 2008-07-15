#include <string>
#include <fstream>

#include "../src/AcdJobConfig.h"

#include "../src/AcdCalibUtil.h"
#include "../src/AcdRangeFit.h"
#include "../src/AcdCalibLoop_Digi.h"

#include "../src/AcdPadMap.h"
#include "../src/AcdCalibMap.h"

int main(int argn, char** argc) {

  // configure
  AcdJobConfig jc("runRangeCalib.exe","This utility runs the range crossover calibration code");
  
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

  AcdCalibLoop_Digi r(AcdCalibData::RANGE,jc.digiChain(),jc.optval_P(),jc.config());
  if ( jc.pedFileName() != "" ) {
    r.readCalib(AcdCalibData::PEDESTAL,jc.pedFileName().c_str());
  }
  
  r.go(jc.optval_n(),jc.optval_s());    

  AcdRangeFitLibrary rangeFitter(AcdRangeFitLibrary::Counting);
  AcdCalibMap* ranges = r.fit(rangeFitter,AcdCalibData::RANGE,AcdCalib::H_RANGE);

  std::string textFile = jc.outputPrefix() + "_range.txt";
  std::string xmlFile = jc.outputPrefix() + "_range.xml";
  std::string outputHistFile = jc.outputPrefix() + "_range.root";
  std::string outputRootFile = jc.outputPrefix() + "_rangeFit.root";
  std::string outputPlotFile = jc.outputPrefix() + "_range_";

  r.writeHistograms(AcdCalib::H_RANGE, outputHistFile.c_str());
  ranges->writeTxtFile(textFile.c_str(),jc.instrument().c_str(),jc.timeStamp().c_str(),rangeFitter.algorithm(),r);
  ranges->writeXmlFile(xmlFile.c_str(),jc.instrument().c_str(),jc.timeStamp().c_str(),rangeFitter.algorithm(),r);
  ranges->writeResultsToTree(outputRootFile.c_str());
  AcdPadMap* padMap = AcdCalibUtil::drawRanges(*(r.getHistMap(AcdCalib::H_RANGE)),*ranges,outputPlotFile.c_str());  
  AcdCalibUtil::saveCanvases(padMap->canvasList(),"",".gif"); 

  return 0;
}






