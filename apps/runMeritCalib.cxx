#include <string>
#include <fstream>

#include "../src/AcdJobConfig.h"

#include "../src/AcdCalibUtil.h"
#include "../src/AcdCalibLoop_Svac.h"
#include "../src/AcdPadMap.h"
#include "../src/AcdCalibMap.h"
#include "../src/AcdGainFit.h"
#include "../src/AcdRangeFit.h"
#include "../src/AcdVetoFit.h"
#include "../src/AcdCnoFit.h"
#include "../src/AcdPedestalFit.h"


int main(int argn, char** argc) {

  // configure
  AcdJobConfig jc("runMeritCalib.exe","This utility checks the various calibrations on svac files");

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
  AcdCalibLoop_Svac r(AcdCalibData::MERITCALIB,jc.svacChain(),jc.config());  
  r.setMIPFromSvacValue(jc.optval_m());

  if ( ! r.readCalib(AcdCalibData::PEDESTAL,jc.pedFileName().c_str()) ) return AcdJobConfig::MissingInput;
  if ( ! r.readCalib(AcdCalibData::HIGH_RANGE,jc.rangeFileName().c_str()) ) return AcdJobConfig::MissingInput;
  if ( ! r.readCalib(AcdCalibData::GAIN,jc.gainFileName().c_str()) ) return AcdJobConfig::MissingInput;

  // run!
  r.go(jc.optval_n(),jc.optval_s()); 

  r.makeRatioPlots();

  // do fits
  AcdPedestalFitLibrary pedFitter(AcdPedestalFitLibrary::MeanValue);
  AcdCalibMap* peds = r.fit(pedFitter,AcdCalibData::PEDESTAL,AcdCalib::H_RAW);
  if ( peds == 0 ) return AcdJobConfig::ProccessFail;

  AcdGainFitLibrary gainFitter(&CalibData::AcdGainFitDesc::instance(),AcdGainFitLibrary::GaussP1);
  AcdCalibMap* gains = r.fit(gainFitter,AcdCalibData::GAIN,AcdCalib::H_GAIN);
  if ( gains == 0 ) return AcdJobConfig::ProccessFail;
  
  AcdRangeFitLibrary rangeFitter(AcdRangeFitLibrary::Counting);
  AcdCalibMap* ranges = r.fit(rangeFitter,AcdCalibData::RANGE,AcdCalib::H_RANGE);
  if ( ranges == 0 ) return AcdJobConfig::ProccessFail;

  AcdVetoFitLibrary vetoFitter(AcdVetoFitLibrary::Counting);
  AcdCalibMap* veto = r.fit(vetoFitter,AcdCalibData::VETO,AcdCalib::H_VETO);
  if ( veto == 0 ) return AcdJobConfig::ProccessFail;

  AcdCnoFitLibrary cnoFitter(AcdCnoFitLibrary::Counting);
  AcdCalibMap* cno = r.fit(cnoFitter,AcdCalibData::CNO,AcdCalib::H_CNO);
  if ( cno == 0 ) return AcdJobConfig::ProccessFail;  

  std::string outputPrefix = jc.outputPrefix();

  std::string ped_algorithm = pedFitter.algorithm();
  if ( ! peds->writeOutputs( outputPrefix, ped_algorithm, jc.instrument(), jc.timeStamp(), true) )
    return AcdJobConfig::OutputFail;

  std::string gain_algorithm = gainFitter.algorithm();
  if ( ! gains->writeOutputs( outputPrefix, gain_algorithm, jc.instrument(), jc.timeStamp(), true) )
    return AcdJobConfig::OutputFail;

  std::string range_algorithm = rangeFitter.algorithm();
  if ( ! ranges->writeOutputs( outputPrefix, range_algorithm, jc.instrument(), jc.timeStamp(), true) )
    return AcdJobConfig::OutputFail;
  
  std::string veto_algorithm = vetoFitter.algorithm();
  if ( ! veto->writeOutputs( outputPrefix, veto_algorithm, jc.instrument(), jc.timeStamp(), true) )
    return AcdJobConfig::OutputFail;

  std::string cno_algorithm = cnoFitter.algorithm();
  if ( ! cno->writeOutputs( outputPrefix, cno_algorithm, jc.instrument(), jc.timeStamp(), true) )
    return AcdJobConfig::OutputFail;

  if ( ! r.fillMeritCalib() ) {
    std::cerr << "Failed to fill merit calib" << std::endl;
    return AcdJobConfig::ProccessFail;
  }

  AcdCalibMap* check = r.getCalibMap(AcdCalibData::MERITCALIB);
  if ( check == 0 ) return AcdJobConfig::ProccessFail;
  
  std::string check_algorithm("check");
  if ( ! check->writeOutputs( outputPrefix, check_algorithm, jc.instrument(), jc.timeStamp()) )
    return AcdJobConfig::OutputFail;

  return AcdJobConfig::Success;
}







