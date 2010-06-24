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
   b_CTBCALGamProb(0),
   b_CTBCPFGamProb(0),
   b_CTBClassLevel(0),
   b_CTBBestEnergyProb(0),
   b_CTBBestLogEnergy(0),
   b_CTBCORE(0),
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

  m_tout->Branch("CTBTKRGamProb",&m_CTBTKRGamProb,"CTBTKRGamProb/D");
  m_tout->Branch("CTBCALGamProb",&m_CTBCALGamProb,"CTBCALGamProb/D");
  m_tout->Branch("CTBCPFGamProb",&m_CTBCPFGamProb,"CTBCPFGamProb/D");
  m_tout->Branch("CTBBestLogEnergy",&m_CTBBestLogEnergy,"CTBBestLogEnergy/D");
  m_tout->Branch("CTBBestEnergyProb",&m_CTBBestEnergyProb,"CTBBestEnergyProb/D");
  m_tout->Branch("CTBCORE",&m_CTBCORE,"CTBCORE/D");
  m_tout->Branch("CTBClassLevel",&m_CTBClassLevel,"CTBClassLevel/I");
  m_tout->Branch("Tkr1SSDVeto",&m_Tkr1SSDVeto,"Tkr1SSDVeto/D");
  m_tout->Branch("CalEnergyRaw",&m_CalEnergyRaw,"CalEnergyRaw/D");
  m_tout->Branch("CalMIPRatio",&m_CalMIPRatio,"CalMIPRatio/D");
  m_tout->Branch("CalCsIRLn",&m_CalCsIRLn,"CalCsIRLn/D");
  m_tout->Branch("Tkr1NumHits",&m_Tkr1NumHits,"Tkr1NumHits/D");
  m_tout->Branch("Tkr1KalEne",&m_Tkr1KalEne,"Tkr1KalEne/D");
  m_tout->Branch("xAcd",&m_xAcd,"xAcd/D");
  m_tout->Branch("yAcd",&m_yAcd,"yAcd/D");
  m_tout->Branch("zAcd",&m_zAcd,"zAcd/D");
  m_tout->Branch("acdFace",&m_acdFace,"acdFace/I");
  m_tout->Branch("mipsPocaA",&m_mipsPocaA,"mipsPocaA/D");
  m_tout->Branch("mipsPocaB",&m_mipsPocaB,"mipsPocaB/D");
  m_tout->Branch("actDist",&m_actDist,"actDist/D");
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

  meritChain->SetBranchAddress("CTBTKRGamProb",&m_CTBTKRGamProb_in);
  meritChain->SetBranchAddress("CTBCALGamProb",&m_CTBCALGamProb_in);
  meritChain->SetBranchAddress("CTBCPFGamProb",&m_CTBCPFGamProb_in);
  meritChain->SetBranchAddress("CTBBestEnergyProb",&m_CTBBestEnergyProb_in);
  meritChain->SetBranchAddress("CTBBestLogEnergy",&m_CTBBestLogEnergy_in);
  meritChain->SetBranchAddress("CTBCORE",&m_CTBCORE_in);
  meritChain->SetBranchAddress("CTBClassLevel",&m_CTBClassLevel_in);

  meritChain->SetBranchAddress("Tkr1SSDVeto",&m_Tkr1SSDVeto_in);
  meritChain->SetBranchAddress("CalEnergyRaw",&m_CalEnergyRaw_in);
  meritChain->SetBranchAddress("CalMIPRatio",&m_CalMIPRatio_in);
  meritChain->SetBranchAddress("CalCsIRLn",&m_CalCsIRLn_in);

  svacChain->SetBranchAddress("EvtTime",&m_timeStamp);
  svacChain->SetBranchAddress("FswDGNState",&m_FswDGNState);
  svacChain->SetBranchAddress("TkrNumTracks",&m_TkrNumTracks);
  svacChain->SetBranchAddress("Tkr1KalEne",&m_Tkr1KalEne_in);
  svacChain->SetBranchAddress("Tkr1NumHits",&m_Tkr1NumHits_in);
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
  b_CTBCALGamProb->GetEntry(m_idx);	
  b_CTBCPFGamProb->GetEntry(m_idx);	
  b_CTBBestEnergyProb->GetEntry(m_idx);	
  b_CTBBestLogEnergy->GetEntry(m_idx);	
  b_CTBCORE->GetEntry(m_idx);	
  b_CTBClassLevel->GetEntry(m_idx);	

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
  
  m_CTBTKRGamProb = m_CTBTKRGamProb_in;
  m_CTBCALGamProb = m_CTBCALGamProb_in;
  m_CTBCPFGamProb = m_CTBCPFGamProb_in;
  m_CTBClassLevel = m_CTBClassLevel_in;
  m_CTBBestEnergyProb = m_CTBBestEnergyProb_in;
  m_CTBBestLogEnergy = m_CTBBestLogEnergy_in;
  m_CTBCORE = m_CTBCORE_in;  
  m_CalMIPRatio = m_CalMIPRatio_in;  
  m_CalEnergyRaw = m_CalEnergyRaw_in;
  m_CalCsIRLn = m_CalCsIRLn_in;
  m_Tkr1SSDVeto = m_Tkr1SSDVeto_in;
  m_Tkr1KalEne = m_Tkr1KalEne_in;
  m_Tkr1NumHits = m_Tkr1NumHits_in;

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
  b_CTBCALGamProb = meritChain->GetBranch("CTBCALGamProb");
  if ( b_CTBCALGamProb == 0 ) return kFALSE;
  b_CTBCPFGamProb = meritChain->GetBranch("CTBCPFGamProb");
  if ( b_CTBCPFGamProb == 0 ) return kFALSE;
  b_CTBBestLogEnergy = meritChain->GetBranch("CTBBestLogEnergy");
  if ( b_CTBBestLogEnergy == 0 ) return kFALSE;
  b_CTBBestEnergyProb = meritChain->GetBranch("CTBBestEnergyProb");
  if ( b_CTBBestEnergyProb == 0 ) return kFALSE;
  b_CTBCORE = meritChain->GetBranch("CTBCORE");
  if ( b_CTBCORE == 0 ) return kFALSE;
  b_CTBClassLevel = meritChain->GetBranch("CTBClassLevel");
  if ( b_CTBClassLevel == 0 ) return kFALSE;

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
