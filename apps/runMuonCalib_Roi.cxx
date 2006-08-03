#include <string>
#include <fstream>

#include "../src/AcdJobConfig.h"

#include "../src/AcdCalibUtil.h"
#include "../src/AcdGainFitLibrary.h"
#include "../src/AcdMuonRoiCalib.h"
#include "../src/AcdPadMap.h"


int main(int argn, char** argc) {

  // configure
  AcdJobConfig jc("runMuonCalib_Tkr.exe","This utility runs the muon MIP calibration code");

  if ( ! jc.parse(argn,argc) ) {
    return 1;
  }

  /// build filler & run over events
  AcdMuonRoiCalib r(jc.digiChain(),jc.optval_P(),jc.config());
  r.setCalType(AcdCalibBase::GAIN);        

  bool removePeds = true;
  if ( jc.pedFileName() != "" ) {
    r.readPedestals(jc.pedFileName().c_str());
    removePeds = false;
  }
  r.go(jc.optval_n(),jc.optval_s());    

  // do fits
  AcdGainFitLibrary gainFitter(AcdGainFitLibrary::P5,removePeds);
  AcdGainFitMap* gains = r.fitGains(gainFitter);

  // output
  std::string gainTextFile = jc.outputPrefix() + "_gain.txt";
  std::string gainXmlFile = jc.outputPrefix() + "_gain.xml";
  std::string outputHistFile = jc.outputPrefix() + "_gain.root";
  std::string psFile = jc.outputPrefix() + "_gain_";

  r.writeHistograms(AcdCalibBase::GAIN, outputHistFile.c_str());
  gains->writeTxtFile(gainTextFile.c_str(),jc.instrument().c_str(),jc.timeStamp().c_str(),gainFitter.algorithm(),r);
  gains->writeXmlFile(gainXmlFile.c_str(),jc.instrument().c_str(),jc.timeStamp().c_str(),gainFitter.algorithm(),r);

  std::string psFile_log = psFile + "log_";
  std::string psFile_lin = psFile + "lin_";

  AcdPadMap* logPads(0);
  AcdPadMap* linPads(0);
  AcdHistCalibMap* hists = r.getHistMap(AcdCalibBase::GAIN);
 
  logPads = AcdCalibUtil::drawMips(*hists,*gains,kTRUE,psFile_log.c_str());    
  AcdCalibUtil::saveCanvases(logPads->canvasList());

  linPads = AcdCalibUtil::drawMips(*hists,*gains,kFALSE,psFile_lin.c_str());
  AcdCalibUtil::saveCanvases(linPads->canvasList());  

  return 0;
}







