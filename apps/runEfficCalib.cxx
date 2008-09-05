#include <string>
#include <fstream>

#include "../src/AcdJobConfig.h"

#include "../src/AcdCalibUtil.h"
#include "../src/AcdEfficLoop.h"

#include <TFile.h>

int main(int argn, char** argc) {

  // configure
  AcdJobConfig jc("runEfficCalib.exe","This utility runs the ACD efficiency checking code");

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
  if ( ! jc.checkMerit() ) return AcdJobConfig::MissingInput;  

  /// build filler & load calibs
  AcdEfficLoop r(AcdCalibData::MERITCALIB,jc.svacChain(),jc.meritChain(),jc.config());

  TString foutName = jc.outputPrefix() + "_effic.root";
  TFile* fout = r.makeOutput(foutName);

  // run!
  r.go(jc.optval_n(),jc.optval_s());    

  // output
  fout->Write();

  return AcdJobConfig::Success;
}







