#include <string>
#include <fstream>

#include "../src/AcdJobConfig.h"

#include "../src/AcdCalibUtil.h"
#include "../src/AcdStripChart.h"
#include "../src/AcdStripFit.h"
#include "../src/AcdCalibMap.h"


int main(int argn, char** argc) {

  // configure
  AcdJobConfig jc("runStripChart.exe","This utility time series strip chart code");

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

  /// build filler & run over events
  std::string outputTimestampFile = jc.outputPrefix() + "_timestamps.txt";
  AcdStripChart r(jc.digiChain(), jc.optval_b(), outputTimestampFile.c_str() );
  //r.setCalType(AcdCalibBase::TIME_PROF_PHA);        

  bool removePeds(true);
  if ( jc.pedFileName() != "" && jc.reconChain() != 0 ) {
    r.readCalib(AcdCalibData::PEDESTAL,jc.pedFileName().c_str());
    removePeds = false;
  }
  r.go(jc.optval_n(),jc.optval_s());    

  // strip chart output
  std::string outputPhaHistFile = jc.outputPrefix() + "_PhaStrip.root";
  std::string outputHitHistFile = jc.outputPrefix() + "_HitStrip.root";
  std::string outputVetoHistFile = jc.outputPrefix() + "_VetoStrip.root";  
    
  // do fits

  std::string textFile = jc.outputPrefix() + "_strip.txt";
  
  std::string outputPhaTxtFile = jc.outputPrefix() + "_PhaStrip.txt";
  std::string outputHitTxtFile = jc.outputPrefix() + "_HitStrip.txt";
  std::string outputVetoTxtFile = jc.outputPrefix() + "_VetoStrip.txt";

  std::cout << std::endl;

  AcdStripFitLibrary fitPha(AcdStripFitLibrary::Minuit,AcdCalib::MEAN_ABSOLUTE,100,-50.,50.);  
  AcdCalibMap* phaStripMap = r.fit(fitPha,AcdCalibData::TIME_PROF,AcdCalib::H_TIME_PHA);
  phaStripMap->writeTxtFile(outputPhaTxtFile.c_str(),jc.instrument().c_str(),jc.timeStamp().c_str(),fitPha.algorithm(),r);
  r.writeHistograms(AcdCalib::H_TIME_PHA, outputPhaHistFile.c_str());
  Bool_t ok = fitPha.test(*phaStripMap,-30.,30.,"FAIL!  ","pha time analysis");

  AcdStripFitLibrary fitHit(AcdStripFitLibrary::Minuit,AcdCalib::MEAN_RELATIVE,100,0.,20.);
  AcdCalibMap* hitStripMap = r.fit(fitHit,AcdCalibData::TIME_PROF,AcdCalib::H_TIME_HIT);
  hitStripMap->writeTxtFile(outputHitTxtFile.c_str(),jc.instrument().c_str(),jc.timeStamp().c_str(),fitHit.algorithm(),r);
  r.writeHistograms(AcdCalib::H_TIME_HIT, outputHitHistFile.c_str());
  ok = fitHit.test(*hitStripMap,-1.,3.,"FAIL!  ","hit rate time analysis");

  AcdStripFitLibrary fitVeto(AcdStripFitLibrary::Minuit,AcdCalib::MEAN_RELATIVE,100,0.,20.);
  AcdCalibMap* vetoStripMap = r.fit(fitVeto,AcdCalibData::TIME_PROF,AcdCalib::H_TIME_VETO);
  vetoStripMap->writeTxtFile(outputVetoTxtFile.c_str(),jc.instrument().c_str(),jc.timeStamp().c_str(),fitVeto.algorithm(),r);
  r.writeHistograms(AcdCalib::H_TIME_VETO, outputVetoHistFile.c_str());
  ok = fitVeto.test(*vetoStripMap,-1.,3.,"FAIL!  ","veto rate time analysis");


  return 0;
}







