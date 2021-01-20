
#include "AcdCalibLoop_Linked.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TChain.h"

#include "AcdHistCalibMap.h"
#include "AcdCalibMap.h"
#include "AcdCalibUtil.h"

#include "digiRootData/DigiEvent.h"
#include "reconRootData/ReconEvent.h"
#include "reconRootData/AcdAssoc.h"
#include "reconRootData/AcdTkrHitPocaV2.h"
#include "CalibData/Acd/AcdCalibObj.h"
#include <TMath.h>

#include <cassert>
#include <cmath>
#include <fstream>

using std::cout;
using std::cerr;
using std::endl;
using std::string;


AcdCalibLoop_Linked::AcdCalibLoop_Linked(AcdCalibData::CALTYPE t, 
					 TChain *digiChain, 
					 TChain *reconChain, 
					 TChain *meritChain,					 
					 Bool_t requirePeriodic, AcdKey::Config config)
  :AcdCalibBase(t,config), 
   m_digiEvent(0),
   m_reconEvent(0),
   m_requirePeriodic(requirePeriodic),
   m_pedHists(0),
   m_peakHists(0),
   m_rangeHists(0),
   m_threshHists(0){

  if ( digiChain ){
    setChain(AcdCalib::DIGI,digiChain);
  }
  if ( reconChain ){
    setChain(AcdCalib::RECON,reconChain);
  }
  if ( meritChain ){
    setChain(AcdCalib::MERIT,meritChain);
  }
  
  switch ( t ) {
  case AcdCalibData::MERITCALIB:
    m_pedHists = bookHists(AcdCalib::H_RAW,4096,-0.5,4095.5);
    m_peakHists = bookHists(AcdCalib::H_GAIN,500,-0.005,4.995);
    m_rangeHists = bookHists(AcdCalib::H_RANGE,4096,-0.5,4095.5,2);
    m_threshHists = bookHists(AcdCalib::H_VETO,256,-0.5,4095.5,3);    
    break;
  case AcdCalibData::GAIN:
    m_peakHists = bookHists(AcdCalib::H_GAIN,100,0.,3.);
    break;
  default:
    break;
  }

  Bool_t ok = attachChains();
  if ( ! ok ) {
    cerr << "ERR:  Failed to attach to input chains."  << endl;
  }
}


AcdCalibLoop_Linked::~AcdCalibLoop_Linked() 
{
  if (m_digiEvent) delete m_digiEvent;	
  if (m_reconEvent) delete m_reconEvent;
}

Bool_t AcdCalibLoop_Linked::attachChains() {
  TChain* digiChain = getChain(AcdCalib::DIGI);
  if (digiChain != 0) {
    m_digiEvent = 0;
    digiChain->SetBranchAddress("DigiEvent", &m_digiEvent);
    digiChain->SetBranchStatus("*",0);  // disable all branches
    // activate desired brances
    digiChain->SetBranchStatus("m_acd*",1);
    digiChain->SetBranchStatus("m_timeStamp", 1); 
    digiChain->SetBranchStatus("m_eventId", 1);
    digiChain->SetBranchStatus("m_runId", 1);
    digiChain->SetBranchStatus("m_gem", 1);
  }
  
  TChain* reconChain = getChain(AcdCalib::RECON);
  if (reconChain != 0) {
    m_reconEvent = 0;
    reconChain->SetBranchAddress("ReconEvent", &m_reconEvent);
    reconChain->SetBranchStatus("*",0);  // disable all branches
    // activate desired brances
    reconChain->SetBranchStatus("m_acdV2",1);
  }
				
  TChain* meritChain = getChain(AcdCalib::MERIT);
  if (meritChain != 0) {
    //meritChain->SetBranchAddress("", &);
    meritChain->SetBranchStatus("*",0);  // disable all branches
    // activate desired brances
    //meritChain->SetBranchStatus("Acd2*",1);
  }

  return kTRUE;
}

Bool_t AcdCalibLoop_Linked::readEvent(int ievent, Bool_t& filtered, 
				      int& runId, int& evtId, Double_t& timeStamp) {
  
  if(m_digiEvent) m_digiEvent->Clear();

  filtered = kFALSE;
  TChain* digiChain = getChain(AcdCalib::DIGI);
  if(digiChain) { 
    digiChain->GetEvent(ievent);
    evtId = m_digiEvent->getEventId(); 
    runId = m_digiEvent->getRunId();
    timeStamp = m_digiEvent->getTimeStamp();
    switch ( calType () ) {
    case AcdCalibData::MERITCALIB:
      if ( m_requirePeriodic &&
	   (m_digiEvent->getGem().getConditionSummary() != 32 &&  
	    m_digiEvent->getGem().getConditionSummary() != 128 ) )
	filtered = kTRUE;
      break;
    default:
      break;
    } 
  }

  TChain* reconChain = getChain(AcdCalib::RECON);
  if(reconChain) { 
    reconChain->GetEvent(ievent);
  }

  TChain* meritChain = getChain(AcdCalib::MERIT);
  if(meritChain) { 
    meritChain->GetEvent(ievent);
  }
    
  return kTRUE;
}


void AcdCalibLoop_Linked::useEvent(Bool_t& used) {

  used = kFALSE;

  if ( calType() == AcdCalibData::MERITCALIB ) {
    fillDigiInfo(used);
    return;
  } else if ( calType() == AcdCalibData::GAIN ) {
    fillRecoGainInfo(used);
    return;
  }
}



