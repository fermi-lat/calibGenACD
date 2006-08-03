#include <string>
#include <fstream>

#include "../src/AcdJobConfig.h"

#include "../src/AcdCalibUtil.h"
#include "../src/AcdMeritCalib.h"

int main(int argn, char** argc) {

  // configure
  AcdJobConfig jc("runPedestal.exe","This utility makes pedestal files from input digi files");

  if ( ! jc.parse(argn,argc) ) {
    return 1;
  }

  // build filler & run over events  
  AcdMeritCalib r(*jc.digiChain(),*jc.reconChain(),*jc.meritChain());
  if ( jc.pedFileName() != "" ) {
    r.readPedestals(jc.pedFileName().c_str());
  }
  if ( jc.gainFileName() != "" ) {
    r.readGains(jc.gainFileName().c_str());
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







