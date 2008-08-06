#include <string>
#include <fstream>

#include "../src/AcdJobConfig.h"

#include "../src/AcdCalibUtil.h"
#include "../src/AcdCarbonFit.h"
#include "../src/AcdCalibLoop_Svac.h"
#include "../src/AcdPadMap.h"
#include "../src/AcdCalibMap.h"


int main(int argn, char** argc) {

  // configure
  AcdJobConfig jc("runCarbonCalib.exe","This utility runs the Carbon peak calibration code on SVAC files");

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
  AcdCalibLoop_Svac r(AcdCalibData::CARBON,jc.svacChain(),jc.config());  
  r.setGCRCalibValue(jc.optval_G());

  if ( ! r.readCalib(AcdCalibData::PED_HIGH,jc.pedHighFileName().c_str() ) ) return AcdJobConfig::MissingInput;

  // run!
  r.go(jc.optval_n(),jc.optval_s()); 

  // do fits
  AcdCarbonFitLibrary gainFitter(AcdCarbonFitLibrary::Gauss);  
  AcdCalibMap* gains = r.fit(gainFitter,AcdCalibData::CARBON,AcdCalib::H_GAIN,jc.refFileName().c_str());
  if ( gains == 0 ) return AcdJobConfig::ProccessFail;

  // output
  std::string algorithm = gainFitter.algorithm();
  if ( ! gains->writeOutputs( jc.outputPrefix(), algorithm, jc.instrument(), jc.timeStamp()) ) return AcdJobConfig::OutputFail;

  return AcdJobConfig::Success;
}







