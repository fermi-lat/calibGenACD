#include "AcdHighRange.h"

#include "TH1F.h"
#include "TChain.h"
#include "TF1.h"

#include "AcdHistCalibMap.h"
#include "AcdCalibUtil.h"
#include "AcdCalibMap.h"

#include <fstream>



AcdHighRange::AcdHighRange(TChain* svacChain, AcdMap::Config config)
  :AcdCalibBase(AcdCalibData::HIGH_RANGE,config),
   m_histMap(0){

  setChain(AcdCalib::SVAC,svacChain);
  
  m_histMap = bookHists(AcdCalib::H_HIGH_RANGE,100,0.,1000.);

  Bool_t ok = attachChains();
  if ( ! ok ) {
    std::cerr << "ERR:  Failed to attach to input chains."  << std::endl;
  }
}


AcdHighRange::~AcdHighRange() 
{
}

Bool_t AcdHighRange::attachChains() {
  TChain* svacChain = getChain(AcdCalib::SVAC);
  if (svacChain != 0) {
    ;
  }
  return kTRUE;
}


Bool_t AcdHighRange::convertTxt(const char* txtFileName) {
  ifstream infile(txtFileName);
  if ( ! infile.good() ) return kFALSE;

  int id, iPmt;
  Float_t pars[5];
  Float_t maxVal;
  UInt_t idx(0);
  while ( ! infile.eof() ) {
    infile >> iPmt >> id >> pars[0] >> pars[1] >> pars[2] >> pars[3] >> pars[4] >> maxVal;
    if ( !infile.good() ) {
      if ( infile.eof() ) return kTRUE;
      std::cerr << "Read line falied at line " << idx << std::endl;
      return kFALSE;
    }
    idx++;
    UInt_t key = AcdMap::makeKey(iPmt,id);
    TH1* h = m_histMap->getHist(key);
    if ( h == 0) {
      std::cerr << "No hist for key " << key << std::endl;
      return kFALSE;
    }
    fillHistFromPars(pars,*h,maxVal);
  }
  return kTRUE;
}


void AcdHighRange::fillHistFromPars(Float_t oldPars[5], TH1& theHist, Float_t maxVal ) {
  TF1 fOld("convert","pol4(0)",0.,maxVal);
  fOld.SetParameter(0,oldPars[0]);
  fOld.SetParameter(1,oldPars[1]);
  fOld.SetParameter(2,oldPars[2]);
  fOld.SetParameter(3,oldPars[3]);
  fOld.SetParameter(4,oldPars[4]);
  theHist.Add(&fOld);
}



Bool_t AcdHighRange::readEvent(int ievent, Bool_t& filtered, 
				   int& /* runId */, int& /*evtId*/) {
  
  TChain* svacChain =getChain(AcdCalib::SVAC);
  if(svacChain) { 
    svacChain->GetEvent(ievent);
  }

  return kTRUE;
}


void AcdHighRange::useEvent(Bool_t& used) {
  used = kFALSE;
}
