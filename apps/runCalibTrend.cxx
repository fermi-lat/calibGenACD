#include <string>
#include <fstream>

#include "../src/AcdJobConfig.h"

#include "../src/AcdCalibUtil.h"
#include "../src/AcdTrendCalib.h"

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
  
  AcdCalibData::CALTYPE cType = AcdCalibData::NONE;
  const std::string& refFileName = jc.refFileName();
  if ( refFileName.find("_ped.xml") != refFileName.npos ) {
    cType = AcdCalibData::PEDESTAL;
  } else if ( refFileName.find("_gain.xml") != refFileName.npos ) {
    cType = AcdCalibData::GAIN;
  } else if ( refFileName.find("_veto.xml") != refFileName.npos ) {
    cType = AcdCalibData::VETO;
  } else if ( refFileName.find("_range.xml") != refFileName.npos ) {
    cType = AcdCalibData::RANGE;
  } else if ( refFileName.find("_cno.xml") != refFileName.npos ) {
    cType = AcdCalibData::CNO;
  } else if ( refFileName.find("_highRange.xml") != refFileName.npos ) {    
    cType = AcdCalibData::HIGH_RANGE;
  } else if ( refFileName.find("_coherentNoise.xml") != refFileName.npos ) {
    cType = AcdCalibData::COHERENT_NOISE;
  } else if ( refFileName.find("_ribbon.xml") != refFileName.npos ) {
    cType = AcdCalibData::RIBBON;
  } else if ( refFileName.find("_pedHigh.xml") != refFileName.npos ) {
    cType = AcdCalibData::PED_HIGH;
  } else if ( refFileName.find("_carbon.xml") != refFileName.npos ) {
    cType = AcdCalibData::CARBON;
  } else if ( refFileName.find("_vetoFit.xml") != refFileName.npos ) {
    cType = AcdCalibData::VETO_FIT;
  } else if ( refFileName.find("_cnoFit.xml") != refFileName.npos ) {
    cType = AcdCalibData::CNO_FIT;
  } else if ( refFileName.find("_check.xml") != refFileName.npos ) {
    cType = AcdCalibData::MERITCALIB;
  } else {
    std::cerr << "Can't recognize calibraiton type from file " << refFileName << std::endl;
    return AcdJobConfig::MissingInput;
  }

  /// build filler & load calibs
  AcdTrendCalib r(cType);
  if ( ! r.readCalib(refFileName,kTRUE) ) return AcdJobConfig::MissingInput;

  const std::list<std::string>& theArgs = jc.theArgs();
  for ( std::list<std::string>::const_iterator itr = theArgs.begin(); 
	itr != theArgs.end(); itr++ ) {
    const std::string& anArg = *itr; 
    if ( anArg.find(".txt") != anArg.npos ||
	 anArg.find(".lst") != anArg.npos ) {
      std::vector<std::string> inFiles;
      if ( ! AcdJobConfig::getFileList(anArg.c_str(),inFiles) ) return AcdJobConfig::MissingInput;
      for ( std::vector<std::string>::const_iterator itr2 = inFiles.begin();
	    itr2 != inFiles.end(); itr2++) {
	if ( ! r.readCalib(*itr2) ) return AcdJobConfig::MissingInput;
      }	      
    } else {
      if ( ! r.readCalib(anArg) ) return AcdJobConfig::MissingInput;
    }
  }

  // do stuff
  if ( ! r.fillHistograms() ) return AcdJobConfig::ProccessFail;

  // output
  if ( ! r.writeOutputs( jc.outputPrefix(), jc.timeStamp()) ) return AcdJobConfig::OutputFail;
 
  return AcdJobConfig::Success;
}



