#include <string>
#include <fstream>

#include "../src/AcdJobConfig.h"

#include "../src/AcdCalibUtil.h"
#include "../src/AcdCalibLoop_Svac.h"
#include "../src/AcdPadMap.h"
#include "../src/AcdCalibMap.h"
#include "../src/AcdGainFit.h"
#include "../src/AcdRangeFit.h"


int main(int argn, char** argc) {

  // configure
  AcdJobConfig jc("runMeritCalib.exe","This utility checks the PHA calibration code on digi files");

  Int_t parseValue = jc.parse(argn,argc); 
  switch ( parseValue ) {
  case AcdJobConfig::Success: // ok to proceed
    break;  
  case AcdJobConfig::HelpMsg: // called -h option terminate processesing normally
    return AcdJobConfig::Success; 
  default: 
    return parseValue;  // parse failed, return failure code
  }

  if ( ! jc.checkSvac() ) return AcdJobConfig::MissingInput;  

  // build filler & load calibs
  AcdCalibLoop_Svac r(AcdCalibData::MERITCALIB,jc.svacChain(),jc.optval_L(),jc.optval_G(),jc.config());  
  bool removePeds = true;

  if ( ! r.readCalib(AcdCalibData::PEDESTAL,jc.pedFileName().c_str()) ) return AcdJobConfig::MissingInput;
  if ( ! r.readCalib(AcdCalibData::HIGH_RANGE,jc.rangeFileName().c_str()) ) return AcdJobConfig::MissingInput;
  if ( ! r.readCalib(AcdCalibData::GAIN,jc.gainFileName().c_str()) ) return AcdJobConfig::MissingInput;

  // run!
  r.go(jc.optval_n(),jc.optval_s()); 

  // do fits
  AcdGainFitLibrary gainFitter(&CalibData::AcdGainFitDesc::instance(),AcdGainFitLibrary::P5,removePeds);
  AcdCalibMap* gains = r.fit(gainFitter,AcdCalibData::GAIN,AcdCalib::H_GAIN);
  if ( gains == 0 ) return AcdJobConfig::ProccessFail;
  
  AcdRangeFitLibrary rangeFitter(AcdRangeFitLibrary::Counting);
  AcdCalibMap* ranges = r.fit(rangeFitter,AcdCalibData::RANGE,AcdCalib::H_RANGE);
  if ( ranges == 0 ) return AcdJobConfig::ProccessFail;

  std::string gain_algorithm = gainFitter.algorithm();
  if ( ! gains->writeOutputs( jc.outputPrefix(), gain_algorithm, jc.instrument(), jc.timeStamp()) ) 
    return AcdJobConfig::OutputFail;

  std::string range_algorithm = rangeFitter.algorithm();
  if ( ! ranges->writeOutputs( jc.outputPrefix(), range_algorithm, jc.instrument(), jc.timeStamp()) )
    return AcdJobConfig::OutputFail;

  return AcdJobConfig::Success;
}







