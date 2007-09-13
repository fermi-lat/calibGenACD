#include <string>
#include <fstream>

#include "../src/AcdJobConfig.h"

#include "../src/AcdCalibUtil.h"
#include "../src/AcdVetoFitLibrary.h"
#include "../src/AcdStripChart.h"
#include "../src/AcdStripFit.h"


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
    r.readPedestals(jc.pedFileName().c_str());
    removePeds = false;
  }
  r.go(jc.optval_n(),jc.optval_s());    

  // strip chart output
  std::string outputPhaHistFile = jc.outputPrefix() + "_PhaStrip.root";
  std::string outputHitHistFile = jc.outputPrefix() + "_HitStrip.root";
  std::string outputVetoHistFile = jc.outputPrefix() + "_VetoStrip.root";  
  r.writeHistograms(AcdCalibBase::H_TIME_PHA, outputPhaHistFile.c_str());
  r.writeHistograms(AcdCalibBase::H_TIME_HIT, outputHitHistFile.c_str());
  r.writeHistograms(AcdCalibBase::H_TIME_VETO, outputVetoHistFile.c_str());
  
  // do fits
  AcdStripFit fitPha("StripPha",AcdCalibUtil::MEAN_ABSOLUTE,100,-50.,50.);  
  AcdStripFit fitHit("StripHit",AcdCalibUtil::MEAN_RELATIVE,100,0.,20.);
  AcdStripFit fitVeto("StripVeto",AcdCalibUtil::MEAN_RELATIVE,100,0.,20.);


  std::string textFile = jc.outputPrefix() + "_strip.txt";
  
  std::string outputPhaTxtFile = jc.outputPrefix() + "_PhaStrip.txt";
  std::string outputHitTxtFile = jc.outputPrefix() + "_HitStrip.txt";
  std::string outputVetoTxtFile = jc.outputPrefix() + "_VetoStrip.txt";
 
  std::string outputPhaDeltaFile = jc.outputPrefix() + "_PhaDelta.root";
  std::string outputHitDeltaFile = jc.outputPrefix() + "_HitDelta.root";
  std::string outputVetoDeltaFile = jc.outputPrefix() + "_VetoDelta.root";

  AcdStripFitMap phaStripMap;
  fitPha.fitAll(phaStripMap,*(r.getHistMap(AcdCalibBase::H_TIME_PHA)));
  phaStripMap.writeTxtFile(outputPhaTxtFile.c_str(),jc.instrument().c_str(),jc.timeStamp().c_str(),fitPha.algorithm(),r);
  fitPha.fittedHists().writeHistograms(outputPhaDeltaFile.c_str());

  AcdStripFitMap hitStripMap;
  fitHit.fitAll(hitStripMap,*(r.getHistMap(AcdCalibBase::H_TIME_HIT)));
  hitStripMap.writeTxtFile(outputHitTxtFile.c_str(),jc.instrument().c_str(),jc.timeStamp().c_str(),fitHit.algorithm(),r);
  fitHit.fittedHists().writeHistograms(outputHitDeltaFile.c_str());

  AcdStripFitMap vetoStripMap;
  fitVeto.fitAll(vetoStripMap,*(r.getHistMap(AcdCalibBase::H_TIME_VETO)));
  vetoStripMap.writeTxtFile(outputVetoTxtFile.c_str(),jc.instrument().c_str(),jc.timeStamp().c_str(),fitVeto.algorithm(),r);
  fitVeto.fittedHists().writeHistograms(outputVetoDeltaFile.c_str());

  std::cout << std::endl;

  Bool_t ok = phaStripMap.test(-30.,30.,"FAIL!  ","pha time analysis");
  ok = hitStripMap.test(-1.,3.,"FAIL!  ","hit rate time analysis");
  ok = vetoStripMap.test(-1.,3.,"FAIL!  ","veto rate time analysis");

  return 0;
}







