#include <string>
#include <fstream>

#include "../src/AcdJobConfig.h"

#include "../src/AcdCalibUtil.h"
#include "../src/AcdCarbonFit.h"
#include "../src/AcdCalibLoop_Svac.h"
#include "../src/AcdPadMap.h"
#include "../src/AcdCalibMap.h"


int main(int argn, char** argc) {

  // configure
  AcdJobConfig jc("runCarbonCalib.exe","This utility runs the Carbon peak calibration code on SVAC files");

  Int_t parseValue = jc.parse(argn,argc); 
  switch ( parseValue ) {
  case 0: // ok to proceed
    break;  
  case 1: // called -h option terminate processesing normally
    return 0; 
  default: 
    return parseValue;  // parse failed, return failure code
  }


  AcdCalibLoop_Svac r(AcdCalibData::CARBON,jc.svacChain(),jc.optval_L(),jc.optval_C(),jc.config());  
  bool removePeds = true;
  if ( jc.pedFileName() != "" ) {
    removePeds = false;
    r.readCalib(AcdCalibData::PEDESTAL,jc.pedFileName().c_str());
  }
  if ( jc.rangeFileName()  != "" ) {
    r.readCalib(AcdCalibData::RANGE,jc.rangeFileName().c_str());
  }
  if ( jc.pedHighFileName()  != "" ) {
    r.readCalib(AcdCalibData::PED_HIGH,jc.pedHighFileName().c_str());
  }  
  AcdHistCalibMap* hists(0);
  AcdCarbonFitLibrary gainFitter(AcdCarbonFitLibrary::Gauss,removePeds);  
  if ( jc.svacChain() != 0 ) {
    /// build filler & run over events    
    r.go(jc.optval_n(),jc.optval_s());   
    std::string outputHistFile = jc.outputPrefix() + "_highRange.root";
    r.writeHistograms(AcdCalib::H_GAIN, outputHistFile.c_str());
    hists = r.getHistMap(AcdCalib::H_GAIN);
  } else if ( jc.inputHistFile() != "" ) {
    hists = r.readHistMap(AcdCalib::H_GAIN, jc.inputHistFile().c_str());
  } else {
    std::cerr << "No input " << std::endl;
    jc.usage();
    return 2;
  }
  
  // do fits
  AcdCalibMap* gains = r.fit(gainFitter,AcdCalibData::GAIN,AcdCalib::H_GAIN);

  // output
  std::string gainTextFile = jc.outputPrefix() + "_highRange.txt";
  std::string gainXmlFile = jc.outputPrefix() + "_highRange.xml";
  std::string psFile = jc.outputPrefix() + "_highRange_";
  std::string psFile_log = psFile + "log_";
  std::string psFile_lin = psFile + "lin_";
  std::string outputRootFile = jc.outputPrefix() + "_highRangeFit.root";

  gains->writeTxtFile(gainTextFile.c_str(),jc.instrument().c_str(),jc.timeStamp().c_str(),gainFitter.algorithm(),r);
  gains->writeXmlFile(gainXmlFile.c_str(),jc.instrument().c_str(),jc.timeStamp().c_str(),gainFitter.algorithm(),r);
  gains->writeResultsToTree(outputRootFile.c_str());
 
  AcdPadMap* logPads = AcdCalibUtil::drawMips(*hists,*gains,kTRUE,psFile_log.c_str());    
  AcdCalibUtil::saveCanvases(logPads->canvasList(),"",".gif");

  AcdPadMap* linPads = AcdCalibUtil::drawMips(*hists,*gains,kFALSE,psFile_lin.c_str());
  AcdCalibUtil::saveCanvases(linPads->canvasList(),"",".gif");  

  return 0;
}







