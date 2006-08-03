#include <string>
#include <fstream>

#include "../src/AcdJobConfig.h"

#include "../src/AcdCalibUtil.h"
#include "../src/AcdVetoFitLibrary.h"
#include "../src/AcdMuonRoiCalib.h"

#include "../src/AcdPadMap.h"

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

  AcdMuonRoiCalib r(jc.digiChain(),jc.optval_P(),jc.config());
  r.setCalType(AcdCalibBase::VETO);        
  if ( jc.pedFileName() != "" ) {
    r.readPedestals(jc.pedFileName().c_str());
  }
  
  r.go(jc.optval_n(),jc.optval_s());    

  r.makeVetoRatio();

  AcdVetoFitLibrary vetoFitter(AcdVetoFitLibrary::Counting);
  AcdVetoFitMap* vetos = r.fitVetos(vetoFitter);

  std::string textFile = jc.outputPrefix() + "_veto.txt";
  std::string xmlFile = jc.outputPrefix() + "_veto.xml";
  std::string psFile = jc.outputPrefix() + "_veto_";
  std::string outputHistFile = jc.outputPrefix() + "_vetoFrac.root";
  std::string outputHistFileRaw = jc.outputPrefix() + "_veto_all.root";
  std::string outputHistFileVeto = jc.outputPrefix() + "_veto_veto.root";

  r.writeHistograms(AcdCalibBase::VETO_FRAC, outputHistFile.c_str());
  r.writeHistograms(AcdCalibBase::RAW, outputHistFileRaw.c_str());
  r.writeHistograms(AcdCalibBase::VETO, outputHistFileVeto.c_str());
  vetos->writeTxtFile(textFile.c_str(),jc.instrument().c_str(),jc.timeStamp().c_str(),vetoFitter.algorithm(),r);
  vetos->writeXmlFile(xmlFile.c_str(),jc.instrument().c_str(),jc.timeStamp().c_str(),vetoFitter.algorithm(),r);

  AcdPadMap* padMap(0);
  padMap = AcdCalibUtil::drawVetos(*(r.getHistMap(AcdCalibBase::VETO)),*(r.getHistMap(AcdCalibBase::RAW)),*vetos,psFile.c_str());
  AcdCalibUtil::saveCanvases(padMap->canvasList());  

  return 0;
}







