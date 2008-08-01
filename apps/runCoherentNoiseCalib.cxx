#include <string>
#include <fstream>

#include "../src/AcdJobConfig.h"

#include "../src/AcdCalibUtil.h"
#include "../src/AcdCoherentNoise.h"
#include "../src/AcdCoherentNoiseFit.h"

#include "../src/AcdPadMap.h"
#include "../src/AcdCalibMap.h"

int main(int argn, char** argc) {

  // configure
  AcdJobConfig jc("runCoherentNoiseCalib.exe","This utility runs the coherent noise analysis");

  Int_t parseValue = jc.parse(argn,argc); 
  switch ( parseValue ) {
  case AcdJobConfig::Success: // ok to proceed
    break;  
  case AcdJobConfig::HelpMsg: // called -h option terminate processesing normally
    return AcdJobConfig::Success; 
  default: 
    return parseValue;  // parse failed, return failure code
  }

  if ( ! jc.checkDigi() ) return AcdJobConfig::MissingInput;

  /// build filler & load calibs
  AcdCoherentNoise r(jc.digiChain(), 529, 2529, 200., jc.config() );

  if ( ! r.readCalib(AcdCalibData::PEDESTAL,jc.pedFileName().c_str()) ) return AcdJobConfig::MissingInput;

  // run!
  r.go(jc.optval_n(),jc.optval_s());    

  // do fits  
  r.fillHistograms();
  AcdCoherentNoiseFitLibrary fitCoherent(AcdCoherentNoiseFitLibrary::Minuit);
  AcdCalibMap* fitMap = r.fit(fitCoherent,AcdCalibData::COHERENT_NOISE,AcdCalib::H_COHERENT_NOISE,jc.refFileName().c_str());
  if ( fitMap == 0 ) return AcdJobConfig::ProccessFail;

  // output
  std::string algorithm = fitCoherent.algorithm();
  if ( ! fitMap->writeOutputs( jc.outputPrefix(), algorithm, jc.instrument(), jc.timeStamp()) ) return AcdJobConfig::OutputFail;
  
  return AcdJobConfig::Success;
}








