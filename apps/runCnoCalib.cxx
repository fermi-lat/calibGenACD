#include <string>
#include <fstream>

#include "../src/AcdJobConfig.h"

#include "../src/AcdCalibUtil.h"
#include "../src/AcdCnoFit.h"
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

  AcdCalibLoop_Digi r(AcdCalib::CNO,jc.digiChain(),jc.optval_P(),jc.config());
  if ( jc.pedFileName() != "" ) {
    r.readCalib(AcdCalib::PEDESTAL,jc.pedFileName().c_str());
  }
  
  r.go(jc.optval_n(),jc.optval_s());    

  r.makeRatioPlots();

  AcdCnoFitLibrary cnoFitter(AcdCnoFitLibrary::Counting);
  AcdCalibMap* cnos = r.fit(cnoFitter,AcdCalib::CNO,AcdCalib::H_FRAC);

  std::string textFile = jc.outputPrefix() + "_cno.txt";
  std::string xmlFile = jc.outputPrefix() + "_cno.xml";
  std::string psFile = jc.outputPrefix() + "_cno_";
  std::string outputHistFile = jc.outputPrefix() + "_cnoFrac.root";
  std::string outputHistFileRaw = jc.outputPrefix() + "_cno_all.root";
  std::string outputHistFileCno = jc.outputPrefix() + "_cno_cno.root";

  r.writeHistograms(AcdCalib::H_VETO, outputHistFile.c_str());
  r.writeHistograms(AcdCalib::H_RAW, outputHistFileRaw.c_str());
  r.writeHistograms(AcdCalib::H_FRAC, outputHistFileCno.c_str());
  cnos->writeTxtFile(textFile.c_str(),jc.instrument().c_str(),jc.timeStamp().c_str(),cnoFitter.algorithm(),r);
  cnos->writeXmlFile(xmlFile.c_str(),jc.instrument().c_str(),jc.timeStamp().c_str(),cnoFitter.algorithm(),r);

  AcdPadMap* padMap(0);
  padMap = AcdCalibUtil::drawCnos(*(r.getHistMap(AcdCalib::H_VETO)),*(r.getHistMap(AcdCalib::H_RAW)),*cnos,psFile.c_str());
  AcdCalibUtil::saveCanvases(padMap->canvasList());  

  return 0;
}







