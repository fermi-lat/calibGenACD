#include <string>
#include <fstream>

#include "../src/AcdJobConfig.h"
#include "../src/AcdCalibMap.h"
#include "CalibData/Acd/AcdCalibEnum.h"
#include "CalibData/Acd/AcdCalibObj.h"

#include <TString.h>

int main(int argn, char** argc) {

  // configure
  AcdJobConfig jc("makeResultTree.exe","This utility makes calibration reports");

  Int_t parseValue = jc.parse(argn,argc); 
  switch ( parseValue ) {
  case AcdJobConfig::Success: // ok to proceed
    break;  
  case AcdJobConfig::HelpMsg: // called -h option terminate processesing normally
    return AcdJobConfig::Success; 
  default: 
    return parseValue;  // parse failed, return failure code
  }

  if ( jc.theArgs().size() != 1 ) {
    std::cerr << "calibReport.exe takes exactly one input" << std::endl;
    jc.usage();
    return AcdJobConfig::IllegalOption;
  }

  AcdCalibData::CALTYPE cType = AcdCalibData::NONE;
  std::string inName = jc.theArgs().front();
  if ( inName.find("_ped.") != inName.npos ) {
    cType = AcdCalibData::PEDESTAL;
  } else if ( inName.find("_gain.") != inName.npos ) {
    cType = AcdCalibData::GAIN;
  } else if ( inName.find("_veto.") != inName.npos ) {
    cType = AcdCalibData::VETO;
  } else if ( inName.find("_range.") != inName.npos ) {
    cType = AcdCalibData::RANGE;
  } else if ( inName.find("_cno.") != inName.npos ) {
    cType = AcdCalibData::CNO;
  } else if ( inName.find("_carbon.") != inName.npos ) {
    cType = AcdCalibData::CARBON;
  } else if ( inName.find("_highRange.") != inName.npos ) {
    cType = AcdCalibData::HIGH_RANGE;
  } else if ( inName.find("_coherentNoise.") != inName.npos ) {
    cType = AcdCalibData::COHERENT_NOISE;
  } else if ( inName.find("_rib.") != inName.npos ) {
    cType = AcdCalibData::RIBBON;
  } else if ( inName.find("_check.") != inName.npos ) {
    cType = AcdCalibData::MERITCALIB;
  } else {
    std::cerr << "Could not recognize input file type for file " <<  inName << std::endl;
    return AcdJobConfig::IllegalOption;
  }

  const CalibData::AcdCalibDescription* desc = CalibData::AcdCalibDescription::getDesc(cType);
  AcdCalibMap theCalib(*desc);
  if ( ! theCalib.readXmlFile(inName.c_str()) ) {
    std::cerr << "Failed to read input file " << inName << std::endl;
    return AcdJobConfig::MissingInput;
  }

  TString outName = inName.c_str();
  outName.ReplaceAll(".xml","Pars.root");
  if ( ! theCalib.writeResultsToTree(outName) ) return AcdJobConfig::OutputFail;

  return AcdJobConfig::Success;
}







