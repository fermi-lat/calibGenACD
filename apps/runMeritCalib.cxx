#include <string>
#include <fstream>

#include "../src/AcdJobConfig.h"

#include "../src/AcdCalibUtil.h"
#include "../src/AcdCalibLoop_Merit.h"
#include "TFile.h"
#include "TTree.h"

int main(int argn, char** argc) {

  // configure
  AcdJobConfig jc("runMeritCalib.exe","This utility runs analysis on the Merit ntuple");
  
  Int_t parseValue = jc.parse(argn,argc); 
  switch ( parseValue ) {
  case 0: // ok to proceed
    break;  
  case 1: // called -h option terminate processesing normally
    return 0; 
  default: 
    return parseValue;  // parse failed, return failure code
  }

  Bool_t okToContinue = jc.checkDigi();
  okToContinue &=  jc.checkRecon();
  okToContinue &=  jc.checkSvac();
  if ( ! okToContinue ) return 1; // no input, fail

  // build filler & run over events  
  AcdCalibLoop_Merit r(*jc.digiChain(),*jc.reconChain(),*jc.meritChain());
  if ( jc.pedFileName() != "" ) {
    r.readCalib(AcdCalibData::PEDESTAL,jc.pedFileName().c_str());
  }
  if ( jc.gainFileName() != "" ) {
    r.readCalib(AcdCalibData::GAIN,jc.gainFileName().c_str());
  }

  r.go(jc.optval_n(),jc.optval_s());    

  std::string outputHistFile = jc.outputPrefix() + "_acdMiss.root";
  TFile* fout = TFile::Open(outputHistFile.c_str(),"RECREATE");
  if ( fout != 0 ) {
    fout->cd();
    r.outputTree()->Write();
    fout->Close();
    delete fout;
  }

  r.writeTxtHeader(std::cout);

  return 0;
}







