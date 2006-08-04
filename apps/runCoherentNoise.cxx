#include <string>
#include <fstream>

#include "../src/AcdJobConfig.h"

#include "../src/AcdCalibUtil.h"
#include "../src/AcdVetoFitLibrary.h"
#include "../src/AcdCoherentNoise.h"
#include "../src/AcdCoherentNoiseFit.h"

int main(int argn, char** argc) {

  // configure
  AcdJobConfig jc("runCoherentNoise.exe","This utility runs the coherent noise analysis");

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
  AcdCoherentNoise r(jc.digiChain(), 529, 2529, jc.optval_b(), jc.config() );
  r.setCalType(AcdCalibBase::COHERENT_NOISE);

  if ( jc.pedFileName() != "" ) {
    r.readPedestals(jc.pedFileName().c_str());
  }
  r.go(jc.optval_n(),jc.optval_s());    

  r.fillHistograms();

  // do fits
  AcdCoherentNoiseFitMap fitMap;
  AcdCoherentNoiseFit fitCoherent("FitCoherent",0);
  fitCoherent.fitAll(fitMap,*(r.getHistMap(AcdCalibBase::COHERENT_NOISE)));
  

  // strip chart output
  std::string outputHistFile = jc.outputPrefix() + "_Profile.root";
  r.writeHistograms(AcdCalibBase::COHERENT_NOISE, outputHistFile.c_str());

  std::string outputTxtFile = jc.outputPrefix() + "_Profile.txt";
  fitMap.writeTxtFile(outputTxtFile.c_str(),jc.instrument().c_str(),jc.timeStamp().c_str(),fitCoherent.algorithm(),r);

  return 0;
}







