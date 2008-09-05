#include "AcdEfficLoop.h"

#include <TChain.h>
#include <TFile.h>

#include <cassert>
#include <cmath>
#include <TMath.h>


AcdEfficLoop::AcdEfficLoop(AcdCalibData::CALTYPE t, 
			   TChain* svacChain, TChain* meritChain,
			   AcdKey::Config config)
  :AcdCalibBase(t,config),
   b_FswDGNState(0),
   b_TkrNumTracks(0),
   b_CTBTKRGamProb(0),
   b_CalEnergyRaw(0),
   b_CalCsIRLn(0),   
   b_Tkr1SSDVeto(0),
   b_Tkr1KalEne(0),
   b_Tkr1NumHits(0),
   b_AcdMips(0),
   b_AcdTkrPointX(0),
   b_AcdTkrPointY(0),
   b_AcdTkrPointZ(0),
   b_AcdTkrPointFace(0),
   b_AcdPocaDoca(0),
   b_AcdPocaTileID(0),   
   b_timeStamp(0),
   m_fout(0),
   m_tout(0){
  
  setChain(AcdCalib::SVAC,svacChain);
  setChain(AcdCalib::MERIT,meritChain);
  
  Bool_t ok = attachChains();
  if ( ! ok ) {
    std::cerr << "ERR:  Failed to attach to input chains."  << std::endl;
  }
}


AcdEfficLoop::~AcdEfficLoop() {
}

TFile* AcdEfficLoop::makeOutput(const char* fileName) {
  m_fout = new TFile(fileName,"RECREATE");
  m_tout = new TTree("AcdEffic","ACD Efficiency measurement");

  m_tout->Branch("CTBTKRGamProb",&m_CTBTKRGamProb,"CTBTKRGamProb/F");
  m_tout->Branch("Tkr1SSDVeto",&m_Tkr1SSDVeto,"Tkr1SSDVeto/F");
  m_tout->Branch("CalEnergyRaw",&m_CalEnergyRaw,"CalEnergyRaw/F");
  m_tout->Branch("CalMIPRatio",&m_CalMIPRatio,"CalMIPRatio/F");
  m_tout->Branch("CalCsIRLn",&m_CalCsIRLn,"CalCsIRLn/F");
  m_tout->Branch("Tkr1NumHits",&m_Tkr1NumHits,"Tkr1NumHits/I");
  m_tout->Branch("Tkr1KalEne",&m_Tkr1KalEne,"Tkr1KalEne/F");
  m_tout->Branch("xAcd",&m_xAcd,"xAcd/F");
  m_tout->Branch("yAcd",&m_yAcd,"yAcd/F");
  m_tout->Branch("zAcd",&m_zAcd,"zAcd/F");
  m_tout->Branch("acdFace",&m_acdFace,"acdFace/I");
  m_tout->Branch("mipsPocaA",&m_mipsPocaA,"mipsPocaA/F");
  m_tout->Branch("mipsPocaB",&m_mipsPocaB,"mipsPocaB/F");
  m_tout->Branch("actDist",&m_actDist,"actDist/F");
  m_tout->Branch("actDistID",&m_actDistID,"actDistID/I");
  return m_fout;
}

