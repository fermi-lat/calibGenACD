#include <string>
#include <fstream>
#include <TApplication.h>

#include "../src/AcdJobConfig.h"

#include "../src/AcdCalibUtil.h"
#include "../src/AcdGainFit.h"
//#include "../src/AcdCalibLoop_Svac.h"
#include "../src/AcdCalibLoop_Recon.h"
#include "../src/AcdPadMap.h"
#include "../src/AcdCalibMap.h"


int main(int argn, char** argc) {
  
  // configure
  AcdJobConfig jc("runMipCalib.exe","This utility runs the MIP calibration code on SVAC files");

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

  if ( ! jc.checkRecon() ) return AcdJobConfig::MissingInput;  

//  if ( ! jc.checkMerit() ) return AcdJobConfig::MissingInput;  

//  if ( ! jc.checkSvac() ) return AcdJobConfig::MissingInput;  


  /// build filler & load calibs
  AcdCalibLoop_Recon r(AcdCalibData::GAIN, jc.digiChain(), jc.reconChain(), jc.config());

//  AcdCalibLoop_Recon r(AcdCalibData::GAIN,jc. digiChain(), jc.reconChain(), jc.meritChain(), jc.config());

//  AcdCalibLoop_Svac r(AcdCalibData::GAIN,jc.svacChain(),jc.config());
  if ( ! r.readCalib(AcdCalibData::PEDESTAL,jc.pedFileName().c_str()) ) return AcdJobConfig::MissingInput;

  // run!

  if (  jc.eventList() != "" ){
    std::cout << jc.eventList() << std::endl;
    FILE *file = fopen(jc.eventList().c_str(), "r");
    std::vector<int> EvtRecon;
    int EvtID;
    while(fscanf(file, "%i ", &EvtID) > 0) {
      EvtRecon.push_back(EvtID);
    }
    r.go_list(EvtRecon);    
  }
  else {
    r.go(jc.optval_n(),jc.optval_s());    
  }
  // do fits
  AcdGainFitLibrary gainFitter(&CalibData::AcdGainFitDesc::instance(),AcdGainFitLibrary::GaussP1);
  AcdCalibMap* gains = r.fit(gainFitter,AcdCalibData::GAIN,AcdCalib::H_GAIN, jc.refFileName().c_str());
  if ( gains == 0 ) return AcdJobConfig::ProccessFail;

  // output
  std::string algorithm = gainFitter.algorithm();
  if ( ! gains->writeOutputs( jc.outputPrefix(), algorithm, jc.instrument(), jc.timeStamp()) ) return AcdJobConfig::OutputFail;

  return AcdJobConfig::Success;
}







