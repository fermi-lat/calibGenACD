#include <string>
#include <fstream>

#include "../src/AcdJobConfig.h"

#include "../src/AcdCalibUtil.h"
#include "../src/AcdRibbonFit.h"
#include "../src/AcdCalibLoop_Svac.h"
#include "../src/AcdPadMap.h"
#include "../src/AcdCalibMap.h"


int main(int argn, char** argc) {

  // configure
  AcdJobConfig jc("runMuonCalib_Svac.exe","This utility runs the muon MIP calibration code on SVAC files");

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
  AcdCalibLoop_Svac r(AcdCalibData::RIBBON,jc.svacChain(),jc.optval_L(),jc.config());

  bool removePeds(true);
  if ( jc.pedFileName() != "" && jc.svacChain() != 0 ) {
    r.readCalib(AcdCalibData::PEDESTAL,jc.pedFileName().c_str());
    removePeds = false;
  }
  r.go(jc.optval_n(),jc.optval_s());    

  // do fits
  AcdRibbonFitLibrary ribFitter(AcdGainFitLibrary::P5,removePeds);
  AcdCalibMap* ribs = r.fit(ribFitter,AcdCalibData::RIBBON,AcdCalib::H_RIBBONS);

  // output
  std::string ribTextFile = jc.outputPrefix() + "_rib.txt";
  std::string ribXmlFile = jc.outputPrefix() + "_rib.xml";
  std::string outputHistFile = jc.outputPrefix() + "_rib.root";
  std::string outputRootFile = jc.outputPrefix() + "_ribFit.root";
  std::string psFile = jc.outputPrefix() + "_rib_";
  std::string psFile_log = psFile + "log_";
  std::string psFile_lin = psFile + "lin_";

  r.writeHistograms(AcdCalib::H_RIBBONS, outputHistFile.c_str());
  ribs->writeTxtFile(ribTextFile.c_str(),jc.instrument().c_str(),jc.timeStamp().c_str(),ribFitter.algorithm(),r);
  ribs->writeXmlFile(ribXmlFile.c_str(),jc.instrument().c_str(),jc.timeStamp().c_str(),ribFitter.algorithm(),r);
  ribs->writeResultsToTree(outputRootFile.c_str());
 

  AcdHistCalibMap* hists = r.getHistMap(AcdCalib::H_RIBBONS);
 
  AcdPadMap* logPads = AcdCalibUtil::drawRibbons(*hists,*ribs,kTRUE,psFile_log.c_str());    
  AcdCalibUtil::saveCanvases(logPads->canvasList(),"",".gif");

  AcdPadMap* linPads = AcdCalibUtil::drawRibbons(*hists,*ribs,kFALSE,psFile_lin.c_str());
  AcdCalibUtil::saveCanvases(linPads->canvasList(),"",".gif");  

  return 0;
}