Bool_t AcdEfficLoop::attachChains() {
  TChain* svacChain  = getChain(AcdCalib::SVAC);
  TChain* meritChain = getChain(AcdCalib::MERIT);

  svacChain->SetBranchStatus("*",1);
  meritChain->SetBranchStatus("*",1);

  if ( svacChain == 0 || meritChain == 0 ) return kFALSE;

  if ( svacChain->GetEntries() != meritChain->GetEntries() ) {
    std::cerr << "# of entires mismatch: svac " << svacChain->GetEntries()
	      << " merit " << meritChain->GetEntries() << std::endl;
    return kFALSE;
  }

  meritChain->SetBranchAddress("CTBTKRGamProb",&m_CTBTKRGamProb);
  meritChain->SetBranchAddress("Tkr1SSDVeto",&m_Tkr1SSDVeto);
  meritChain->SetBranchAddress("CalEnergyRaw",&m_CalEnergyRaw);
  meritChain->SetBranchAddress("CalMIPRatio",&m_CalMIPRatio);
  meritChain->SetBranchAddress("CalCsIRLn",&m_CalCsIRLn);

  svacChain->SetBranchAddress("EvtTime",&m_timeStamp);
  svacChain->SetBranchAddress("FswDGNState",&m_FswDGNState);
  svacChain->SetBranchAddress("TkrNumTracks",&m_TkrNumTracks);
  svacChain->SetBranchAddress("Tkr1KalEne",&m_Tkr1KalEne);
  svacChain->SetBranchAddress("Tkr1NumHits",&m_Tkr1NumHits);
  svacChain->SetBranchAddress("AcdMips",&(m_AcdMips[0][0]));
  svacChain->SetBranchAddress("AcdTkrPointX",&(m_AcdTkrPointX[0]));
  svacChain->SetBranchAddress("AcdTkrPointY",&(m_AcdTkrPointY[0]));
  svacChain->SetBranchAddress("AcdTkrPointZ",&(m_AcdTkrPointZ[0]));
  svacChain->SetBranchAddress("AcdTkrPointFace",&(m_AcdTkrPointFace[0]));
  svacChain->SetBranchAddress("AcdPocaDoca",&(m_AcdPocaDoca[0][0]));
  svacChain->SetBranchAddress("AcdPocaTileID",&(m_AcdPocaTileID[0][0]));

  return kTRUE;
}



Bool_t AcdEfficLoop::readEvent(int ievent, Bool_t& filtered, 
			       int& /* runId */, int& /*evtId*/, Double_t& timeStamp) {
  
  TChain* svacChain =getChain(AcdCalib::SVAC);
  TChain* meritChain =getChain(AcdCalib::MERIT);

  if ( svacChain == 0 || meritChain == 0 ) return kFALSE;

  m_idx = meritChain->LoadTree(ievent);  
  if ( m_idx < 0 ) return kFALSE;
  m_idx = svacChain->LoadTree(ievent);
  if ( m_idx < 0 ) return kFALSE;

  if ( m_idx == 0 ) {
    if ( ! getBranches() ) return kFALSE;
  }

  b_timeStamp->GetEntry(m_idx);
  timeStamp = m_timeStamp;

  b_FswDGNState->GetEntry(m_idx);
  if ( m_FswDGNState != 3 ) { 
    filtered = kTRUE;
    return kTRUE;
  }

  b_TkrNumTracks->GetEntry(m_idx);  
  if ( m_TkrNumTracks != 1 ) { 
    filtered = kTRUE;
    return kTRUE;
  }

  filtered = kFALSE;
  return kTRUE;
}

void AcdEfficLoop::useEvent(Bool_t& used) {

  b_CalEnergyRaw->GetEntry(m_idx);
  b_CalMIPRatio->GetEntry(m_idx);
  b_CalCsIRLn->GetEntry(m_idx);
  b_CTBTKRGamProb->GetEntry(m_idx);	
  b_Tkr1SSDVeto->GetEntry(m_idx);
  b_Tkr1KalEne->GetEntry(m_idx);
  b_Tkr1NumHits->GetEntry(m_idx);  
  b_AcdMips->GetEntry(m_idx);  

  b_AcdTkrPointX->GetEntry(m_idx);
  b_AcdTkrPointY->GetEntry(m_idx);
  b_AcdTkrPointZ->GetEntry(m_idx);
  b_AcdTkrPointFace->GetEntry(m_idx);

  b_AcdPocaDoca->GetEntry(m_idx); 
  b_AcdPocaTileID->GetEntry(m_idx); 
  
  m_xAcd = m_AcdTkrPointX[0];
  m_yAcd = m_AcdTkrPointY[0];
  m_zAcd = m_AcdTkrPointZ[0];
  m_acdFace = m_AcdTkrPointFace[0];

  m_actDist   = m_AcdPocaDoca[0][0] > m_AcdPocaDoca[0][1] ? m_AcdPocaDoca[0][0] :  m_AcdPocaDoca[0][1];
  m_actDistID = m_AcdPocaDoca[0][0] > m_AcdPocaDoca[0][1] ? m_AcdPocaTileID[0][0] :  m_AcdPocaTileID[0][1];

  if ( m_actDistID >= 0 ) {
    m_mipsPocaA = m_AcdMips[m_actDistID][0];
    m_mipsPocaB = m_AcdMips[m_actDistID][1];	
  } else {
    m_mipsPocaA = 0.;
    m_mipsPocaB = 0.;
  }

  m_tout->Fill();
  used = kTRUE;
}

