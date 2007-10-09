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
  AcdJobConfig jc("runCnoCalib.exe","This utility runs the cno set point calibration code");
  
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
  std::string outputHistFileLow = jc.outputPrefix() + "_range_low.root";
  std::string outputHistFileHigh = jc.outputPrefix() + "_range_high.root";

  r.writeHistograms(AcdCalib::H_RAW, outputHistFileLow.c_str());
  r.writeHistograms(AcdCalib::H_RANGE, outputHistFileHigh.c_str());
  ranges->writeTxtFile(textFile.c_str(),jc.instrument().c_str(),jc.timeStamp().c_str(),rangeFitter.algorithm(),r);
  ranges->writeXmlFile(xmlFile.c_str(),jc.instrument().c_str(),jc.timeStamp().c_str(),rangeFitter.algorithm(),r);

  return 0;
}







