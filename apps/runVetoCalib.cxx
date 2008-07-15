#include <string>
#include <fstream>

#include "../src/AcdJobConfig.h"

#include "../src/AcdCalibUtil.h"
#include "../src/AcdVetoFit.h"
#include "../src/AcdCalibLoop_Digi.h"

#include "../src/AcdPadMap.h"
#include "../src/AcdCalibMap.h"

int main(int argn, char** argc) {

  // configure
  AcdJobConfig jc("runVetoCalib.exe","This utility runs the veto set point calibration code");
  
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

  AcdCalibLoop_Digi r(AcdCalibData::VETO,jc.digiChain(),jc.optval_P(),jc.config());
  if ( jc.pedFileName() != "" ) {
    r.readCalib(AcdCalibData::PEDESTAL,jc.pedFileName().c_str());
  }
  
  r.go(jc.optval_n(),jc.optval_s());    

  r.makeRatioPlots();

  AcdVetoFitLibrary vetoFitter(AcdVetoFitLibrary::Counting);
  AcdCalibMap* vetos = r.fit(vetoFitter,AcdCalibData::VETO,AcdCalib::H_FRAC);

  std::string textFile = jc.outputPrefix() + "_veto.txt";
  std::string xmlFile = jc.outputPrefix() + "_veto.xml";
  std::string psFile = jc.outputPrefix() + "_veto_";
  std::string outputHistFile = jc.outputPrefix() + "_vetoFrac.root";
  std::string outputRootFile = jc.outputPrefix() + "_vetoFit.root";
  std::string outputHistFileRaw = jc.outputPrefix() + "_veto_all.root";
  std::string outputHistFileVeto = jc.outputPrefix() + "_veto_veto.root";

  r.writeHistograms(AcdCalib::H_VETO, outputHistFile.c_str());
  r.writeHistograms(AcdCalib::H_RAW, outputHistFileRaw.c_str());
  r.writeHistograms(AcdCalib::H_FRAC, outputHistFileVeto.c_str());
  vetos->writeTxtFile(textFile.c_str(),jc.instrument().c_str(),jc.timeStamp().c_str(),vetoFitter.algorithm(),r);
  vetos->writeXmlFile(xmlFile.c_str(),jc.instrument().c_str(),jc.timeStamp().c_str(),vetoFitter.algorithm(),r);
  vetos->writeResultsToTree(outputRootFile.c_str());

  AcdPadMap* padMap = 
    AcdCalibUtil::drawVetos(*(r.getHistMap(AcdCalib::H_VETO)),*(r.getHistMap(AcdCalib::H_RAW)),*vetos,psFile.c_str());
  AcdCalibUtil::saveCanvases(padMap->canvasList(),"",".gif");  

  return 0;
}







