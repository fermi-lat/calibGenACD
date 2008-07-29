#include <string>
#include <fstream>

#include "../src/AcdJobConfig.h"

#include "../src/AcdCalibUtil.h"
#include "../src/AcdVetoFit.h"
#include "../src/AcdCalibLoop_Digi.h"

#include "../src/AcdPadMap.h"
#include "../src/AcdCalibMap.h"

int main(int argn, char** argc) {

  // configure
  AcdJobConfig jc("runVetoCalib.exe","This utility runs the veto set point calibration code");
  
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
  AcdCalibLoop_Digi r(AcdCalibData::VETO,jc.digiChain(),jc.optval_P(),jc.config());
  if ( ! r.readCalib(AcdCalibData::PEDESTAL,jc.pedFileName().c_str()) ) return AcdJobConfig::MissingInput;

  // run!
  r.go(jc.optval_n(),jc.optval_s());    

  // do fits
  r.makeRatioPlots();
  AcdVetoFitLibrary vetoFitter(AcdVetoFitLibrary::Counting);
  AcdCalibMap* vetos = r.fit(vetoFitter,AcdCalibData::VETO,AcdCalib::H_VETO, jc.refFileName().c_str());
  if ( vetos == 0 ) return AcdJobConfig::ProccessFail;

  // output
  std::string algorithm = vetoFitter.algorithm();
  if ( ! vetos->writeOutputs( jc.outputPrefix(), algorithm, jc.instrument(), jc.timeStamp()) ) return AcdJobConfig::OutputFail;

  return AcdJobConfig::Success;
}