Bool_t AcdEfficLoop::getBranches() {
  TChain* svacChain =getChain(AcdCalib::SVAC);
  TChain* meritChain =getChain(AcdCalib::MERIT);
  if ( svacChain == 0 || 
       meritChain == 0 ) return kFALSE;

  b_CTBTKRGamProb = meritChain->GetBranch("CTBTKRGamProb");
  if ( b_CTBTKRGamProb == 0 ) return kFALSE;
  b_Tkr1SSDVeto = meritChain->GetBranch("Tkr1SSDVeto");
  if ( b_Tkr1SSDVeto == 0 ) return kFALSE;
  b_CalEnergyRaw = meritChain->GetBranch("CalEnergyRaw");
  if ( b_CalEnergyRaw == 0 ) return kFALSE;
  b_CalMIPRatio = meritChain->GetBranch("CalMIPRatio");
  if ( b_CalMIPRatio == 0 ) return kFALSE;
  b_CalCsIRLn = meritChain->GetBranch("CalCsIRLn");
  if ( b_CalCsIRLn == 0 ) return kFALSE;

  b_timeStamp = svacChain->GetBranch("EvtTime");
  if ( b_timeStamp == 0 ) return kFALSE;
  b_FswDGNState = svacChain->GetBranch("FswDGNState");
  if ( b_FswDGNState == 0 ) return kFALSE;
  b_TkrNumTracks = svacChain->GetBranch("TkrNumTracks");
  if ( b_TkrNumTracks == 0 ) return kFALSE;
  b_Tkr1KalEne = svacChain->GetBranch("Tkr1KalEne");  
  if ( b_Tkr1KalEne == 0 ) return kFALSE;
  b_Tkr1NumHits = svacChain->GetBranch("Tkr1NumHits");  
  if ( b_Tkr1NumHits == 0 ) return kFALSE;
  b_AcdMips = svacChain->GetBranch("AcdMips");
  if ( b_AcdMips == 0 ) return kFALSE;
  b_AcdTkrPointX = svacChain->GetBranch("AcdTkrPointX");
  if ( b_AcdTkrPointX == 0 ) return kFALSE;
  b_AcdTkrPointY = svacChain->GetBranch("AcdTkrPointY");
  if ( b_AcdTkrPointY == 0 ) return kFALSE;
  b_AcdTkrPointZ = svacChain->GetBranch("AcdTkrPointZ");
  if ( b_AcdTkrPointZ == 0 ) return kFALSE;
  b_AcdTkrPointFace = svacChain->GetBranch("AcdTkrPointFace");
  if ( b_AcdTkrPointFace == 0 ) return kFALSE;
  b_AcdPocaDoca = svacChain->GetBranch("AcdPocaDoca");
  if ( b_AcdPocaDoca == 0 ) return kFALSE;
  b_AcdPocaTileID = svacChain->GetBranch("AcdPocaTileID");
  if ( b_AcdPocaTileID == 0 ) return kFALSE;
  
  return kTRUE;  
}
