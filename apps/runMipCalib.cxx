#include <string>
#include <fstream>

#include "../src/AcdJobConfig.h"

#include "../src/AcdCalibUtil.h"
#include "../src/AcdGainFit.h"
#include "../src/AcdCalibLoop_Svac.h"
#include "../src/AcdPadMap.h"
#include "../src/AcdCalibMap.h"


int main(int argn, char** argc) {

  // configure
  AcdJobConfig jc("runMipCalib.exe","This utility runs the MIP calibration code on SVAC files");

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
  AcdCalibLoop_Svac r(AcdCalibData::GAIN,jc.svacChain(),jc.optval_L(),jc.config());

  bool removePeds(true);
  if ( jc.pedFileName() != "" && jc.svacChain() != 0 ) {
    r.readCalib(AcdCalibData::PEDESTAL,jc.pedFileName().c_str());
    removePeds = false;
  }
  r.go(jc.optval_n(),jc.optval_s());    

  // do fits
  AcdGainFitLibrary gainFitter(&CalibData::AcdGainFitDesc::instance(),AcdGainFitLibrary::P5,removePeds);
  AcdCalibMap* gains = r.fit(gainFitter,AcdCalibData::GAIN,AcdCalib::H_GAIN);

  // output
  std::string gainTextFile = jc.outputPrefix() + "_gain.txt";
  std::string gainXmlFile = jc.outputPrefix() + "_gain.xml";
  std::string outputHistFile = jc.outputPrefix() + "_gain.root";
  std::string psFile = jc.outputPrefix() + "_gain_";
  std::string psFile_log = psFile + "log_";
  std::string psFile_lin = psFile + "lin_";
  std::string outputRootFile = jc.outputPrefix() + "_gainFit.root";

  r.writeHistograms(AcdCalib::H_GAIN, outputHistFile.c_str());
  gains->writeTxtFile(gainTextFile.c_str(),jc.instrument().c_str(),jc.timeStamp().c_str(),gainFitter.algorithm(),r);
  gains->writeXmlFile(gainXmlFile.c_str(),jc.instrument().c_str(),jc.timeStamp().c_str(),gainFitter.algorithm(),r);
  gains->writeResultsToTree(outputRootFile.c_str());

  AcdHistCalibMap* hists = r.getHistMap(AcdCalib::H_GAIN);
 
  AcdPadMap* logPads = AcdCalibUtil::drawMips(*hists,*gains,kTRUE,psFile_log.c_str());    
  AcdCalibUtil::saveCanvases(logPads->canvasList(),"",".gif");
  AcdPadMap* linPads = AcdCalibUtil::drawMips(*hists,*gains,kFALSE,psFile_lin.c_str());
  AcdCalibUtil::saveCanvases(linPads->canvasList(),"",".gif");  

  return 0;
}







