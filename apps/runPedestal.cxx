#include <string>
#include <fstream>

#include "../src/AcdJobConfig.h"

#include "../src/AcdCalibLoop_Digi.h"
#include "../src/AcdPedestalFit.h"
#include "../src/AcdCalibMap.h"
#include "../src/AcdPadMap.h"
#include "../src/AcdCalibUtil.h"

int main(int argn, char** argc) {

  // configure
  AcdJobConfig jc("runPedestal.exe","This utility makes pedestal files from input digi files");

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

  // build filler 
  AcdCalibLoop_Digi r(AcdCalibData::PEDESTAL,jc.digiChain(),jc.optval_P(),jc.config());

  // run!
  r.go(jc.optval_n(),jc.optval_s());    

  // do fits
  AcdPedestalFitLibrary pedFitter(AcdPedestalFitLibrary::MeanValue);
  AcdCalibMap* peds = r.fit(pedFitter,AcdCalibData::PEDESTAL,AcdCalib::H_RAW,jc.refFileName().c_str());
  if ( peds == 0 ) return AcdJobConfig::ProccessFail;

  // output
  std::string algorithm = pedFitter.algorithm();
  if ( ! peds->writeOutputs( jc.outputPrefix(), algorithm, jc.instrument(), jc.timeStamp()) ) return AcdJobConfig::OutputFail;

  return AcdJobConfig::Success;
}