void AcdCalibLoop_Linked::fillDigiInfo(Bool_t& used) {

  const TObjArray* acdDigiCol = m_digiEvent->getAcdDigiCol();
  if (!acdDigiCol) return;

  int nAcdDigi = acdDigiCol->GetLast() + 1;
  for(int i = 0; i != nAcdDigi; ++i) {

    AcdDigi* acdDigi = static_cast<AcdDigi*>(acdDigiCol->At(i));
    assert(acdDigi != 0);

    const AcdId& acdId = acdDigi->getId();
    int id = acdId.getId();

    if ( ! AcdKey::channelExists( id ) ) continue;

    int rng0 = acdDigi->getRange(AcdDigi::A);    
    float pmt0 = (float)acdDigi->getPulseHeight(AcdDigi::A);

    int rng1 = acdDigi->getRange(AcdDigi::B);
    float pmt1 = (float)acdDigi->getPulseHeight(AcdDigi::B);

    Bool_t useA = rng0 == 0 && pmt0 > 0;
    Bool_t useB = rng1 == 0 && pmt1 > 0;

    UInt_t keyA = AcdKey::makeKey(AcdDigi::A,id);
    UInt_t keyB = AcdKey::makeKey(AcdDigi::B,id);

    Float_t pedA(0.);    
    Float_t pedB(0.);
    
    const AcdCalibMap* calib(0);
    const CalibData::AcdCalibObj* calibA(0);
    const CalibData::AcdCalibObj* calibB(0);
    switch ( calType() ){
    case AcdCalibData::MERITCALIB:
      pedA = getPeds(keyA);
      pedB = getPeds(keyB);
      if ( pedA < 0 || pedB < 0 ) return;
      pmt0 -= pedA;
      pmt1 -= pedB;
      if ( useA ) fillHist(*m_pedHists, id, AcdDigi::A, pmt0);
      if ( useB ) fillHist(*m_pedHists, id, AcdDigi::B, pmt1);
      if ( rng0 == 0 ) {
	fillHist(*m_rangeHists, id, AcdDigi::A, pmt0, 0);
	fillHist(*m_threshHists, id, AcdDigi::A, pmt0);
	if ( acdDigi->getVeto(AcdDigi::A) ) fillHist(*m_threshHists, id, AcdDigi::A, pmt0, 1);
      } else {
	fillHist(*m_rangeHists, id, AcdDigi::A, pmt0, 1);
      }
      if ( rng1 == 0 ) {
	fillHist(*m_rangeHists, id, AcdDigi::B, pmt1, 0);
	fillHist(*m_threshHists, id, AcdDigi::B, pmt1);
	if ( acdDigi->getVeto(AcdDigi::B) ) fillHist(*m_threshHists, id, AcdDigi::B, pmt1, 1);
      } else {
	fillHist(*m_rangeHists, id, AcdDigi::B, pmt1, 1);
      }
      break;
    default:
      break;
    }      
  }
}

/// Fill gain variables from reco information
void AcdCalibLoop_Linked::fillRecoGainInfo(Bool_t& used) {
  
  const TClonesArray& acdTkrAssocCol = m_reconEvent->getAcdReconV2()->getTkrAssocCol();
  UInt_t nTkrAssoc = acdTkrAssocCol.GetEntries();
  for ( UInt_t i(0); i < nTkrAssoc; i++ ) {
    const AcdAssoc* anAssoc = dynamic_cast<const AcdAssoc*>(acdTkrAssocCol.At(i));
    if (anAssoc == 0) continue;
    if ( anAssoc->getTrackIndex() !=0 ) continue;  // First track only
    if (! anAssoc->getUpward()) continue;            
    UInt_t nTkrHit = anAssoc->nAcdHitPoca();
    
    for ( UInt_t iHit(0); iHit < nTkrHit; iHit++ ) {
      const AcdTkrHitPocaV2* aHitPoca = anAssoc->getHitPoca(iHit);
      if ( ! aHitPoca->hasHit() ) continue;
      if ( aHitPoca->getDoca() < 10. ) continue;  // require 10 mm in from the edge      
      used = true;      
      fillHist(*m_peakHists, aHitPoca->getId().getId(), AcdDigi::A, aHitPoca->mipsPmtA()*aHitPoca->getCosTheta());
      fillHist(*m_peakHists, aHitPoca->getId().getId(), AcdDigi::B, aHitPoca->mipsPmtB()*aHitPoca->getCosTheta());      
    }    
  }
  return;
}

/// get the MIP value for one pmt
bool AcdCalibLoop_Linked::getMipValue(UInt_t key, UInt_t range, UInt_t pha, Float_t& mips) {
  if ( range == 0 ) {    
    const CalibData::AcdCalibObj* mip = getCalibMap(AcdCalibData::GAIN)->get(key);
    const CalibData::AcdCalibObj* ped = getCalibMap(AcdCalibData::PEDESTAL)->get(key);
    if ( mip == 0 || ped == 0 ) return false;
    mips = (pha - ped->operator[](0) ) / mip->operator[](0);
  } else {
    const CalibData::AcdCalibObj* calib = getCalibMap(AcdCalibData::HIGH_RANGE)->get(key);
    if ( calib == 0 ) return false;
    float pedRed = pha - calib->operator[](0);
    if ( pedRed < 0 ) {
      mips = 3.;
      return true;
    }
    float fromSat = calib->operator[](2) - pedRed;
    if ( fromSat < 50 ) {
      mips = 1000.;
      return true;
    }
    float top =  calib->operator[](2) * pedRed;
    float bot =  calib->operator[](1) * fromSat;
    mips = top/bot;
    if ( mips < 3.5 ) {
      mips = 3.5;
    } if ( mips > 1000) {
      mips = 1000;
    }
  }
  return true;
}
