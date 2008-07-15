#include <string>
#include <fstream>

#include "../src/AcdJobConfig.h"
#include "../src/AcdCalibMap.h"
#include "CalibData/Acd/AcdCalibEnum.h"
#include "CalibData/Acd/AcdCalibObj.h"

int main(int argn, char** argc) {

  // configure
  AcdJobConfig jc("convertXmlToTxt.exe","This utility makes converts calibrations from xml to txt");

  Int_t parseValue = jc.parse(argn,argc); 
  switch ( parseValue ) {
  case 0: // ok to proceed
    break;  
  case 1: // called -h option terminate processesing normally
    return 0; 
  default: 
    return parseValue;  // parse failed, return failure code
  }


  AcdCalibData::CALTYPE cType = AcdCalibData::NONE;
  std::string inName = jc.inputFileName();

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
  } else if ( inName.find("_highRange.") != inName.npos ) {
    cType = AcdCalibData::HIGH_RANGE;
  } else if ( inName.find("_coherentNoise.") != inName.npos ) {
    cType = AcdCalibData::COHERENT_NOISE;
  } else if ( inName.find("_rib.") != inName.npos ) {
    cType = AcdCalibData::RIBBON;
  } else {
    std::cerr << "Could not recognize input file type for file " <<  inName << std::endl;
    return 4;
  }

  const CalibData::AcdCalibDescription* desc = CalibData::AcdCalibDescription::getDesc(cType);
  AcdCalibMap calib( *desc );

  calib.readXmlFile( inName.c_str() );
  std::string outName = inName;
  outName.replace( inName.find(".xml"), 4, std::string("Fit.root"));
  calib.writeTxt(std::cout);

  calib.writeResultsToTree(outName.c_str());

  return 0;
}







