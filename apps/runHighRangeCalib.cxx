#include <string>
#include <fstream>

#include "../src/AcdJobConfig.h"

#include "../src/AcdCalibUtil.h"
#include "../src/AcdMetaCalib.h"
#include "../src/AcdHighRangeFit.h"

#include "../src/AcdPadMap.h"
#include "../src/AcdCalibMap.h"

int main(int argn, char** argc) {

  // configure
  AcdJobConfig jc("runHighRangeCalib.exe","This utility runs the high range calibration code");
  
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
  AcdMetaCalib r(AcdCalibData::HIGH_RANGE);
  if ( ! r.readCalib(AcdCalibData::PEDESTAL,jc.pedFileName().c_str()) ) return AcdJobConfig::MissingInput;
  if ( ! r.readCalib(AcdCalibData::PED_HIGH,jc.pedHighFileName().c_str()) ) return AcdJobConfig::MissingInput;
  if ( ! r.readCalib(AcdCalibData::GAIN,jc.gainFileName().c_str()) ) return AcdJobConfig::MissingInput;
  if ( ! r.readCalib(AcdCalibData::CARBON,jc.carbonFileName().c_str()) ) return AcdJobConfig::MissingInput;
  if ( ! r.readCalib(AcdCalibData::RANGE,jc.rangeFileName().c_str()) ) return AcdJobConfig::MissingInput;
 
  if ( ! r.fillHighRangeData() ) {
    return AcdJobConfig::ProccessFail;
  }

  // do fits
  AcdHighRangeFitLibrary hrFitter(AcdHighRangeFitLibrary::Convert);
  AcdCalibMap* hrFits = r.fit(hrFitter,AcdCalibData::HIGH_RANGE,AcdCalib::H_HIGH_RANGE, jc.refFileName().c_str());
  if ( hrFits == 0 ) return AcdJobConfig::ProccessFail;

  // output
  std::string algorithm = hrFitter.algorithm();
  if ( ! hrFits->writeOutputs( jc.outputPrefix(), algorithm, jc.instrument(), jc.timeStamp()) ) return AcdJobConfig::OutputFail;
 
  return AcdJobConfig::Success;
}







