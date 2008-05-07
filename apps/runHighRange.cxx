#include <string>
#include <fstream>

#include "../src/AcdJobConfig.h"

#include "../src/AcdCalibUtil.h"
#include "../src/AcdHighRange.h"
#include "../src/AcdHighRangeFit.h"

#include "../src/AcdPadMap.h"
#include "../src/AcdCalibMap.h"

int main(int argn, char** argc) {

  // configure
  AcdJobConfig jc("runHighRange.exe","This utility runs the coherent noise analysis");

  Int_t parseValue = jc.parse(argn,argc); 
  switch ( parseValue ) {
  case 0: // ok to proceed
    break;  
  case 1: // called -h option terminate processesing normally
    return 0; 
  default: 
    return parseValue;  // parse failed, return failure code
  }

  Bool_t okToContinue = jc.checkSvac();
  if ( ! okToContinue ) return 1; // no input, fail

  /// build filler & run over events
  AcdHighRange r(jc.svacChain(), jc.config() );

  if ( jc.pedFileName() != "" ) {
    r.readCalib(AcdCalibData::PEDESTAL,jc.pedFileName().c_str());
  }
  // r.go(jc.optval_n(),jc.optval_s());    
  if ( ! r.convertTxt("highRange.txt") ) {
    std::cerr << "Failed to convert highRange.txt" << std::endl;
    return 5;
  }

  // do fits  
  AcdHighRangeFitLibrary fitHR(AcdHighRangeFitLibrary::Convert);
  AcdCalibMap* fitMap = r.fit(fitHR,AcdCalibData::HIGH_RANGE,AcdCalib::H_HIGH_RANGE);
  
  // and dump to the text file
  std::string outputTxtFile = jc.outputPrefix() + "_HighRange.txt";
  std::string outputXmlFile = jc.outputPrefix() + "_HighRange.xml";
  fitMap->writeTxtFile(outputTxtFile.c_str(),jc.instrument().c_str(),jc.timeStamp().c_str(),fitHR.algorithm(),r);
  fitMap->writeXmlFile(outputXmlFile.c_str(),jc.instrument().c_str(),jc.timeStamp().c_str(),fitHR.algorithm(),r);

  // strip chart output

  // to root file
  std::string outputHistFile = jc.outputPrefix() + "_HighRange.root";
  r.writeHistograms(AcdCalib::H_HIGH_RANGE, outputHistFile.c_str());

  // to ps file
  std::string outputPsFile = jc.outputPrefix() + "_HighRange_";
  AcdPadMap* padMap(0);
  padMap = AcdCalibUtil::drawStripCharts(*(r.getHistMap(AcdCalib::H_HIGH_RANGE)),outputPsFile.c_str());  
  AcdCalibUtil::saveCanvases(padMap->canvasList());  


  return 0;
}







