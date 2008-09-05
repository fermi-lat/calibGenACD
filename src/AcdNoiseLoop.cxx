#include "AcdNoiseLoop.h"

#include <TChain.h>
#include <TFile.h>

#include <cassert>
#include <cmath>
#include <TMath.h>


AcdNoiseLoop::AcdNoiseLoop(AcdCalibData::CALTYPE t, 
			   TChain* svacChain,
			   AcdKey::Config config)
  :AcdCalibBase(t,config),
   b_GemConditionsWord(0),
   b_TkrNumTracks(0),
   b_CalEnergyRaw(0),
   b_AcdMips(0),
   b_AcdHitMap(0),
   b_timeStamp(0),
   m_fout(0),
   m_tout(0){
  
  setChain(AcdCalib::SVAC,svacChain);
  
  Bool_t ok = attachChains();
  if ( ! ok ) {
    std::cerr << "ERR:  Failed to attach to input chains."  << std::endl;
  }
}


AcdNoiseLoop::~AcdNoiseLoop() {
}

TFile* AcdNoiseLoop::makeOutput(const char* fileName) {
  m_fout = new TFile(fileName,"RECREATE");
  m_tout = new TTree("AcdNoise","ACD Noise Occ. measurement");

  m_tout->Branch("GemConditionsWord",&m_GemConditionsWord,"GemConditionsWord/I");
  m_tout->Branch("GemDeltaEventTime",&m_GemDeltaEventTime,"GemDeltaEventTime/i");
  m_tout->Branch("GemDeltaWindowOpenTime",&m_GemDeltaWindowOpenTime,"GemDeltaWindowOpenTime/i");
  m_tout->Branch("TkrNumTracks",&m_TkrNumTracks,"TkrNumTracks/I");
  m_tout->Branch("CalEnergyRaw",&m_CalEnergyRaw,"CalEnergyRaw/F");
  m_tout->Branch("nAcdHit_0p1",&m_nAcdHit_0p1,"nAcdHit_0p1/I");
  m_tout->Branch("nAcdHit_0p25",&m_nAcdHit_0p25,"nAcdHit_0p25/I");
  m_tout->Branch("nAcdVeto",&m_nAcdVeto,"nAcdVeto/I");

  return m_fout;
}

Bool_t AcdNoiseLoop::attachChains() {
  TChain* svacChain  = getChain(AcdCalib::SVAC);

  svacChain->SetBranchStatus("*",1);

  if ( svacChain == 0  ) return kFALSE;

  svacChain->SetBranchAddress("EvtTime",&m_timeStamp);
  svacChain->SetBranchAddress("GemConditionsWord",&m_GemConditionsWord);
  svacChain->SetBranchAddress("GemDeltaEventTime",&m_GemDeltaEventTime);
  svacChain->SetBranchAddress("GemDeltaWindowOpenTime",&m_GemDeltaWindowOpenTime);
  svacChain->SetBranchAddress("TkrNumTracks",&m_TkrNumTracks);
  svacChain->SetBranchAddress("CalEnergyRaw",&m_CalEnergyRaw);

  svacChain->SetBranchAddress("AcdMips",&(m_AcdMips[0][0]));
  svacChain->SetBranchAddress("AcdHitMap",&(m_AcdHitMap[0][0]));

  return kTRUE;
}



Bool_t AcdNoiseLoop::readEvent(int ievent, Bool_t& filtered, 
			       int& /* runId */, int& /*evtId*/, Double_t& timeStamp) {
  
  TChain* svacChain =getChain(AcdCalib::SVAC);

  if ( svacChain == 0 ) return kFALSE;

  m_idx = svacChain->LoadTree(ievent);
  if ( m_idx < 0 ) return kFALSE;

  if ( m_idx == 0 ) {
    if ( ! getBranches() ) return kFALSE;
  }

  b_timeStamp->GetEntry(m_idx);
  timeStamp = m_timeStamp;

  b_GemConditionsWord->GetEntry(m_idx);
  if ( (m_GemConditionsWord & 32)  == 0) { 
    filtered = kTRUE;
    return kTRUE;
  }

  filtered = kFALSE;
  return kTRUE;
}

void AcdNoiseLoop::useEvent(Bool_t& used) {

  b_GemDeltaEventTime->GetEntry(m_idx);  
  b_GemDeltaWindowOpenTime->GetEntry(m_idx);  
  b_TkrNumTracks->GetEntry(m_idx);  
  b_CalEnergyRaw->GetEntry(m_idx);
  b_AcdMips->GetEntry(m_idx);  
  b_AcdHitMap->GetEntry(m_idx);  

  m_nAcdHit_0p1 = m_nAcdHit_0p25 = m_nAcdVeto = 0;
  for ( UInt_t i(0); i < 604; i++ ) {
    if ( m_AcdMips[i][0] > 0.1 ||
	 m_AcdMips[i][1] > 0.1 ) m_nAcdHit_0p1++;
    if ( m_AcdMips[i][0] > 0.25 ||
	 m_AcdMips[i][1] > 0.25 ) m_nAcdHit_0p25++;
    if ( m_AcdHitMap[i][0] > 0 ||
	 m_AcdHitMap[i][1] > 0 ) m_nAcdVeto++;
  }

  m_tout->Fill();
  used = kTRUE;
}

Bool_t AcdNoiseLoop::getBranches() {
  TChain* svacChain =getChain(AcdCalib::SVAC);
  if ( svacChain == 0  ) return kFALSE;

  b_timeStamp = svacChain->GetBranch("EvtTime");
  if ( b_timeStamp == 0 ) return kFALSE;
  b_GemConditionsWord = svacChain->GetBranch("GemConditionsWord");
  if ( b_GemConditionsWord == 0 ) return kFALSE;
  b_GemDeltaEventTime = svacChain->GetBranch("GemDeltaEventTime");
  if ( b_GemDeltaEventTime == 0 ) return kFALSE;
  b_GemDeltaWindowOpenTime = svacChain->GetBranch("GemDeltaWindowOpenTime");
  if ( b_GemDeltaWindowOpenTime == 0 ) return kFALSE;

  b_TkrNumTracks = svacChain->GetBranch("TkrNumTracks");
  if ( b_TkrNumTracks == 0 ) return kFALSE;
  b_CalEnergyRaw = svacChain->GetBranch("CalEnergyRaw");
  if ( b_CalEnergyRaw == 0 ) return kFALSE;
  b_AcdMips = svacChain->GetBranch("AcdMips");
  if ( b_AcdMips == 0 ) return kFALSE;
  b_AcdHitMap = svacChain->GetBranch("AcdHitMap");
  if ( b_AcdHitMap == 0 ) return kFALSE;
  
  return kTRUE;  
}
