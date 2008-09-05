#include <string>
#include <fstream>

#include "../src/AcdJobConfig.h"

#include "../src/AcdCalibUtil.h"
#include "../src/AcdDacFit.h"
#include "../src/AcdMetaCalib.h"

#include "../src/AcdPadMap.h"
#include "../src/AcdCalibMap.h"

int main(int argn, char** argc) {

  // configure
  AcdJobConfig jc("runCnoFitCalib.exe","This utility runs the veto set point calibration code");
  
  Int_t parseValue = jc.parse(argn,argc); 
  switch ( parseValue ) {
  case AcdJobConfig::Success: // ok to proceed
    break;  
  case AcdJobConfig::HelpMsg: // called -h option terminate processesing normally
    return AcdJobConfig::Success; 
  default: 
    return parseValue;  // parse failed, return failure code
  }

  /// build filler & load calibs
  AcdMetaCalib r(AcdCalibData::CNO_FIT);
  if ( ! r.readCalib(AcdCalibData::PED_HIGH,jc.pedHighFileName().c_str()) ) return AcdJobConfig::MissingInput;
  if ( ! r.readCalib(AcdCalibData::CARBON,jc.carbonFileName().c_str()) ) return AcdJobConfig::MissingInput;

  // run!
  const std::list<std::string>& theArgs = jc.theArgs();
  for ( std::list<std::string>::const_iterator itr = theArgs.begin(); 
	itr != theArgs.end(); itr++ ) {
    const std::string& anArg = *itr;
    std::string::size_type pos = anArg.find(':');
    if ( pos == anArg.npos ) {
      jc.usage();
      return AcdJobConfig::IllegalOption;
    }
    std::string settings(anArg,0,pos);
    std::string setPoints(anArg,pos+1);
    if ( ! r.readSettingsSetPointPair(settings,setPoints ) ) {
      return AcdJobConfig::MissingInput;
    }
  }
  if ( ! r.fillCnoThresholds() ) {
    return AcdJobConfig::ProccessFail;
  }

  // do fits
  AcdDacFitLibrary dacFitter(AcdDacFitLibrary::Linear,kTRUE);
  AcdCalibMap* vetos = r.fit(dacFitter,AcdCalibData::CNO_FIT,AcdCalib::H_VETO_FIT, jc.refFileName().c_str(),AcdKey::NoSkirt);  
  if ( vetos == 0 ) return AcdJobConfig::ProccessFail;
  if ( ! r.copyPeakVals() ) return AcdJobConfig::ProccessFail;

  // output
  std::string algorithm = dacFitter.algorithm();
  if ( ! vetos->writeOutputs( jc.outputPrefix(), algorithm, jc.instrument(), jc.timeStamp()) ) return AcdJobConfig::OutputFail;

  return AcdJobConfig::Success;
}







