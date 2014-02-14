#include <string>
#include <fstream>

#include "../src/AcdJobConfig.h"

#include "../src/AcdCalibLoop_OVL.h"
#include "../src/AcdPedestalFit.h"
#include "../src/AcdGainFit.h"
#include "../src/AcdCalibMap.h"
#include "../src/AcdPadMap.h"
#include "../src/AcdCalibUtil.h"

int main(int argn, char** argc) {

  // configure
  AcdJobConfig jc("runOverlayCalib.exe","This utility check the overlay files");

  Int_t parseValue = jc.parse(argn,argc); 
  switch ( parseValue ) {
  case AcdJobConfig::Success: // ok to proceed
    break;  
  case AcdJobConfig::HelpMsg: // called -h option terminate processesing normally
    return AcdJobConfig::Success; 
  default: 
    return parseValue;  // parse failed, return failure code
  }

  if ( ! jc.checkOverlay() ) return AcdJobConfig::MissingInput;  

  // build filler 
  AcdCalibLoop_OVL r(AcdCalibData::PEDESTAL,jc.ovlChain(),jc.config(),jc.optval_d(),jc.optval_2(),jc.optval_1());
  if ( ! r.readCalib(AcdCalibData::GAIN,jc.gainFileName().c_str()) ) return AcdJobConfig::MissingInput;
 
  // run!
  r.go(jc.optval_n(),jc.optval_s());    

  // do fits
  AcdPedestalFitLibrary pedFitter(AcdPedestalFitLibrary::None);
  AcdCalibMap* peds = r.fit(pedFitter,AcdCalibData::PEDESTAL,AcdCalib::H_RAW,jc.refFileName().c_str());
  if ( peds == 0 ) return AcdJobConfig::ProccessFail;

  AcdGainFitLibrary gainFitter(&CalibData::AcdGainFitDesc::instance(),AcdGainFitLibrary::None);
  AcdCalibMap* gains = r.fit(gainFitter,AcdCalibData::GAIN,AcdCalib::H_GAIN,jc.refFileName().c_str());
  if ( gains == 0 ) return AcdJobConfig::ProccessFail;

  // output
  std::string algorithm = pedFitter.algorithm();
  if ( ! peds->writeOutputs( jc.outputPrefix(), algorithm, jc.instrument(), jc.timeStamp()) ) return AcdJobConfig::OutputFail;

  std::string algorithm_gain = gainFitter.algorithm();
  if ( ! gains->writeOutputs( jc.outputPrefix(), algorithm_gain, jc.instrument(), jc.timeStamp()) ) return AcdJobConfig::OutputFail;

  return AcdJobConfig::Success;
}







