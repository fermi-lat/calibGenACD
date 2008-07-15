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

  AcdCalibLoop_Digi r(AcdCalibData::CNO,jc.digiChain(),jc.optval_P(),jc.config());
  if ( jc.pedFileName() != "" ) {
    r.readCalib(AcdCalibData::PEDESTAL,jc.pedFileName().c_str());
  }
  
  r.go(jc.optval_n(),jc.optval_s());    

  r.makeRatioPlots();

  AcdCnoFitLibrary cnoFitter(AcdCnoFitLibrary::Counting);
  AcdCalibMap* cnos = r.fit(cnoFitter,AcdCalibData::CNO,AcdCalib::H_FRAC);

  // output
  std::string outputTxtFile = jc.outputPrefix() + "_cno.txt";
  std::string outputXmlFile = jc.outputPrefix() + "_cno.xml";
  std::string outputPlotFile = jc.outputPrefix() + "_cno_";
  std::string outputHistFile = jc.outputPrefix() + "_cnoFrac.root";
  std::string outputHistFileRaw = jc.outputPrefix() + "_cno_all.root";
  std::string outputHistFileCno = jc.outputPrefix() + "_cno_cno.root";
  std::string outputRootFile = jc.outputPrefix() + "_cnoFit.root";

  r.writeHistograms(AcdCalib::H_VETO, outputHistFile.c_str());
  r.writeHistograms(AcdCalib::H_RAW, outputHistFileRaw.c_str());
  r.writeHistograms(AcdCalib::H_FRAC, outputHistFileCno.c_str());
  cnos->writeTxtFile(outputTxtFile.c_str(),jc.instrument().c_str(),jc.timeStamp().c_str(),cnoFitter.algorithm(),r);
  cnos->writeXmlFile(outputXmlFile.c_str(),jc.instrument().c_str(),jc.timeStamp().c_str(),cnoFitter.algorithm(),r);
  cnos->writeResultsToTree(outputRootFile.c_str());  
  AcdPadMap* padMap = 
    AcdCalibUtil::drawCnos(*(r.getHistMap(AcdCalib::H_VETO)),*(r.getHistMap(AcdCalib::H_RAW)),*cnos,outputPlotFile.c_str());
  AcdCalibUtil::saveCanvases(padMap->canvasList(),"",".gif");  

  return 0;
}







