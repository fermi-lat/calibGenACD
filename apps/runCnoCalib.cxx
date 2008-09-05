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
  case AcdJobConfig::Success: // ok to proceed
    break;  
  case AcdJobConfig::HelpMsg: // called -h option terminate processesing normally
    return AcdJobConfig::Success; 
  default: 
    return parseValue;  // parse failed, return failure code
  }

  if ( ! jc.checkDigi() ) return AcdJobConfig::MissingInput;

  // build filler & load calibs
  AcdCalibLoop_Digi r(AcdCalibData::CNO,jc.digiChain(),jc.optval_P(),jc.config());
  if ( ! r.readCalib(AcdCalibData::PED_HIGH,jc.pedHighFileName().c_str() ) ) return AcdJobConfig::MissingInput;
  
  // run!
  r.go(jc.optval_n(),jc.optval_s());    

  // do fits
  r.makeRatioPlots();
  AcdCnoFitLibrary cnoFitter(AcdCnoFitLibrary::Counting);
  AcdCalibMap* cnos = r.fit(cnoFitter,AcdCalibData::CNO,AcdCalib::H_CNO,jc.refFileName().c_str(),AcdKey::NoSkirt);
  if ( cnos == 0 ) return AcdJobConfig::ProccessFail;

  // output
  std::string algorithm = cnoFitter.algorithm();
  if ( ! cnos->writeOutputs( jc.outputPrefix(), algorithm, jc.instrument(), jc.timeStamp()) ) return AcdJobConfig::OutputFail;

  return AcdJobConfig::Success;
}







