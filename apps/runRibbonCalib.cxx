#include <string>
#include <fstream>

#include "../src/AcdJobConfig.h"

#include "../src/AcdCalibUtil.h"
#include "../src/AcdRibbonFit.h"
#include "../src/AcdCalibLoop_Svac.h"
#include "../src/AcdPadMap.h"
#include "../src/AcdCalibMap.h"


int main(int argn, char** argc) {

  // configure
  AcdJobConfig jc("runMuonCalib_Svac.exe","This utility runs the muon MIP calibration code on SVAC files");

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

  /// build filler & load calibs
  AcdCalibLoop_Svac r(AcdCalibData::RIBBON,jc.svacChain(),jc.config());

  if ( !  r.readCalib(AcdCalibData::PEDESTAL,jc.pedFileName().c_str()) ) return AcdJobConfig::MissingInput;

  // run!
  r.go(jc.optval_n(),jc.optval_s());    

  // do fits
  AcdRibbonFitLibrary ribFitter(AcdGainFitLibrary::GaussP1);
  AcdCalibMap* ribs = r.fit(ribFitter,AcdCalibData::RIBBON,AcdCalib::H_RIBBONS, jc.refFileName().c_str(),AcdKey::Ribbons);
  if ( ribs == 0 ) return AcdJobConfig::ProccessFail;

  // output
  std::string algorithm = ribFitter.algorithm();
  if ( ! ribs->writeOutputs( jc.outputPrefix(), algorithm, jc.instrument(), jc.timeStamp()) ) return AcdJobConfig::OutputFail;

  return AcdJobConfig::Success;  
}







